#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
using namespace std;
#include <queue>
#include <cmath>
#include <vector>
class listener : public rclcpp::Node
{
private:
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscription_;
    std::queue<int> q;
    std::vector<int> q2;
    int count = 0;
    int r ;
    double r2;
    template<typename T>
    T absoluteDifference(const T&a,const T&b)
    {
        T number = abs(a-b);
        return number;


    }
    //回调函数
    void do_callback(const std_msgs::msg::Int32& msg)
    {   


        q.push(msg.data);
            
        RCLCPP_INFO(this->get_logger(),"收到速度: %d",msg.data);
        //输出订阅到的消息
        if(q.size()>2)
        {   
            q.pop();
            RCLCPP_INFO(this->get_logger(),"队列: [%d %d]",q.back(),q.front());
            RCLCPP_INFO(this->get_logger(),"处理速度: %d",q.front());
            r = absoluteDifference<int>(q.back(),q.front());
            r2 = absoluteDifference<double>(q.back(),q.front());
            if(r>=3)
            {
                RCLCPP_INFO(this->get_logger(),"WARNING");
            }
            else
            {
                RCLCPP_INFO(this->get_logger(),"NORMAL");
            }
        }
        else
        {
            RCLCPP_INFO(this->get_logger(),"队列: [%d]",q.back());
            RCLCPP_INFO(this->get_logger(),"暂不处理 ");
            r = absoluteDifference<int>(q.back(),0);
            r2 = absoluteDifference<double>(q.back(),0);
            if(r>=3)
            {
                RCLCPP_INFO(this->get_logger(),"WARNING");
            }
            else
            {
                RCLCPP_INFO(this->get_logger(),"NORMAL");
            }
        }
        q2.push_back(r);
        if(q2.size()>4)
        {
            q2.pop_back();
        }
        for (auto it = q2.begin(); it != q2.end(); ++it)
        {
            if(*it >= 3)
            {
                count++;
            }
        }
        if(count>2)
        {
            RCLCPP_INFO(this->get_logger(),"System State: UNSTABLE");
            count =0;
        }
        else
        {
            RCLCPP_INFO(this->get_logger(),"System State: STABLE");
        }
    }
public:
    listener():Node("listener_node_cpp")
    {
        RCLCPP_INFO(this->get_logger(),"订阅方创建");
        //创建订阅方
        subscription_ = this->create_subscription<std_msgs::msg::Int32>("chatter",10,bind(&listener::do_callback,this,placeholders::_1));//订阅的topic,qos队列长度，callback回调函数   传入的回调函数的参数用占位符
    }
};



int main(int argc,char const* argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(make_shared<listener>());
    rclcpp::shutdown();
    return 0;
}