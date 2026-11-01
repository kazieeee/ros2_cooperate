#include <memory>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>  // 多线程头文件
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

using std::placeholders::_1;

// 函数模板：算差值
template <typename T>
T absoluteDifference(T a, T b) {
    return (a > b) ? (a - b) : (b - a);
}

// 多线程工作函数（在类外面定义）
// 注意：history 必须传引用 &
void calc_worker(int current, int prev, std::vector<int>& history, rclcpp::Logger logger) {
    int diff = absoluteDifference(current, prev);
    
    history.push_back(diff);
    if (history.size() > 3) {
        history.erase(history.begin());
    }
    
    int warn_count = 0;
    for (int d : history) {
        if (d >= 15) warn_count++;
    }
    
    if (warn_count >= 2) {
        RCLCPP_WARN(logger, "System State: UNSTABLE");
    } else {
        RCLCPP_INFO(logger, "System State: STABLE");
    }
}

class MySubscriber : public rclcpp::Node {
public:
    MySubscriber() : Node("subscriber"), prev_speed_(0) {
        sub_ = this->create_subscription<std_msgs::msg::Int32>(
            "chatter", 10, std::bind(&MySubscriber::topic_cb, this, _1));
    }

private:
    void topic_cb(const std_msgs::msg::Int32::SharedPtr msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        
        speed_queue_.push(msg->data);
        
        if (speed_queue_.size() >= 2) {
            int current = speed_queue_.front();
            speed_queue_.pop();
            
            // 【核心】开多线程，调用 calc_worker 函数
            // std::ref(diff_his_) 保证传递的是引用，logger 是为了传打印工具
            std::thread t(calc_worker, current, prev_speed_, std::ref(diff_his_), this->get_logger());
            t.join(); // 等待子线程结束
            
            prev_speed_ = current;
        }
    }

    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr sub_;
    std::queue<int> speed_queue_;
    std::vector<int> diff_his_;
    std::mutex mtx_;
    int prev_speed_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MySubscriber>());
    rclcpp::shutdown();
    return 0;
}