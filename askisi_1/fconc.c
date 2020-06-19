#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int main(int argc, char **argv){
        int fd1,fd2,fdout;
        char *outputfile ;
        ssize_t length ;
        char buff[1024];

        if ((argc<3) || (argc>4)){
                printf("Usage ./fconc infile1 infile2 [outfile (default:fconc.out)]");
                return 1 ;
        }

        fd1=open(argv[1],O_RDONLY);
        if(fd1<0){
                printf(" No such file or directory\n");
                return 1;
        }

        fd2=open(argv[2],O_RDONLY);
        if(fd2<0){
                printf("No such file or directory\n");
                return 1;
        }

        if(argc==4){
                outputfile=argv[3] ;
        }
        else{
                outputfile="fconc.out";
        }

        fdout=open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR ) ;

        if(fdout<0){
                printf("Can't open output file !\n") ;
        }

        read(fd1,buff,sizeof(buff)-1);
        length=strlen(buff);
        write(fdout,buff,length);

        memset(buff,0,sizeof(buff));

        read(fd2,buff,sizeof(buff)-1);
        length=strlen(buff);
        write(fdout,buff,length);

        close(fd1);
        close(fd2);
        close(fdout);

        return 0;

}
