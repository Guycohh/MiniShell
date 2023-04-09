//ex3
//This program it's a simple program based in a simulator of a program shell to get commands from the user.
//The program will represent every command like a real command of the shell.
//Guy Cohen 206463606
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#define INPUT_SIZE 512
#include <sys/types.h>
char** countWordsAndChars(char str []);// count words and chars for each input and separate it into array
void askForInput();// ask for input from the use, with while loop
char** history(char arr[] , char ** str_arr);// this function responsible to give the right order from the history file
void printHistory(char str []);
void freeFunc(char **str);// free allocated memory
void pipeCase1(char str [], int ind1);// going to take care of one pipe case.
void pipeCase2(char str [], int ind1, int ind2);// going to take care of two pipe case.
void ampersandCase(char str[] ,char** words);
void nohupCase(char str[] ,char** words);
void nohupAndAmpersandCase(char str[] ,char** words);
void handler(int num);

char input[INPUT_SIZE];// input from user, size of 512


int wCounter=0;// words counter
int cCounter=0;// chars counter
int totalWords=0;// total words per running
int numOfCommand=-1;
int pipeCounter;// how many pipes per command
int totalPipes=0;// total pipes per running
int index1=0;// first pipe index
int index2=0;// second pipe index
bool flagNohup =false;
pid_t process[INPUT_SIZE];
int pro=0;

int main() {
    memset(process, 0, INPUT_SIZE);
    char cwd[256];
    if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("get cwd error");
    }
    else{
        printf("%s>", cwd);// print pwd like a real shell
    }

    askForInput();
    return 0;
}

//this func is working with a loop, the loop is going to ask for an input, and while the input is differed from "done",
//it is continued ask for one.
//for each input, the son process is going to count words, chars and in addition to organize the words of it on array.
//The father process is going to wait till the son done.
void askForInput() {
    char cwd[256];
    if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("get cwd error");// print pwd like a real shell
    }

    pid_t x; //process
    char** arrOfWords;// This is going to be an array of words from input.
//    char input[INPUT_SIZE];// input from user, size of 512

    while (fgets(input, INPUT_SIZE, stdin)) { // while user give input
        totalWords += wCounter;// count how many words were writen till now
        input[strlen(input) - 1] = '\0';// "enter char"
        arrOfWords = countWordsAndChars(input);
        char help[INPUT_SIZE];

        int y=0;
        for( y=0; input[y]== ' '; y++){
            y++;
        }// where the input is really start


        int e=0 ;
        for( e=(int)strlen(input)-1; input[e]== ' ' ; ){
            e--;
        }// where the input is really end

        if (arrOfWords == NULL) {
            printf("%s>", cwd);
            continue;
        }

        else if((strstr(*arrOfWords, "nohup")!=0) && input[e]=='&'){
            flagNohup=true;
            freeFunc(arrOfWords);
            input[e]=0;
            nohupAndAmpersandCase(input ,arrOfWords);
            printf("%s>", cwd);
            flagNohup=false;
        }

        else if(strstr(*arrOfWords, "nohup")!=0){
            nohupCase(input, arrOfWords);
            printf("%s>", cwd);
            freeFunc(arrOfWords);
        }

        else if((strcmp(arrOfWords[wCounter-1], "&")== 0) || input[strlen(input)-1]=='&'){// case of ampersand command
            ampersandCase(input, arrOfWords);
            freeFunc(arrOfWords);
        }
        else if (pipeCounter == 1) {// one pipe case
                freeFunc(arrOfWords);
                pipeCase1(input, index1);
                printf("%s>", cwd);
                pipeCounter=0;
        }
        else if(pipeCounter == 2){// two pipe case
            freeFunc(arrOfWords);
            int t1=0;
            while(input[t1]==' '){// t1= the start of the input
                t1++;
            }
            pipeCase2(input, index1, index2);
            printf("%s>", cwd);
        }

        else {// regular command
            x = fork();
            if (x == 0) {// this is the son process
                if (execvp(arrOfWords[0], arrOfWords) == -1) {
                    freeFunc(arrOfWords);
                    perror("\nThe process failed!");
                    exit(1);
                }
                freeFunc(arrOfWords);
                exit(0);
            } else if (x < 0) {
                freeFunc(arrOfWords);
                perror("\nFork failed!\n");
                exit(1);
            } else {// this is the father process
                wait(NULL);
                printf("%s>", cwd);
                freeFunc(arrOfWords);
            }
        }
    }
    freeFunc(arrOfWords);
}

