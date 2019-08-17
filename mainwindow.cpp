#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "windows.h"
#include "QVector"
#include "QDebug"
#include "stdio.h"
#include "QMessageBox"
#include <stdlib.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

/*
 * 功能：读取注册表下的子项
 * 参数：hkey:注册表的根
 *      lpSubkey:注册表根下的路径
 *      retArray:返回要查找的路径下的值的数组
 * 返回值：无
 */
static void Get_Regedit(HKEY hkey,LPCSTR lpSubKey,QVector<QString> &retArray);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //时间类初始化
    time = new QTimer(this);
    connect(time,&QTimer::timeout,this,&MainWindow::myThread);
    time->start(3000);
    //状态栏显示
    ui->statusBar->showMessage("程序运行中...");
    //初始化串口的显示
    this->Get_Serial_Port();
    QStringList temp;
    //波特率的显示
    temp << "9600" << "4800" << "19200" << "38400" << "57600" << "115200";
    ui->comboBox_2->addItems(temp);
    //数据位的显示
    temp.clear();
    temp << "8" << "5" << "6" << "7";
    ui->comboBox_3->addItems(temp);
    //奇偶检验位的显示
    temp.clear();
    temp << "0" << "1" << "2";
    ui->comboBox_4->addItems(temp);
    //停止位的显示
    temp.clear();
    temp << "1" << "1.5" << "2";
    ui->comboBox_5->addItems(temp);

    this->Serial = new QSerialPort(nullptr);
}

MainWindow::~MainWindow()
{
    delete ui;
}
/*
 * 功能：获取电脑中串口的端口
 * 参数：无
 * 返回值：无
 */
void MainWindow::Get_Serial_Port()
{
    QVector<QString> retArray;
    ui->comboBox->clear();
    Get_Regedit(HKEY_LOCAL_MACHINE,\
                "HARDWARE\\DEVICEMAP\\SERIALCOMM",\
                retArray);

    qDebug() << retArray.size();

    QVector<QString>::iterator iter;
    for (iter=retArray.begin();iter!=retArray.end();iter++)
    {
        qDebug() <<  *iter << "\0";
        ui->comboBox->addItem(*iter);
    }
}
/*
 * 功能：点击pushButton按钮功能,打开串口
 * 参数：无
 * 返回值：无
 */
void MainWindow::on_pushButton_clicked()
{
    if(!Serial->isOpen())
    {
        qDebug() << ui->comboBox->currentText();
        //设置串口的端口名称
        Serial->setPortName(ui->comboBox->currentText());
        //toInt：将字符串转换为数字
        //设置串口的波特率
        Serial->setBaudRate((ui->comboBox_2->currentText()).toInt(nullptr,10));
        //设置串口的数据位
        Serial->setDataBits((QSerialPort::DataBits((ui->comboBox_3->currentText()).toInt(nullptr,10))));
        //设置串口的奇偶校验位
        Serial->setParity(QSerialPort::Parity((ui->comboBox_4->currentText()).toInt(nullptr,10)));
        //设置串口的停止位
        Serial->setStopBits(QSerialPort::StopBits((ui->comboBox_5->currentText()).toInt(nullptr,10)));
        //设置串口的流
        Serial->setFlowControl(QSerialPort::NoFlowControl);
        BOOL isSerial = Serial->open(QIODevice::ReadWrite);
        if(!isSerial)
        {
            qDebug() << "串口打开错误！";
            return;
        }
        //创建一个信号与槽，使得串口有数据可以读取的时候可以执行readData()函数
        connect(Serial,&QSerialPort::readyRead,this,&MainWindow::readData);
        ui->pushButton->setText("已启动");
    }
    else
    {
        ui->pushButton->setText("启动");
        Serial->close();
    }

}

/*
 * 功能：读取注册表下的子项
 * 参数：hkey:注册表的根
 *      lpSubkey:注册表根下的路径
 *      retArray:返回要查找的路径下的值的数组
 * 返回值：无
 */
