#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include "qj.h"
#include <QListWidgetItem>
#include <map>
#include <algorithm>
#include <vector>
#include <QPalette>
#include <QPainter>
#include <QPixmap>

using namespace std;

map<int,QString> m_lrc;
map<int,QString> songName;
vector<int> v_lrc_time;

int currentTimeNum;
int totalTimeNum;

extern map<int ,QString> lrc_analysis(map<int ,QString> m_lrc, char *lrc[], int row);
extern int lrc_cut(char *data, char *buf[]);
extern void read_lrc(char *lrc_name);
extern void *deal_write_lrc(void *arg);


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    kongjian();
    process = new QProcess(this);        //将指针指向一个进程
    play_song("艺术家们.mp3");
    flag = true;
    gedan();
    QListWidgetItem *item = new QListWidgetItem(" 音 乐 列 表 ");
    ui->listWidget->addItem(item);
    QStringList list;
    list<<" "<<p[0]<<p[1]<<p[2]<<p[3]<<p[4];
    ui->listWidget->addItems(list);

    item = ui->listWidget->item(num+2);   //fan xian
    item->setSelected(true);

    ui->pushButton->setIcon(QIcon(""));
    ui->pushButton->setIcon(QIcon(":/resource/pause.svg"));
    ui->pushButton->setIconSize(QSize(45,45));

    ui->toolButton->setIcon(QIcon(":/resource/mute_1.png"));
    ui->toolButton->setIconSize(QSize(45,45));

    char num1[8] = "50";
    process->write("volume 50 1\n");
    ui->volume->setValue(50);
    ui->label_volume->setText((const QString &)num1);
    //process->setProcessChannelMode(QProcess::MergedChannels);
    //ui->mainToolBar
    //创建一个线程
    pthread_t falg_lrc;
    pthread_create(&falg_lrc,NULL,deal_write_lrc,(void *)ui);
    pthread_detach(falg_lrc);

    ui->toolButton_4->setIcon(QIcon(":/resource/loop_one.svg"));
    ui->listWidget_2->addItem("暂无歌词");

   // QListWidgetItem *item = new QListWidgetItem();
    ui->listWidget_2->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}
MainWindow::~MainWindow()
{
    process->write("quit\n");
    delete ui;
}

void MainWindow::play_song(QString file_name)
{
    QStringList args;
    args << "-slave";
    args << "-quiet";
    args << "-idle";
    args << file_name;
    jingyin = false;
    process->start("mplayer", args);        //启动该进程，并传入参数args

    flag = true;

    timeClock = new QTimer(this);
    timeClock->start(10);
    connect(timeClock,SIGNAL(timeout()),this,SLOT(timeDone()));  //定时更新进度条

    timeClock1 = new QTimer(this);
    timeClock1->start(1000);
    connect(timeClock1,SIGNAL(timeout()),this,SLOT(timedone()));

    connect(ui->volume, &QSlider::valueChanged, [=](){
        int i = ui->volume->value();
        char volume[32] = "";
        sprintf(volume, "volume %d 1\n", i);
        process->write(volume);
        // process->write("pause\n");
        char buf[16] = "";
        sprintf(buf, "%02d%", i);
        ui->label_volume->setText((const QString &)buf);
    });
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter *painter = new QPainter(this);
        QPixmap pix;
        pix.load(":/resource/background.jpg");
        painter->drawPixmap(this->rect(),pix);

}

void MainWindow::on_pushButton_clicked()
{

    if(flag == true)
    {
        ui->pushButton->setIcon(QIcon(":/resource/play.svg"));
        ui->pushButton->setIconSize(QSize(45,45));

        timeClock->stop();
        process->write("pause\n");
        flag = !flag;
    }
    else
    {
        ui->pushButton->setIcon(QIcon(":/resource/pause.svg"));
        ui->pushButton->setIconSize(QSize(45,45));
        timeClock->start(10);
        process->write("pause\n");
        flag = !flag;
    }
}

