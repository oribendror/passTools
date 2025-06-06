#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct strnC{
    char str[128];
    int num;
};

int isIn(int *, int ,int);
int commandToCode(char *);
int generatePaswd(char ***,char *, const char *);
int* matchPartPaswd(char *, const char *);
int regex_compare(char *, char *);
int isSubstr(char *, char *);
void initStrArr(char **, int);
void freeStrArr(char **, int);
void printStrArr(char **, int);
void initStrnCArr(struct strnC *, int);


int main(int argc, char* argv[]) {
    static const int HELP_CODE = 1;
    static const int GENERATE_CODE = 2;
    static const int PARTIAL_CODE = 3;
    static const int STRENGTHEN_CODE = 4;
    static const char WORDLIST[] = "/home/oribd/Documents/wordlists/rockyou.txt";

    if (argc > 3) {
        printf("Error: expected 1 command and possibly a password\n");
        return 0;
    }

    if (argc < 2) {
        printf("Error: must pass command from {--help/-h, --generate/-g, --partial/-p, --strengthen/-s} and a password\n");
        return 0;
    }

    int command = commandToCode(argv[1]);

    if (command == HELP_CODE) {
        FILE *helpFile = fopen("help.txt", "r");
        if (helpFile == NULL) {
            printf("Error: help message not found\n");
            return 0;
        }

        char line[128];
        while (fgets(line, sizeof(line), helpFile)) {
            printf("%s", line);
        }

        fclose(helpFile);
        return 1;
    }

    if (argc < 3) {
        printf("Error: for commands other than --help, must provide password\n");
        return 0;
    }

    // Handle generate mode
    if (command == GENERATE_CODE) {
        char **generatedPaswd = NULL;
        int genAmount = generatePaswd(&generatedPaswd, argv[2], WORDLIST);
        if (!genAmount) {
            printf("No matching passwords found. Please try a different regex.\n");
            return 0;
        }
        printf("Matching passwords found:\n");
        printStrArr(generatedPaswd, genAmount);
        freeStrArr(generatedPaswd, genAmount);
        free(generatedPaswd);
        return 1;
    }

    // Handle partial match mode
    if (command == PARTIAL_CODE) {
        char *full = "yourRealPasswordHere"; // You may want to ask user or hardcode for now
        char *part = argv[2];
        int result = codeForPaswd(full, part);

        switch (result) {
            case 1:
                printf("Match type: prefix\n");
                break;
            case 2:
                printf("Match type: suffix\n");
                break;
            case 3:
                printf("Match type: substring\n");
                break;
            case 4:
                printf("Match type: scattered subsequence\n");
                break;
            default:
                printf("No match found\n");
        }
        return 1;
    }

    printf("Did not recognize any valid flags\n");
    return 1;
}


int isIn(int *array, int arrLen, int x){
    for(int i=0; i<arrLen; i++){
        if(array[i] == x){
            return 1;
        }
    }
    return 0;
}

int commandToCode(char command[]){
    if(!strcmp(command, "--help") || !strcmp(command, "-h")){
        return 1;
    }
    if(!strcmp(command, "--generate") || !strcmp(command, "-g")){
        return 2;
    }
    if(!strcmp(command, "--partial") || !strcmp(command, "-p")){
        return 3;
    }
    if(!strcmp(command, "--strengthen") || !strcmp(command, "-s")){
        return 4;
    }
    return 0;
}

int generatePaswd(char ***generatedPaswd,char paswd_regex[], const char WORDLIST[]){
    *generatedPaswd = malloc(5 * sizeof(char *));
    initStrArr(*generatedPaswd, 5);
    int j=0;
    FILE *wordlist = fopen(WORDLIST, "r");

    if(wordlist == NULL){
        printf("Wordlist not found \n");
        return 0;
    }

    char paswd[128];
    while(fgets(paswd, sizeof(paswd), wordlist)){
        paswd[strcspn(paswd, "\n")] = '\0';  // removes trailing newline if any
        if(strlen(paswd)!=strlen(paswd_regex)){
            continue;
        }
        if(!regex_compare(paswd_regex, paswd)){
            continue;
        }

        strcpy((*generatedPaswd)[j], paswd);
        j++;
        if(j == 5){
            return j;
        }
    }
    if(j > 0){
        return j;
    }
    freeStrArr(*generatedPaswd, 5);
    free(*generatedPaswd);
    return 0;
}

