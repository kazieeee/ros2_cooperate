#include <memory>
#include <vector>
#include <mutex>
#include <string>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using std::placeholders::_1;

class MySubscriber : public rclcpp::Node {
public:
    MySubscriber() : Node("my_subscriber") {
        subscription_ = this->create_subscription<std_msgs::msg::String>(
            "chatter", 10, std::bind(&MySubscriber::topic_callback, this, _1));
        timer_ = this->create_wall_timer(
            std::chrono::seconds(1), std::bind(&MySubscriber::timer_callback, this));
    }
private:
    void topic_callback(const std_msgs::msg::String::SharedPtr msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        msg_cache_.push_back(msg->data);
    }
    void timer_callback() {
        std::lock_guard<std::mutex> lock(mtx_);
        RCLCPP_INFO(this->get_logger(), "目前总共收到了 %zu 条消息", msg_cache_.size());
        for (const auto& m : msg_cache_) {
            RCLCPP_INFO(this->get_logger(), "历史消息: %s", m.c_str());
        }
    }
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::vector<std::string> msg_cache_;
    std::mutex mtx_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MySubscriber>());
    rclcpp::shutdown();
    return 0;
}
