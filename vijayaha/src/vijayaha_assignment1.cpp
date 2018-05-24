/**
 * @vijayaha_assignment1
 * @author  Vijaya harshavardhan palla <vijayaha@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include "../include/global.h"
#include "../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include<bits/stdc++.h>
#define CLIENTS 10
#define MAXDATASIZE 1024
#define STDIN 0
using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
char *port;
char *ipAddress;
int commonCommands(char *command,int type);
int sendall(int s, char *buf, int *len);
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
     int saved_errno = errno;
     while(waitpid(-1, NULL, WNOHANG) > 0);
     errno = saved_errno;
}
    //             // get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
      {
       if (sa->sa_family == AF_INET) {
          return &(((struct sockaddr_in*)sa)->sin_addr);
          }
       return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
        struct clients {
        int id;
        char ip[MAXDATASIZE];
        char status[256];
        char hostName[256];
        int port;
        int mRec;
        int mSent;
        string blckIpLst;
	vector <string> buffer;
        clients():mRec(0),mSent(0){}
        };
        clients client[CLIENTS],tempS;
void sortStructure(int nClients){
       for(int sI=0;sI<nClients;sI++){
           for(int jI=sI+1;jI<nClients;jI++){
                 if(client[sI].port>client[jI].port){
                     tempS=client[sI];
                     client[sI]=client[jI];
                     client[jI]=tempS;
                     }
                 }
            }	
}	
int returnStrucObjIndex(int nClients,char ipAdd[]){
	for(int index=0;index,nClients;index++){
	  if(strcmp(client[index].ip,ipAdd)==0)
		return index;
	}
	return -1;
}
int serverInit(char *port){
/*	struct clients {
        int id;
        char ip[MAXDATASIZE];
        char status[256];
	char hostName[256];
        int port;
	int mRec;
	int mSent;
	string blckIpLst;
	clients():mRec(0),mSent(0){}
        };
	clients client[CLIENTS],tempS;*/
	int nClients=0;
        int portNo;
	fd_set master; 
	fd_set read_fds; 
	int fdmax;
	int listener;
	int newfd;
	struct sockaddr_storage remoteaddr;
	struct sockaddr_in addr;

	socklen_t addrlen;
	int nbytes;
	char remoteIP[INET6_ADDRSTRLEN];
        char buf[MAXDATASIZE];// = (char *) malloc(sizeof(MAXDATASIZE));
        char *ipAddr;
 	char msg[MAXDATASIZE]; 
	char hostN[1024];
	char service[20];
	int yes=1;  
	int i, j, rv;
	struct addrinfo hints, *ai, *p;
        struct timeval tv;
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
        vector <string> args;
        char *charTokens;
	std::vector<char> buffer(MAXDATASIZE);
	std::string message;
	string cmdInput;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
    	fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
	    exit(1);
	}
	for(p = ai; p != NULL; p = p->ai_next) {
	    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	    if (listener < 0) {
	    continue;
	     }

	    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
	    	close(listener);
		continue; 
		}
		break;
	 }

	if (p == NULL) {
	    fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
		 }
	    freeaddrinfo(ai); 
	if (listen(listener, CLIENTS) == -1) {
	    perror("listen");
	    exit(3);
	 }
        FD_SET(STDIN, &master);

	FD_SET(listener, &master);
	fdmax = listener; 
	for(;;) {
	    read_fds = master; 
           // FD_SET(STDIN, &read_fds);
	    if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
	        perror("select");
		exit(4);
		 }
	    for(i = 0; i <= fdmax; i++) {
		if (FD_ISSET(i, &read_fds)) {
		    nbytes = 0;
				
	            if (i == listener) {
	                addrlen = sizeof remoteaddr;
	                newfd = accept(listener,
	                    (struct sockaddr *)&remoteaddr,
	                    &addrlen);
	                if (newfd == -1) {
	                    perror("accept");
	                } else {
	                    FD_SET(newfd, &master);
	                    if (newfd > fdmax) {    
                            fdmax = newfd;
                            }
			addrlen = sizeof(addr);
                        int res = getpeername(newfd, (struct sockaddr *)&addr, &addrlen);
			getnameinfo((struct sockaddr *)&addr, sizeof addr, hostN, sizeof hostN, NULL, NULL, 0);
                        ipAddr = inet_ntoa(addr.sin_addr);
                        printf("selectserver: new connection from %s from %d porton "
                        "socket %d \n",ipAddr,client[nClients].port,newfd);
                             }

			int clientIndex = -1;
			for(int index=0;index<nClients;index++){
				        if(strcmp(client[index].ip,ipAddr)==0){
                                                cout << "Client already present\n";
                                                clientIndex=index;
                                                break;
                                        }		
			}
			if(clientIndex !=-1){
			  client[clientIndex].id=newfd;
			  string sMsg;
			  for(int index=0;index<client[clientIndex].buffer.size();index++){
				sMsg=client[clientIndex].buffer[index];
		                char temp[MAXDATASIZE];
                                strcpy(temp,sMsg.c_str());
                                charTokens=strtok(temp, " ");
                                 args.clear();
                                 while(charTokens)
                                 {
                                 args.push_back(charTokens);
                                 charTokens = strtok(NULL, " ");
                                 }
                                string toMsg=sMsg;
                                toMsg = toMsg.substr(toMsg.find(args[0])+args[0].length()+1);
                                        if (send(newfd, sMsg.c_str(), strlen(sMsg.c_str()), 0) == -1) {
                                                perror("send");
                                            }
                                                cse4589_print_and_log("[RELAYED:SUCCESS]\n");
                                                cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", args[0].c_str(),ipAddr, toMsg.c_str());
                                                cse4589_print_and_log("[RELAYED:END]\n");
				}
			strcpy(client[clientIndex].status,"logged-in");
			client[clientIndex].buffer.clear();
			}
			else{
			client[nClients].id = newfd;
			strcpy(client[nClients].ip,ipAddr);
			strcpy(client[nClients].hostName,hostN);
			cout << "Connected hostname : "<<hostN<<"\n";
			client[nClients].port=ntohs(addr.sin_port);
			
			strcpy(client[nClients].status,"logged-in");
			nClients++;
			}
           		 }
		    else if(i==0){
			fgets(buf,sizeof(buf),stdin);
			charTokens=strtok(buf, " ");
			buf[strcspn(buf, "\n")] = 0;
                        args.clear();
                        while(charTokens)
                         {
                               args.push_back(charTokens);
                               charTokens = strtok(NULL, " ");
                         }
			cout << "User Input : "<<buf<<"\n" << "Arguments size: "<<args.size()<<"\n";
                         
                        if(args.size()==1){
                          char *temp = new char[args[0].length() + 1];
                          strcpy(temp, args[0].c_str());
			 if(strcmp(args[0].c_str(),"STATISTICS")==0){
                               if(nClients==0)
                                 continue;
				sortStructure(nClients);
			        cse4589_print_and_log("[STATISTICS:SUCCESS]\n");
                                for(int lsI=0;lsI<nClients;lsI++){
				cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", client[lsI].id,client[lsI].hostName,client[lsI].mSent,client[lsI].mRec,client[lsI].status);
                                }
                                cse4589_print_and_log("[STATISTICS:END]\n");
				}
		         else if(strcmp(args[0].c_str(),"LIST")==0){
				if(nClients==0)
				 continue;
				sortStructure(nClients);
				cse4589_print_and_log("[LIST:SUCCESS]\n");			
				for(int lsI=0;lsI<nClients;lsI++){
				cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", client[lsI].id,client[lsI].hostName,client[lsI].ip,client[lsI].port);
				}	 	
				cse4589_print_and_log("[LIST:END]\n");
				}
			 else		
                          commonCommands(temp,0);
                          delete [] temp;
                        }
			else if(strcmp(args[0].c_str(),"BLOCKED")==0 &&  args.size()==2){
  	 		        sortStructure(nClients);
                                int clientIndex=-1;// = returnStrucObjIndex(nClients,temp);	
				for(int index=0;index<nClients;index++){
					if(args[1].find(client[index].ip)!= string::npos ){
						cout << "Inside if\n";
						clientIndex=index;
						break;
					}
				}
			     if(/*inet_pton(AF_INET, args[1].c_str(), &(addr.sin_addr))<=0 || */clientIndex == -1){
                          	 cse4589_print_and_log("[BLOCKED:ERROR]\n");
	                         cse4589_print_and_log("[BLOCKED:END]\n");
       		                 continue;
                	        }
			        cse4589_print_and_log("[BLOCKED:SUCCESS]\n");
                                for(int lsI=0;lsI<nClients;lsI++){
				if(client[clientIndex].blckIpLst.find(client[lsI].ip) != string::npos)
                                cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", client[lsI].id,client[lsI].hostName,client[lsI].ip,client[lsI].port);
                                }
                                cse4589_print_and_log("[BLOCKED:END]\n");	
				}
                         args.clear();
			}
             	    else {
	               	if ((nbytes = recv(i, buf, 1024, 0)) <= 0) {
                	 if (nbytes == 0) {

                            printf("selectserver: socket %d hung up\n", i);
                         }
			 else {
				printf("Buf : %s",buf);
                            perror("recv");
                         }
                        close(i); // bye!
                        FD_CLR(i, &master); 
                    	}
		        else {
 			 int res = getpeername(i, (struct sockaddr *)&addr, &addrlen);
                         ipAddr = inet_ntoa(addr.sin_addr);
			 string sMsg = ipAddr;
			 buf[nbytes]='\0';
			 char temp[MAXDATASIZE];
			 strcpy(temp,buf);
			 charTokens=strtok(temp, " ");
 			 cout << "Message from client :  "<<buf<<"\n";	
			 args.clear();
			 while(charTokens)
			 {
			       args.push_back(charTokens);
			       charTokens = strtok(NULL, " ");
   			 }
			if(strcmp(args[0].c_str(),"REFRESH")==0){
                                printf("Inside Refresh\n");
                                cout<<"Refreshing the list of logged in clients\n";
                         }
			else if(strcmp(args[0].c_str(),"LOGOUT")==0){
				cout << "Logging out :"<<ipAddr<<"\n";
			 for(int index=0;index<nClients;index++){
				if(strcmp(client[index].ip,ipAddr)==0){
					strcpy(client[index].status,"logged-out");
					cout<<"Logged out "<<"\n";
					break;
					}
				
			}
			}	
			else if(strcmp(args[0].c_str(),"PORT")==0){
				portNo=strtol(args[1].c_str(),NULL,10);
				client[nClients-1].port=portNo;
					
			}
			else if(strcmp(args[0].c_str(),"BLOCK")==0){
				for(int index=0;index<nClients;index++){
                                  if(strcmp(client[index].ip,ipAddr)==0){
					client[index].blckIpLst+=" "+args[1];
					cout << "Added "<<args[1]<<" to "<<ipAddr<<" block list"<<"\n";
				  }
				}
			}
                        else if(strcmp(args[0].c_str(),"UNBLOCK")==0){
                                for(int index=0;index<nClients;index++){
                                  if(strcmp(client[index].ip,ipAddr)==0){
					client[index].blckIpLst = client[index].blckIpLst.substr(client[index].blckIpLst.find(args[1])+args[1].length());
//                                        client[index].blckIpLst+=" "+args[1];
  //                                      cout << "Added "<<args[1]<<" to "<<ipAddr<<" block list"<<"\n";
                                  }
                                }
                        }

                        else if(strcmp(args[0].c_str(),"SEND")==0){
                                cout << "To IP Address :"<< args[1] << "\n";
				string toMsg=buf;
				toMsg = toMsg.substr(toMsg.find(args[1])+args[1].length());
				cout << "To Message : "<<toMsg<<"\n";
				sMsg+=toMsg;			
				for(int index=0;index<nClients;index++){
					if(strcmp(client[index].ip,ipAddr)==0)
						client[index].mSent++;
					if(strcmp(client[index].ip,args[1].c_str())==0){
						cout << "To Client found : "<< client[index].blckIpLst<<"\n";
						if((client[index].blckIpLst).find(ipAddr) != string::npos)
						 continue;
						cout << "To port : "<<client[index].port<<"\n";						 
						if(strcmp(client[index].status,"logged-out")==0){
							(client[index].buffer).push_back(sMsg);
							continue;
						}
	                                    if (send(client[index].id, sMsg.c_str(), strlen(sMsg.c_str()), 0) == -1) {
		                                        perror("send");
                                                                           }	
						client[index].mRec++;		
						//break;
						toMsg = toMsg.substr(1);
						cse4589_print_and_log("[RELAYED:SUCCESS]\n");
						cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", ipAddr,args[1].c_str(), toMsg.c_str());
                		                cse4589_print_and_log("[RELAYED:END]\n");	
					}			
				}
                           }
		        else if(strcmp(args[0].c_str(),"BROADCAST")==0){
                                string toMsg=buf;
                                toMsg = toMsg.substr(9);
                                sMsg+=toMsg;
				cout << "Message to broadcast :"<< sMsg<<"\n";
				int toIndex=0;
		           for(j = 1 ; j <= fdmax; j++) {
                          
                            if (FD_ISSET(j, &master)) {
                             
                                if (j != listener && j!=i) {
				  for(int index=0;index<nClients;index++){
			  	      if(client[index].id==j && j!=i ){  							    		
						if((client[index].blckIpLst).find(ipAddr) != string::npos)
                                                 continue;
						if(strcmp(client[index].status,"logged-out")==0){
							cout << "Logging into buffer"<<"\n";
                                                        (client[index].buffer).push_back(sMsg);
                                                        continue;
                                                }	
	                                        if(send(j, sMsg.c_str(), strlen(sMsg.c_str()), 0) == -1) {
        	                                    perror("send");
                                                    }
						 client[index].mRec++;
						 break;
						 }
		
					}
                                      }
                                                        }
                                     }
                                        toMsg = toMsg.substr(1);
                                        cse4589_print_and_log("[RELAYED:SUCCESS]\n");
                                        cse4589_print_and_log("msg from:%s, to:255.255.255.255\n[msg]:%s\n", ipAddr, toMsg.c_str());
                                        cse4589_print_and_log("[RELAYED:END]\n");
			  for(j=0;j<nClients;j++ ){
				 if(client[j].id==i){
					client[j].mSent++;
					break;
					}
					
				}
		        	//delete [] bMsg;	
		   	  }
	                 }
			}							
			args.clear();
			
                      }
                  }  
            } 
        
 

