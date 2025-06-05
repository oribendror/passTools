#include <stdio.h>;
#include <stdlib.h>;
#include <stdio.h>;
#include <string.h>;
#include <ctype.h>;

int main(int argc, char* argv[]){
    
    static const int HELP_CODE = 1;
    static const int GENERATE_CODE = 2;
    static const int PARTIAL_CODE = 3;
    static const int STRENGTHEN_CODE = 4;
    static const char WORDLIST[] = "rockyou.txt";

    if(argc > 3){
        printf("Error: expected 1 command and a possibly a password \n")
        return 0;
    }
    if(argc < 2){
        printf("Error: must pass command from { --help/-h, --generate/-g, --partial/-p, --strengthen/-s} and a password \n")
        return 0;
    }

    for(int i=1; i<argc; i++){
        if(commandToCode(argv[i] == HELP_CODE)){
            FILE *helpFile = fopen("help.txt", "r")
            
            if(helpFile == NULL){
                printf("Error: help message not found \n");
                return 0;
            }

            char line[128];
            
            while(fgets(line, sizeof(line),helpFile)){
                printf("%s", line);
            }
            return 1;
        }
    }

    int codesFound[3] = {0};

    for(int i=1; i<argc; i++){
        if(strcmp(isIn(codesFound, commandToCode(argv[i]))), "1"){
            printf("Error: duplicate commands aren't allowed \n");
            return 0;
        }
    }

    for(int i=1; i<argc; i++){
        if(commandToCode(argv[1]) == GENERATE_CODE){
            char **
            int genSuccess = generatePaswd();
            if(!genSuccess){
                printf("no matching passwords found, please try a different regex \n");
                return 0;
            }
            printf("matching passwords found: \n");
            printStrArr(generated, 5);
            return 1;
        }
        if(commandToCode(argv[1]) == PARTIAL_CODE){
            int matchSuccess = matchPartPaswd();
        }
        if(commandToCode(argv[1]) == STRENGTHEN_CODE){
            stregthePaswd();
        }
    }
    
    return 1;
}

int isIn(int array[], int x){
    for(int i=0; i<sizeof(array); i++){
        if(array[i] == x){
            return 1;
        }
    }
    return 0;
}

int commandToCode(char command[]){
    if(strcmp(command, "--help") || strcmp(command, "-h")){
        return 1;
    }
    if(strcmp(command, "--crack") || strcmp(command, "-c")){
        return 2;
    }
    if(strcmp(command, "--partial") || strcmp(command, "-p")){
        return 3;
    }
    if(strcmp(command, "--strengthen") || strcmp(command, "-s")){
        return 4;
    }
    return 0;
}

int generatePaswd(char ***generatedPaswd,char paswd_regex[], char WORDLIST[]){
    *generatedPaswd = malloc(5 * sizeof(char *));
    initStrArr(*generatedPaswd, 5);
    int j=0;
    FILE *wordlist = fopen(WORDLIST, "r");
    int consistent = 1;
    char paswd[128];
    while(fgets(paswd, sizeof(paswd), wordlist)){
        if(sizeof(paswd)!=sizeof(paswd_regex)){
            continue;
        }
        for(int i=0; i<sizeof(paswd_regex); i++){
            if(!regex_compare(paswd_regex, paswd)){
                consistent = 0;
                break;
            }
        }
        if(consistent){
            strcpy(*generatedPaswd[j], paswd);
            j++;
            if(j == 5){
                return 1;
            }
        }
        consistent = 1;
    }
    if(j > 0){
        return 1;
    }
    free(*generatedPaswd);
    return 0;
}

int matchPartPaswd(char partPaswd[], char WORDLIST[]){
    int prefix = 1;
    int suffix = 1;
    int sequence;
    int scattered = 1;
    char matchedPaswd[]; 

    int k=0;
    int found = 0;

    FILE *wordlist = fopen(WORDLIST, "r");
    char paswd[128];

    while (fgets(paswd, sizeof(paswd), wordlist)){
        for(int i=0; i<sizeof(partPaswd), i++){
            if(paswd[i] != partPaswd[i]){
                prefix = 0;
            }
            if(paswd[sizeof(paswd) - i] != partPaswd[sizeof(partPaswd) - i]){
                suffix = 0;
            }
            for(int j=0; j<sizeof(paswd); j++){
                if(paswd[j] == partPaswd[i] && j > k){
                    found = 1;
                    k = j;
                    break;
                }
            }
            if(found == 0){
                scattered = 0;
                return 0;
            }
        }
        sequence = isSubstr(char paswd[], char partPaswd[]);
        if(prefix){return 1};
        if(suffix){return 2};
        if(sequence){return 3};
        if(scattered){return 4};
        return 0;
    }
    
}

int strengthenPaswd(char currPaswd[], char WORDLIST[]){
    
}

int regex_compare(char regex[], char str[]){
    for(int i=0; i<sizeof(regex); i++){
        if(regex[i] == '*' && !isalpha(str[i])){
            return 0;
        }
        if(regex[i] == '#' && !isdigit(str[i])){
            return 0;
        }
        if(regex[i] == '@' && isalnum(str[i])){
            return 0;
        }
        if(isalnum(regex[i]) && regex[i] != str[i]){
            return 0;
        }
    }
    return 1;
}

int isSubstr(char str[], char part[]){
    for(int i=0; i<sizeof(str); i++){
        int isSub = 1;
        for(int j=0; j<sizeof(part); j++){
            if(str[i] != part(j)){
                isSub = 0;
                break;
            }
        }
        if(isSub){
            return 1;
        }
    }
    return 0;
}

void initStrArr(char *strArr[], int numStrings){
    for(int i=0; i < numStrings; i++){
        strArr[i] = malloc(128 * sizeof(char));
    }
}

void freeStrArr(char *strArr[], int numStrings){
    for(int i=0; i<numStrings; i++){
        free(strArr[i]);
    }
}

void printStrArr(char *strArr[], int numStrings){
    for(int i=0; i<numStrings; i++){
        printf("%s\n", strArr[i])
    }
}