#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<stdbool.h>
#include<unistd.h>
#include<windows.h>
#include<stdlib.h>

#define MAX_FILENAME_LEN 1000

void print_command(int argc,char * argv[]){
    for(int i = 1; i < argc;i++){
        fprintf(stdout, "%s", argv[i]);
    }
    fprintf(stdout, "\n");
}

void make_config(char * username,char * email){
    
}

int make_hidden_dir(char * cwd,char * dir_name){
    char path[MAX_FILENAME_LEN];
    strcpy(path,cwd);
    strcat(path,"\\");
    strcat(path,dir_name);
    char command[MAX_FILENAME_LEN]="attrib +h ";
    strcat(command,path);
    if(system(command) == 0)
        return 1;
    return 0;
}
//bool function to check if a ".neogit" directory exsist in our working directory or it's parent directories
bool check_for_repo(char * cwd){
    char temp_dir[MAX_FILENAME_LEN];//to have the current directory(when we are moving in parent directories)
    char root_dir[4];//to have the root directory that we are in it
    strncpy(root_dir,cwd,3);//getting the root from the working directory path
    root_dir[3]='\0';
    bool exists= false;//a boolean variable to check the existance of ".neogit" 
    struct dirent * entry;//moves on files of our directory(from dirent.h library)
    do {
        DIR * dir = opendir(" . ");//opening current  directory
        if(dir == NULL)
            return 1;
        while((entry = readdir(dir)) != NULL){
            //now we check the condition that we already have ".neogit" in our directory
            //DT_DIR:directory type,here we use it to only check the directories(and not files) 
            if(entry->d_type == DT_DIR && !strcmp(entry->d_name,".neogit")){
                exists=true;  
            }
        }
        closedir(dir);    
        if(getcwd(temp_dir,sizeof(temp_dir)) == NULL)
            return 1;
        if(strcmp(temp_dir,root_dir) != 0) {
            if(chdir("..") != 0)
                return 1;
        }
           
    }while(strcmp(temp_dir,root_dir) != 0); //check the conditions till we get to the root
    if(chdir(cwd) != 0)//going back to our working directory
        return 1;
    return exists;
}
//function to respond on neogit init command
int run_init(int argc,char * argv[]){
    char cwd [MAX_FILENAME_LEN];//to have the path to our working directory
    if(getcwd(cwd,sizeof(cwd)) == NULL)
        return 1;
    //now we check the existance of ".neogit" in our repo
    bool exists=check_for_repo(cwd);
    if(exists){
        perror("neogit repository already exsists");
    }
    else{
        if(mkdir(".neogit") != 0)//making the ".neogit" in out working directory
            return 1;
        if(make_hidden_dir(cwd,".neogit") != 0)
            return 1;
    }
    return 0;
    
}

int main(int argc, char *argv[]) {
    if(argc<2){
        perror("Please input a valid command:");
        return 1;//to show that program failed
    }
    //now we start to check which command is entered
    else if(!strcmp(argv[1], "init")){
        run_init(argc,argv);
    }
    else if(!strcmp(argv[1], "config")){
        fprintf(stdout,"config command has been inputed!\n");
    }
    else if(!strcmp(argv[1], "add")){
        fprintf(stdout,"add command has been inputed!\n");
    }
    else if(!strcmp(argv[1], "status")){
        fprintf(stdout,"status command has been inputed!\n");
    }
    else if(!strcmp(argv[1], "commit")){
        fprintf(stdout,"commit command has been inputed!\n");
    }
    else{
        fprintf(stdout,"Not a valid command!\n");
    }
    return 0;
}
