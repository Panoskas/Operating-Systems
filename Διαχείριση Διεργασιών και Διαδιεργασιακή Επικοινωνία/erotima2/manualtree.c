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


void manual_forks(struct tree_node *ptr)
{
        int i =0;
        int status;
        pid_t pid;

        change_pname(ptr -> name);

        while (i< ptr -> nr_children) //generate children
        {
                printf (" Starting from %s and i will generate %d children \n", ptr->name, ptr->nr_children-i);
                if (!fork())
                {
                        change_pname((ptr->children+ i) ->name);
                        if ((ptr -> children +i) -> nr_children ==0)   //generate leaf
                        {
                                printf(" Starting from %s and i will sleep for now \n", (ptr -> children +i) -> name);
                                sleep(SLEEP_TREE_SEC);
                                printf(" Child %s and exiting now\n", (ptr -> children+i) ->name);
                                exit (0);
                        }
                else {
                       manual_forks ( ptr-> children +i); //middle node
                }
                }
                i++;
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
        int status;
        id_t pid;

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

        sleep (SLEEP_SEC);
        show_pstree(pid);

        pid = wait(&status);
        explain_wait_status(pid, status);
        return 0;
}
