#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "proc-common.h"


#define SLEEP_SEC 10
#define SLEEP_TREE_SEC 3
void fork_procs(void){

        pid_t B,C,D,p;
        int status;
        fprintf(stderr,"A will create B \n");

        B = fork();
        if (B<0){
                perror("cant fork B");
                exit(1);
        }
        else if (B==0){
                fprintf(stderr,"B will create D \n ");
                D = fork();
                //D child
                if (D<0){
                        perror("cant fork D \n");
                        exit(1);
                }
                else if (D==0){
                        change_pname("D\n");
                        fprintf (stderr,"D is sleeping \n");
                        sleep (SLEEP_TREE_SEC);
                        fprintf(stderr,"exiting D \n");
                        exit(13);
                }
                else
                //B child
                {
                change_pname("B \n");
                fprintf(stderr,"B is sleeping \n");
                sleep (SLEEP_TREE_SEC);
                fprintf(stderr, "exiting B \n");
                exit(19);
                }
        }
                else
        {
        //C child
        fprintf(stderr,"A will create C \n");
        C = fork();
        if (C<0){
                perror("cant fork C \n");
                exit(1);
        }
        else if (C==0){
                change_pname("C\n");
                fprintf(stderr,"C is sleeping \n");
                sleep(SLEEP_TREE_SEC);
                fprintf (stderr, "exiting C \n");
                exit(17);
        }

        //A father
        else {
                change_pname("A\n");
                p=wait(&status);
                explain_wait_status(p, status);
                p = wait(&status);
                explain_wait_status(p, status);
                sleep(SLEEP_TREE_SEC);
                fprintf(stderr,"exiting A \n");
                exit(16);
        }
        }




}


int main(void){

        int status;
        pid_t p;
        p=fork();

        //fork failed
        if (p<0){
                perror("fork");
                exit(1);
        }

        //when u enter "child state" call fork _procs
        if (p==0){
                fork_procs();

                exit(1);
        }

        show_pstree(p);


        sleep(SLEEP_SEC);
        p = wait(&status);
        explain_wait_status(p,status); //prints helpfull message depending on status
        return 0;
}
