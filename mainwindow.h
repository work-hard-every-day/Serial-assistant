#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    /*
     * 功能：获取电脑中串口的端口
     * 参数：无
     * 返回值：无
     */
    void Get_Serial_Port(void);
    /*
     * 功能：当串口有数据的时候执行
     * 参数：无
     * 返回值：无
     */
    void readData(void);
    /*
     * 功能：每个3s执行的任务
     * 参数：无
     * 返回值：无
     */
    void myThread(void);

private slots:
    /*
     * 功能：点击pushButton按钮功能
     * 参数：无
     * 返回值：无
     */
    void on_pushButton_clicked();
    /*
     * 功能：点击清空按钮功能，清空显示区的显示
     * 参数：无
     * 返回值：无
     */
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow  *ui;
    //串口类指针
    QSerialPort     *Serial;
    //时间类指针
    QTimer          *time;
};

#endif // MAINWINDOW_H
