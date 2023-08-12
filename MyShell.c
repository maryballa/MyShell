#include <stdlib.h>   
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "MyShell.h"

#define MAX_CMD_LENGTH 100
#define MAX_NUM_PARAMS 10



int main(){    
    char cmd[MAX_CMD_LENGTH+1];   				//every char user types is inserted into cmd (not inserted yet)
    char * params[MAX_NUM_PARAMS+1];    			//every word of the command is inserted in param (not inserted yet)
    char * argv1[MAX_NUM_PARAMS+1] = {0};    			//every char user types before or after the "|" sumbol (for pipe sitution, not inserted yet)
    char * argv2[MAX_NUM_PARAMS+1] = {0};    			//every char user types before or after the "|" sumbol (for pipe sitution, not inserted yet)
    int j, i, x;    
    int f=1, in=0, out=0;    					//switches that take the opposite vaule when "|", "<", ">" exist
    
    int fd[2];
    pipe(fd);
    		
    while(1){
        printf("my_shell$ "); 				//for a more like prompt appereance     
     
        if(fgets(cmd, sizeof(cmd), stdin) == NULL) break; 	//reading the line user entered      
        if(cmd[strlen(cmd)-1] == '\n') { 			//proper ending check    
            cmd[strlen(cmd)-1] = '\0';    
        }    
        
        j=splitcmnd(cmd, params); 				//seperating the line into individual strings | j equals numbero f the words
               
        if(strcmp(params[0], "exit")==0) break; 		//exiting the program when exit is typed 
        	
        if(strcmp(params[0], "help")==0){	
        	help(params);
        	continue;
        }
        
        for(i=0; i<j; i++){ 				//check if "|" symbol exists meaning a different approach will be used for pipe 
            if(strcmp(params[i], "|")==0){    
            	f=0;      					//in order to recognise from f's value whether we have a pipe or not
                break;						// as long as at least one char is "|" then surely we have a pipe, no need to check the rest
            }               
        }
    	
        if(f==0){						//when we have a pipe
            for(x=0; x<i; x++){    
               argv1[x]=params[x];				//scanning before the "|" symbol of the line entered
            } 
        	
            int z=0;     
            for(x=i+1; x < j; x++){     
                argv2[z]=params[x];				//scanning after the "|" symbol of the line entered
                z++;
            }        
             
             execpipe(argv1, argv2);				
    
         } else if(f==1){
         
         	 execrdr(params);				// when we dont have a pipe
         }        
    } 
    return 0;
}

int splitcmnd(char* cmd, char** params){ 			//function for line turning into seperate strings
    int i,n=-1;
    
    for(i=0; i<MAX_NUM_PARAMS; i++){				
        params[i]=strsep(&cmd, " ");				//cutting the word between spaces and inserts it into param
        n++;
        if(params[i]==NULL) break;				//in case it is smaller than expected
    }
    return(n);							//num of words
}

void execpipe (char ** argv1, char ** argv2){

    int fds[2];
    pipe(fds);
    int i;
    int status;
    pid_t wpid;
    pid_t pid = fork();
    
    if (pid==-1){ 						//means we have error    
        printf("error fork!!\n"); 
		exit(1);  
    } 
    
    if (pid==0) { 						//child process
        close(fds[1]);
        dup2(fds[0], 0);
        close(fds[0]);
        execvp(argv2[0], argv2); 				//run command AFTER pipe character 
        
        printf("unknown command\n");
        
    } else { 							//parent process
    
        close(fds[0]);
        dup2(fds[1], 1);
        close(fds[1]);
        execvp(argv1[0], argv1); 				//run command BEFORE pipe character 
        
        printf("unknown command\n");  
        do {
     	 	wpid = waitpid(pid, &status, WUNTRACED);	//parent waits the child to finish
       } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

 								//function for redirection ( '<' , '>' )
void execrdr(char ** argv){

	int status;
	pid_t pid = fork();
	
	if (pid==0){          					//child process
    		int i,in=0,out=0;
    		char input[64],output[64];				//input for copying the next word from <, output for copying the next word from <
    								
    		for(i=0; argv[i]!='\0';i++){				//finds where '<' or '>' occurs and make that argv[i] = NULL , so that command wont't read that
    	
        		if(strcmp(argv[i],"<")==0){        
           			argv[i]=NULL;
           			strcpy(input,argv[i+1]);
           		 	in=1;           
        		}               

        		if(strcmp(argv[i],">")==0){
            			argv[i]=NULL;
            			strcpy(output,argv[i+1]);
            			out=1;
        		}         
    	}
    	
    	if(in){  						//if '<' char was found  
        	int fd0;
        	if ((fd0 = open(input, O_RDONLY, 0)) < 0){
            		perror("Couldn't open input file");
           		exit(1);
        	}           
  			
        	dup2(fd0, 0);				 	//STDIN_FILENO here can be replaced by 0 	
        	close(fd0); 
    	}

    	if(out){						//if '>' char was found 

        	int fd1;
        	if ((fd1=creat(output , 0644)) < 0) {
            		perror("Couldn't open the output file");
            		exit(1);
        	}           

        	dup2(fd1, STDOUT_FILENO); 			//1 here can be replaced by STDOUT_FILENO
        	close(fd1);
    	}

    	execvp(argv[0], argv);
    	printf("unknown command\n");
     		 
	} else if (pid == -1){					//when error 
        	printf("fork() failed!\n");
        	exit(1);      
    		} else{                                 	//parent process
        		while (!(wait(&status) == pid)) ;      //parent waits the child to finish
    		}
}


int help(char **args)
{
  	int i;
  	printf("\n");
  	printf("[University of Thessaly - DIB | @Copyrights2022 ]\n\n");
  	printf("\n");
  	printf("\n");
  	printf("*****************************************************");
  	//printf("The following are built in:\n");

  	/*for (i = 0; i < num_builtins(); i++) {
  	  printf("  %s\n", builtin_str[i]);
  	}*/

  	printf("\nUse the man command for information on other programs.\n");
  	printf("\n\n\n");
  		
  	FILE *fptr; 
  	char c;
  	fptr = fopen("library.txt", "r");				//open file
  	  
  	if (fptr == NULL){
  	    printf("Cannot open file \n");
  	    exit(0);
  	}
  	
  	c = fgetc(fptr);						
  	while (c != EOF){						//read contents from file
  	    printf ("%c", c);
  	    c = fgetc(fptr);
  	}
  	fclose(fptr);
  	return 1;
}
