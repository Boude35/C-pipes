#include <iostream>
#include <csignal>
#include <cstdio> 
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <map>

#include "Command.hpp"



//boolean that will make the program remember when there is a pipeout before a command
bool input = false;
//creating the pipe array
int pipeInfo[2];
//the map will hold the proces running in the background
map<int, Command> mapa;


//output the pid and command when there is a signal
void signalhand(int sig)
{
	//pid of the process
	int endpid = waitpid(0, NULL, WNOHANG);
	
	//if it is in the set output the message
	if(mapa.find(endpid) != mapa.end())
		cout << "Completed: PID= " << endpid << " : " << mapa.find(endpid) -> second << endl;
}

int main(void)
{
    Command com;
	
     //execute the signal handler when a child process ends
     signal(SIGCHLD, signalhand);
    
    // prompt for and read in first command. 
    cout << ">>>> ";
    com.read();
    
    //execute while the imput is not exit
    while(com.name() != "exit")
    { 
    	
    	//if the command is cd
        if(com.name() == "cd")
        {
    		chdir(com.args()[1].c_str());//use chdir() to change the directory we are working in
        } 
        else 
        {
        	//create the pipe before forking
		if(com.pipeOut())
			int myPipe = pipe(pipeInfo);
		
		int pid = fork();//duplicate the process
		
		if(com.redirIn() && pid == 0 /*child*/) //if there is an < sign and we are in the child process
		{
			FILE *fp = fopen(com.inputRedirectFile().c_str(), "r"); //open the file after the <
			dup2(fileno(fp), fileno(stdin));//use it as the std input
		}
		
		if(input && pid == 0) //if there is an | before a command
		{
			close(pipeInfo[1]);//close the writer in the child
			dup2(pipeInfo[0], fileno(stdin)); // sets the input for the other/s commands in the pipe
			input = false; //reset the value
			
		}
		
		if(com.redirOut() && pid == 0 /*child*/) //if there is an > sign and we are in the child process
		{
			FILE *fp = fopen(com.outputRedirectFile().c_str(), "w"); //open the file after the >
			dup2(fileno(fp), fileno(stdout)); //use it as the std output
		}
		
		if(com.pipeOut())
		{
			if (pid == 0)
			{
			//insert the values in the pipe	
			close(pipeInfo[0]); //close the reader
			dup2(pipeInfo[1], fileno(stdout)); 
			
			}
			else //if it is a parent process
			{
				//close the writer which can cause problems if open
				close(pipeInfo[1]);
				//input is true
				input = true;
			}
			
		}
		
		if (pid == 0) // if we are a child
		{
			//read the command by converting a string into a char
			int size = com.numArgs() + 1;
			char* args[size];	
			for(int i = 0; i < com.numArgs(); i++)
			{
				args[i] = (char *) com.args()[i].c_str();
			}
			args[com.numArgs()] = NULL;
			//execute the process
	       	execvp(args[0], args);
		}
		
		// if not backgrounded by & and not backgrounded by | and parent process
		if(!com.backgrounded() && !com.pipeOut() && pid != 0) 
		{
			//wait for the process to end
	       	waitpid(pid, NULL, 0);
	       	
		}
		else{
			//add a child value running in the background to the set
			mapa[pid] = com;
		}
        }
  
        
        
        // prompt for and read next command
	cout << ">>>> ";
        com.read();
    }
    
    cout << "Thank you for using mini-shell. We now return you to your regularly scheduled shell!" << endl;
}










