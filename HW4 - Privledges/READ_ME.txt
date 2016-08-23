Name: Nathan Truong
SID: 998163923
Assignment: Homework #4 problem 3

-After compiling the code through make, use the executable "runpriv" to star the program.
-The program will first check the UID (Hard coded to mine, can change the varibale UID), 
if it does not match, the program exits.
-The program will then prompt the user for thier CAS password, if entered wrong, program exits.
-The program will then check for sniff's exisitence, sniff's UID, modification time, and privledges.If any fail, the program exits.
-If all the checks are passed, then the function changePermissions will be called, and
change the owner to root, the group ID and the the security to 4450.

To run execve I forked the process into a child and parent to run kinit, and chown.
Otherwise, the program was written very straighfoward and following the prompt. I ran through
all of the checks first before making any changes. 

Below are the sources I used to implement the forking as well as things like how to use chown
or to use stat to get information on the files.

Sources used:
http://codewiki.wikidot.com/c:system-calls:stat
http://pubs.opengroup.org/onlinepubs/009695399/basedefs/sys/stat.h.html
http://www.thegeekstuff.com/2012/03/c-process-control-functions/
http://man7.org/linux/man-pages/man2/stat.2.html
http://unix.stackexchange.com/questions/2464/timestamp-modification-time-and-created-time-of-a-file
http://linux.die.net/man/2/stat
http://linux.die.net/man/1/chown
http://stackoverflow.com/questions/13098620/using-stat-to-check-if-a-file-is-executable-in-c
http://man7.org/linux/man-pages/man2/execve.2.html
http://stackoverflow.com/questions/13098620/using-stat-to-check-if-a-file-is-executable-in-c
http://man7.org/linux/man-pages/man2/chmod.2.html
http://www.techonthenet.com/linux/commands/chown.php
