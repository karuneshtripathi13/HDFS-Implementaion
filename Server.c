#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define IP_PROTOCOL 0
#define PORT_NO 15050
#define Default_size 1000
#define Key 88
#define sendrecvflag 0
#define nofile "File Not Found!"
#define BLOCK_SIZE 5000

void clearBuf(char* b) {
 int i;
 for (i = 0; i < sizeof(b); i++)
 b[i] = '\0';
}

char Cipher(char ch)
{
return ch^Key;
}


 int s1=0,s2=0,s3=0;
 
 int RecieveFile1(char* buf, int s, FILE* smf,FILE* smfb)
{
 int i,size;
 char ch;
 for (i = 0; i < s; i++) {
 ch = buf[i];
 size=s;
 ch = Cipher(ch);
 if (ch == EOF)
 return 1;
 else
 {
 putc(ch,smf);
 putc(ch,smfb);
 }
 }
 return 0;
}


int RecieveFile(char* buf, int s,char *fl1,char* fl1b,char *fl2,char* fl2b,char *fl3,char* fl3b,char* mas,char* masb)
{
FILE* fp;FILE* fpb;
FILE* ms;FILE* msb;
 int i,size;
 char ch;
 for (i = 0; i < s; i++) {
 ch = buf[i];
 size=s;
 ch = Cipher(ch);
 if (ch == EOF)
 return 1;
 else
 {
 ms=fopen(mas,"a");
msb=fopen(masb,"a");
 if(s1<BLOCK_SIZE)
{
fp=fopen(fl1,"a");
putc(ch,fp);
fpb=fopen(fl1b,"a");
putc(ch,fpb);
if(s1==0)
{
fprintf(ms,"%s\n",fl1);
fprintf(msb,"%s\n",fl1b);
}
 s1++;
 }
 else if(s2<BLOCK_SIZE)
 {
fp=fopen(fl2,"a");
 putc(ch,fp);
fpb=fopen(fl2b,"a");
 putc(ch,fpb);
 if(s2==0)
{
fprintf(ms,"%s\n",fl2);
fprintf(msb,"%s\n",fl2b);
}
 s2++;
 }
 else
 {
fp=fopen(fl3,"a");
 putc(ch,fp);
fpb=fopen(fl3b,"a");
 putc(ch,fpb);
 if(s3==0)
{
fprintf(ms,"%s\n",fl3);
fprintf(msb,"%s\n",fl3b);
s3++;
}
 }
 fclose(fp);
 fclose(fpb);
 fclose(ms);
 fclose(msb);
 }
 }
 return 0;
}



