
#include "qj.h"
#include "dirent.h"
#include<stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>


#define  FilePath "/home/edu/test"
int save_volume = 0;
char *p[128] = {""};
void kongjian()
{
    int t = 0;
    for(;t<128;t++)
    {
        p[t] = (char *)calloc(1,100);
    }
}
int f = 0;
int num = 4;
bool flag = true;
bool ts = true;
int ind = 0;
void gedan()
{
    int i = 0;
    int j = 0;
    int filesize = 0;
        DIR *dir = NULL;
        struct dirent *entry;
        if((dir = opendir(FilePath))==NULL)
        {
            printf("opendir failed!");
            return;
        }
    else
        {
             while(entry=readdir(dir))
            {
                i++;
                //printf("filename%d = %s\n",i,entry->d_name);  //输出文件或者目录的名称
                if(strstr(entry->d_name,"mp3") != NULL)
                {
                    //printf("***\n");
                    p[j++] = entry->d_name;
                    //printf("%s\n",p[j-1]);
                }
            }
             closedir(dir);
        }
}
//歌词读取
char *lrc_duqu()
{
    //打开歌词文件
    FILE *fp = fopen("简单爱.lrc", "r");
    if (fp == NULL)
    {
        perror("fopen");
        return NULL;
    }

    fseek(fp, 0, 2);
    long len = ftell(fp);
    rewind(fp);
    //开辟堆区空间
    char *file = (char *)calloc(1, len + 1);
    //把歌词读入开辟的堆区空间中
    fread(file, len, 1, fp);
    fclose(fp);
    //printf("%s\n",file);
    return file;
}

LRC *insert_link(LRC *head, LRC tem)
{
    //1.为待插入的节点申请空间
    LRC *pi = (LRC *)calloc(1, sizeof(LRC));
    //2.将tem的数据赋值给*pi
    *pi = tem;
    pi->next = NULL;

    //3.判断链表是否存在
    if (head == NULL) //链表不存在
    {
        head = pi;
    }
    else //链表存在
    {
        //逐个节点寻找插入点的位置
        LRC *pf = head, *pb = head;
        //按照num从小到大排序 和 防止链表查询越界
        while ((pb->time < pi->time) && (pb->next != NULL))
        {
            //pf记录pb移动之前的位置
            pf = pb;
            //pb往下一个节点移动
            pb = pb->next;
        }
        //判断插入点的位置 前 中 尾

        if (pb == head && pb->time >= pi->time) //前
        {
            pi->next = head;
            head = pi;
        }
        else if (pb->next == NULL && pb->time < pi->time) //尾部
        {
            pb->next = pi;
            // pi->next=NULL;
        }
        else
        {
            pf->next = pi;
            pi->next = pb;
        }
    }
    return head;
}
