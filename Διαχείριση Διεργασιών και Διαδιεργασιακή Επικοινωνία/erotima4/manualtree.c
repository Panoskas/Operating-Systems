#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>


#include "proc-common.h"
#include "tree.h"


#define SLEEP_SEC 1
#define SLEEP_TREE_SEC 0.5


void manual_forks(struct tree_node *ptr,int pd)
{
        int i =0;
        int status,result,*number,temp;
        pid_t pid;

        change_pname(ptr -> name);
        int fd[2*ptr->nr_children];

        while (i< ptr-> nr_children) //generate children
        {
                if(pipe(fd + 2*i)<0){
                        perror("pipe creation");
                        printf("create a new pipe");
                        exit(1);
                }

                if (!fork())
                {
                        change_pname((ptr->children+ i) ->name);
                        if ((ptr -> children +i) -> nr_children ==0)   //generate leaf
                        {
                                sleep(SLEEP_SEC);

                                int num=atoi((ptr->children +i)->name);
                                close(fd[2*i]);
                                if(write(fd[2*i+1],&num,sizeof(int)) != sizeof(int)){
                                        perror("write to pipe");
                                }

                                exit (0);
                        }
                        else{
                                manual_forks(ptr->children +i,fd[2*i +1]);
                        }
                }

                i++;
        }


        number=(int * ) malloc(ptr->nr_children *sizeof(int));
                
        for(i=0;i<ptr->nr_children;i++){
                if(read(fd[2*i],&temp,sizeof(int)) != sizeof(int)){
                        perror("read from pipe");
                }

                *(number +i )=temp;
        }



        if(strcmp(ptr->name, "+")==0){
                result= *(number+1) + *number;
        }
        else{
                result = *(number+1) * *number ;
        }

        if(write(pd,&result,sizeof(int)) != sizeof(int)){
                perror("write to pipe");
        }

        for (i=0; i<ptr -> nr_children; i++)
        {
                pid = wait(&status);
                explain_wait_status(pid, status);
        }
        exit(0);
}




int main (int argc, char **argv)

{
        struct tree_node *root;
        int status, fd[2], result;
        id_t pid;

        if (argc != 2)
        {
                fprintf (stderr, " Please enter a file to generate its tree \n");
                exit(1);
        }

        if(pipe(fd)<0){
        perror("Initial pipe");
        exit(1);
        }


        if (open(argv[1], O_RDONLY) < 0)
        {
                perror (" Something went wrong");
        }

        root = get_tree_from_file(argv[1]);

        // creation of 1st child

        pid = fork();
        if (pid < 0)
        {
                perror(" Something went wrong");
                exit (2);
        }
        if ( pid == 0)
        {
                close(fd[0]);
                manual_forks(root,fd[1]);
                exit(0);
        }

        sleep (SLEEP_TREE_SEC);
        show_pstree(pid);

        close(fd[1]);
        if(read(fd[0],&result,sizeof(int)) != sizeof(int)){
                perror("read from pipe");
        }

        pid = wait(&status);
        explain_wait_status(pid, status);
        printf("result= %d\n",result);
        return 0;
}

                                                