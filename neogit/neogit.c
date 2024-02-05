//neogit project-farbod fattahi-student no:402106231
#include<stdio.h>
#include<string.h>
#include<dirent.h>//to manage directories
#include<stdbool.h>//to use bool variables
#include<unistd.h>//to use getcwd
#include<stdlib.h>
#include<windows.h>

#define MAX_FILENAME_LEN 1000

//prototypes
void backspace(char *,int);
int check_global_id(char *,char *);
int make_hidden_dir(char *,char*);
bool check_for_repo(char *);
int run_init(int ,char ** );
int run_config(int ,char ** );
int make_config(char [],char []);
int run_add(int ,char **);
int add_to_staging(char *);
int run_branch(int ,char **);
int add_sev_to_staging(int ,char **);
int run_reset(int ,char **);
int remove_from_staging(char *);
int remove_sev_from_staging(int ,char **);

void print_command(int argc,char * argv[]){
    for(int i = 1; i < argc;i++){
        fprintf(stdout, "%s", argv[i]);
    }
    fprintf(stdout, "\n");
}

//function to remove the first n characters of a string
void backspace(char * str,int n){
    int len=strlen(str);
    for(int i=0;i<len;i++){
        str[i]=str[i+n];
    }
    str[len-1]='\0';
}

//function to check for global infos
int check_global_id(char username[],char email[]) {
    char * path_to_glob="C:\\neogit\\global.txt";
    FILE * file = fopen(path_to_glob,"r");
    if(file == NULL)
        return 1;
    char line[MAX_FILENAME_LEN];
    fgets(username,MAX_FILENAME_LEN,file);
    fgets(email,MAX_FILENAME_LEN,file);
    backspace(username,9);
    username[strlen(username)-1]='\0';
    backspace(email,6);
    email[strlen(email)-1]='\0';
    fclose(file);
    return 0;
}

//function to make config.txt in ".neogit"
int make_config(char * username,char * email){
    FILE * configs = fopen(".neogit\\config.txt","w");
    if(configs == NULL){
        printf("error");
        return 1;}
    fprintf(configs,"username:%s\n",username);
    fprintf(configs,"email:%s\n",email);
    fclose(configs);
    return 0;
}

//function to make an exsisting directory hidden
int make_hidden_dir(char * cwd,char * dir_name){
    char path[MAX_FILENAME_LEN];
    strcpy(path,cwd);
    strcat(path,"\\");
    strcat(path,dir_name);
    char command[MAX_FILENAME_LEN]="attrib +h ";
    strcat(command,path);
    if(system(command) != 0)
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
            return 0;
        while((entry = readdir(dir)) != NULL){
            //now we check the condition that we already have ".neogit" in our directory
            //DT_DIR:directory type,here we use it to only check the directories(and not files) 
            if(entry->d_type == DT_DIR && !strcmp(entry->d_name,".neogit")){
                exists=true;  
            }
        }
        closedir(dir);    
        if(getcwd(temp_dir,sizeof(temp_dir)) == NULL)
            return 0;
        if(strcmp(temp_dir,root_dir) != 0) {
            if(chdir("..") != 0)
                return 0;
        }
           
    }while(strcmp(temp_dir,root_dir) != 0); //check the conditions till we get to the root
    if(chdir(cwd) != 0)//going back to our working directory
        return 0;
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
        if(mkdir(".neogit\\staging") != 0)
            return 1;
        char username[MAX_FILENAME_LEN];
        char email[MAX_FILENAME_LEN];
        check_global_id(username,email);
        make_config(username,email);
        FILE * cur_branch = fopen(".neogit\\cur_branch.txt","w");
        fprintf(cur_branch,"%s\n","master");
        fclose(cur_branch);
        FILE * branchs = fopen(".neogit\\all_branchs.txt","w");
        fprintf(branchs,"%s\n","master");
        fclose(branchs);
    }
    return 0;
    
}

