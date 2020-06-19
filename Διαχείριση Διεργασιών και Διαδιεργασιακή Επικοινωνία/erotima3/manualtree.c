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


#define SLEEP_SEC 10
#define SLEEP_TREE_SEC 3

int signalok=0;

void sighandler(){
        signalok=1;
}

void manual_forks(struct tree_node *ptr)
{
        int i =0;
        int status;
        pid_t pid;
        pid_t childpid[ptr->nr_children];

        change_pname(ptr -> name);

        while (i< ptr -> nr_children) //generate children
        {
                childpid[i]=fork();
                if (childpid[i]==0)
                {
                        change_pname((ptr->children+ i) ->name);
                        if ((ptr -> children +i) -> nr_children ==0)   //generate leaf
                        {
                                signal(SIGCONT,sighandler);
                                kill(getpid(),SIGSTOP);
                                printf("%s\n", (ptr -> children+i) ->name);
                                exit (0);
                        }
                else {
                       manual_forks ( ptr-> children +i); //middle node
                }
                }
                i++;
        }

        signal(SIGCONT,sighandler);
        kill(getpid(),SIGSTOP);

        for (i=0; i<ptr -> nr_children; i++)
        {
                kill(childpid[i],SIGCONT);
                pid = wait(&status);
                explain_wait_status(pid, status);
        }


        printf("%s\n",ptr->name);
        exit(0);
}




int main (int argc, char **argv)

{
        struct tree_node *root;
        int status;
        pid_t pid;

        if (argc != 2)
        {
                fprintf (stderr, " Please enter a file to generate its tree \n");
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
                manual_forks(root);
                exit(0);
        }

        sleep (SLEEP_TREE_SEC);
        show_pstree(pid);

        kill(pid,SIGCONT);


        pid = wait(&status);
        explain_wait_status(pid, status);
        return 0;
}
