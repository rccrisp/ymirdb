#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char ** argv){
    FILE * fp;
    FILE * fp_in;
    FILE * fp_out;

    
    char * in = ".in";
    char * out = ".out";

    fp = fopen("given_test.txt", "r");

    char filename[100];
    memset(filename,'\0',sizeof(argv[1]));
    strcpy(filename,argv[1]);
    char * test_in = strncat(filename, in, sizeof(in));
    fp_in = fopen(test_in, "w");
    strcpy(filename,argv[1]);
    char * test_out = strncat(filename, out, sizeof(out));
    fp_out = fopen(test_out, "w");

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    bool command = false;

    while((read = getline(&line, &len, fp))!= -1){
        if(line[0] == '>'){
            command = true;
            fputs(&line[2],fp_in);
        }else{
            if(command){
                command = false;
                fputs("> ",fp_out);
            }
            fputs(line,fp_out);
        }
    }

    fclose(fp);
    fclose(fp_in);
    fclose(fp_out);
    if(line){
        free(line);
    }
}