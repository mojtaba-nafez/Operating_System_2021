#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main(void)
{
    /* 
        In Unix and Unix-like computer operating systems, a file descriptor is
        a unique identifier (handle) for a file or other input/output resource,
        such as a pipe or network socket.
    */
    int fd[2], nbytes;
            //  fd[0]: point to reading end of pipe
            //  fd[1]: point to writing end of pipe
 
    pid_t childpid;
    char string[] = "Hello, world!\n";
    char readbuffer[80];

    pipe(fd);

    if((childpid = fork()) == -1)
    {
        printf("fork failed\n");
        exit(1);
    }

    if(childpid == 0)
    {
        /* Child process closes up input side of pipe */
        close(fd[0]);

        /* Send "string" through the output side of pipe */
        write(fd[1], string, (strlen(string)+1));
        exit(0);
    }
    else
    {
        /* Parent process closes up output side of pipe */
        close(fd[1]);

        /* Read in a string from the pipe */
        // nbytes: 0: EOF      number: number of readed word. 
        nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
        printf("Received string: %s", readbuffer);
    }

    return(0);
}