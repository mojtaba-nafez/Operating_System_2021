#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main(void)
{

    int fd[2], nbytes;
    pid_t  childpid;
    char string1[] = "Hello, world #1! ";
    char string2[] = "Hello, world #2! ";
    char string3[] = "Hello, world #3! ";
    char string4[] = "Hello, world #4! ";
    
    char readbuffer[19];
    char readbuffer2[19];

    pipe(fd);
    childpid = fork();
    if( childpid == -1)
    {
        printf("fork failed\n");
        exit(1);
    }

    if(childpid == 0)    // parent
    {
        // Child process closes up input side of pipe 
         close(fd[0]);


        // ssize_t write(int __fd, const void * __buf, size_t __n);
        // write N byte of BUF to FD.
        write(fd[1], string1, (strlen(string1)+1));
        write(fd[1], string2, (strlen(string2)+1));
        
        
        close(fd[1]);
    }
    else                 // child

        //close(fd[1]);

        read(fd[0], readbuffer2, sizeof(readbuffer2));
        printf("Received string: %s   %d\n", readbuffer2, sizeof(readbuffer2));
    

    return(0);
}

