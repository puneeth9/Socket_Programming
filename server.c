#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#define SIZE 1024
void send_file(int fd, int sockfd)
{
  int n;
  char data[SIZE] = {0};
  int size;
  int file_size=lseek(fd,0,SEEK_END);
  lseek(fd,0,SEEK_SET);
  char* string_file_size;
  string_file_size=malloc(1024);
  sprintf(string_file_size,"%d",file_size);
  send(sockfd,string_file_size,sizeof(string_file_size),0);
  //printf("file size=%d\n",file_size);
  while(file_size>0) 
  {
    size=read(fd,data,SIZE);
    if(size==0)
    {
      //printf("break\n");
      break;
    }
    //printf("size=%d\n",size);
    //printf("%s",data);
    //printf("Wrote data on stdout\n");
    //printf("size=%d\n",size);
    int write_return=write(sockfd, data, size);
    //printf("return value of write = %d\n",write_return);
    /*if ( == -1) 
    {
      perror("[-]Error in sending file.");
      exit(1);
    }*/
    memset(data,0,SIZE);
    file_size=file_size-size;
  }
}
int main(){
  char *ip = "127.0.0.1";
  int port = 8080;
  int e;

  int sockfd, new_sock;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;
  char buffer[SIZE];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e < 0) {
    perror("[-]Error in bind");
    exit(1);
  }
  printf("[+]Binding successfull.\n");
  if(listen(sockfd,10) == 0)
  {
    printf("[+]Listening....\n");
  }
  else
  {
    perror("[-]Error in listening");
    exit(1);
  }

  addr_size = sizeof(new_addr);
  new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
  while(1)
  {
    char command[1024]={0};
    //printf("Initial Command = %s\n",command);
    recv(new_sock,command,1024,0);
    //printf(" received data = %s\n",command);
    char *file[5000];
    char *token;
    token=strtok(command," ");
    int file_number=0;
    while(token!=NULL)
    {
      file[file_number]=token;
      file_number++;
      token=strtok(NULL," ");
    }
    if(strcmp(file[0],"exit")==0)
    {
      break;
    }
    else if(strcmp(file[0],"get")==0)
    {
      for(int i=1;i<file_number;i++)
      {
        int fd=open(file[i],O_RDONLY,0644);
        send_file(fd,new_sock);
      }
    }
    else
    {
      printf("Invalid command");
    }
    for(int i=0;i<strlen(command);i++)
    {
        command[i] = '\0';
    }
  }
  return 0;
}