// This function is return an array of words and write to file the input to file.txt. And it is count word and chars for each input.
//In addition, it is responsible on some process, for example, it is find key words like:"history", "done", "cd".
char** countWordsAndChars(char str[]) {
    pipeCounter=0;
    int exclamationMark=0;// '!'
    char pipe='|';
    FILE *fp;// open file.
    fp = fopen("file.txt", "a+");// open file to a+
    if(fp == NULL)
    {
        printf("cannot open file");
        exit(1);
    }

    int y=0;
    while(str[y]==' '){
        y++;
    }
    if(y== strlen(str)){
        fclose(fp);
        return NULL;
    }

    wCounter = 0; // words counter
    cCounter = 0; // chars counter
    int i = 0;  // index in order to scan the user input
    int len = (int)strlen(str);
    //int start=0; // when is the first word.

    numOfCommand++;
    // index i is where the input is start
    // index len is the last char in our input
    int place=0;
    int temp=0;
    int j;
    char *word=(char*)malloc(sizeof (char )*INPUT_SIZE);// Going to hand words.
    char **str_array=(char**)malloc(sizeof(char*)*INPUT_SIZE);// array size of how many words you have.
    if(word==NULL){// malloc failed.
        fclose(fp);
        exit(1);
    }
    if(str_array==NULL){// malloc failed.
        fclose(fp);
        exit(1);
    }
    while (i<=len){
        if(str[i] != ' ' && str[i] != 0){
            wCounter++;
            j=i;
            while(str[i] != ' ' && str[i] != 0){
                cCounter++;
                i++;
            }

            str_array[place]=(char *) malloc((i-j)*sizeof(char));
            if(str_array[place]==NULL){// malloc failed.
                fclose(fp);
                exit(1);
            }
            while (j<i){// j is a start of word , j is the end of the same word
                word[temp]=str[j];// copy char by char.
                if(word[temp]==pipe){
                    if(pipeCounter==0){
                        index1=j;
                        pipeCounter++; // how many pipes we have.
                    }

                    else if(pipeCounter==1){
                        index2=j;
                        pipeCounter++; // how many pipes we have.
                    }
                    else{
                        perror("sorry,there is no support for more than 2 pipes");
                        return NULL;
                    }
                }
                if(word[temp]== '!'){
                    exclamationMark++;
                }
                temp++;
                j++;
            }
            word[i]='\0';
            temp=0;
            strcpy(str_array[place], word);// copy the word to the right place
            memset(word, 0, strlen(word));
            place++;
        }
        else{
            i++;
        }
    }
    free(word);
    word=NULL;
    str_array[wCounter]=NULL;// null is located in the right place

    if(strcmp(str_array[0], "cd")==0){//if our input is "cd"
        printf("Command not supported (YET)\n");
        for (int i = 0; i<=wCounter; i++) {
            free(str_array[i]);
        }
        free(str_array);
        str=NULL;
        fclose(fp);
        return NULL;
    }

    if(wCounter==1 &&strcmp(str_array[0], "done")==0 && str_array[1]==NULL){//if our input is "done"
        fprintf(fp, "\n%s", str); // write to file
//        printf("\nNum of commands: %d", numOfCommand);
//        printf("\nNumber of pipes: %d \n", totalPipes);
        printf("See you next time !\n");
        freeFunc(str_array);
        fclose(fp);
        for(int i=0; i< pro ; i++){
            kill(process[i], SIGKILL);
        }
        exit(1);// we want to end this program
    }

    if(strcmp(str_array[0], "history")==0 && str_array[1]==NULL){//if our input is "history"
        fprintf(fp, "\n%s", str); // write to file
        printHistory(str_array[0]);
        freeFunc(str_array);
        fclose(fp);
        return NULL;
    }

    if(**str_array== '!' && str_array[1]==NULL && pipeCounter==0 ){// in this case the program want to check if the user try to use a history command
        char arr[INPUT_SIZE];
        fclose(fp);
        return history(arr, str_array);
    }
    if(flagNohup==false){
        fprintf(fp, "\n%s", str); // write to file only process that might work
    }

    fclose(fp);
    return str_array;
}