//function for the config command
int run_config(int argc,char * argv[]) {
    if(!strcmp(argv[2],"-global")) {
        if(!strcmp(argv[3],"user.name")){
            FILE * file=fopen("C:\\neogit\\global.txt","w");
            fprintf(file,"username:%s\n",argv[4]);
            FILE * local=fopen(".neogit\\config.txt","w");
            if(local == NULL)
                return 1;
            fprintf(local,"username:%s\n",argv[4]);
        }
        else if(!strcmp(argv[3],"user.email")){
            FILE * file=fopen("C:\\neogit\\global.txt","a");
            fprintf(file,"email:%s\n",argv[4]);
            FILE * local=fopen(".neogit\\config.txt","w");
            if(local == NULL)
                return 1;
            fprintf(local,"email:%s\n",argv[4]);
        }
        else if(!strcmp(argv[3],"alias.")){
            FILE * alias_glob=fopen("C:\\neogit\\alias.txt","a");
            fprintf(alias_glob,"neogit %s %s\n",argv[4],argv[5]);
            fclose(alias_glob);
        }
        else
            perror("invalid command");
    }
    else{
        if(!strcmp(argv[2],"user.name")){
            FILE * file= fopen(".neogit\\config.txt","w");
            fprintf(file,"username:%s\n",argv[3]);
        }
        else if(!strcmp(argv[2],"user.email")){
            FILE * file = fopen(".neogit\\config.txt","a");
            fprintf(file,"email:%s\n",argv[3]);
        }
        else if(!strcmp(argv[2],"alias.")){
            FILE * alias_loc=fopen(".neogit\\alias.txt","a");
            fprintf(alias_loc,"neogit %s %s\n",argv[3],argv[4]);
            fclose(alias_loc);
        }
        else
            perror("Invalid command");
    }
}

//function for add command
int run_add(int argc,char * argv[]){
    if(argc < 3){
        perror("no file is chosen");
        return 1;
    }
    if(!strcmp(argv[2],"-f")){
        return add_sev_to_staging(argc,argv);
    }
    else if(!strcmp(argv[2],"-n")){
        // show all files in root directory and check if they're staged
        struct dirent *root;
        struct dirent *staged;
        DIR *dir = opendir(".");
        DIR *staging = opendir(".neogit\\staging");

        if (dir == NULL || staging == NULL)
            return 1;

        while ((root = readdir(dir)) != NULL) {
            int exists = 0;
            // ignore "." and ".." and ".neogit"
            if (!strcmp(root->d_name, ".") || !strcmp(root->d_name, "..") || !strcmp(root->d_name, ".neogit"))
                continue;

            rewinddir(staging); // Reset the directory pointer to the beginning of the stream

            while ((staged = readdir(staging)) != NULL) {
                if (!strcmp(root->d_name, staged->d_name)) {
                    exists = 1;
                    break;
                }
            }

            fprintf(stdout, "%s - %s\n", root->d_name, exists ? "Staged" : "Not Staged");
        }

        closedir(dir);
        closedir(staging);
    }
    else
        return add_to_staging(argv[2]);
}

//function to add file to staging area
int add_to_staging(char * pathtofile){
    char command[MAX_FILENAME_LEN];
    char cwd[MAX_FILENAME_LEN];
    getcwd(cwd,sizeof(cwd));
    strcat(cwd,"\\");
    strcat(cwd,pathtofile);
    sprintf(command,"copy %s %s",cwd,".neogit\\staging");
    if(system(command) != 0)
        fprintf(stdout,"File not found");
    else {
        fprintf(stdout,"File %s added succesfully\n",pathtofile);
        FILE * file = fopen (".neogit\\lastadd.txt","w");
        fprintf(file,"%s\n",pathtofile);  
        fclose(file); 
    }
    return 0;
}