return 0;
}
//
int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;
    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
}
    *len = total; // return number actually sent here
//	cout <<"Total bytes sent : "<<total <<"\n";
    return n==-1?-1:0; // return -1 on failure, 0 on success
}
//
int clientLogin(char *servPort,char *serverAddr){
    int sockfd, numbytes;
    fd_set master;
    fd_set read_fds;
    fd_set write_fds;
    FD_ZERO(&master);	
    FD_ZERO(&read_fds);
    int fdmax,nbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in sa;
    int rv;
    char s[INET6_ADDRSTRLEN];
    char toIP[INET6_ADDRSTRLEN];	
    string cmdInput;
    vector <string> args;
    char *charTokens;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(serverAddr, servPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
	}
   for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
    }
  	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
	continue;
	 }
	break; 
	}
	if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return -1;
	}
        cse4589_print_and_log("[LOGIN:SUCCESS]\n");
        cse4589_print_and_log("[LOGIN:END]\n");
	char *portMsg=(char *)malloc(strlen("PORT "));
	strcpy(portMsg,"PORT ");
	strcat(portMsg,port);
	send(sockfd,portMsg,strlen(portMsg),0); 	
       inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
        printf("\nconnected to %s\n", s);
        freeaddrinfo(servinfo); // all done with this structure
        FD_SET(STDIN, &master);
        FD_SET(sockfd, &master);
	//FD_SET(sockfd,&write_fds);
	fdmax=sockfd;
       while(1){
	 read_fds=master;
         if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
               perror("select");
               exit(4);
            }	
         if(FD_ISSET(STDIN, &read_fds)){
                fgets(buf,sizeof(buf),stdin);
                buf[strcspn(buf, "\n")] = 0;
		char temp[MAXDATASIZE];
		strcpy(temp,buf);
                charTokens=strtok(temp, " ");
                args.clear();
                while(charTokens)
                  {
                    args.push_back(charTokens);
                    charTokens = strtok(NULL, " ");
                  }
                if(args.size()==1){
                char *temp = new char[args[0].length() + 1];
                strcpy(temp, args[0].c_str());
		if(strcmp(args[0].c_str(),"LOGOUT")==0){
		  send(sockfd,"LOGOUT",6,0);
		  return 0;
		}
		else	
                commonCommands(temp,0);
                delete [] temp;
                        }
	        else{
                  if(strcmp(args[0].c_str(),"SEND")==0 && args.size()>=3){
               		if(inet_pton(AF_INET, args[1].c_str(), &(sa.sin_addr))<=0){
	                cse4589_print_and_log("[SEND:ERROR]\n");
        	        cse4589_print_and_log("[SEND:END]\n");
                	continue;
               		}
		       int len = strlen(buf);
		//	send(sockfd,buf,MAXDATASIZE,0);
		        if (sendall(sockfd,buf,&len) == -1) {
	                 perror("sendall");
	              	 printf("We only sent %d bytes because of the error!\n", len);
		        }
                        cse4589_print_and_log("[SEND:SUCCESS]\n");
                        cse4589_print_and_log("[SEND:END]\n");
                	}
		   else if(strcmp(args[0].c_str(),"BLOCK")==0 && args.size()==2){
			if(inet_pton(AF_INET, args[1].c_str(), &(sa.sin_addr))<=0){
                        cse4589_print_and_log("[BLOCK:ERROR]\n");
                        cse4589_print_and_log("[BLOCK:END]\n");
                        continue;
                        }
			cse4589_print_and_log("[BLOCK:SUCCESS]\n");
                        cse4589_print_and_log("[BLOCK:END]\n");
                       int len = strlen(buf);
                       if (sendall(sockfd,buf,&len) == -1) {
                         perror("sendall");
                         printf("We only sent %d bytes because of the error!\n", len);
                            }
			}
                   else if(strcmp(args[0].c_str(),"UNBLOCK")==0 && args.size()==2){
                        if(inet_pton(AF_INET, args[1].c_str(), &(sa.sin_addr))<=0){
                        cse4589_print_and_log("[UNBLOCK:ERROR]\n");
                        cse4589_print_and_log("[UNBLOCK:END]\n");
                        continue;
                        }
                        cse4589_print_and_log("[UNBLOCK:SUCCESS]\n");
                        cse4589_print_and_log("[UNBLOCK:END]\n");
                       int len = strlen(buf);
                       if (sendall(sockfd,buf,&len) == -1) {
                         perror("sendall");
                         printf("We only sent %d bytes because of the error!\n", len);
                            }
                        }	
                   else if(strcmp(args[0].c_str(),"BROADCAST")==0 && args.size()>=2){
                       int len = strlen(buf);
                       if (sendall(sockfd,buf,&len) == -1) {
                         perror("sendall");
                         printf("We only sent %d bytes because of the error!\n", len);
          	            }
			
                        }
        		}
	   }
	  else if(FD_ISSET(sockfd,&read_fds)){
			cout << "Ready to receive\n";
		//	buf = malloc((MAXDATASIZE + 1) * sizeof(char));
                        if ((nbytes = recv(sockfd, buf,1024 ,0)) <= 0) {
                         if (nbytes == 0) {

                            printf("Server closed its connection");
			    return 0;	
                         }
                         else {
                                printf("Buf : %s",buf);
                            perror("recv");
                         }		
			}
			else{
				buf[nbytes]='\0';
			        char temp[MAXDATASIZE];
                         	strcpy(temp,buf);
	                        charTokens=strtok(temp, " ");
        	                 args.clear();
                	         while(charTokens)
                        	 {
                                 args.push_back(charTokens);
                                 charTokens = strtok(NULL, " ");
                         	 }
                                string toMsg=buf;
                                toMsg = toMsg.substr(toMsg.find(args[0])+args[0].length()+1);
                                cout << "Message Received: "<<toMsg<<"\n";

	                        cse4589_print_and_log("[RECEIVED:SUCCESS]\n");
				cse4589_print_and_log("msg from:%s\n[msg]:%s\n", args[0].c_str(), toMsg.c_str());
                                cse4589_print_and_log("[RECEIVED:END]\n");				
					
			}
		continue;
	 }
	} 
	  		
        return 0;
    }
