＃串口调试助手该程序使用Qt框架，C ++语言编译而成

项目文件介绍：

    main.cpp            该文件为该程序的入口程序

    mainwindow.h        该文件为该程序的主要声明部分
    
    mainwindow.cpp      该文件为该程序的主要定义部分
    
    mainwindow.ui       该文件为该程序的ui界面设计
    
    界面.png            界面的显示效果

该文件中获取串口是通过读取Windows系统下的注册表中的信息得到的

    - 使用Qt中的定时器来每个3s读取一次注册表

串口通信方面：通过使用Qt的封装的QSerialPort来实现
