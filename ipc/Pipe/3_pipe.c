#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main(void)
{

    int fd[2], nbytes;
    pid_t  childpid;
    char string1[] = "Hello, world #1! \n";
    char string2[] = "Hello, world #2! \n";
    char string3[] = "Hello, world #3! \n";
    char string4[] = "Hello, world #4! \n";
    
    char readbuffer[19];
    int fd2[2];
    pipe(fd2);

    pipe(fd);
    childpid = fork();
    if( childpid == -1)
    {
        printf("fork failed\n");
        exit(1);
    }
    if(childpid == 0)    // parent
    { 

        write(fd[1], string1, (strlen(string1)+1));
        write(fd[1], string2, (strlen(string2)+1));
       


        nbytes=read(fd[0], readbuffer, sizeof(readbuffer));
        printf("process: %d, nbytes: %d, Received: %s \n", getpid(),nbytes, readbuffer);
        nbytes=read(fd[0], readbuffer, sizeof(readbuffer));
        printf("process: %d, nbytes: %d, Received: %s \n", getpid(),nbytes, readbuffer);
        
        close(fd[0]);
        close(fd[1]);
    }
    else{                 // child
        write(fd[1], string3, (strlen(string3)+1));
        write(fd[1], string4, (strlen(string4)+1));        

        nbytes=read(fd[0], readbuffer, sizeof(readbuffer));
        printf("process: %d, nbytes: %d, Received: %s \n", getpid(),nbytes, readbuffer);
        nbytes=read(fd[0], readbuffer, sizeof(readbuffer));
        printf("process: %d, nbytes: %d, Received: %s \n", getpid(),nbytes, readbuffer);

        
        close(fd[0]);
        close(fd[1]);
    }
    return(0);
}