#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

/*
Chase Kirchner
UNIX Shell
*/

/* function prototyping */
void	split(char* sentence, char** output);
void	statusCheck(int pid, int status, char*);

//arg char input
void
main(int argc, char** argv){
	int debug = false;
	char* cmd = nil;
	/* command (arg) processing */
	int arg;

	//C library getopt for processing commands
	while((arg = getopt(argc, argv, "Enter:")) != -1){
		switch(arg){
		case '1':
			debug = true;
			break;
		case '2': 
			//GNU argument set
			cmd = optarg;
			break;
		case '3':
			fprintf(stdout, "Chase's Shell: Enter Command\n");
		default:
			exit(1);
		}
	}
	//set Shell name
	if(cmd == nil)
		cmd = "Shellsh> ";
	//system call
	int Start = true;

	//Start process
	while(Start){
		char process[256];
		//Background Variables
		int backgroundPID = -1;
		int backgroundStatus = -1;
		// Check background process status 
		//Set WNOHANG
		backgroundPID = waitpid(-1, &backgroundStatus, WNOHANG);
		if(debug)
			fprintf(stderr, "Background Status: %d\nBackground PID: %d\n", backgroundStatus, backgroundPID);
		//Child Exits		
		if(backgroundPID > 0) {
			char* backgroundName = calloc(256, sizeof(char));
			
			fprintf(stderr, "Child Left");
			statusCheck(backgroundPID, backgroundStatus, backgroundName);
			
			//Must free the process
			free(backgroundName);
		}
		
		// Sleep for necessary command pauses
		usleep(100);
		fprintf(stdout, "%s", cmd);
		int i;
		
		//Background child check
		int length = strlen(process);
		int active = false;

		//Active process set and check
		if(process[length - 1] == '&'){
			active = true;
			//set empty value
			process[length - 2] = '\0';
		}

		if(debug)
			fprintf(stderr, "%s\n", process);

		// Spaces/etc "false" commands
		if(strlen(process) == 0)
			continue;
		
		//Built in commands
		if(strncmp(process, "exit", 4) == 0){
			Start = false;
			continue;
		//PPID check
		}else if(strncmp(process, "ppid", 4) == 0){
			fprintf(stdout, "%d\n", getppid());
		//PID check
		}else if(strncmp(process, "pid", 3) == 0){
			fprintf(stdout, "%d\n", getpid());
		//CD check
		}else if(strcmp(process, "cd") == 0){
			char* stream;
			//retrieve environment
			stream = getenv("HOME");
			if(stream == nil){
				fprintf(stderr, "Error Locating $HOME\n");
				continue;
			}
			if(debug)
				fprintf(stderr, "CWD: %s\n", stream);
			if(chdir(stream) < 0)
				fprintf(stderr, "CWD Error\n");
		
		//Print Working Directory
		}else if(strncmp(process, "pwd", 3) == 0){
			char* stream = calloc(256, sizeof(char));
			if(stream == getcwd(stream, 256))
				fprintf(stdout, "%s\n", stream);
			else
				fprintf(stderr, "CWD Error\n");
			free(stream);
		}else if(strncmp(process, "cd ", 3) == 0){
			char* stream = calloc(253, sizeof(char));
			strncpy(stream, process + 3, 253);
			if(debug)
				fprintf(stderr, "Change Directory to: %s\n", stream);
			if(chdir(stream) < 0)
				fprintf(stderr, "CWD Error\n");
			free(stream);
		//Get process
		}else if(strncmp(process, "get", 3) == 0){
			//char initialization for calls
			char* key;
			char* name = calloc(252, sizeof(char));

			if((int)strlen(process) < 4){
				fprintf(stderr, "Variable Error\n");
				free(name);
				continue;
			}
			
			//Retrieve environment
			strncpy(name, process + 4, 252);
			key = getenv(name);
			if(key == nil){
				if(name[0] == '\t' || name[0] == ' ')
					fprintf(stderr, "Variable Error\n");
				else
					fprintf(stderr, "Variable Error\n");
				
				
				free(name);
				continue;
			}
			char checkValue[256];
			strcpy(checkValue, name);
			int len = strlen(name) + 1;

			checkValue[strlen(name)] = '=';
			checkValue[strlen(name)+1] = '\0';
			int pair = true;

			//Key Pair checks
			for(i = 0; i < strlen(key) && i < len; i++){
				if(debug)
					fprintf(stderr, "key 1: %d\nValue key 2: %c\n", key[i]), checkValue[i];
				if(checkValue[i] != key[i]){
					pair = false;
					break;
				}
			}
			if(pair)
				fprintf(stdout, "%s\n", key + i);
			else
				fprintf(stdout, "%s\n", key);
			free(name);
		
		//Set Process
		}else if(strncmp(process, "set", 3) == 0){
			char* stack = calloc(256, sizeof(char));
			strcpy(stack, process);
			char* output[256];
			split(stack, output);
			if(debug)
				fprintf(stderr, "Input: %s ;\n Output: %s\n", process, output[1]);
			for(i = 1; i < 256; i++){
				if(output[i] == nil){
					if(debug)
						fprintf(stderr, "Arguments Needed: %d\n", i);
					break;
				}
				if(strlen(output[i]) < 1){
					break;
				}
			}
			int location = i;
			//location check
			if(location < 2){
				fprintf(stderr, "Variable Error\n");
				free(stack);
				continue;
			}
			
			char* name = output[1];
			char* key = calloc(252, sizeof(char));

			//Char array key set to location
			//set at /home/..
			int locationIndex = 0;
			int error = 0;
			int empty = false;
			
			if(location == 2)
				empty = true;
			if((int)strlen(process) < 4){
				fprintf(stderr, "Variable Error\n");
				free(key);
				free(stack);
				continue;
			}
			
			for(i = 4; i < 256; i++){
				if(process[i] == ' '){
					locationIndex = i + 1;
					if(debug)
						fprintf(stderr, "Space located: %d\n", i);
					break;
				}
				if(process[i] == '\0'){
					if(debug)
						fprintf(stderr, "Null at: %d\n", i);
					locationIndex = i - 2;
					empty = true;
					break;
				}
				if(i == 255){
					fprintf(stderr, "Variable Error\n");
					error = 1;
					free(key);
					break;
				}
			}
			if(error > 0){
				free(stack);
				continue;
			}
			free(key);
			free(stack);
		}else{
			//Command Check
			if(debug)
				fprintf(stderr, "Waiting for Prompt:\n");
			char* args[256];
			split(process, args);
			int pid;
			int status;
			pid = fork();
			//fork issues
			if(pid < 0){
				fprintf(stderr, "Fork Error\n");
				exit(1);
			//fork worked
			}else if(pid == 0){
				fprintf(stderr, "%d from %s\n", getpid(), *args);
				int error = execvp(*args, args);
				if(error < 0){
					fprintf(stderr, "Command Error in $PATH?\n");
					exit(2);
				}
			}else{
				// Parent process check
				if(!active){
					waitpid(pid, &status, 0);
					statusCheck(pid, status, *args);
				}
			}
		}
	}
	fprintf(stderr, "Exit \n");
}
// Command splits into char 
void
split(char* sentence, char** output)
{
	//Check for null character/newline
	while(*sentence != '\0'){
		while(*sentence == ' ' || *sentence == '\n')
			*sentence++ = '\0';
		*output++ = sentence;
		while(*sentence != '\0' && *sentence != ' ' && *sentence != '\n')
			sentence++;
	}
	//Zero character
	*output = '\0';
}
// GCC status checks 
void
statusCheck(int pid, int statusCheck, char* name)
{
	// GCC status checks
	if (WIFSIGNALED(statusCheck)){
		fprintf(stderr, "%s %d Terminated. Status: %d\n", name, pid, WTERMSIG(statusCheck));
	}
	else if(WIFEXITED(statusCheck)){
		fprintf(stderr, "%s %d Exited. Status: %d\n", name, pid, WEXITSTATUS(statusCheck));
	}

}
