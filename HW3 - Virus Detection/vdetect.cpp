//Nathan Truong
//SID: 998163923
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unordered_map>
using namespace std;

// Flags for exit code status and command line arguements
bool s = false;
bool q = false;
bool malformed = false;
bool found =  false;

// This function reads in the command line files
// fileName is the name of the file being read
// fileContent is the string that holds the content of the files concatenated
void readFiles(string fileName, string* fileContent) {
  if (fileContent == NULL) {
    return;
  } 

  char textChar;
  ifstream fileIn(fileName.c_str());
  
  if (fileIn) {
    while(fileIn >> noskipws >> textChar) {
  		*fileContent += textChar;
    }
    fileIn.close();
  }
}  

// This function compares the string in the files to the vdetect file
// fileContent contains the content of the file saved as a string.
// fileName is the name of the file being read
// parsedNS is the map that holds the signatures and names.
void detectFiles(string fileContent, string fileName, unordered_map<string, string>* parsedNS) {
  if (parsedNS == NULL) {
    return;
  } 

  string substring;
  // A double loop used to find the matches Ex. abc, the loop would do a, ab, abc, b, bc etc.
  for(int i = 0; i < (int) fileContent.length(); i++) {
    for(int j = i; j < (int) fileContent.length(); j++) {
      substring += fileContent[j]; 
      auto x = parsedNS->find(substring);
      if (x != parsedNS->end()) {
        found = true;
        if(q == false) {
          cout << fileName << "(" << i << "): matched signature " << x->second << endl;  
        }
        if (s == true) {
          return;
        }
      }
      else {
        continue;
      }
    }
    substring = "";
  }
}

// This function parses the name and string and loads them into a map
// fileName is the name of file being read.
// vdetect is the vector that holds the lines for the fileName.
// parsedNS is the map of names and signatures.
void readVdetect(string fileName, vector<string>* vdetect, unordered_map<string, string>* parsedNS) {
  string line;
  int colon;
  int lineCount = 1;
  bool colonExist;

  if (parsedNS == NULL || vdetect == NULL) {
    return;
  } 

  ifstream fileIn(fileName.c_str());
  
  if (fileIn) {
    while(getline(fileIn,line)) {
      string name;
      string sig;
      lineCount++;
      if (line.length() > 0) {
        if (line[0] == '#') {
          // Checks for comments
          continue;
        }
        for(int i = 0; i < (int) line.length(); i++) {
          if (line[i] == ':') {
            colonExist = true;
            colon = i;
            string temp;
            for(int j = (int) name.length() - 1; j >= 0; j--) {
              if (!(name[j] == ' ' &&  temp == "")) {
                temp = name[j] + temp; // this if statment is to remove the white space before
              }
            } 
            name = temp;
            break;
          }
          else {
            if (!(name == "" && line[i] == ' '))  {
              name += line[i]; // this if stament used to remove the trailing white spaces
            }
          }
        }
        for(int i = colon + 1; i < (int) line.length(); i++) {
          if (line[i] == '\n' || line[i] == '\r') {
            break;
          }
          if (!(sig == "" && line[i] == ' '))  {
              sig += line[i]; // this if statment used to remove the white spaces before
          }
        }
        string newString;
        // This loop is to check and see if there are hex values, convert and return them
        for(int i = 0; i < (int) sig.length(); i++) {
          string hexTemp;
          if (sig[i] == '\\' && (i + 3) < (int) sig.length()) {
            if(sig[i + 1] == 'x') {
              if ((sig[i + 2] >= 'A' && sig[i + 2] <= 'F') || (sig[i + 2] >= 'a' && sig[i + 2] <= 'f') 
                || (sig[i + 2] >= '0' && sig[i + 2] <= '9')) {
                if ((sig[i + 3] >= 'A' && sig[i + 3] <= 'F') || (sig[i + 3] >= 'a' && sig[i + 3] <= 'f') 
                || (sig[i + 3] >= '0' && sig[i + 3] <= '9')) {
                  hexTemp = sig[i + 2] + sig[i + 3];
                  char c = 0;
                  // The if statements below are used to manually convert to hex
                  if (tolower(sig[ i + 2]) >= 'a' && tolower(sig[ i + 2]) <= 'f') {
                    c += (10 + tolower(sig[i + 2]) - 'a') * 16;
                  }
                  else {
                    c  += (sig[i + 2] - '0') * 16;
                  }
                  if (tolower(sig[ i + 3]) >= 'a' && tolower(sig[ i + 3]) <= 'f') {
                    c += (10 + tolower(sig[i + 3]) - 'a');
                  }
                  else {
                    c  += (sig[i + 3] - '0');
                  }
                  newString += c;
                  i += 3;
                  continue;
                }
              }
            }
          }
          newString += sig[i];
        }
        if(colonExist ==  false) {
          malformed = true; 
          if(q == false) {
            cout << "line " << lineCount - 1 << ": Malformed line in " << fileName << "\n";
          }
          continue;
        }
        if (colonExist == true && name != "" && newString != "") {
          parsedNS->emplace(newString, name); // pushes the name and signitures into the map
          vdetect->push_back(line);
          colonExist = false;
        }
      }  // End if  
    }  // End while
    fileIn.close();
  }
}  //Adds in line by line

