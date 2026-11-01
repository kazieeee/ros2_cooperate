// 引入ROS2核心头文件，包含节点、发布、订阅、定时器、日志等基础API
#include "rclcpp/rclcpp.hpp"
// 引入标准字符串消息接口，std_msgs::msg::String就是ROS自带的字符串消息类型
#include "std_msgs/msg/int32.hpp"
// 使用std命名空间，后续string、bind、shared_ptr等就不用写std::了，简化代码
using namespace std;
using namespace std::chrono_literals; // 专门给 1s 500ms
#include <queue>
// 自定义发布节点类 Talker，继承 rclcpp::Node（ROS所有节点都必须继承这个基类）
class Talker : public rclcpp::Node
{
private:
    // 计数变量，用来给消息编号，每发一条消息数字+1
    int count_[6]= {2,5,4,8,3,6};
    int time = 0;
    
    // 发布器智能指针：模板参数是消息类型String，SharedPtr是共享智能指针，自动管理内存，不用手动delete
    rclcpp::Publisher<std_msgs::msg::Int32 >::SharedPtr publisher_;
    // 定时器智能指针：定时触发回调函数，周期性执行发布逻辑
    rclcpp::TimerBase::SharedPtr timer_;

    // 定时器回调函数：到设定时间就自动执行这个函数
    void on_timer()
    {
        // 创建一个int类型消息对象，这个就是要发给订阅方的消息包 

        
        int number = count_[time];

        auto message = std_msgs::msg::Int32();

        message.data = number;

        // RCLCPP_INFO：ROS日志打印宏，相当于printf，输出信息日志
        // this->get_logger() 获取当前节点的日志器
        
        RCLCPP_INFO(this->get_logger(), "服务端发布的消息:%d ",message.data);

        // 调用发布器的publish方法，把封装好的消息发送到话题chatter上
        publisher_->publish(message);
        time++;
        //time2++;
        if(time>=6)
        {
            time = 0;
            
        }


    }

public:
    // 构造函数：节点创建的时候自动执行
    // Node("talker_node_cpp")：调用父类构造，给节点起名字叫 talker_node_cpp
    Talker() :Node("talker_node_cpp")
    {
        // 节点启动时打印日志，证明节点成功创建
        RCLCPP_INFO(this->get_logger(), "发布节点创建");

        // 创建发布器
        // 参数1：话题名 chatter，订阅者必须用完全一样的话题名才能收到消息
        // 参数2：队列深度10，消息缓存队列，当消息发得太快，来不及处理时最多缓存10条，超出丢弃旧消息
        publisher_ = this->create_publisher<std_msgs::msg::Int32>("chatter", 10);

        // 创建定时器
        // 第一个参数：1s，每隔1秒触发一次回调
        // bind(&Talker::on_timer, this)：绑定类内成员函数on_timer作为回调
        // 类成员函数自带隐藏参数this，bind必须传入this，否则编译报错
        timer_ = this->create_wall_timer(1s, bind(&Talker::on_timer, this));
    }
};

// 程序入口main函数
int main(int argc, char const* argv[])
{
    // rclcpp初始化：解析命令行参数，启动ROS2通信底层，必须写在最前面
    rclcpp::init(argc, argv);

    // make_shared<Talker>() 创建Talker节点对象（智能指针）
    // rclcpp::spin() 回旋函数（自旋）：阻塞在这里，持续监听定时器、话题等事件，等待回调触发
    // spin不结束，后面代码不会跑；一旦节点退出，spin才返回
    rclcpp::spin(make_shared<Talker>());

    // ROS2关闭，释放资源，清理通信上下文
    rclcpp::shutdown();
    return 0;
}