#include <chrono>
#include <memory>
#include <vector>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

using namespace std::chrono_literals;

class MyPublisher : public rclcpp::Node {
public:
    MyPublisher() : Node("publisher"), index_(0) {
        // 创建发布者，话题名为 "chatter"
        pub_ = this->create_publisher<std_msgs::msg::Int32>("chatter", 10);
        // 创建定时器，1秒触发一次
        timer_ = this->create_wall_timer(1s, std::bind(&MyPublisher::timer_cb, this));
    }

private:
    void timer_cb() {
        auto msg = std_msgs::msg::Int32();
        
        // 从数组中提取当前数据
        msg.data = data_array_[index_];
        RCLCPP_INFO(this->get_logger(), "发送: %d", msg.data);
        pub_->publish(msg);
        
        // 索引加1，并循环（如果到末尾就回到0）
        index_ = (index_ + 1) % data_array_.size();
    }
    
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    // 题目要求的数组
    std::vector<int> data_array_ = {10, 20, 15, 35, 40, 5};
    size_t index_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MyPublisher>());
    rclcpp::shutdown();
    return 0;
}
