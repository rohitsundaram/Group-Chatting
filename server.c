#include <stdio.h>
#include<limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
	
#define PORT 4950
#define BUFSIZE 1024

struct users{
int id;
char name[10];
int status;
} currentusers[10];
static int counter=0;

void printall(){
int k;
for(k=0;k<counter;k++){
printf("\nName %d:%s\nStatus %d:%d\nId %d:%d\n",k,currentusers[k].name,k,currentusers[k].status,k,currentusers[k].id);
}
}

int check_user(char acname[BUFSIZE]){
FILE *fp;
size_t len = 0;
ssize_t read;
char line[BUFSIZE];
fp=fopen("user_data.txt","r");
while (( fread ( line,sizeof(line), 1, fp ))) {
	if(strncmp(line,acname,(read-1))==0){
             fclose(fp);
		return 1;
         }printf("Am in");
     }
       fclose(fp);
	return 0;
}

validate_user(int sockfd)
{
char recv_buf[BUFSIZE],acname[BUFSIZE];
	if ((recv(sockfd, recv_buf, BUFSIZE, 0)) <= 0) {
		printf("error\n");
	}
       //printf("%s\n",recv_buf);
	

	if(check_user(recv_buf)){
	 //He is registered so let him proceed 
	printf("Welcome %s",recv_buf);
	strcpy(currentusers[counter].name,recv_buf);
	if (send(sockfd,"1", sizeof("1"), 0) == -1) {
		printf("error");
	}
	}
	else{
	//Now we know that he is not registered!
	printf("%s You are not registered!",recv_buf);//Just stopping him for time being	
	fflush(stdout);
	strcpy(acname,recv_buf);
	if (send(sockfd,"0", sizeof("0"), 0) == -1) {
		printf("error");
	}
	strncpy(recv_buf,"\0",BUFSIZE);
	if ((recv(sockfd, recv_buf, BUFSIZE, 0)) <= 0) {
		printf("error\n");
	}
	else{
	FILE *fp;
	fp=fopen("user_data.txt","a");
	fwrite(acname,1,sizeof(acname),fp);
	//fwrite("\n",1,sizeof("\n"),fp);
	fclose(fp);
	printf("Registered Successfully!\n");	
	strcpy(currentusers[counter].name,acname);
	}
	}
	strncpy(recv_buf,"\0",BUFSIZE);
	fflush(stdout);	
	//printf("acname=%s\n",acname);	
	currentusers[counter].status=1;
	currentusers[counter].id=sockfd;
	 //He is online
	printall();
	printf("\nName %d:%s\nStatus %d:%d\nId %d:%d\n",counter,currentusers[counter].name,counter,currentusers[counter].status,counter,currentusers[counter].id);
	counter++;
}					
void send_to_all(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master,int cmd)
{
	if (FD_ISSET(j, master)){
		if(cmd==1){
			if(i==j){
                        if (send(j, recv_buf, nbytes_recvd, 0) == -1) {
                                perror("send");
                }
		}
		}
		else{
			if (j != sockfd && j!=i) {
                        if (send(j, recv_buf, nbytes_recvd, 0) == -1) {
                                perror("send");
                        }
		    }
		}	
	}
}	
		
void send_recv(int i, fd_set *master, int sockfd, int fdmax)
{
	int nbytes_recvd, j,k;
	char list[100];
	char recv_buf[BUFSIZE], buf[BUFSIZE];
	char userleft[100];
	if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
		if (nbytes_recvd == 0) {
			printf("socket %d hung up\n", i);
			for(k=0;k<=counter;k++){
				if(i==currentusers[k].id && currentusers[k].status==1){
					currentusers[k].status=0;
					strcpy(userleft,currentusers[k].name);
					strcat(userleft," has just left group chat!");
					printf("\n%s just left group chat!\nStatus:%d\n",currentusers[k].name,currentusers[k].status);
		 for(j = 4; j <= fdmax; j++){
                        send_to_all(j, i, sockfd, sizeof(userleft), userleft, master,0);
                }	
			
				}
			}
		}else {
			perror("recv");
		}
		
		close(i);
		FD_CLR(i, master);
	}else { 
	//	printf("%s\n", recv_buf);
		 //printf("server received %s.",recv_buf);
            if(strncmp(recv_buf,"$cu",(nbytes_recvd-1))==0){
                printf("Check user began!");
                //fflush(stdout);
                printf("Sending list\n");
			for(k=0;k<counter;k++){
                	if(currentusers[k].status==1)
				sprintf(list,"%s",currentusers[k].name);
                for(j =4; j<=fdmax; j++){
                //printf("Sending it to id:%d",currentusers[k].id);
	
			send_to_all(j, i,sockfd, sizeof(list),list, master,1);
		}
		strcpy(list,"\0");
                }
		fflush(stdout);
	
                printf("Completed Sending list\n");
        }
		else
		 for(j = 0; j <= fdmax; j++){
                        send_to_all(j, i, sockfd, nbytes_recvd, recv_buf, master,0);
                }	
}
		}
void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
	socklen_t addrlen;
	int newsockfd;
	
	addrlen = sizeof(struct sockaddr_in);
	if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
		perror("accept");
		exit(1);
	}else {
		FD_SET(newsockfd, master);
		if(newsockfd > *fdmax){
			*fdmax = newsockfd;
		}
		printf("new connection from %s on port %d \n",inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
	}
		
					validate_user(newsockfd);					
}
	
void connect_request(int *sockfd, struct sockaddr_in *my_addr)
{
	int yes = 1;
		
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
		
	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(4950);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(my_addr->sin_zero, '\0', sizeof my_addr->sin_zero);
		
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
		
	if (bind(*sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}
	if (listen(*sockfd, 10) == -1) {
		perror("listen");
		exit(1);
	}
	printf("\nTCPServer Waiting for client on port 4950\n");
	fflush(stdout);
}
int main()
{
	fd_set master;
	fd_set read_fds;
	int fdmax, i;
	int sockfd= 0;
	struct sockaddr_in my_addr, client_addr;
	
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	connect_request(&sockfd, &my_addr);
	FD_SET(sockfd, &master);
	
	fdmax = sockfd;
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
		
		for (i = 0; i <= fdmax; i++){
			if (FD_ISSET(i, &read_fds)){
				if (i == sockfd){
					connection_accept(&master, &fdmax, sockfd, &client_addr);
		
	}
				
				else
					send_recv(i, &master, sockfd, fdmax);
			}
		}
	}
	return 0;
}
	