int* matchPartPaswd(char partPaswd[], const char WORDLIST[]) {
    struct strnC *matches = malloc(10 * sizeof(struct strnC));
    int total = 0;

    FILE *wordlist = fopen(WORDLIST, "r");
    if (!wordlist) {
        printf("Error: wordlist not found\n");
        return NULL;
    }

    char paswd[128];

    // Pass 1: Prefixes
    rewind(wordlist);
    while (fgets(paswd, sizeof(paswd), wordlist) && total < 10) {
        paswd[strcspn(paswd, "\n")] = '\0';
        if (codeForPaswd(paswd, partPaswd) == 1) {
            strcpy(matches[total++].str, paswd);
        }
    }

    // Pass 2: Suffixes
    rewind(wordlist);
    while (fgets(paswd, sizeof(paswd), wordlist) && total < 10) {
        paswd[strcspn(paswd, "\n")] = '\0';
        if (codeForPaswd(paswd, partPaswd) == 2) {
            strcpy(matches[total++].str, paswd);
        }
    }

    // Pass 3: Sequences
    rewind(wordlist);
    while (fgets(paswd, sizeof(paswd), wordlist) && total < 10) {
        paswd[strcspn(paswd, "\n")] = '\0';
        if (codeForPaswd(paswd, partPaswd) == 3) {
            strcpy(matches[total++].str, paswd);
        }
    }

    // Pass 4: Scattered
    rewind(wordlist);
    while (fgets(paswd, sizeof(paswd), wordlist) && total < 10) {
        paswd[strcspn(paswd, "\n")] = '\0';
        if (codeForPaswd(paswd, partPaswd) == 4) {
            strcpy(matches[total++].str, paswd);
        }
    }

    fclose(wordlist);

    // Print result (for testing)
    for (int i = 0; i < total; i++)
        printf("Match %d: %s\n", i + 1, matches[i].str);

    // Return if needed — or pass back in another form
    free(matches); // remove this line if returning matches

    return NULL;
}



int codeForPaswd(char paswd[], char partPaswd[]){
    int prefix = 1;
    int suffix = 1;
    int sequence;
    int scattered = 1;

    int k = -1; // last matched index for scattered
    int found;

    int lenPaswd = strlen(paswd);
    int lenPart = strlen(partPaswd);

    // Check prefix and suffix
    for(int i = 0; i < lenPart; i++){
        // prefix check
        if(paswd[i] != partPaswd[i]){
            prefix = 0;
        }

        // suffix check
        if(paswd[lenPaswd - 1 - i] != partPaswd[lenPart - 1 - i]){
            suffix = 0;
        }
    }

    // Check scattered subsequence
    for(int i = 0; i < lenPart; i++){
        found = 0;
        for(int j = k + 1; j < lenPaswd; j++){
            if(paswd[j] == partPaswd[i]){
                found = 1;
                k = j;
                break;
            }
        }
        if(!found){
            scattered = 0;
            break;
        }
    }

    sequence = isSubstr(paswd, partPaswd);

    if(!prefix && !suffix && !sequence && !scattered){
        return 0;
    }
    if(prefix){
        return 1;
    }
    if(suffix){
        return 2;
    }
    if(sequence){
        return 3;
    }
    if(scattered){
        return 4;
    }

    return 0; // fallback (should not get here)
}



int regex_compare(char regex[], char str[]){
    for(int i=0; i<strlen(regex); i++){
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
    int lenStr = strlen(str);
    int lenPart = strlen(part);

    if(lenPart > lenStr) return 0;  // can't be substring if longer

    for(int i = 0; i <= lenStr - lenPart; i++){  // <= so substring fits
        int isSub = 1;
        for(int j = 0; j < lenPart; j++){
            if(str[i + j] != part[j]){
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
        printf("%s\n", strArr[i]);
    }
}

void initStrnCArr(struct strnC arr[], int size){
    for(int i=0; i<size; i++){
        arr[i].num = 0;
        strcpy(arr[i].str, "");
    }
}
