/*
    NAME OF AUTHOR: MANJEET KUMAR
    ROLL NO.: 20CS60R47
    ASSIGNMENT NO.: 04
    FILE NAME : server.cpp
*/

#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fstream>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include<sstream>
#include <sys/time.h>

#define MAXLINE 1000
#define TRUE   1  
#define FALSE  0
using namespace std;


int main(int argc, char *argv[])
{
    int PORT;
    stringstream geek(argv[1]);
    geek >> PORT;
    int opt = TRUE;   
    int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30 , activity, i , p , sd;   
    int max_sd;   
    struct sockaddr_in address;
    char buffer[MAXLINE];
    fd_set readfds;  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    } 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    } 

    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );

    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    }
    cout<<"waiting for client to connect.."<<endl;
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }
    if (listen(master_socket, 3) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }
    addrlen = sizeof(address);
    while(TRUE)   
    {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;
        for( i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }

        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }
            cout<<endl<<"Connected with client with socket number "<<inet_ntoa(address.sin_addr)<<":"<<ntohs(address.sin_port)<<endl;  
            for (i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;     
                    break;   
                }   
            }   
        }

        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];
            if (FD_ISSET( sd , &readfds))   
            {
                int k=0,j=0;
                char command[MAXLINE],filename[MAXLINE];
                memset(buffer,0,MAXLINE);
                int p = read( sd, buffer,MAXLINE);
                while(!isspace(buffer[k]))
                    command[j++] = buffer[k++];
                command[j]='\0';
                k=k+1;
                j=0;
                
                if(!strcmp(command,"QUIT"))
                {
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    cout<<endl<<"Client socket number "<<inet_ntoa(address.sin_addr)<<":"<<ntohs(address.sin_port)<<" sent message:"<<command<<endl;
                    cout<<"Closing connection with client socket number "<<ntohs(address.sin_port)<<endl;
                    write(sd,"QUIT",strlen("QUIT"));
                    client_socket[i] = 0;
                }

                else if(!strcmp(command,"LIST"))
                {
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    cout<<endl<<"Client socket number "<<inet_ntoa(address.sin_addr)<<":"<<ntohs(address.sin_port)<<" sent message:"<<command<<endl;
                    FILE *in;
                    char temp[MAXLINE];
                    if(!(in = popen("ls", "r")))
                        cout<<"error"<<endl;
                    memset(temp,0,MAXLINE);
                    cout<<"Sending list of files to client "<<inet_ntoa(address.sin_addr)<<":"<<ntohs(address.sin_port)<<endl;
                    while(fgets(temp, sizeof(temp), in)!='\0' )
                    {
                        write(sd, temp, MAXLINE);
                        memset(temp,0,MAXLINE);                   
                    }
                    write(sd,"0",MAXLINE);
                    pclose(in);
                }

                

                else if (!strcmp(command,"RETR"))
                {
                    while(!isspace(buffer[k]))
                        filename[j++] = buffer[k++];
                    filename[j]='\0';

                    ifstream fp1;
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);	
                    cout<<endl<<"Client socket number "<<inet_ntoa(address.sin_addr)<<":"<<ntohs(address.sin_port)<<" sent message:"<<buffer<<endl;
                    fp1.open(filename,ios::in);
                    memset(buffer,0,MAXLINE);
                    
                    if (!fp1.is_open())
                    {
                        cout<<"Error in openning file at server side"<<endl;
                        send(sd,"0",MAXLINE,0);
                    }
                    else
                    { 
                        send(sd,"1",MAXLINE,0);
                        memset(buffer,0,MAXLINE);
                        read(sd,buffer,MAXLINE);
                        if(!strcmp(buffer,"0"))
                        {
                            cout<<"Error in creating file at client side"<<endl;
                        }
                        else
                        {
                            strcpy(buffer,"1");
                            string str;
                            cout<<"Sending file "<<filename<<" to client "<<inet_ntoa(address.sin_addr)<<":"<<ntohs(address.sin_port)<<endl;
                            while(std::getline(fp1,str))
                            {
                                int x;
                                memset(buffer,0,MAXLINE);
                                for(x=0;x<str.length();x++)
                                    buffer[x] = str[x];
                                buffer[x]='\n';
                                buffer[x+1]='\0';
                                str.clear();
                                send(sd,buffer,MAXLINE,0);
                            }
                            send(sd,"0",MAXLINE,0);
                            fp1.close(); 
                        }      
                    }
                }

                else if (!strcmp(command,"STOR"))
                {
                    while(!isspace(buffer[k]))
                        filename[j++] = buffer[k++];
                    filename[j]='\0';
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    cout<<endl<<"Client socket number "<<inet_ntoa(address.sin_addr)<<":"<<ntohs(address.sin_port)<<" sent message:"<<buffer<<endl;
                    ofstream fp;
                    memset(buffer,0,MAXLINE);
                    read(sd,buffer,MAXLINE);   
                    if(strcmp(buffer,"1")==0)
                    {
                        fp.open(filename,ios::out);
                        if(!fp.is_open())
                        {
                            cout<<"Error in creating file at server side\n";
                            write(sd,"0",MAXLINE);
                        }
                        else
                        {
                            write(sd,"1",MAXLINE);
                            while(1)
                            {
                                memset(buffer,0,MAXLINE);
                                read(sd,buffer,MAXLINE);
                                if(!strcmp(buffer,"0"))
                                    break;
                                fp << buffer;    
                            }
                            fp.close();
                            cout<<"File "<<filename<<" recieved"<<endl;
                        }
                    }
                    else
                    {
                        cout<<"Error in openning file at client side"<<endl;
                    }
                    
                }

                else if (!strcmp(command,"DELE"))
                {
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    cout<<endl<<"Client socket number "<<inet_ntoa(address.sin_addr)<<":"<<ntohs(address.sin_port)<<" sent message:"<<buffer<<endl;
                    while(!isspace(buffer[k]))
                        filename[j++] = buffer[k++];
                    filename[j]='\0';
                    if (remove(filename) == 0) 
                    {
                        cout<<"File "<<filename<<" deleted successfully"<<endl; 
                        write(sd,"1",MAXLINE);
                    }
                    else
                    {
                        cout<<"Unable to delete the file "<<filename<<endl;
                        write(sd,"0",MAXLINE);
                    }
                }
                else if(!strcmp(command,"CODEJUD"))
                {
                    memset(filename,0,MAXLINE);
                    while(!isspace(buffer[k]))
                        filename[j++] = buffer[k++];
                    filename[j]='\0';
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    cout<<endl<<"Client socket number "<<inet_ntoa(address.sin_addr)<<":"<<ntohs(address.sin_port)<<" sent message:"<<buffer<<endl;
                    ofstream fp;
                    memset(buffer,0,MAXLINE);
                    read(sd,buffer,MAXLINE); 
                    int go=1;  
                    if(strcmp(buffer,"1")==0)
                    {
                    
                        fp.open(filename,ios::out);
                        if(!fp.is_open())
                        {
                            go=0;
                            cout<<"Error in creating file at server side\n";
                            write(sd,"0",MAXLINE);
                        }
                        else
                        {
                            write(sd,"1",MAXLINE);
                            while(1)
                            {
                                memset(buffer,0,MAXLINE);
                                read(sd,buffer,MAXLINE);
                                if(!strcmp(buffer,"0"))
                                    break;
                                fp << buffer;    
                            }
                            fp.close();
                        }
                    }
                    else
                    {
                        go=0;
                        cout<<filename<<" file not present at client side"<<endl;
                    }
                    //FILE RECIEVED
                    if(go==1)
                    {    
                        char prog[MAXLINE],input_prog[MAXLINE],output_prog[MAXLINE],error_prog[MAXLINE],test_prog[MAXLINE];
                        memset(prog,0,MAXLINE);
                        memset(input_prog,0,MAXLINE);
                        memset(output_prog,0,MAXLINE);
                        memset(error_prog,0,MAXLINE);
                        memset(test_prog,0,MAXLINE);
                        k=0;
                        while(filename[k]!='.')
                        {
                            prog[k]=filename[k];
                            k++;
                        }
                        prog[k]='\0';
                        strcpy(input_prog,"input_");
                        strcat(input_prog,prog);
                        strcat(input_prog,".txt");
                    
                        strcpy(output_prog,"output_");
                        strcat(output_prog,prog);
                        strcat(output_prog,".txt");

                        strcpy(error_prog,"error_");
                        strcat(error_prog,prog);
                        strcat(error_prog,".txt");

                        strcpy(test_prog,"testcase_");
                        strcat(test_prog,prog);
                        strcat(test_prog,".txt");

                        char comd[MAXLINE];
                        memset(comd,0,MAXLINE);
                        string inp,str;
                        ifstream fp1,fp3,fp5,fp6,fp7;
                        ofstream fp2,fp4;
                        int inp_present=1;
                        fp1.open(input_prog,ios::in);
                        if(!fp1.is_open())
                        {
                            inp_present = 0;   
                        }
                        fp7.open(test_prog,ios::in);
                        if(!fp7.is_open())
                        {
                            write(sd,"Test case file not present at server side",MAXLINE);  
                        }
                        fp7.close();
                        //fp4.open(output_prog,ios::trunc);
                        strcpy(comd,"g++ ");
                        strcat(comd,filename);
                        strcat(comd," 2>");
                        strcat(comd,error_prog);
                        //COMPILATION PHASE
                        int a=system(comd);
                        if(a==0)
                        {
                            write(sd,"1",MAXLINE);
                            fp4.open(output_prog,ios::trunc);
                            int b,flag=1;
                            //EXECUTION PHASE
                            if(inp_present == 1)
                            {
                                while(getline(fp1,inp))
                                { 
                                    fp2.open("temp_input.txt",ios::trunc);
                                    fp2<<inp<<endl;
                                    fp2.close();
                                    time_t start, end;
                                    time(&start);
                                    b = system("./a.out< temp_input.txt >temp_output.txt ");
                                    time(&end);
                                    float time_taken = float(end - start);
                                    if(b!=0)
                                    {
                                        write(sd,"0",MAXLINE); //RUNTIME ERROR
                                        flag=0;
                                        break;
                                    }
                                    if(time_taken > 1)
                                    {
                                        write(sd,"-1",MAXLINE);
                                        flag=0;
                                        break;
                                    }
                                    fp3.open("temp_output.txt",ios::in);
                                    getline(fp3,str);
                                    fp4<<str;
                                    fp4<<endl;
                                    fp3.close();
                                }
                            }
                            else
                            {
                                fp2.open("temp_input.txt",ios::trunc);
                                fp2<<inp<<endl;
                                fp2.close();
                                time_t start, end;
                                time(&start);
                                b = system("./a.out< temp_input.txt >temp_output.txt ");
                                time(&end);
                                float time_taken = float(end - start);
                                if(b!=0)
                                {
                                    write(sd,"0",MAXLINE); //RUNTIME ERROR
                                    flag=0;
                                    break;
                                }
                                if(time_taken > 1)
                                {
                                    write(sd,"-1",MAXLINE);
                                    flag=0;
                                    break;
                                }
                                fp3.open("temp_output.txt",ios::in);
                                getline(fp3,str);
                                fp4<<str;
                                fp4<<endl;
                                fp3.close();
                            }
                            
                            if(flag==1)
                            {
                                write(sd,"1",MAXLINE);
                                //MATCHING PHASE
                                fp6.open(output_prog,ios::in);
                                fp7.open(test_prog,ios::in);
                                string s1,s2;
                                int flag1=1;
                                char tok1[MAXLINE],tok2[MAXLINE];
                                memset(tok1,0,MAXLINE);
                                memset(tok2,0,MAXLINE);
                                s1.clear();
                                s2.clear();
                                int m=0,n=0;
                                while(getline(fp6,s1) && getline(fp7,s2) )
                                {
                                    int indx1=0;
                                    while(m<s1.length())
                                    {
                                        if(s1[m]=='\r')
                                            m++;
                                        else
                                        {
                                            tok1[indx1++]=s1[m++];
                                        }
                                    }
                                    tok1[indx1] = '\0';

                                    int indx2=0;
                                    while(n<s2.length())
                                    {
                                        if(s2[n]=='\r')
                                            n++;
                                        else
                                        {
                                            tok2[indx2++]=s2[n++];
                                        }    
                                    }
                                    tok2[indx2] = '\0';

                                    if(strcmp(tok1,tok2)!=0)
                                    {
                                        flag1=0;
                                        write(sd,"0",MAXLINE);
                                        break;
                                    }
                                    
                                    m=0;
                                    n=0;
                                    memset(tok1,0,MAXLINE);
                                    memset(tok2,0,MAXLINE);
                                    s1.clear();
                                    s2.clear();
                                } 
                                if(flag1==1)
                                {
                                    write(sd,"1",MAXLINE);
                                }
                                fp6.close();
                                fp7.close();
                            }
                            //remove(output_prog);
                            remove("temp_input.txt");
                            remove("temp_output.txt");
                            fp4.close();
                        }
                        else
                        {
                            write(sd,"0",MAXLINE);
                            fp5.open(error_prog,ios::in);
                            memset(buffer,0,MAXLINE);
                            
                            if (!fp5.is_open())
                            {
                                cout<<"Error in openning file at server side"<<endl;
                                send(sd,"0",MAXLINE,0);
                            }
                            else
                            { 
                                send(sd,"1",MAXLINE,0);
                                memset(buffer,0,MAXLINE);
                                string r;
                                while(getline(fp5,r))
                                {
                                    int x;
                                    memset(buffer,0,MAXLINE);
                                    for(x=0;x<r.length();x++)
                                        buffer[x] = r[x];
                                    buffer[x]='\n';
                                    buffer[x+1]='\0';
                                    r.clear();
                                    write(sd,buffer,MAXLINE);
                                }
                                write(sd,"0",MAXLINE);
                                fp5.close();
                            } 
                        }       
                        remove(error_prog);
                    }   
                        
                } 
                else
                {
                    cout<<"Invalid command from client!"<<endl;
                    memset(buffer,0,MAXLINE );
                }    
            }
        }
    }
    return 0;
}

