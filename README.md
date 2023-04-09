# Mini shell program

Authored by Guy Cohen

==Description ex3.c==

This program it’s a simple program based in a simulator of a program shell to get commands from the user.
the program separate the users input into words. For every input  the program is getting, it produce an array made of the words in it. The program will execute the command will simulating.
the program will represent every command like a real command of the shell. 
The program can to get an infinite quantity of commands with out limit. If you decide to finish the program just put the word "done" , otherwise the program will not finish. till now my program could execute basic commands. 
For now the program have the power to execute commands like "history" command , and in addition execute the commands from history (with writing "![the right number that appears on history]").
for this exercise version the program can support the commands of type pipe '|' , for now the user will can make commands with 1-2 pipes commands.
In addition, this version gives the option to execute commands in background. right commands can be execute to a nohup.txt file with "nohup" --> (for example: " nohup [command]") . Another new option, is to give command like "[command]&", it will give the option to continue work while another command is going to another place.
There is the option to combine " nohup [command] &" , processes will continue even if the program was closed.

Program DATABASE:
1.	input =the main data base of this program, it is an array of characters, in size of 512, it saves the input from user.  The program will analyze this array with differ functions. the program will orginize the words of it in an 	array that was dinamicly allocated and will work on it.  
2.	file.txt = a text file that is going to hold the history of inputs from the user, the program will save each kind of it.
3. 	nohup.txt = inputs from user that will be writen like "nohup [command] &" will be writen to there

functions:

1.	askForInput- This func is working with a loop, the loop is going to ask for an input,
	and while the input is differed from "done", it is continued ask for one.
	for each input, there is a son process that is going to count words, chars and in addition to organize the words of it on array. The father process is going to wait till the son done.
2.	countWordsAndChars- This function gets the input from the user and counts the amount of words and chars in it.
   	 it is start with checking edge condition, it checks if the input is empty.
    	the function counts when a word starts and when it ends, it will calculate the chars count by "end - start". it will continue till the last char or last whitespace.
  	in addition, for each input it will seperate it to words and will locate it in the suitable place on array that will returned finally.

 	each input will be written to file.txt . and in addition, the amount of 
    	for "history" input, the fuction will print the hisrory withe using specific function .
    	for "done" input, the function will exit the program.
3.	 history- this function aim is to find the right place in history and sending it to "countWordsAndChars" function, she will execute the right command .

4.	 pipeCase1 / pipeCase2 – those functions were written to execute pipes
     	 command.
	 pipeCase1 is responsible to execute commands with 1 pipe sign, 
	 pipeCase2 is responsible to execute commands with 2 pipe signs.

5.	 ampersandCase- The ampersand character at the end of the command brings us to this function.
	 This character puts the command to run in the background while allowing the user to enter additional commands.
 	 The father process here does not "wait".
6.	nohupCase- Allows commands to continue running even if the program has ended.
	The output of the son process will go to a file named nohup.txt.
	This function ignores the SIGHUP signal, so the program does not end when the shell exits.



==Program Files==
Ex3.c
==How to compile?==
compile: gcc ex3.c -o ex3
run: ./ex3


==Input:==
1.	string from the user, that smaller then 512 chars.

==Output:==
1.	The program will execute the command  simulating like a real shell.