//function to add several files to staging area
int add_sev_to_staging(int argc,char * argv[]){
    FILE * file=fopen(".neogit\\lastadd.txt","w");
    fclose(file);
    for(int i=3;i<argc;i++){
        char command[MAX_FILENAME_LEN];
        char cwd[MAX_FILENAME_LEN];
        getcwd(cwd,sizeof(cwd));
        strcat(cwd,"\\");
        strcat(cwd,argv[i]);
        sprintf(command,"copy %s %s",cwd,".neogit\\staging");
        if(system(command) != 0){
            fprintf(stdout,"File %s not found\n",argv[i]);
        }
        else{
            fprintf(stdout,"File %s added succesfully\n",argv[i]);
            file = fopen (".neogit\\lastadd.txt","a");
            fprintf(file,"%s\n",argv[i]);  
            fclose(file);
        }   
    }
    return 0;
}

//function for branch command
int run_branch(int argc,char * argv[]){
    if(argc == 2){
        FILE * file = fopen(".neogit\\all_branchs.txt","r");
        char line[MAX_FILENAME_LEN];
        while(fgets(line,sizeof(line),file) != NULL){
            fprintf(stdout,"%s",line);
        }
        fclose(file);
    }
    else if (argc == 3){
        FILE * all = fopen(".neogit\\all_branchs.txt","a+");
        char line[MAX_FILENAME_LEN];
        while(fgets(line,sizeof(line),all) != NULL){
            if(strcmp(argv[2],line) == 0){
                fprintf(stdout,"This branch already exists!");
                return 1;
            }
        }
        fprintf(all,"%s\n",argv[2]);
        fclose(all);
        FILE * cur = fopen(".neogit\\cur_branch.txt","w");
        fprintf(cur,"%s",argv[2]);
        fclose(cur);
    }
    else 
        fprintf(stdout,"Invalid command");
}

//function for reset command
int run_reset(int argc,char * argv[]){
    if(argc < 3)
        fprintf(stdout,"No file is chosen!");
    else if (!strcmp(argv[2],"-undo")){
        //remove the last add
        FILE * add=fopen(".neogit\\lastadd.txt","r");
        char file[MAX_FILENAME_LEN];
        while(fgets(file,sizeof(file),add) != NULL){
            int len=strlen(file);
            file[len-1]='\0';
            remove_from_staging(file);
        }
        fclose(add);
    }
    else if(!strcmp(argv[2],"-f")){
        return remove_sev_from_staging(argc,argv);
    }
    else{
        return remove_from_staging(argv[2]);
    }
}

//function to remove file from staging area
int remove_from_staging(char * pathtofile) {
    char command[MAX_FILENAME_LEN];
    sprintf(command,"del .neogit\\staging\\%s",pathtofile);
    if(system(command) != 0) {
        return 1;
    }
    return 0;
}

//function to remove several files
int remove_sev_from_staging (int argc,char * argv[]){
    for(int i=3;i<argc;i++){
        char command[MAX_FILENAME_LEN];
        sprintf(command,"del .neogit\\staging\\%s" , argv[i]);
        if(system(command) != 0) {
            return 1;
        }   
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
        run_config(argc,argv);
    }
    else if(!strcmp(argv[1], "add")){
        run_add(argc,argv);
    }
    else if(!strcmp(argv[1], "status")){
        fprintf(stdout,"status command has been inputed!\n");
    }
    else if(!strcmp(argv[1], "commit")){
        fprintf(stdout,"commit command has been inputed!\n");
    }
    else if(!strcmp(argv[1], "reset")){
        run_reset(argc,argv); 
    }
    else if(!strcmp(argv[1], "log")){
        fprintf(stdout,"log command has been inputed!\n");   
    }
    else if(!strcmp(argv[1], "branch")){
        run_branch(argc,argv);  
    }
    else if(!strcmp(argv[1], "checkout")){
        fprintf(stdout,"checkout command has been inputed!\n");   
    }
    else{
        fprintf(stdout,"Not a valid command!\n");
    }
    return 0;
}