static void Get_Regedit(HKEY hkey,LPCSTR lpSubKey,QVector<QString> &retArray)
{
    HKEY  phkey = nullptr;
    BOOL isSuccess = false;
    /*
     * 功能：打开注册表，返回值为是否打开成功
     */
    isSuccess = RegOpenKeyA(hkey,lpSubKey,&phkey);
    if(isSuccess != ERROR_SUCCESS)
    {
        qDebug() << "注册表打开失败！";
        return;
    }
    qDebug() << "注册表打开成功！";
    /*
     * 功能：读取注册表下的子项
     */
    DWORD i =0;
    LSTATUS retCode = ERROR_SUCCESS;

    CHAR achValue[MAX_VALUE_NAME];
    DWORD cchValue = MAX_VALUE_NAME;
    BYTE Data[MAX_VALUE_NAME];
    DWORD cbData = MAX_VALUE_NAME;
    do
    {
        cchValue = MAX_VALUE_NAME;
        cbData = MAX_VALUE_NAME;
        achValue[0] = '\0';
        Data[0] = '\0';
        QString temp = "";
        retCode = RegEnumValueA(phkey, i,achValue,&cchValue,nullptr,nullptr,Data,&cbData);

        if (retCode == ERROR_SUCCESS && achValue[0] != '\0')
        {
            qDebug() << i++ << achValue << " ";
            BYTE j = 0;
            while(Data[j] != '\0')
                temp += (CHAR)(Data[j++]);
            qDebug() << temp;
            retArray.append(temp);
        }
    }while(achValue[0] != '\0');
    /*
     * 功能：关闭注册表，返回值为是否打开成功
     */
    isSuccess = RegCloseKey(phkey);
    if(isSuccess != ERROR_SUCCESS)
    {
        qDebug() << "注册表关闭失败！";
        return;
    }
    qDebug() << "注册表关闭成功！";
    return;
}

/*
 * 功能：点击清空按钮功能，清空显示区的显示
 * 参数：无
 * 返回值：无
 */
void MainWindow::on_pushButton_2_clicked()
{
    ui->textBrowser->setText("");
}

/*
 * 功能：当串口有数据的时候执行,在显示区域显示
 *      串口接受到的值
 * 参数：无
 * 返回值：无
 */
void MainWindow::readData(void)
{
    //是否选择了该按钮,选择以16进制进行输出
    if(ui->radioButton->isChecked())
    {
        QByteArray temp = Serial->readAll().toHex();
        for(int i = 0;i < temp.length();++i)
        {
            //在16进制开始加入"0x"
            if(i % 2 == 0)
                ui->textBrowser->insertPlainText("0x");
            ui->textBrowser->insertPlainText((QString)temp.at(i));
            //在16进制结束加上空格" "
            if(i % 2 == 1)
                ui->textBrowser->insertPlainText(" ");
        }
    }
    //没有选择则按照ASCII码输出
    else
        ui->textBrowser->insertPlainText(Serial->readAll());
    ui->textBrowser->moveCursor(QTextCursor::End);
}
/*
 * 功能：向串口中发送数据
 * 参数：无
 * 返回值：无
 */
void MainWindow::on_pushButton_3_clicked()
{
    //判断串口是否处于打开状态
    if(Serial->isOpen())
    {
        QByteArray temp = ui->textEdit->toPlainText().toUtf8();
        qDebug() << temp;
        Serial->write(temp);
    }
    else
    {
        //串口没有连接的时候发送数据就会出错
        QMessageBox messageBox(QMessageBox::Icon(2),"警告","串口未连接",QMessageBox::Yes,nullptr);
        messageBox.exec();
    }
}
/*
 * 功能：清空发送区
 * 参数：无
 * 返回值：无
 */
void MainWindow::on_pushButton_4_clicked()
{
    ui->textEdit->clear();
}
/*
 * 功能：退出程序
 * 参数：无
 * 返回值：无
 */
void MainWindow::on_pushButton_5_clicked()
{
    if(Serial->isOpen())
        Serial->close();
    this->close();
}
/*
 * 功能：每个3s执行的任务,判断端口和串口是否打开
 * 参数：无
 * 返回值：无
 */
void MainWindow::myThread()
{
    qDebug() << "线程OK ";
    if(Serial->isReadable())
        ui->pushButton->setText("已启动");
    else
        ui->pushButton->setText("启动");
    this->Get_Serial_Port();
}
