#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define IP_PROTOCOL 0
#define IP_ADDRESS "127.0.0.1"
#define PORT_NO 15050
#define Default_Size 1000
#define sendrecvflag 0
#define key 88
#define nofile "File Not Found!"
void clearBuf(char* b) {
 int i;
 for (i = 0; i < Default_Size; i++)
 b[i] = '\0';
}
char Cipher(char ch)
{
return ch^key;
}

int SendFile(FILE* fp, char* buf, int s) {
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



int RecieveFile(char* buf, int s)
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
 printf("%c", ch);
 }
 return 0;
}
int main() {
 int sockfdescriptor, nBytes,ch[1];
 struct sockaddr_in addr_con;
 int addrlen = sizeof(addr_con);
 addr_con.sin_family = AF_INET;
 addr_con.sin_port = htons(PORT_NO);
 addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);
 char net_buf[Default_Size];
 FILE* fp;
 sockfdescriptor = socket(AF_INET, SOCK_DGRAM,IP_PROTOCOL);
 if (sockfdescriptor < 0)
 printf("\nfile descriptor not received!!\n");
 else
 printf("\nfile descriptor %d received\n", sockfdescriptor);
 while (1) {
 printf("\n Enter:\n 1. To Open File\n 2. To Upload new file\n 3. To Delete File\n 4. To Disconnect\n Choice: ");
 scanf("%d",&ch[0]);
sendto(sockfdescriptor, ch, sizeof(ch), 0, (struct sockaddr*)&addr_con, addrlen);

char path[3][200];
clearBuf(path[0]);
clearBuf(path[1]);
clearBuf(path[2]);
 if(ch[0]==1)
 {
 printf("\nEnter name of the file to be opened: ");
 scanf("%s", net_buf);
 sendto(sockfdescriptor, net_buf, Default_Size,sendrecvflag, (struct sockaddr*)&addr_con,addrlen);
  nBytes = recvfrom(sockfdescriptor, path, sizeof(path),sendrecvflag,(struct sockaddr*)&addr_con,&addrlen);
 printf("\n---------Receiving Data---------\n\n");
  if(strlen(path[0])!=0)
  {
 while (1) {
 clearBuf(net_buf);
 nBytes = recvfrom(sockfdescriptor, net_buf, Default_Size,sendrecvflag,(struct sockaddr*)&addr_con,&addrlen);
 if (RecieveFile(net_buf, Default_Size)) {
 break;
 }
 }
 }
   if(strlen(path[1])!=0)
  {
 while (1) {
 clearBuf(net_buf);
 nBytes = recvfrom(sockfdescriptor, net_buf, Default_Size,sendrecvflag,(struct sockaddr*)&addr_con,&addrlen);
 if (RecieveFile(net_buf, Default_Size)) {
 break;
 }
 }
 }
  if(strlen(path[2])!=0)
  {
 while (1) {
 clearBuf(net_buf);
 nBytes = recvfrom(sockfdescriptor, net_buf, Default_Size,sendrecvflag,(struct sockaddr*)&addr_con,&addrlen);
 if (RecieveFile(net_buf, Default_Size)) {
 break;
 }
 }
 }
  if((strlen(path[0])==0)&&(strlen(path[1])==0)&&(strlen(path[2])==0))
  printf("File not found!\n");
 }
 
 
 else if(ch[0]==2)
 {
 int flag[1];
 printf("\nEnter name of the file to be uploaded: ");
 scanf("%s", net_buf);
 sendto(sockfdescriptor, net_buf, Default_Size,sendrecvflag, (struct sockaddr*)&addr_con,addrlen);
 fp = fopen(net_buf, "r");
 printf("\nFile Name Received: %s\n", net_buf);
 if (fp == NULL)
 {
 flag[0]=1;
   printf("\n-------------------------------\n");
 printf("\nFile not found!\n");
   printf("\n-------------------------------\n");
    sendto(sockfdescriptor, flag, sizeof(flag),sendrecvflag, (struct sockaddr*)&addr_con,addrlen);
   continue;
 }
 else
 {
 flag[0]=0;
 sendto(sockfdescriptor, flag, sizeof(flag),sendrecvflag, (struct sockaddr*)&addr_con,addrlen);
 printf("\nFile Successfully opened!\n");
 }
 fseek(fp, 0L, SEEK_END); 
 long int fs[1];
 fs[0]=ftell(fp);
 fseek(fp, 0, SEEK_SET);
 sendto(sockfdescriptor, fs, sizeof(fs), 0, (struct sockaddr*)&addr_con, addrlen);
 printf("\n---------Data Sent---------\n\nUploading file....\n");
 while (1) {
 if (SendFile(fp, net_buf, Default_Size)) {
 sendto(sockfdescriptor, net_buf, Default_Size,sendrecvflag,(struct sockaddr*)&addr_con, addrlen);
 break;
 }
 sendto(sockfdescriptor, net_buf, Default_Size,sendrecvflag,(struct sockaddr*)&addr_con, addrlen);
 clearBuf(net_buf);
 }
 if (fp != NULL)
 fclose(fp);
 printf("\nFile Successfully Uploaded.\n");
 }
 
 else if(ch[0]==3)
 {
 int flag[1];
 printf("\nEnter name of the file to be Deleted: ");
 scanf("%s", net_buf);
 sendto(sockfdescriptor, net_buf, Default_Size,sendrecvflag, (struct sockaddr*)&addr_con,addrlen);
 nBytes = recvfrom(sockfdescriptor, flag, sizeof(flag),sendrecvflag,(struct sockaddr*)&addr_con,&addrlen);
 if(flag[0]==1)
 {
 printf("\n-------------------------------\n");
 printf("\nFile Successfully Deleted!\n");
 }
 if(flag[0]==0)
 {
 printf("\n-------------------------------\n");
 printf("\nFile not found!\n");
 }
 }
 
 else if(ch[0]==4)
 exit(0);
 
 else
 printf("\nEnter correct choice.\n");
 printf("\n-------------------------------\n");
 }
 return 0;
 }