//This function is going find the right order from the history.
char** history(char arr[] , char ** str_arr){
    FILE *fp;
    fp = fopen("file.txt", "r");// open file to read
    if(fp == NULL)// if open failed
    {
        printf("cannot open file");
        fclose(fp);
        exit(1);
    }
    strcpy(arr, str_arr[0] );
    int num = atoi(&arr[1]);// take the word and make it num
    if (num != 0) {
        int k = 0;
        while (k != num && fgets(arr, 512, fp)) {
            k++;
        }
        if (k < num) {
            printf("\nNOT IN. HISTORY\n");
            freeFunc(str_arr);
            fclose(fp);
            return NULL;
        }
        arr[strlen(arr) - 1] = '\0';
        strcpy(input, arr);
        freeFunc(str_arr);
        fclose(fp);

        return countWordsAndChars(arr);
    }
    fclose(fp);
    freeFunc(str_arr);
    return NULL;
}

//this func is print the history file.
void printHistory(char str []){
    int i=0; // in order to print the first place as "0.    ....."
    FILE *fp;
    fp = fopen("file.txt", "r"); // open file for read
    if(fp == NULL){
        fclose(fp);
        perror("cannot open this file\n");
        exit(1);
    }

    while (fgets(str, INPUT_SIZE, fp) != NULL) {
        printf("%d   %s", i+1, str);
        i++;
    }

    printf("\n");
    fclose(fp);
}
//free memory function
void freeFunc(char** str){// this function responsible on free memory.
    for (int i = 0; str[i]!=0; i++) {
        free(str[i]);
    }
    free(str);
    str=NULL;
}

//in case we have a command with 1 pipe
void pipeCase1(char str [], int ind1){
    int t=0;
    while(str[t]==0){
        t++;
    }

    char** arrOfWords_left;
    char** arrOfWords_right;

    char leftSide[INPUT_SIZE];
    char rightSide[INPUT_SIZE];// Going to hand words.
    memset(leftSide, '\0', INPUT_SIZE);
    memset(rightSide, '\0', INPUT_SIZE);

    char cwd[256];
    if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("get cwd error");
    }

    int i=0;
    while (i<ind1){
        leftSide[i]=str[i];// copy the word to the right place
        i++;
    }
    i = ind1+1;
    leftSide[i]=0;
    int j=0;
    while (str[i] != 0){
        rightSide[j]=str[i];// copy the word to the right place
        j++;
        i++;
    }
    arrOfWords_left = countWordsAndChars(leftSide);// arrOfWords_left is array of words of the left command
    arrOfWords_right = countWordsAndChars(rightSide);// arrOfWords_right is array of words of the right command
    numOfCommand-=2;
    totalPipes+=1;// count pipes of the whole program

    int pipeFd1[2];
    pid_t leftPid, rightPid;
    int status;
    if(pipe(pipeFd1)<0){// pipe was crated
        perror("pipe error!"), exit(1);
    }
    if((leftPid =fork())==0){//first son process
        dup2(pipeFd1[1], STDOUT_FILENO);//the output to is going now to the pipe
        close(pipeFd1[0]);
        close(pipeFd1[1]);

        if(execvp(arrOfWords_left[0], arrOfWords_left)==-1){
            freeFunc(arrOfWords_left);
            freeFunc(arrOfWords_right);
            perror("\nThe process failed");
            exit(1);
        }
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_right);
        perror("\nThe process failed");
        exit(1);
    }
    else if(leftPid<0){
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_right);
        perror("\nFork failed!\n");
        exit(1);
    }
    else if((rightPid=fork())==0){
        dup2(pipeFd1[0], STDIN_FILENO);// th input is now going into the standard input, stdin will be a reader from pipe
        close(pipeFd1[0]);
        close(pipeFd1[1]);
        if( execvp(arrOfWords_right[0], arrOfWords_right)==-1){
            freeFunc(arrOfWords_left);
            freeFunc(arrOfWords_right);
            perror("\nThe process failed");
            exit(1);
        }
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_right);
        exit(1);
    }
    else if(rightPid<0){
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_right);
        perror("\nFork failed!\n");
        exit(1);
    }
    else{
        close(pipeFd1[0]);
        close(pipeFd1[1]);
        wait(&status);
        wait(&status);
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_right);
        return;
    }
}

