// Pratik Desai Shell
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>


// Flags 
int run_flag = 1;
int wait_flag = 1;




void redirection_input(char *fname){
   // close(0);
    //printf("In r_i");
    int fd = open(fname, O_RDONLY);
    dup2(fd, 0);
    close(fd);
    
}

void redirection_output(char *fname){
    int fd1 = open(fname, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    dup2(fd1, 1);
    close(fd1);
}

void execute(char *args[]){
    pid_t pid;

    if(strcmp(args[0], "exit")!= 0){
        pid = fork();
        //If child process
        if( pid < 0){
            fprintf(stderr, "Fork didn't happen");
        }
        else if(pid == 0){
            execvp(args[0], args);
        }else{
            //parent process
            if(wait_flag){
                //printf("In wait\n");
                waitpid(pid, NULL, 0);
            }else{
                wait_flag = 0;
            }

        }
        redirection_input("/dev/tty");
        redirection_output("/dev/tty");
    }
    else{
        run_flag = 0;
    }
}

void c_pipe(char *args[])
{
    int fd[2];
    pipe(fd);

    dup2(fd[1], 1);
    close(fd[1]);

    //printf("args = %s\n", *args);

    execute(args);

    dup2(fd[0], 0);
    close(fd[0]);
}

char *get_tokens(char *input){
    int i;
    int j = 0;
    char *tokenized = (char *)malloc((1024* 2) * sizeof(char));

    // add spaces around special characters
    for (i = 0; i < strlen(input); i++) {
        if (input[i] != '>' && input[i] != '<' && input[i] != '|') {
            tokenized[j++] = input[i];
        } else {
            tokenized[j++] = ' ';
            tokenized[j++] = input[i];
            tokenized[j++] = ' ';
        }
    }
    tokenized[j++] = '\0';

    // add null to the end
    char *end;
    end = tokenized + strlen(tokenized) - 1;
    end--;
    *(end + 1) = '\0';

    return tokenized;
}

int main(void){

    // Command line arguments to be stored in this array
    char *args[1024];

    while(run_flag){
        printf("Pratik$ ");
        fflush(stdout);


        // Array to hold input line
        char input[1024];
        //int n = read(0,input,1024);
        fgets(input, 1024, stdin);

        /*

        for(int i = 0 ; i < n; i++){
            printf("%c", input[i]);
        }*/

        // To tokenize the input commands
        char *token;
        token = get_tokens(input);

        if (token[strlen(token) - 1] == '&') {
            wait_flag = 0;
            token[strlen(token) - 1] = '\0';
        }

        char *arguments = strtok(token," ");
        int i = 0;

        while(arguments){
            //printf("%c", *arguments);

            if(*arguments == '<'){
                redirection_input(strtok(NULL," "));
            }else if(*arguments == '>'){
                redirection_output(strtok(NULL, " "));
            }else if(*arguments == '|'){
                args[i] = NULL;
                c_pipe(args);
                i = 0;
                
            }else{
                args[i] = arguments;
                i++;

            }
            

            arguments = strtok(NULL, " ");
        }
        args[i] = NULL;

        execute(args);

        //fgets(input, 1024, stdin);

    }

    return 0;
}
