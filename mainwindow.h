#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QTime>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend void *deal_play(void *arg);
public:
    QProcess *process;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
   void play_song(QString file_name);
   void paintEvent(QPaintEvent *event);

private slots:
   void back_message_slots();
   //void deal_nowtime();
   void timeDone();  //定时槽函数
   void timedone();
   void on_pushButton_clicked(); //暂停
   //void on_pushButton_2_clicked();//减小音量
   //void on_pushButton_3_clicked();//增大音量
   void on_toolButton_clicked();//静音
   void on_toolButton_3_clicked();//下一首

   void on_listWidget_currentRowChanged();

   void on_toolButton_2_clicked();

   void on_horizontalSlider_sliderMoved(int position);

  // void on_horizontalSlider_valueChanged(int value);



   void on_toolButton_4_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timeClock;
    QTimer *timeClock1;
    bool jingyin;//静音标志
};



#endif // MAINWINDOW_H
