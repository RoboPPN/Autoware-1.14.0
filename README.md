## ROS上的串口通信
编译通过后，插上串口并给串口赋予权限，将cpp上的'ser.setPort("/dev/ttyUSB1");'修改成你自己串口的设备号，
最后在终端运行：

```shell
rosrun serial_port_node serial_port_node
```
