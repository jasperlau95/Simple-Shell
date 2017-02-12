/*
* A Simple Shell
* Press Ctrl-D to end the program
*
*/

/* Header Declaration */
#define MAXARGS 20
#define MAXLOG 50
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
/* Function Declaration */
int getUserInput(char* input);
int tokenizeInput(char* input);
char *makestring(char * buf);
int execute(char * arglist[]);
int logCount = 0;
char * loglist[MAXLOG];

/* Functions */
int main(int argc,char* argv[])
{
  signal(SIGINT,SIG_IGN);
  signal(SIGTERM,SIG_IGN);
  signal(SIGQUIT,SIG_IGN);
  signal(SIGTSTP,SIG_IGN);
  int isExit = 0;

  do {
    char rawInput[255];
    isExit = getUserInput(rawInput);
    if(isExit) break;
    
    char *temp;
    temp = malloc(strlen(rawInput)+1);
    strcpy(temp, rawInput);
    loglist[logCount++] = temp;

    if (strstr(rawInput,"&&") != NULL)
    {
      int numcoms = 0; //number of commands
      char* comlist[20]; //command list(at most 10-1 commands)
      char* token = strtok(rawInput,"&");

      while(token != NULL)
      {
        comlist[numcoms++] = makestring(token);
        token = strtok(NULL,"&");
      }

      int itr = 0, flag = 0; //flag != 0 means command not found
      for (itr; itr < numcoms; itr++)
      {
        flag = tokenizeInput(comlist[itr]);
        if (flag) //if command not found, break
          break;
      }  
    }

    else if (strstr(rawInput,"||") != NULL)
    {
      int numcoms = 0; //number of commands
      char* comlist[20]; //command list(at most 10-1 commands)
      char* token = strtok(rawInput,"|");

      while(token != NULL)
      {
        comlist[numcoms++] = makestring(token);
        token = strtok(NULL,"|");
      }

      int itr = 0, flag = 0; //flag != 0就command not found
      for (itr; itr < numcoms; itr++)
      {
        flag = tokenizeInput(comlist[itr]);
        if (flag == 0)
          break;
      }  
    }

    else
      tokenizeInput(rawInput);

  } while(isExit = 1);
  return 0;
}

/*
  GetUserInput()
  - To parse User Input and remove new line character at the end.
  - Copy the cleansed input to parameter.
  - Return 1 if encountered EOF (Ctrl-D), 0 otherwise.
*/
int getUserInput(char* input)
{
  char buf[255];
  char *s = buf;
  char cwd[PATH_MAX+1];
  getcwd(cwd,PATH_MAX+1);
  printf("[3150 Shell:%s]=> ",cwd);
  if(fgets(buf,255,stdin) == NULL) //read error or EOF(Ctrl+d)
  {
    putchar('\n');
    return 1;
  }
  
  // Remove \n
  for(;*s != '\n';s++);
  *s = '\0';

  strcpy(input,buf);
  return 0;
}
/*
  tokenizeInput()
  - Tokenize the string stored in parameter, delimiter is space
  - In given code, it only prints out the token.
  Please implement the remaining part.
*/

int tokenizeInput(char* input)
{
  char buf[255];
  strcpy(buf,input);
  int numargs = 0;
  char* arglist[MAXARGS+1];
  int flag; //1 is command not found
  char* token = strtok(buf," ");

  while(token != NULL)
  {
    arglist[numargs++] = makestring(token);
    token = strtok(NULL," ");
  }
  arglist[numargs] = NULL;

  flag = execute(arglist);
  
  return flag;
}

int execute(char * arglist[])
{
  int flag = 0; //1 is command not found
  if (strncmp(arglist[0],"log",3) == 0)
    {
      int i = 1;
      for (i; i <= logCount; i++)
        printf("[%d]: %s\n", i, loglist[i-1]);
    } 

  else if (strcmp(arglist[0],"bye") == 0)
  {
    if (arglist[1] != NULL)
    {
      printf("bye: wrong number of arguments\n");
      flag = 1;
    }
    else
      exit(0);
  }
  
  else if (strcmp(arglist[0],"gofolder") == 0)
    {
      if (arglist[1] != NULL)
	    {
	      if (arglist[2] != NULL)
        {
          printf("gofolder: wrong number of arguments\n");
          flag = 1;
        }

	      else if(chdir(arglist[1])< 0)
        {
         printf("{%s}: cannot change directory\n", arglist[1]);
         flag = 1;
        }
    	}

	    else
      {
        printf("gofolder: wrong number of arguments\n");
        flag = 1;
      }
    }

  else
  {
    int status, i;
    if (!fork())
    {
      setenv("PATH","/bin:/usr/bin:.",1);

      execvp(arglist[0], arglist);
      if (errno == ENOENT)
      {
        printf("{%s}: command not found\n", arglist[0]);
        exit(1);
      }
      exit(0);
    }

    else 
    {
      wait(&status);
      i = WEXITSTATUS(status);
    }
    flag = i;
  } 
  
  return flag;
}

char *makestring(char * buf)
{
    char *cp;

    buf[strlen(buf)] = '\0';
    cp = malloc(strlen(buf)+1);
    strcpy(cp, buf);
    return cp;
}
