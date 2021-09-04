/*
    NAME OF AUTHOR: MANJEET KUMAR
    ROLL NO.: 20CS60R47
    ASSIGNMENT NO.: 04
    FILE NAME : client.cpp
*/

#include<iostream>
#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include <netdb.h>
#include<sstream>
#include <stdlib.h>
#include<fstream>

using namespace std;
#define MAXLINE 1000

int main(int argc,char *argv[])
{
    int PORT;
    stringstream geek(argv[1]);
    geek >> PORT;
    char buffer[MAXLINE];
    int sockfd=0;
    if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
    {
        cout<<"Socket creation error";
        return -1;
    }

    struct hostent *server;
    server = gethostbyname("localhost");

    struct sockaddr_in serv_addr;
    bzero((char *)&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0 )
    {
        cout<<"Connection failed!";
        exit(EXIT_FAILURE);
    }
    cout<<"Connected to the Server"<<endl;
    while(1)
    {
        int m,n,o,j=0,k=0;
        char command[MAXLINE];
        memset(buffer,0,MAXLINE);
        cout<<endl<<"Please enter command to the server:";
        fgets(buffer, MAXLINE, stdin);
        write(sockfd,buffer,MAXLINE);
        while(!isspace(buffer[k]))
            command[j++] = buffer[k++];
        command[j]='\0';

        if(!strcmp(command,"QUIT"))
        {
            cout<<"Client disconnected from server"<<endl;
            exit(1);
        }
        
        else if(!strcmp(command,"LIST"))
        {
            while(1)
            { 		  
                read(sockfd, buffer, MAXLINE);  
                if(!strcmp(buffer,"0"))
                    break;            
                cout<<buffer;
                memset(buffer,0,MAXLINE);
            }
        }
        else if(!strcmp(command,"RETR"))
        {
            ofstream fp1;
            char filename[MAXLINE];
            j=0;
            k=k+1;
            while(!isspace(buffer[k]))
                filename[j++] = buffer[k++];
            filename[j]='\0';
            memset(buffer,0,MAXLINE);
            recv(sockfd,buffer,MAXLINE,0);
            if(!strcmp(buffer,"1"))
            {
                fp1.open(filename,ios::out);
                if(!fp1.is_open())
                {
                    cout<<"Error in creating file at client side"<<endl;
                    write(sockfd,"0",MAXLINE);
                }
                else
                {
                    write(sockfd,"1",MAXLINE);
                    while(1)
                    {
                        memset(buffer,0,MAXLINE);
                        read(sockfd,buffer,MAXLINE);
                        if(!strcmp(buffer,"0"))
                            break;
                        fp1 << buffer;
                    }
                    fp1.close();
                    cout<<"File download done.\n";
                }
            }
            else
            {
                cerr<<"Error in openning file at server side"<<endl;
            }
        }
        
        else if(!strcmp(command,"STOR"))
        {
            char filename[MAXLINE];
            int p;
            ifstream fp;
            j=0;
            k=k+1;
            while(!isspace(buffer[k]))
                filename[j++] = buffer[k++];
            filename[j]='\0';
            fp.open(filename,ios::in);
            if(fp.is_open())
            {
                write(sockfd,"1",MAXLINE);
                memset(buffer,0,MAXLINE);
                read(sockfd,buffer,MAXLINE);
                if(!strcmp(buffer,"0"))
                {
                    cout<<"Error in creating file in server side"<<endl;
                }
                else
                {
                    string str;
                    while(std::getline(fp,str))
                    {
                        int x;
                        memset(buffer,0,MAXLINE);
                        for(x=0;x<str.length();x++)
                            buffer[x] = str[x];
                        buffer[x]='\n';
                        buffer[x+1]='\0';
                        str.clear();
                        write(sockfd,buffer,MAXLINE);
                    }
                    write(sockfd,"0",MAXLINE);
                    fp.close();
                    cout<<"File upload done.\n";
                }
            }
            else
            {
                write(sockfd,"0",MAXLINE);
                cerr<<"Error in opening file at client side"<<endl;
            }
        }
        
        else if(!strcmp(command,"DELE"))
        {
            char filename[MAXLINE];
            j=0;
            k=k+1;
            while(!isspace(buffer[k]))
                filename[j++] = buffer[k++];
            filename[j]='\0';
            recv(sockfd, buffer,MAXLINE,0); 
            if(!strcmp(buffer,"1"))
                cout<<"File "<<filename<<" deleted successfully"<<endl;
            else
                cout<<"Unable to delete the file "<<filename<<endl;
        }
        else if(!strcmp(command,"CODEJUD"))
        {
            char filename[MAXLINE];
            ifstream fp;
            j=0;
            k=k+1;
            while(!isspace(buffer[k]))
                filename[j++] = buffer[k++];
            filename[j]='\0';
            fp.open(filename,ios::in);
            int go=1;
            if(fp.is_open())
            {
                write(sockfd,"1",MAXLINE);
                memset(buffer,0,MAXLINE);
                read(sockfd,buffer,MAXLINE);

                //SENDIND FILE TO THE SERVER
                if(!strcmp(buffer,"0"))
                {
                    go=0;
                    cout<<"Error in creating file in server side"<<endl;
                }
                else
                {
                    string str;
                    while(std::getline(fp,str))
                    {
                        int x;
                        memset(buffer,0,MAXLINE);
                        for(x=0;x<str.length();x++)
                            buffer[x] = str[x];
                        buffer[x]='\n';
                        buffer[x+1]='\0';
                        str.clear();
                        write(sockfd,buffer,MAXLINE);
                    }
                    write(sockfd,"0",MAXLINE);
                    fp.close();
                }
            }
            else
            {
                go=0;
                write(sockfd,"0",MAXLINE);
                cout<<filename<<" file not present at client side"<<endl;
            }
            //FILE SENT
            if(go==1)
            {
                memset(buffer,0,MAXLINE);
                read(sockfd,buffer,MAXLINE);
                if(!strcmp(buffer,"1"))
                {
                    cout<<"COMPILE_SUCCESS"<<endl;
                    memset(buffer,0,MAXLINE);
                    read(sockfd,buffer,MAXLINE);
                    if(!strcmp(buffer,"1"))
                    {
                        cout<<"RUN_SUCCESS"<<endl;
                        memset(buffer,0,MAXLINE);
                        read(sockfd,buffer,MAXLINE); 
                        if(!strcmp(buffer,"1"))
                        {
                            cout<<"ACCEPTED"<<endl;
                        }  
                        else
                        {
                            cout<<"WRONG_ANSWER"<<endl;
                        }
                    }
                    else if(!strcmp(buffer,"-1"))
                    {
                        cout<<"TLE"<<endl;
                    }
                    else
                    {
                        cout<<"RUN_ERROR"<<endl;
                    }
                }
                else if(!strcmp(buffer,"0"))
                {
                    cout<<"COMPILE_ERROR"<<endl;
                    memset(buffer,0,MAXLINE);
                    read(sockfd,buffer,MAXLINE);
                    if(!strcmp(buffer,"1"))
                    {
                        while(1)
                        {
                            memset(buffer,0,MAXLINE);
                            read(sockfd,buffer,MAXLINE);
                            if(!strcmp(buffer,"0"))
                                break;
                            cout<<buffer;
                        }
                    }

                }
                else
                {
                    cout<<buffer<<endl;
                }
            }
            
        }
        else
        {
            cerr<<"Enter a valid command!"<<endl;
            memset(buffer,0,MAXLINE);
        }
        
       
    }
    return 0;
}