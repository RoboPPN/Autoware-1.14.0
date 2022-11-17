#include <ros/ros.h>
#include <serial/serial.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <geometry_msgs/TwistStamped.h>

serial::Serial ser;

#pragma pack(1)  //因为串口通讯发送是以byte(字节)为单位,Ｃ编译器将按照１个字节对齐
 typedef struct 
    {
        uint8_t start = (unsigned)'S';
        float linear_x;
        float angle_z;
        uint8_t end = (unsigned)'E';
    }Cmd_Car;
#pragma pack()  //取消自定义字节对齐方式

/*注：若不用#pragma pack(1)和#pragma pack()括起来，
则sample按编译器默认方式对齐（成员中size最大的那个）。
即按4字节（folat）对齐，则sizeof(Cmd_Car)==16.成员uint8_t start占了4个字节（其中3个是空字节）；
若用#pragma pack(1)，则sample按1字节方式对齐sizeof(sample)＝＝10.（无空字节），比较节省空间啦，有些场和还可使结构体更易于控制。*/


#pragma pack(1)
typedef struct 
{
    float car_linear_x;
    float car_angler_z;
}Car_Info;
#pragma pack()


void write_callback(const geometry_msgs::TwistStamped::ConstPtr& msg){
    Cmd_Car car_cmd;
    car_cmd.linear_x=msg->twist.linear.x;
    car_cmd.angle_z=-msg->twist.angular.z;

    ROS_INFO_STREAM("Start fream:  " << car_cmd.start);
    ROS_INFO_STREAM("linear_x speed:  " << msg->twist.linear.x);
    ROS_INFO_STREAM("angler_z speed:  " << msg->twist.angular.z);
    ROS_INFO_STREAM("End fream:  " << car_cmd.end);

    ser.write((uint8_t *)&car_cmd, sizeof(car_cmd));
}

int main (int argc, char** argv){
    ros::init(argc, argv, "serial_port_node");
    ros::NodeHandle nh;

    ros::Subscriber write_sub = nh.subscribe("/twist_cmd", 1000, write_callback);
    // ros::Publisher read_pub = nh.advertise<std_msgs::String>("read", 1000);

    
    try
    {
        ser.setPort("/dev/ttyUSB1");
        ser.setBaudrate(115200);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
    }
    catch (serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port ");
        return -1;
    }

    if(ser.isOpen()){
        ROS_INFO_STREAM("Serial Port initialized");
    }else{
        return -1;
    }

    ros::Rate loop_rate(5);
    while(ros::ok()){

        ros::spinOnce();

        if(ser.available()){
            ROS_INFO_STREAM("Reading from serial port");
            // std_msgs::String result;
            // result.data = ser.read(ser.available());
            // ROS_INFO_STREAM("Read: " << result.data);
            // Car_Info car_info;
            // car_info.car_angler_z=ser.read((uint8_t *)&car_info, sizeof(car_info));
            // result.linear.x=car_info.car_linear_x;
            // result.angler.z=car_info.car_angler_z;
            // read_pub.publish(result);
        }
        loop_rate.sleep();

    }
}
