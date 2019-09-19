#ifndef QJ_H
#define QJ_H
extern void gedan();
extern char *p[128];
extern int num;
extern bool flag;
extern bool ts;
extern int ind;
extern char *lrc_duqu();
extern void kongjian();
extern int f;
extern int save_volume;
typedef struct lrc
{
    int time;
    char lrc[256];

   // struct lrc *pre;
    struct lrc *next;
}LRC;

extern LRC *insert_link(LRC *head, LRC tem);
#endif // QJ_H
