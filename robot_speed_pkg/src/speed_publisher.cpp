
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
using namespace std;
using namespace std::chrono_literals; // 专门给 1s 500ms
#include <queue>
class Talker : public rclcpp::Node
{
private:
    int count_[6]= {2,5,4,8,3,6};
    int time = 0;
    
    rclcpp::Publisher<std_msgs::msg::Int32 >::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    void on_timer()
    {

        
        int number = count_[time];

        auto message = std_msgs::msg::Int32();

        message.data = number;

        
        RCLCPP_INFO(this->get_logger(), "服务端发布的消息:%d ",message.data);

        publisher_->publish(message);
        time++;
        //time2++;
        if(time>=6)
        {
            time = 0;
            
        }


    }

public:
    // 构造函数：
    Talker() :Node("talker_node_cpp")
    {
        RCLCPP_INFO(this->get_logger(), "发布节点创建");

        // 创建发布器

        publisher_ = this->create_publisher<std_msgs::msg::Int32>("chatter", 10);

        // 创建定时器

        timer_ = this->create_wall_timer(1s, bind(&Talker::on_timer, this));
    }
};


int main(int argc, char const* argv[])
{

    rclcpp::init(argc, argv);


    rclcpp::spin(make_shared<Talker>());

    rclcpp::shutdown();
    return 0;
}
