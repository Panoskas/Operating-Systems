#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

#include <sys/wait.h>
#include <sys/types.h>

#include "proc-common.h"
#include "request.h"

/* Compile-time parameters. */
#define SCHED_TQ_SEC 2                /* time quantum */
#define TASK_NAME_SZ 60               /* maximum size for a task's name */

struct process {
        pid_t p;
        struct process*next;
        char *name;
        int data;};
struct process *head = NULL;
struct process *newnode = NULL;
struct process *last = NULL;


/*
 * SIGALRM handler
 */
static void
sigalrm_handler(int signum)
{
        printf (" Scheduler : going to stop process [id]: %d\n", head->p);
        kill(head->p, SIGSTOP);
}

/* 
 * SIGCHLD handler
 */
static void
sigchld_handler(int signum)
{
        pid_t pid;
        int status;
        for (;;)
        {
                pid = waitpid(-1, &status, WUNTRACED | WNOHANG);
                if (pid<0)
                {
                        perror("waitpid");
                        exit(1);
                }
                if (pid == 0)
                        break;
                explain_wait_status(pid, status); //p has the pid of the child that has changed its status

                if (WIFEXITED(status) || WIFSIGNALED(status)) //if terminated or killed by a signal remove it from list
                        //a child has died
                {
                        fprintf(stderr," Child is dead through SIGCHLD. Exiting... \n");
                        struct process* delete = NULL;
                        struct process*temp;
                        temp = head;
                        while (temp != NULL)
                        {
                                if (temp ->p == pid && temp==head) //if the head of the list is deleted
                                                {
                                                        if (temp ->next ==NULL)
                                                                        {
                                                                                free(temp);
                                                                                printf ("Child is dead. Exiting... \n");
                                                                                exit(0);
                                                                        }
                                                        else
                                                        {
                                                                head = temp -> next;
                                                                free(temp);
                                                        }
                                                }
                                else if (temp->p == pid && temp==last) // if the last element of the list is deleted
                                {
                                        last= delete;
                                        last->next = NULL;
                                        free(temp);
                                }
                                else if (temp -> p == pid) // if i delete a random element in the list
                                {
                                        delete->next=temp->next;
                                        free(temp);
                                }
                                else //if i dont enter any node after last element
                                {
                                        delete=temp;
                                        temp=temp->next;
                                        continue;
                                }
                                break;
                        }
                }
                if (WIFSTOPPED(status))
                        //a child has stopped due to SIGSTOP/SIGTSTP (scheduler), etc
                {
                        fprintf (stderr,"Child has been stopped, moving to the next one... \n");
                        last -> next = head;
                        last = head;
                        struct process *temp;
                        temp = head;
                        head = head->next;
                        temp->next = NULL;
                }
                alarm (SCHED_TQ_SEC);
                kill(head->p,SIGCONT);
        }
}

/* Install two signal handlers.
 * One for SIGCHLD, one for SIGALRM.
 * Make sure both signals are masked when one of them is running.
 */
static void
install_signal_handlers(void)
{
        sigset_t sigset;
        struct sigaction sa;
        sa.sa_handler = sigchld_handler;
        sa.sa_flags = SA_RESTART;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGCHLD);
        sigaddset(&sigset, SIGALRM);
        sa.sa_mask = sigset;
        if (sigaction(SIGCHLD, &sa, NULL) < 0) {
                perror("sigaction: sigchld");
                exit(1);
        }

        sa.sa_handler = sigalrm_handler;
        if (sigaction(SIGALRM, &sa, NULL) < 0) {
                perror("sigaction: sigalrm");
                exit(1);
        }

        /*
         * Ignore SIGPIPE, so that write()s to pipes
         * with no reader do not result in us being killed,
         * and write() returns EPIPE instead.
         */
        if (signal(SIGPIPE, SIG_IGN) < 0) {
                perror("signal: sigpipe");
                exit(1);
        }
}

int main(int argc, char *argv[])
{
        int nproc;
        /*
         * For each of argv[1] to argv[argc - 1],
         * create a new child process, add it to the process list.
         */

        char executable[] = "prog";
        char *newargv[] = {executable, NULL, NULL, NULL};
        char *newenviron[] = {NULL};
        nproc = argc - 1; /* number of proccesses goes here */



        pid_t mypid;
        int i;
        for (i=0; i<nproc; i++)
        {
                mypid = fork();
                if (mypid < 0)
                {
                        perror("Error with fork \n");
                        exit(1);
                }
                if (mypid == 0)
                {
                        raise (SIGSTOP);
                        printf ("i am %s with PID: %ld \n", argv[0], (long)getpid());
                        printf("About to replace myself with the executable %s \n",executable);
                        sleep(2);

                        execve(executable,newargv,newenviron);

                        perror("execve");
                        exit(1);
                }
                else
                {
                        if (i==0)
                        {
                        {
                                head=(struct process*)malloc(sizeof(struct process));
                                if (head == NULL) printf ("Something went wrong with malloc \n");
                                head -> p=mypid;
                                head ->next = NULL;
                                head-> data=i+1;
                                head->name=argv[i+1];
                                last =head;
                        }
                        else
                        {
                                newnode=(struct process*)malloc(sizeof(struct process));
                                if (newnode == NULL) printf ("something went wrong with malloc \n");
                                newnode ->p=mypid;
                                newnode->next = NULL;
                                newnode->data= i+1;
                                newnode -> name= argv[i+1];
                                last -> next =newnode;
                                last=newnode;
                        }
                }
        }

        /* Wait for all children to raise SIGSTOP before exec()ing. */
        wait_for_ready_children(nproc);

        /* Install SIGALRM and SIGCHLD handlers. */
        install_signal_handlers();

        if (nproc == 0) {
                fprintf(stderr, "Scheduler: No tasks. Exiting...\n");
                exit(1);
        }
        //set the alarm on
        alarm(SCHED_TQ_SEC);

        //start the first process
        kill(head->p,SIGCONT);


        /* loop forever  until we exit from inside a signal handler. */
        while (pause())
                ;

        /* Unreachable */
        fprintf(stderr, "Internal error: Reached unreachable point\n");
        return 1;
}
                                                              