int SendFile(FILE* fp, char* buf, int s)
{
 int i, len;
 if (fp == NULL) {
 strcpy(buf, nofile);
 len = strlen(nofile);
 buf[len] = EOF;
 for (i = 0; i <= len; i++)
 buf[i] = Cipher(buf[i]);
 return 1;
}
 char ch, ch2;
 for (i = 0; i < s; i++) {
 ch = fgetc(fp);
 ch2 = Cipher(ch);
 buf[i] = ch2;
 if (ch == EOF)
 return 1;
 }
return 0;
}
int main() {
 int sockfdescriptor, nBytes,ch[1];
 long int fs[1];
 struct sockaddr_in addr_con;
 int addrlen = sizeof(addr_con);
 addr_con.sin_family = AF_INET;
 addr_con.sin_port = htons(PORT_NO);
 addr_con.sin_addr.s_addr = INADDR_ANY;
 char net_buf[Default_size];
 FILE* fp;FILE* smf;FILE* smfb;
 sockfdescriptor = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
 if (sockfdescriptor < 0)
 printf("\nfile descriptor not received!!\n");
 else
 printf("\nfile descriptor %d received\n", sockfdescriptor);
 if (bind(sockfdescriptor, (struct sockaddr*)&addr_con, sizeof(addr_con)) == 0)
 printf("\nSuccessfully binded!\n");
 else
 printf("\nBinding Failed!\n");
 while (1) {
 printf("\n----------------START---------------\n");
 printf("\nWaiting for choice...");
recvfrom(sockfdescriptor, ch, sizeof(ch), 0, (struct sockaddr*)&addr_con, &addrlen);
 printf("\nChoice is: %d",ch[0]);
 
 
 if(ch[0]==1)
 {
 printf("\nWaiting for file name...\n");
 clearBuf(net_buf);
 nBytes = recvfrom(sockfdescriptor, net_buf,Default_size, sendrecvflag,(struct sockaddr*)&addr_con, &addrlen);
 
 char masp[]="MASTER/";
int as=strlen(net_buf);
int bs=strlen(masp);
int s=as+bs;
char* mas=(char*)malloc(s*sizeof(char));
strcpy(mas,strcat(masp,net_buf));
 fp = fopen(mas, "r");
 printf("\nFile Name Received: %s\n", net_buf);
 if (fp == NULL)
 {
   char path[3][200];
 clearBuf(path[0]); clearBuf(path[1]); clearBuf(path[2]);
 printf("\nFile open failed!\n");
 sendto(sockfdescriptor, path, sizeof(path),sendrecvflag,(struct sockaddr*)&addr_con, addrlen);
 }
 else
 {
  printf("\nFile Successfully opened!\n");
  char path[3][200];
 clearBuf(path[0]); clearBuf(path[1]); clearBuf(path[2]);
fscanf(fp, "%s\n%s\n%s", path[0],path[1],path[2]);

 sendto(sockfdescriptor, path, sizeof(path),sendrecvflag,(struct sockaddr*)&addr_con, addrlen);

  if(strlen(path[0])!=0)
 {
 printf("%s",path[0]);
 FILE* fps;
  if (fp == NULL)
   printf("\nFile open failed!\n");
  else
   {
   printf("\nFile opened Successfully opened by Master!\n");
   fps = fopen(path[0], "r");
   }
 while (1) {
 if (SendFile(fps, net_buf, Default_size)) {
 sendto(sockfdescriptor, net_buf, Default_size,sendrecvflag,(struct sockaddr*)&addr_con, addrlen);
 break;
 }
 sendto(sockfdescriptor, net_buf, Default_size,sendrecvflag,(struct sockaddr*)&addr_con, addrlen);
 clearBuf(net_buf);
 }
 clearBuf(path[0]);
 if(fps!=NULL)
 fclose(fps);
 }
 
  if(strlen(path[1])!=0)
 {
 printf("%s",path[1]);
 FILE* fps;
  if (fp == NULL)
   printf("\nFile open failed!\n");
  else
   {
   printf("\nFile opened Successfully opened by Master!\n");
   fps = fopen(path[1], "r");
   }
 while (1) {
 if (SendFile(fps, net_buf, Default_size)) {
 sendto(sockfdescriptor, net_buf, Default_size,sendrecvflag,(struct sockaddr*)&addr_con, addrlen);
 break;
 }
 sendto(sockfdescriptor, net_buf, Default_size,sendrecvflag,(struct sockaddr*)&addr_con, addrlen);
 clearBuf(net_buf);
 }
 clearBuf(path[1]);
 if(fps!=NULL)
 fclose(fps);
 }
 
  if(strlen(path[2])!=0)
 {
 printf("%s",path[2]);
 FILE* fps;
  if (fp == NULL)
   printf("\nFile open failed!\n");
  else
   {
   printf("\nFile opened Successfully opened by Master!\n");
   fps = fopen(path[2], "r");
   }
 while (1) {
 if (SendFile(fps, net_buf, Default_size)) {
 sendto(sockfdescriptor, net_buf, Default_size,sendrecvflag,(struct sockaddr*)&addr_con, addrlen);
 break;
 }
 sendto(sockfdescriptor, net_buf, Default_size,sendrecvflag,(struct sockaddr*)&addr_con, addrlen);
 clearBuf(net_buf);
 }
 clearBuf(path[2]);
 if(fps!=NULL)
 fclose(fps);
 }
 
 if (fp != NULL)
 fclose(fp);
 }
 }
 
 
 
 if(ch[0]==2)
 {
 int flag[1];
 printf("\nWaiting for file name...\n");
 clearBuf(net_buf);
 nBytes = recvfrom(sockfdescriptor, net_buf,Default_size, sendrecvflag,(struct sockaddr*)&addr_con, &addrlen);
 nBytes = recvfrom(sockfdescriptor, flag,sizeof(flag), sendrecvflag,(struct sockaddr*)&addr_con, &addrlen);
 if(flag[0]==1)
 {
 printf("\nFile not Found!\n");
 continue;
 }
 else
 printf("\nFile successfully Found!\n");
 nBytes=recvfrom(sockfdescriptor, fs, sizeof(fs), 0, (struct sockaddr*)&addr_con, &addrlen);
 printf("File size %ld bytes\n",fs[0]);
char fn[255];
char temp[Default_size];
strcpy(fn,net_buf);
printf("File name is: %s\n",fn);

char ch;char* t;
 printf("\n---------Data Received---------\n\nUploading file....\n");
 if(fs[0]>BLOCK_SIZE)
 {
char fl1p[]="SERVER1/";
int as=strlen(fn);
int bs=strlen(fl1p);
int s=as+bs;
char* fl1=(char*)malloc(s*sizeof(char));
strcpy(fl1,strcat(fl1p,fn));

char fl1bp[]="SERVER1_BUP/";
bs=strlen(fl1bp);
s=as+bs;
char* fl1b=(char*)malloc(s*sizeof(char));
strcpy(fl1b,strcat(fl1bp,fn));

char fl2p[]="SERVER2/";
bs=strlen(fl2p);
s=as+bs;
char* fl2=(char*)malloc(s*sizeof(char));
strcpy(fl2,strcat(fl2p,fn));

char fl2bp[]="SERVER2_BUP/";
bs=strlen(fl2bp);
s=as+bs;
char* fl2b=(char*)malloc(s*sizeof(char));
strcpy(fl2b,strcat(fl2bp,fn));

char fl3p[]="SERVER3/";
bs=strlen(fl3p);
s=as+bs;
char* fl3=(char*)malloc(s*sizeof(char));
strcpy(fl3,strcat(fl3p,fn));

char fl3bp[]="SERVER3_BUP/";
bs=strlen(fl3bp);
s=as+bs;
char* fl3b=(char*)malloc(s*sizeof(char));
strcpy(fl3b,strcat(fl3bp,fn));

char masp[]="MASTER/";
bs=strlen(masp);
s=as+bs;
char* mas=(char*)malloc(s*sizeof(char));
strcpy(mas,strcat(masp,fn));

char masbp[]="MASTER_BUP/";
bs=strlen(masbp);
s=as+bs;
char* masb=(char*)malloc(s*sizeof(char));
strcpy(masb,strcat(masbp,fn));

 printf("\nLarge File\n");
 while (1) {
 clearBuf(net_buf);
 nBytes = recvfrom(sockfdescriptor, net_buf, Default_size,sendrecvflag,(struct sockaddr*)&addr_con,&addrlen);
 if (RecieveFile(net_buf, Default_size,fl1,fl1b,fl2,fl2b,fl3,fl3b,mas,masb)) {
 break;
 }
 }
 }
 else
 {
 printf("\nSmall File\n");
 char sf1[]="SERVER1/SMALLFILE/";
 char* sf=(char*)malloc((strlen(sf1)+strlen(fn))*sizeof(char));
strcpy(sf,strcat(sf1,fn));

char sf1b[]="SERVER1_BUP/SMALLFILE_BUP/";
char* sfb=(char*)malloc((strlen(sf1b)+strlen(fn))*sizeof(char));
strcpy(sfb,strcat(sf1b,fn));

char p[]="MASTER/";
char* mas=(char*)malloc((strlen(p)+strlen(net_buf))*sizeof(char));
printf("%s",net_buf);
strcpy(mas,strcat(p,net_buf));

char pb[]="MASTER_BUP/";
char* masb=(char*)malloc((strlen(pb)+strlen(net_buf))*sizeof(char));
strcpy(masb,strcat(pb,net_buf));

FILE* ms;
FILE* msb;
ms=fopen(mas,"w");
fprintf(ms,"%s\n",sf);
msb=fopen(masb,"w");
fprintf(msb,"%s\n",sfb);
fclose(ms);
fclose(msb);
smf=fopen(sf,"w");
smfb=fopen(sfb,"w");
 while (1) {
 clearBuf(net_buf);
 nBytes = recvfrom(sockfdescriptor, net_buf, Default_size,sendrecvflag,(struct sockaddr*)&addr_con,&addrlen);
 if (RecieveFile1(net_buf, Default_size, smf,smfb)) {
 break;
 }
 }
 if(smf!=NULL)
 fclose(smf);
 if(smfb!=NULL)
 fclose(smfb);
 }
  printf("\nFile Successfully Uploaded.\n\n---------Data Received---------\n");
  s1=0;
  s2=0;
  s3=0;
 }
 
 
 
 if(ch[0]==3)
 {
 int flag[1];
 printf("\nWaiting for file name to delete...\n");
 clearBuf(net_buf);
 nBytes = recvfrom(sockfdescriptor, net_buf,Default_size, sendrecvflag,(struct sockaddr*)&addr_con, &addrlen);
 char p[]="MASTER/";
 char* mas=(char*)malloc((strlen(p)+strlen(net_buf))*sizeof(char));
 strcpy(mas,strcat(p,net_buf));
 char pb[]="MASTER_BUP/";
 char* masb=(char*)malloc((strlen(pb)+strlen(net_buf))*sizeof(char));
 strcpy(masb,strcat(pb,net_buf));
 fp = fopen(mas, "r");
 if (fp == NULL)
   {
   printf("\nFile Not Found!\n");
   flag[0]=0;
   }
  else
   {
   FILE* fpb;
   fpb = fopen(masb, "r");
   printf("\nMaster opened Successfully!\n");
   flag[0]=1;
   char path[3][200];
   clearBuf(path[0]); clearBuf(path[1]); clearBuf(path[2]);
   fscanf(fp, "%s\n%s\n%s", path[0],path[1],path[2]);
   if (remove(path[0]) == 0) 
      printf("\nDeleted successfully from %s",path[0]); 
   else
      printf("\nUnable to delete the file %s",path[0]);
   if (remove(path[1]) == 0) 
      printf("\nDeleted successfully from %s",path[1]); 
   else
      printf("\nUnable to delete the file %s",path[1]);
   if (remove(path[2]) == 0) 
      printf("\nDeleted successfully from %s",path[2]); 
   else
      printf("\nUnable to delete the file %s",path[2]);
      
   clearBuf(path[0]); clearBuf(path[1]); clearBuf(path[2]);
   fscanf(fpb, "%s\n%s\n%s", path[0],path[1],path[2]);
   if (remove(path[0]) == 0) 
      printf("\nDeleted successfully from %s",path[0]); 
   else
      printf("\nUnable to delete the file %s",path[0]);
   if (remove(path[1]) == 0) 
      printf("\nDeleted successfully from %s",path[1]); 
   else
      printf("\nUnable to delete the file %s",path[1]);
   if (remove(path[2]) == 0) 
      printf("\nDeleted successfully from %s",path[2]); 
   else
      printf("\nUnable to delete the file %s",path[2]);
   fclose(fpb);
   }
   if(fp!=NULL)
   fclose(fp);
 if(flag[0]==1)
 {
 if (remove(mas) == 0) 
      {
      printf("\nDeleted successfully from %s",mas); 
      flag[0]=1;
      }
 else
    {
    printf("\nUnable to delete the file %s",mas);
    flag[0]=0;
    }
 if (remove(masb) == 0) 
      {
      printf("\nDeleted successfully from %s",masb); 
      flag[0]=1;
      }
 else
    {
    printf("\nUnable to delete the file %s",masb);
    flag[0]=0;
    }
  }
 sendto(sockfdescriptor, flag, sizeof(flag),sendrecvflag,(struct sockaddr*)&addr_con, addrlen);
 }
 printf("\n---------------END----------------\n");
 }
 return 0;
}