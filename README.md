# Online-Judge-or-Autograder
SAMPLE COMMAND TO COMPILE AND RUN THE PROGRAM:

Command to compile server program: g++ server.cpp -o server

Command to run server program: ./server <port_no.>

example: ./server 60340


Command to compile server program: g++ client.cpp -o client

Command to run server program: ./client <port_no.>

example: ./client 60340

NOTE: Press ctrl+c to exit from server program

*************************************************************************************************************************************************************************************************************
APPROACH FOR CODEJUD:

Followed 2nd approach almost like Codechef. 
Generating output line by line for each testcase line and appending it in an OUTPUT FILE and then matching the output file with the TESTCASE FILE. If all the output lines are matched then sending ACCEPT else sending WRONG_ANSWER. And at any point of time if there is any TLE or RUNTIME ERROR or COMPILE ERROR then sending a single mssg. 

*************************************************************************************************************************************************************************************************************
FTP COMMANDS:
1. RETR <filename> 
2. STOR <filename> 
3. LIST  
4. QUIT 
5. DELE <filename> 
6. CODEJUD <filename> <ext(c/cpp)> 

Example of FTP Commands:
1. RETR input.txt
2. STOR input.txt 
3. LIST  
4. QUIT 
5. DELE input.txt
6. CODEJUD add.cpp cpp 

*************************************************************************************************************************************************************************************************************
CONSIDERED POSSIBLE ERRORS:

 1. In RETR command if file is not present at server side then this information will be sent to the client. If client already has the file then the file will be overwritten.

 2. In STOR command if file is not present at client side then this information will be sent to the client. If server already has the file then the file will be overwritten.

 3. In DELE command if file is not present at server side then this information will be sent to the client.  
 
 4. In CODEJUD command if ProgramFile at client side or the TestCase file is not present at the server side  then the client will be informed with a suitable message. 

 5. If the client enters any invalid command, then a suitable message will be sent to the client by the server.
  
