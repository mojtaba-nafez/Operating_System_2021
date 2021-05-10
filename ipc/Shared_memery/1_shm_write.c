#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define BUF_SIZE 1024
#define SHM_KEY 0x1234

struct shmseg {
   int cnt;
   int complete;
   char buf[BUF_SIZE];
};

int fill_buffer(char * bufptr, int size);

int main(int argc, char *argv[]) {
   int shmid, numtimes;
   char *bufptr;
   int spaceavailable;
   /*
   allocates a System Virtual shared memory segment
      int shmget(key_t key, size_t size, int shmflg)
         input:
            key: segment identifier :    number or IPC_Private(server+ cliet must child)
            size: shared segment size (in byte)
            shmflag: permission (read:S_IRUSR, write:S_IWUSR , both)
         output:
            shared memory ID
   */
   shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT); // 0644: linux permission (rwx)

   if (shmid == -1) {
      perror("Shared memory");
      return 1;
   }
   /*
   shmat: Attach to the segment to get a pointer to it.   
      void *shmat(int shmid, const void *shmaddr, int shmflg);
         input:
            shmaddr: The attaching address (NuLL: OS choose adrees)
            shflag: user process permission (read or write => 0: assign both RW)
         output:
            pointer to start location of attached the segment
   */
   struct shmseg *shmp;
   shmp =  shmat(shmid, NULL, 0);
   
   ///////////////////////////////////////////////
   if (shmp == (void *) -1) {
      perror("Shared memory attach");
      return 1;
   }
   
   /* Transfer blocks of data from buffer to shared memory */
   bufptr = shmp->buf;
   spaceavailable = BUF_SIZE;
   for (numtimes = 0; numtimes < 5; numtimes++) {
      shmp->cnt = fill_buffer(bufptr, spaceavailable);
      shmp->complete = 0;
      printf("Writing Process: Shared Memory Write: Wrote %d bytes\n", shmp->cnt);
      bufptr = shmp->buf;
      spaceavailable = BUF_SIZE;
      sleep(3);
   }
   printf("Writing Process: Wrote %d times\n", numtimes);
   shmp->complete = 1;

   /*
   detaches the shared memory segment located at the
   ad‐dress specified by shmaddr from the address space
   of the call‐ing process.
   */
   if (shmdt(shmp) == -1) {
      perror("shmdt");
      return 1;
   }
   /*
   shmctl: control operation for a System V shared memory segment.
      IPC_RMID  Mark  the segment to be destroyed.
   */
   if (shmctl(shmid, IPC_RMID, 0) == -1) {
      perror("shmctl");
      return 1;
   }
   printf("Writing Process: Complete\n");
   return 0;
}

int fill_buffer(char * bufptr, int size) {
   static char ch = 'A';
   int filled_count;   
   // It fills the (size -1) blocks of *bufptr with ch.
   memset(bufptr, ch, size - 1);
   // buf=AAAAAAAAAAA
   bufptr[size-1] = '\0';
   // 65-90:A-Z      97-122:a-z
   
   filled_count = strlen(bufptr);
   ch++;


   /// buf:AAAAAAAAAAAA, output: len(AAAAAAAAAAAA)
   return filled_count;
}