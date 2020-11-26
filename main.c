#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

//make them global to easy acess them
char command[2000];
char *args[200];
int flag=0;

// major functions
void ReadAndProcess();
void HandlerSignal();
void LogFile();

int main()
{

    while(1){
     //read and process the command line
     ReadAndProcess();
     //start fork
     pid_t pid=fork();
     if(pid == 0){ // in child
        LogFile("Child running now");
       if( execvp(args[0],args) != 1 )
         printf("command not found\n");
        exit(1);
     }
     else if(pid > 0){ // in parent
      if(flag){
       flag =0;continue;
      }
      waitpid(pid,0,0);
      LogFile("Child  finshed");
     }
     else{ // erorr in fork
      LogFile("Error in fork");
      perror("Error \n");
      exit(1);
     }
     signal(SIGCHLD, HandlerSignal);
    }

    return 0;
}

void ReadAndProcess(){
    //read command line
    fgets(command,2000,stdin);
    //remove '/n' from command
    int i=0;
    while(i<1999 && command[i] != '\n')
        i++;
    command[i]='\0';
    // check interal command exit to terminal prog
    if(strcmp(command,"exit")==0)
        exit(0);
    //tokin the line
     i=0;
     args[i] = strtok(command," ");
    while (args[i]){
        i++;
        args[i] = strtok(NULL," ");
    }
    //check command has & or not
    for(int i=0;i<200 && args[i]!=NULL;i++){
        if(strcmp(args[i], "&") == 0){
            flag=1;
            args[i]="\0";
        }
    }
}

void HandlerSignal() {
int status ;
    pid_t pid;
    //pid = wait(0);
    while(waitpid(-1,&status,WNOHANG)>0){
   // if(pid>0)
     LogFile("Child terminated");
    }
}

void LogFile(char* s)
{
    time_t t=time(0);
    //open file to write or create if first time
    FILE* file = fopen("LogFile.txt", "a");
    fprintf(file, "%s  %s \n",s,ctime(&t));
    fclose(file);

}