// This function checks if the file exists
// file is the name of the file.
bool checkFiles(char* file) {
  if (file == NULL) {
    return false;
  }
  ifstream readFile(file);
  if (readFile)
    return true;
  else {
    if(q == false) {
      perror(file);
    } 
    return false;
  }
}
 
// This fucntion reads the command line arguements, sets flags and add files into a vector
// argc is the number of arguments.
// argv is the list of the command line arguments.
// fileNames is the list of file names.
// newFile is the name of the vdetect file if it was replaced.
void checkCommandLine(int argc, char* argv[], vector<string>* fileNames, string* newFile) {
  if (argv == NULL || fileNames == NULL || newFile == NULL) {
    return;
  } 
  for(int i = 1; i < argc; i++) { 
    if (strcmp(argv[i], "-s") == 0) {
      s = true; 
    }
    else if (strcmp(argv[i], "-q") == 0) {
      q = true;                  
    } 
    else if (strcmp(argv[i], "-d") == 0) {
      if (i + 1 >= argc) { 
        if(q == false) {
          cout << "-d: requires signature file" << "\n";
        }
        exit(0);
      }

      if (checkFiles(argv[i + 1]) == true) {
        *newFile = argv[i + 1];
      }
      i++;      
    }  
    else {
      if (checkFiles(argv[i]) == true)
        fileNames->push_back(argv[i]);
    }
  }
}

// Retrieves the user input when no files specified.
string userInput() {
  int c;
  string userIn;

  while((c = getc(stdin)) !=EOF) {
    userIn += c;
  }
  return userIn;
}


int main(int argc, char* argv[]) {
  vector<string> fileNames;
  vector<string> vdetect;
  unordered_map<string, string> parsedNS;
  
  string input;
  string fileContent;
  string dFile = "vdetect.str";

  checkCommandLine(argc, argv, &fileNames, &dFile);
  readVdetect(dFile, &vdetect, &parsedNS);
  
  if ((int)fileNames.size() == 0) {
    input = userInput();    
    detectFiles(input, dFile, &parsedNS);
  }

  for(int i = 0; i < (int) fileNames.size(); i++) {
  	readFiles(fileNames[i], &fileContent);
    detectFiles(fileContent, dFile, &parsedNS);
  	fileContent = "";
  }

  if (malformed == true && found == false) {
    exit(2);
  }

  if (malformed == true && found == true) {
    exit(3);
  }

  if (found ==  false) {
    exit(0);
  }

  if (found == true) {
    exit(1);
  }
  return 0;
}