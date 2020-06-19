#include <stdio.h>
#include <unistd.h>

void zing(){
        char *name;
        name = getlogin();
        printf("Hello from our team %s \n",name );

}