//in case we have a command with 2 pipes
void pipeCase2(char str [], int ind1, int ind2){
    char** arrOfWords_left;
    char** arrOfWords_middle;
    char** arrOfWords_right;
    totalPipes+=2;
    char leftSide[INPUT_SIZE];
    char middleSide[INPUT_SIZE];
    char rightSide[INPUT_SIZE];// Going to hand words.
    memset(leftSide, '\0', INPUT_SIZE);
    memset(middleSide, '\0', INPUT_SIZE);
    memset(rightSide, '\0', INPUT_SIZE);

    char cwd[256];
    if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("get-cwd error");
    }

    int i=0;
    while (i<ind1){
        leftSide[i]=str[i];// copy the word to the right place
        i++;
    }
    i = ind1+1;
    int j=0;
    while (ind1 < ind2-1){
        middleSide[j]=str[i];// copy the word to the right place
        ind1++;
        j++;
        i++;
    }
    i=ind2+1;
    j=0;
    while (str[i] != 0){
        rightSide[j]=str[i];// copy the word to the right place
        j++;
        i++;
    }

    arrOfWords_left = countWordsAndChars(leftSide);
    arrOfWords_middle = countWordsAndChars(middleSide);
    arrOfWords_right = countWordsAndChars(rightSide);
    numOfCommand-=3;

    int pipeFd2[4];
    pid_t leftPid, middlePid, rightPid;// 3 sons, every son is suitable for a process
    int status;// wait func
    i=0;
    if(pipe(pipeFd2)== -1 || pipe(pipeFd2+2)== -1 ){
        perror("\nproblem with open pipe!\n");
        exit(1);
    }

    if((leftPid=fork())==0){ // this is the left son
        dup2(pipeFd2[1], STDOUT_FILENO);//this part replace the left stdout with write

        while(i<4){// close pipe
            close(pipeFd2[i]);
            i++;
        }
        if(execvp(arrOfWords_left[0], arrOfWords_left)==-1){// exec for the left process
            freeFunc(arrOfWords_left);
            freeFunc(arrOfWords_middle);
            freeFunc(arrOfWords_right);
            perror("\nThe process failed");
            exit(1);
        }
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_middle);
        freeFunc(arrOfWords_right);
    }
    else if(leftPid<0){
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_middle);
        freeFunc(arrOfWords_right);
        perror("\nFork failed!\n");
        exit(1);
    }
    else if((middlePid=fork()) == 0){
        i=0;
        dup2(pipeFd2[0],  STDIN_FILENO);// this part replace stdin with read
        dup2(pipeFd2[3], STDOUT_FILENO);// this part replace stdout with write
        while(i<4){// close pipes
            close(pipeFd2[i]);
            i++;
        }
        if(execvp(arrOfWords_middle[0], arrOfWords_middle)==-1){ // exec for the middle process
            freeFunc(arrOfWords_left);
            freeFunc(arrOfWords_middle);
            freeFunc(arrOfWords_right);
            perror("\nThe process failed");
            exit(1);
        }
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_middle);
        freeFunc(arrOfWords_right);
    }
    else if(middlePid<0){
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_middle);
        freeFunc(arrOfWords_right);
        perror("\nFork failed!\n");
        exit(1);
    }
    else if((rightPid=fork())==0) {
        dup2(pipeFd2[2], STDIN_FILENO);// this part replace stdin with read
        i = 0;
        while (i < 4) {//close pipe
            close(pipeFd2[i]);
            i++;
        }

        if(execvp(arrOfWords_right[0], arrOfWords_right)==-1){
            freeFunc(arrOfWords_left);
            freeFunc(arrOfWords_middle);
            freeFunc(arrOfWords_right);
            perror("\nThe process failed");
            exit(1);
        }
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_middle);
        freeFunc(arrOfWords_right);
    }
    else if(rightPid<0){
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_middle);
        freeFunc(arrOfWords_right);
        perror("\nFork failed!\n");
        exit(1);
    }

    else {
        i = 0;
        while (i < 4) {//close pipe
            close(pipeFd2[i]);
            i++;
        }
        i = 0;
        while (i < 4) {// the parent is waiting for his sons
            wait(&status);
            i++;
        }
        freeFunc(arrOfWords_left);
        freeFunc(arrOfWords_middle);
        freeFunc(arrOfWords_right);
    }
}

