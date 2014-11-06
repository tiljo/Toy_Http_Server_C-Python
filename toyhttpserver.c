#include"networkhdr.h"
#define MYPORT 3490 
#define MAXDATASIZE 100000
#define BACKLOG 10 //pending connections
void sigchld_handler(int s)
{
	while(wait(NULL) > 0);
}

char *make_head(int n,char *s)
{
	char h[100];
	char *p = h;
	char a[]="HTTP/1.1 200 OK\nContent-length: ";
	char b[]="\nContent-Type: ";
	char c[]="\r\n\n";
	char e[20];
	if(!strcmp(s,"jpeg") || !strcmp(s,"png")){
		strcpy(e,"image/");
		strcat(e,s);
	}
	else if(strcmp(s,"txt")){
		strcpy(e,"text/");
		strcat(e,"html");
	}
	sprintf(p,"%s%d%s%s%s",a,n,b,s,c);
	return (p);
}


int main(void)
{

	int sockfd,i,j,n,fd, new_fd,numbytes;
    char buf[MAXDATASIZE];
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr; 
	int sin_size;
	struct sigaction sa;

	char cuf[100];
	char index[20]="/a.txt";
	char *rqst;

	char error[100]="HTTP/1.1 404 OK\nContent-Type: text/html\n\n<html><body><H1>404:Page Not Found</H1></body></html>";
	char reqst[100];
	char head[100];
	chdir("/home/tiljo/www");
	int yes=1;


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYPORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', 8);
	
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))
				== -1) {
			perror("bind");
			exit(1);
		}
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

    while(1){
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,&sin_size)) == -1) {
			perror("accept");
		}
		printf("server: got connection from %s\n",inet_ntoa(their_addr.sin_addr));
		if (!fork()) {
			close(sockfd);

		   if((numbytes = recv(new_fd,buf,MAXDATASIZE-1,0))==-1){
		   perror("recv");
			exit(1);
		   }
	   		if(numbytes==0){
				printf("client closed\n");
	 		
	   		}		
		   buf[numbytes]= '\0';
		   printf("RECVD: %s\n",buf);
		   sscanf(buf,"%s%s",head,reqst);
		   printf("REQSTD FOR:%s\n",reqst);
		   rqst=reqst;

		
			if(!strcmp(rqst,"/"))//no particular reqst from browser
				strcpy(reqst,index);

		    for(i=0;i<strlen(reqst);i++){//removing the '/' from path
			   reqst[i]=reqst[i+1];
		   	}

			struct stat k;
			stat(reqst,&k);
			if(k.st_mode & S_IFDIR){
				strcat(reqst,index);
			}


		
		if(	(fd = open(reqst,O_RDONLY))==-1){
		   perror("open");
		   send(new_fd,error,sizeof(error),0);
		   close(new_fd);
		   exit(1);
		}
		   
		   n=read(fd,buf,sizeof(buf));
			    perror("read");
			char *s;
			if(!strcmp(rqst,"/"))//no particular reqst from browser
				s = "txt";
			else{
				s = strtok(reqst,".");
				s = strtok(NULL,".");
			}
				
				rqst =  make_head(n,s);
				for(i=0;rqst[i]!=0;i++)
					cuf[i]=rqst[i];
				cuf[i]=0;

				char bigarray[strlen(cuf)+n];
				strcpy(bigarray,cuf);

				for(i=strlen(cuf),j=0;j<n;i++,j++){
					bigarray[i]=buf[j];
				}


			if (send(new_fd, bigarray,(strlen(cuf)+n), 0) == -1)
				perror("send");
	
			close(new_fd);
			exit(0);
		}
		close(new_fd); 
	}
	return 0;
}
