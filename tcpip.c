#include<stdio.h>
#include<stdlib.h>
#include<string.h>
 
#define PACKET_TCP_ID_HEAD "B8C1"
#define PACKET_TCP_ID_TAIL "76B7"
 
unsigned char Packet_Filter_TCP(unsigned char Packet[])
{
        unsigned char num=0;
     
        unsigned char *p = Packet;
        char *Head = NULL;
        char *Tail = NULL;
 
        while(1)
        {
            Head = memchr(p,'B',sizeof(p));
            sscanf(Head,"%4s",Head);
     
            if(strcmp(Head,PACKET_TCP_ID_HEAD) == 0)
            {
                printf("�ҵ��˰�ͷ\n");
 
                Tail = memchr(p,'7',sizeof(p));
                sscanf(Tail,"%4s",Tail);
                if(strcmp(Tail,PACKET_TCP_ID_TAIL) == 0)
                {
                    printf("�ҵ��˰�β\n");
                    num++;
                }
                else
                    break;
            }
            else
                break;
        }
         
        Head = memchr(p,'B',sizeof(p));
        sscanf(Head,"%4s",Head);
 
        if(strcmp(Head,PACKET_TCP_ID_HEAD) == 0)
        {
            printf("�ҵ��˰�ͷ\n");
            Tail = memchr(p,'7',sizeof(p));
            sscanf(Tail,"%4s",Tail);
            if(strcmp(Tail,PACKET_TCP_ID_TAIL) == 0)
            {
                printf("�ҵ��˰�β\n");
                num++;
            }
            else
                return 0;
        }
        else
            return 0;
     
    return num;
}
 
int main()
{
    int num;
    unsigned char Packedt[] = "B8C176B71827063516376B7B8C11827063516376B7B8C11827063516376B7";
    num = Packet_Filter_TCP(Packedt);
    printf("������%d�����ݰ�",num);
 
    return 0;
}