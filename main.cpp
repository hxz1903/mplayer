#include "mainwindow.h"
#include <QApplication>
#include "qj.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    flag = true;
    w.show();

    return a.exec();
    /*
    pid_t pid = fork();
    if(pid == 0)//子进程
    {
       //w.show();
       sleep(10);
       w.process->write("pause\n");
       printf("****\n");
    }
    else//父进程
    {
        //ui设计

        //w.process->write("pause\n");
        return a.exec();
    }
    */
}
