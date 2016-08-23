//Nathan Truong
//SID: 998163923 

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#define UID 7004124 //Change this UID for each user 

using namespace std;

//http://codewiki.wikidot.com/c:system-calls:stat
//http://pubs.opengroup.org/onlinepubs/009695399/basedefs/sys/stat.h.html
void checkUID(char* argv) {
  if(argv == NULL) {
    exit(0);
  }

  struct stat fileStat;
  int status = stat(argv, &fileStat);
  if(status < 0) {
    exit(0);
  }

  if(fileStat.st_uid != UID) { 
    cout << "Incorrect UID. Exiting Program." << endl;
    exit(0); 
  } 
}

//Example from http://www.thegeekstuff.com/2012/03/c-process-control-functions/
void checkPW() {
//Creates a child process for the kinit to run
  pid_t childPid;
  int status;
  childPid = fork();

  char *const newArg[] = {"kinit", NULL}; //Arguements to use
  char* const newEnv[] = {"PATH=/bin:/usr/bin", "IFS=\t\n", "SHELL=/pkg/bin/tcsh", NULL}; //Setting the enviorment variables

  if(childPid >= 0) { // Child Process
    if(childPid == 0) {
      execve("/usr/bin/kinit", newArg, newEnv);
    }

    else { // Parent Process
      wait(&status); 
      if(status != 0) {
	cout << "Incorrect password. Exiting Program." << endl;
    	exit(0);
      }
    }
  }

  else {
    perror("Eror; fork()");
    exit(0);
  }
}

//http://codewiki.wikidot.com/c:system-calls:stat
//http://pubs.opengroup.org/onlinepubs/009695399/basedefs/sys/stat.h.html
void checkSniff(string fileName) { 
  time_t currentTime = time(0);
  struct stat sniffStat;
  stat(fileName.c_str(), &sniffStat);  

  ifstream readFile(fileName.c_str());
  if(!readFile) {
    cout << "File: sniff - was not found. Exiting Program." << endl;
    exit(0);
  }

  if(sniffStat.st_uid != UID) {
  	cout << "File: sniff - incorrect UID. Exiting Program." << endl;
    exit(0);
  }

  if((sniffStat.st_mode & S_IXUSR) == 0) {
  	cout << "File: sniff - is not exectuable by owner. Exiting Program." << endl;
    exit(0); 
  }

  if(((sniffStat.st_mode & S_IROTH) != 0) || ((sniffStat.st_mode & S_IWOTH) != 0) || ((sniffStat.st_mode & S_IXOTH) != 0)) {
  	cout << "File: sniff - is exectuable by others. Exiting Program." << endl;
    exit(0); 
  }
  
  if(((currentTime - sniffStat.st_mtime) > 60) || ((currentTime - sniffStat.st_ctime) > 60)) {
     cout << "File: Sniff - was created or modified over 1 minute. Exiting Program." << endl;
     exit(0); 
  }
}

//http://www.thegeekstuff.com/2012/03/c-process-control-functions/
void changePermissions() {
  pid_t childPid;
  int status; 
  childPid = fork();

  char *const newArg[] = {"chown", "root", "sniff", NULL}; // Change to root 0
  char* const otherArg[] = {"chown", "root:proj", "sniff", NULL}; // Change to proj 95
  char* const newEnv[] = {"PATH=/bin:/usr/bin", "IFS=\t\n", "SHELL=/pkg/bin/tcsh", NULL}; //Setting the enviorment variables

  //The first chown to changeto root
  if(childPid >= 0) { // Child Process
    if(childPid == 0) {
      execve("/usr/bin/chown", newArg, newEnv);
      exit(0);
    }

    else { // Parent Process
      wait(&status); 
      if(status != 0) {
	cout << "Error with changing permissions. Exiting Program." << endl;
    	exit(0);
      }
    }
  }

  else {
    perror("Error: fork()");
    exit(0);
  }

  //Refork to run the next chown to change the GID
  childPid = fork();

  if(childPid >= 0) { // Child Process
    if(childPid == 0) {
      execve("/usr/bin/chown", otherArg, newEnv);
    }

    else { // Parent Process
      wait(&status); 
      if(status != 0) {
        cout << "Error with changing permissions. Exiting Program." << endl;
    	exit(0);
      }
    }
  }

  else {
    perror("Error; fork()");
    exit(0);
  }

  //Changes the prermissions of the file
  if(chmod("sniff", 04450) != 0) {
  	exit(0);
  }
}

int main(int argc, char* argv[]) {
  if(argc < 1) {
    exit(0);
  }

  string fileName = "sniff";

  checkUID(argv[0]);
  checkPW();
  checkSniff(fileName);
  changePermissions();

  return 0;
}