//handler function, I am going to use it in ampersandCase function
void handler(int num){
    int status;
    waitpid(-1, &status, WNOHANG);// return immediately if no child has exited.
}
// ampersand case function, the son is going to do his command, but his dad not doing to wait him.
void ampersandCase(char str[], char ** words){
    char cwd[256];
    if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("get-cwd error");
    }

    int e=0 ;
    for( e=(int)strlen(str)-1; str[e]== ' ' ; e--){
        e--;
    }// where the input is really end

    if(str[e]=='&'){// ignore '&'
        str[e]=0;
        words=countWordsAndChars(str);//make a new words array without ampersand
    }
    for(int p=0 ; words[p]!=0 ; p++){
//        printf("\n  %s\n", words[p]);
        if(strcmp(words[p], "&")==0){
            free(words[p]);
            words[p]=NULL;
        }
    }
    signal(SIGCHLD, handler); //parent continue running while getting sigchld from his son.
    pid_t pid; //process
    pid=fork();

    if(pid<0){
        freeFunc(words);
        perror("\nfork() system call from ampersandCase func was failed!");
        exit(1);
    }
    if(pid ==0 ){// son process
        if(execvp(words[0], words) == -1){
            freeFunc(words);
            perror("\nexecvp() system call from ampersandCase func was failed!");
            exit(1);
        }
        freeFunc(words);
    }
    if(pid>0){// parent process, should not wait for his sons
        printf("%s>", cwd);
        process[pro]=pid;
        pro++;
    }
    for (int i = 0; words[i]!=0; i++) {
        free(words[i]);
    }
    free(words);
}

void nohupCase(char str[] ,char** words){

    char **newWordsArr=(char**)malloc(sizeof(char*)*wCounter);// array size of how many words you have.
    int newInd=0;
    for(int p=1 ; p<wCounter +1; p++){// because words array contains "nohup" word in the first place, I want to ignore this word with a new array of pointers.
        newWordsArr[newInd]=words[p];
        newInd++;
    }

    signal(SIGHUP, SIG_IGN);// this program will ignore the sighup signal

    pid_t pid;
    pid=fork();
    if(pid == 0){
        int fd= open( "nohup.txt",  O_WRONLY | O_CREAT | O_APPEND, 0777);// open file to read only, if not exist it will created, a+.
        if(fd==-1){
            perror( "open file failed, (from func nohupCase)" );
            freeFunc(words);
            free(newWordsArr);
            exit( 1);
        }
        int value = dup2(fd,1);// 1 is out
        close(fd);
        if(value==-1){
            perror( "dup2 failed, (from func nohupCase)" );
            freeFunc(words);
            free(newWordsArr);
            exit( 1 );
        }
        if(execvp( newWordsArr[0], newWordsArr)==-1){
            perror( "\nexecvp() system call from nohupCase func was failed!");
            freeFunc(words);
            free(newWordsArr);
            exit( 1 );
        }
        freeFunc(words);
        free(newWordsArr);
    }
    else{
        wait(NULL);
    }
    free(newWordsArr);
}


void nohupAndAmpersandCase(char str[] ,char** words){
    char cwd[256];
    if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("get-cwd error");
    }
    char **newWordsArr=(char**)malloc(sizeof(char*)*wCounter);// array size of how many words you have.
    int newInd=0;
    words= countWordsAndChars(str);
    for(int p=1 ; p<wCounter +1; p++){
        newWordsArr[newInd]=words[p];
        newInd++;
    }


    pid_t pid;
    int fdNohup;
    int status;
    signal(SIGHUP, SIG_IGN);// ignore sighup
    signal(SIGCHLD, handler);
    pid=fork();

    if(pid<0){// fork failed
        perror( "\nfork() system call from nohupAndAmpersandCase func was failed!");
        freeFunc(words);
        free(newWordsArr);
        exit( 1 );
    }
    if(pid==0){// child process
        if(execvp( newWordsArr[0], newWordsArr)==-1){
            perror( "\nexecvp() system call from nohupCase func was failed!");
            freeFunc(words);
            free(newWordsArr);
            exit( 1 );
        }

    }
    else{
        freeFunc(words);// free array
        free(newWordsArr);// free pointer
    }
}