void MainWindow::on_toolButton_clicked()
{

    if(jingyin == false)
    {
        ui->toolButton->setIcon(QIcon(":/resource/mute_0.png"));
        ui->toolButton->setIconSize(QSize(45,45));
        process->write("mute 1\n");
        jingyin = !jingyin;

       // int i = ui->volume->value();
        save_volume = ui->volume->value();
        int i = 0;
        ui->volume->setValue(i);
        char volume[32] = "";
        sprintf(volume, "volume %d 1\n", i);
        process->write(volume);
        // process->write("pause\n");
        char buf[16] = "";
        sprintf(buf, "%02d%", i);
        ui->label_volume->setText((const QString &)buf);
    }
    else
    {
        ui->toolButton->setIcon(QIcon(":/resource/mute_1.png"));
        ui->toolButton->setIconSize(QSize(45,45));
        process->write("mute 0\n");
        jingyin = !jingyin;

        int i = save_volume;
        ui->volume->setValue(i);
        char volume[32] = "";
        sprintf(volume, "volume %d 1\n", i);
        process->write(volume);
        // process->write("pause\n");
        char buf[16] = "";
        sprintf(buf, "%02d%", i);
        ui->label_volume->setText((const QString &)buf);
    }
}

//xia yi shou
void MainWindow::on_toolButton_3_clicked()
{
    gedan();
    num++;
    ui->pushButton->setIcon(QIcon(":/resource/pause.svg"));
    ui->pushButton->setIconSize(QSize(45,45));
    timeClock->start(10);
    flag = true;
    if(*(p[num]) != 0)
    {
        gedan();
        char buf[128] = "loadfile ";
        strcat(buf,p[num]);
        strcat(buf,"\n");
        printf("%s\n",buf);
        process->write(buf);
        gedan();
        char *buf2 = NULL;
        buf2 = strtok(p[num],".");
        sprintf(buf2, "%s%s",p[num],".lrc");
        printf("%s\n",buf2);
        read_lrc(buf2);
        gedan();
        buf2 = strtok(p[num],".");
        sprintf(buf2, "%s%s",p[num],".lrc");
        printf("%s\n",buf2);
        FILE *fpr = fopen(buf2, "r");
        if (fpr == NULL)
        {
            perror("fopen");
            ui->listWidget_2->clear();
            m_lrc.clear();
           // QListWidgetItem *item = new QListWidgetItem();
            ui->listWidget_2->addItem("暂无歌词");
            ui->listWidget_2->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
    }
    else
    {
        num--;
    }

    QListWidgetItem *item = new QListWidgetItem();
    item = ui->listWidget->item(num+2);
    item->setSelected(true);

}

void MainWindow::on_listWidget_currentRowChanged()
{
    ui->pushButton->setIcon(QIcon(":/resource/pause.svg"));
    ui->pushButton->setIconSize(QSize(45,45));
    timeClock->start(10);
    flag = true;
    int i = ui->listWidget->currentRow();
    gedan();
    if(i>=2 && *(p[i-2]) != 0)
    {
        num = i-2;
        char buf[128] = "loadfile ";
        strcat(buf,p[i-2]);
        strcat(buf,"\n");
        process->write(buf);

        char *buf2 = NULL;
        buf2 = strtok(p[num],".");
        sprintf(buf2, "%s%s",p[num],".lrc");
        //printf("%s\n",buf2);
        read_lrc(buf2);
        gedan();
        buf2 = strtok(p[num],".");
        sprintf(buf2, "%s%s",p[num],".lrc");
        printf("%s\n",buf2);
        FILE *fpr = fopen(buf2, "r");
        if (fpr == NULL)
        {
            perror("fopen");
            ui->listWidget_2->clear();
            m_lrc.clear();
            ui->listWidget_2->addItem("暂无歌词");
            //QListWidgetItem *item = new QListWidgetItem();
            ui->listWidget_2->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
    }
}

void MainWindow::back_message_slots()  //更新进度条的槽函数
{
    while(process->canReadLine())
    {
        QByteArray b(process->readLine());

        b.replace(QByteArray("\n"), QByteArray(""));
        QString s(b);

        if (b.startsWith("ANS_LENGTH"))  //输出总时间
        {
            totalTimeNum = s.mid(11).toFloat();

            int totalTimeSec = (int)(totalTimeNum) % 60;  //提取秒
            int totalTimeMin = (int)(totalTimeNum) / 60;  //提取分钟
            QString totalTime = QString("%1:%2").arg(totalTimeMin).arg(totalTimeSec);  //标准格式输出时间
            ui->label_3->setText(totalTime);
            // ui->horizontalSlider->setRange(0,totalTimeNum);
        }
        else if (b.startsWith("ANS_TIME_POSITION"))  //输出当前时间
        {
            currentTimeNum = s.mid(18).toFloat();
            // printf("%d\n",currentTimeNum);
            int currentTimeSec = (int)(currentTimeNum) % 60;
            int currentTimeMin = (int)(currentTimeNum) / 60;
            QString currentTime = QString("%1:%2").arg(currentTimeMin).arg(currentTimeSec);
            ui->label->setText(currentTime);
            // ui->horizontalSlider->setValue(currentTimeNum);
        }
        else if(b.startsWith("ANS_PERCENT_POSITION"))
        {
            QString currentPercent = s.mid(21);
            ind = s.mid(21).toInt();

            ui->label_2->setText(currentPercent + "%");
            ui->horizontalSlider->setValue(ind);
        }
    }

}

void MainWindow::timeDone()
{
    process->write("get_time_pos\n");
    process->write("get_percent_pos\n");
    process->write("get_time_length\n");
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(back_message_slots()));  //信号函数与槽函数连接
}

void MainWindow::timedone()
{
    // printf("%d\n",f);
    if(currentTimeNum == totalTimeNum -1  && f == 0)//dan qu
    {
        // printf("%d\n",num);
        gedan();
        char buf5[128] = "loadfile ";
        strcat(buf5,p[num]);
        strcat(buf5,"\n");
        printf("%s\n",buf5);
        process->write(buf5);

        gedan();
        char *buf2 = NULL;
        buf2 = strtok(p[num],".");
        sprintf(buf2, "%s%s",p[num],".lrc");
        printf("%s\n",buf2);
        read_lrc(buf2);
        gedan();
        buf2 = strtok(p[num],".");
        sprintf(buf2, "%s%s",p[num],".lrc");
        printf("%s\n",buf2);
        FILE *fpr = fopen(buf2, "r");
        if (fpr == NULL)
        {
            perror("fopen");
            ui->listWidget_2->clear();
            m_lrc.clear();

            ui->listWidget_2->addItem("暂无歌词");
            ui->listWidget_2->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
        QListWidgetItem *item = new QListWidgetItem();
        item = ui->listWidget->item(num+2);
        item->setSelected(true);
    }
    if(currentTimeNum == totalTimeNum -1  && f == 1)//shun xu
    {
        gedan();
        printf("******\n");
        if(*(p[num]) != 0 && *(p[num + 1]) != 0)
            num++;
        else
            num = 0;
        char buf5[128] = "loadfile ";
        strcat(buf5,p[num]);
        strcat(buf5,"\n");
        printf("%s\n",buf5);
        process->write(buf5);
        // sleep(1);
        gedan();
        char *buf2 = NULL;
        buf2 = strtok(p[num],".");
        sprintf(buf2, "%s%s",p[num],".lrc");
        printf("%s\n",buf2);
        read_lrc(buf2);
        gedan();
        buf2 = strtok(p[num],".");
        sprintf(buf2, "%s%s",p[num],".lrc");
        printf("%s\n",buf2);
        FILE *fpr = fopen(buf2, "r");
        if (fpr == NULL)
        {
            perror("fopen");
            ui->listWidget_2->clear();
            m_lrc.clear();

            ui->listWidget_2->addItem("暂无歌词");
            ui->listWidget_2->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
        QListWidgetItem *item = new QListWidgetItem();
        item = ui->listWidget->item(num+2);
        item->setSelected(true);
    }
    if(currentTimeNum == totalTimeNum -1  && f == 2)//sui ji
    {
        srand(time(NULL));
        int o = 0;
        while(*(p[o]) != 0)
        {
            o++;
        }
        o = rand()%(o+1);
        gedan();
        char buf5[128] = "loadfile ";
        strcat(buf5,p[o]);
        strcat(buf5,"\n");
        printf("%s\n",buf5);
        process->write(buf5);

        gedan();
        char *buf2 = NULL;
        buf2 = strtok(p[0],".");
        sprintf(buf2, "%s%s",p[0],".lrc");
        printf("%s\n",buf2);
        read_lrc(buf2);
        gedan();
        buf2 = strtok(p[o],".");
        sprintf(buf2, "%s%s",p[o],".lrc");
        printf("%s\n",buf2);
        FILE *fpr = fopen(buf2, "r");
        if (fpr == NULL)
        {
            perror("fopen");
            ui->listWidget_2->clear();
            m_lrc.clear();

            ui->listWidget_2->addItem("暂无歌词");
            ui->listWidget_2->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
        QListWidgetItem *item = new QListWidgetItem();
        item = ui->listWidget->item(o+2);
        item->setSelected(true);
    }

}
//shang yi qu
void MainWindow::on_toolButton_2_clicked()
{

    num--;
    ui->pushButton->setIcon(QIcon(":/resource/pause.svg"));
    ui->pushButton->setIconSize(QSize(45,45));
    timeClock->start(10);
    flag = true;
    if(num >= 0)
    {
        gedan();
        char buf[128] = "loadfile ";
        strcat(buf,p[num]);
        strcat(buf,"\n");
        process->write(buf);
        printf("%s\n",buf);
         gedan();
        m_lrc.clear();
        v_lrc_time.clear();
        char *buf2 = NULL;
        buf2 = strtok(p[num],".");
        sprintf(buf2, "%s%s",p[num],".lrc");
        printf("%s\n",buf2);
        read_lrc(buf2);

        gedan();
        buf2 = strtok(p[num],".");
        sprintf(buf2, "%s%s",p[num],".lrc");
        printf("%s\n",buf2);
        FILE *fpr = fopen(buf2, "r");
        if (fpr == NULL)
        {
            perror("fopen");
            ui->listWidget_2->clear();
            m_lrc.clear();
            ui->listWidget_2->addItem("暂无歌词");
            ui->listWidget_2->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
    }
    else
    {
        num =0;
    }
    QListWidgetItem *item = new QListWidgetItem();
    item = ui->listWidget->item(num+2);
    item->setSelected(true);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    timeClock->stop();
    this->process->write(QString("seek "+QString::number(position)+" 1\n").toUtf8());
    timeClock->start(10);
}

void read_lrc(char *lrc_name)//读歌词
{
    //读歌词文件
    FILE *fpr = fopen(lrc_name, "r");
    if (fpr == NULL)
    {
        printf("*****\n");
        perror("fopen");
        return;
    }
    fseek(fpr, 0, 2);
    long len = ftell(fpr);
    char *file = (char *)calloc(1, len + 1);
    rewind(fpr);
    fread(file, len, 1, fpr);
    fclose(fpr);
    char *lrc_file = file;

    //歌词文件按行切割
    char *buf[128] = {NULL};
    int row = 0;
    row = lrc_cut(lrc_file, buf);

    //逐行解析歌词 并存入链表
    //LRC *head = NULL;
    m_lrc.clear();
    v_lrc_time.clear();
    m_lrc = lrc_analysis(m_lrc, buf, row);
}

int lrc_cut(char *data, char *buf[])
{
    int i = 0;
    buf[i] = strtok(data, "\r\n");
    while (buf[i] != NULL)
    {
        i++;
        buf[i] = strtok(NULL, "\r\n");
    }
    return i;
}

map<int ,QString> lrc_analysis(map<int ,QString> m_lrc, char *lrc[], int row)
{
    int i = 0;
    for (i = 3; i >= 0; i--)
    {
        // char *p = lrc[i];
        char *p = (char *)calloc(1,32);
        sscanf(lrc[i],"%*[^:]:%[^]]]",p);
       // printf("%s\n",p);
        m_lrc.insert(make_pair(i,p));
        v_lrc_time.push_back(i);
    }
    for (i = 4; i < row; i++)
    {
        char *p_lrc = lrc[i];
        while (*p_lrc == '[') //[02:04.94][00:36.09]我想大声宣布 对你依依不舍
        {
            p_lrc += 10;
        }
        char *ps = lrc[i];
        while (*ps == '[')
        {
            int m = 0, s = 0;
            sscanf(ps, "[%d:%d", &m, &s);
            int falg_time  = m*60+s;
            v_lrc_time.push_back(falg_time);
            m_lrc.insert(make_pair(falg_time,p_lrc));
            ps += 10;
        }
    }
    return m_lrc;
}

void *deal_write_lrc(void *arg)
{
    Ui::MainWindow* ui = (Ui::MainWindow*)arg;
    sort(v_lrc_time.begin(),v_lrc_time.end());
    //i = 0;
    while(1)
    {
        usleep(300*1000);
        for_each(m_lrc.begin(),m_lrc.end(),[=](pair<int,QString> val){
            if(currentTimeNum == val.first)
            {
                ui->listWidget_2->clear();
                ui->listWidget_2->addItem(val.second);
                ui->listWidget_2->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            }
        });
    }
}

void MainWindow::on_toolButton_4_clicked()
{
    f++;
    // gedan();
    printf("111\n");

    if(f == 1)//顺序播放
    {
        ui->toolButton_4->setIcon(QIcon(":/resource/loop_all.svg"));
    }

    if(f == 2)//随机播放
    {
        ui->toolButton_4->setIcon(QIcon(":/resource/loop_random.svg"));
    }
    if(f == 3)//单曲循环
    {
        f = 0;
        ui->toolButton_4->setIcon(QIcon(":/resource/loop_one.svg"));

    }
}