int commonCommands(char *command,int type){
        char *ubit_name="vijayaha";
        int portNo;
         if(strcmp(command,"AUTHOR" )==0 ){
              cse4589_print_and_log("[%s:SUCCESS]\n", command);
              cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n",ubit_name);
              cse4589_print_and_log("[%s:END]\n", command);
             }
         else if(strcmp(command,"PORT")==0 ){
              portNo=strtol(port,NULL,10);
	      cse4589_print_and_log("[%s:SUCCESS]\n", command);
              cse4589_print_and_log("PORT:%d\n", portNo);
	      cse4589_print_and_log("[%s:END]\n", command);
             }
         else if(strcmp(command,"IP")==0 ){
	      cse4589_print_and_log("[%s:SUCCESS]\n", command);	
              cse4589_print_and_log("IP:%s\n", ipAddress);
	      cse4589_print_and_log("[%s:END]\n", command);
             }
  	 else if(strcmp(command,"EXIT")==0){	
	  // CLEAR CLIENT STATE

	   // Clear client state	 
	  exit (0); 
	}
	return 0;
}
int initClient(){
	string cmdInput;
        vector <string> args;
        char *charTokens;  
        char *ubit_name="vijayaha";
        int portNo;
        char servIp[256];
        char servPort[256];
        char *temp;
        while(1){
        cout<<"$";
        getline(cin,cmdInput);
	// Taken from stackoverflow 
	string buf;
        stringstream ss(cmdInput); 
	// Till here
	args.clear();
	while (ss >> buf)
         args.push_back(buf);
	if(args.size()==1){
	// Taken from stackoverflow
	  char *temp = new char[args[0].length() + 1];
	  strcpy(temp, args[0].c_str());
	// Till here
	  commonCommands(temp,0);
	  delete [] temp;	
	}
	else{
            if(strcmp(args[0].c_str(),"LOGIN")==0 && args.size()==3){
               char * cstr = new char [args[1].length()+1];
               char * cstr1 = new char [args[2].length()+1];
               std::strcpy (cstr, args[1].c_str());
               std::strcpy (cstr1, args[2].c_str());
 	       int length= strspn(args[1].c_str(), "0123456789.");
	       if(length !=args[1].length()){
		cse4589_print_and_log("[LOGIN:ERROR]\n");
                cse4589_print_and_log("[LOGIN:END]\n");
	        continue;
	       }
	      length= strspn(args[2].c_str(), "0123456789");
	      if(length !=args[2].length()){
		cse4589_print_and_log("[LOGIN:ERROR]\n");
                cse4589_print_and_log("[LOGIN:END]\n");
	        continue;
	        }		
               int sockId = clientLogin(cstr1,cstr);
	       if(sockId == -1){
		cout << "command failed";
		cse4589_print_and_log("[%s %s %s:ERROR]\n", args[0].c_str(),args[1].c_str(),args[2].c_str());
		cse4589_print_and_log("[%s %s %s:END]\n", args[0].c_str(),args[1].c_str(),args[2].c_str());
		}		
	       else{
		//break;				
		}
             }    
        }
       }
 } 
int initIpAddr(){
	/*Part of this code has been referred from 
     "https://stackoverflow.com/questions/212528/get-the-ip-address-of-the-machine/215333#215333"*/
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    const char* googleDns = "8.8.8.8";
    uint16_t dnsPort = 53;
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(googleDns);
    serv.sin_port = htons(dnsPort);

    int err = connect(sock, (const sockaddr*) &serv, sizeof(serv));

    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (sockaddr*) &name, &namelen);

    ipAddress = inet_ntoa(name.sin_addr);
    close(sock);
}
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/* Clear LOGFILE*/
        fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
        char *source=argv[1];
        port=argv[2];

        int portNo=strtol(port,NULL,10);
	if(argc<2){
	cout<<"Two Arguments required\n";
	return 1;
	}
	initIpAddr();
	if(strcmp(source,"s")==0)
	serverInit(port);
	else if(strcmp(source,"c")==0){
	//initClient(port,"stones","17 send 127.12 Hello");	
	initClient();
	}
	return 0;
}
