#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Structure for a password and match type (used in partial matching)
typedef struct {
    char str[128];
    int num;
} strnC;

// Function declarations
int isIn(int *, int, int);
int commandToCode(char *);
int generatePaswd(char ***, char *, const char *);
strnC* matchPartPaswd(char *, const char *);
int regex_compare(char *, char *);
int checkPrefix(char *, char *);
int checkSuffix(char *, char *);
int checkScattered(char *, char *);
int checkSubstr(char *, char *);
void initStrArr(char **, int);
void freeStrArr(char **, int);
void printStrArr(char **, int);
void initStrnCArr(strnC *, int);

int main(int argc, char* argv[]) {
    // Define command codes and wordlist path
    static const int HELP_CODE = 1;
    static const int GENERATE_CODE = 2;
    static const int PARTIAL_CODE = 3;
    static const int STRENGTHEN_CODE = 4; // unused in current code
    static const char WORDLIST[] = "/home/oribd/Documents/wordlists/rockyou.txt";

    if (argc > 3) {
        printf("Error: expected 1 command and possibly a password\n");
        return 0;
    }

    if (argc < 2) {
        printf("Error: must pass command from {--help/-h, --generate/-g, --partial/-p} and a password\n");
        return 0;
    }

    // Get numeric code for the given command
    int command = commandToCode(argv[1]);

    // Show help message
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

    // For all other commands, a password must be provided
    if (argc < 3) {
        printf("Error: for commands other than --help, must provide password\n");
        return 0;
    }

    // Handle password generation mode using regex
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

    // Handle partial match mode (prefix, suffix, substring, scattered subsequence)
    if (command == PARTIAL_CODE) {
        char *part = argv[2];
        strnC *matches = matchPartPaswd(part, WORDLIST);
        char *paswd;
        int i = 0;

        // Count how many matches were found (up to 10)
        while (i < 10 && matches[i].num != 0) {
            i++;
        }

        printf("%d matches found\n", i);

        // Print the match results with type description
        for (int j = 0; j < i; j++) {
            paswd = matches[j].str;
            int type = matches[j].num;

            switch (type) {
                case 1:
                    printf("%s, matchType: prefix\n", paswd);
                    break;
                case 2:
                    printf("%s, matchType: suffix\n", paswd);
                    break;
                case 3:
                    printf("%s, matchType: substring\n", paswd);
                    break;
                case 4:
                    printf("%s, matchType: scattered subsequence\n", paswd);
                    break;
                default:
                    printf("%s, matchType: unknown (%d)\n", paswd, type);
            }
        }

        // Free memory for match results
        free(matches);
        return 1;
    }

    // If command not recognized
    printf("Did not recognize any valid flags\n");
    return 1;
}

// Returns 1 if x is in array, else 0
int isIn(int *array, int arrLen, int x) {
    for (int i = 0; i < arrLen; i++) {
        if (array[i] == x) return 1;
    }
    return 0;
}

// Maps string command to integer code
int commandToCode(char command[]) {
    if (!strcmp(command, "--help") || !strcmp(command, "-h")) return 1;
    if (!strcmp(command, "--generate") || !strcmp(command, "-g")) return 2;
    if (!strcmp(command, "--partial") || !strcmp(command, "-p")) return 3;
    return 0;
}

// Generates passwords that match a regex-style pattern
int generatePaswd(char ***generatedPaswd, char paswd_regex[], const char WORDLIST[]) {
    *generatedPaswd = malloc(5 * sizeof(char *));
    initStrArr(*generatedPaswd, 5);
    int j = 0;

    FILE *wordlist = fopen(WORDLIST, "r");
    if (wordlist == NULL) {
        printf("Wordlist not found \n");
        return 0;
    }

    char paswd[128];
    while (fgets(paswd, sizeof(paswd), wordlist)) {
        paswd[strcspn(paswd, "\n")] = '\0';  // remove newline
        if (strlen(paswd) != strlen(paswd_regex)) continue;
        if (!regex_compare(paswd_regex, paswd)) continue;

        strcpy((*generatedPaswd)[j], paswd);
        j++;
        if (j == 5) return j;
    }

    if (j > 0) return j;

    // If no matches found, clean up memory
    freeStrArr(*generatedPaswd, 5);
    free(*generatedPaswd);
    return 0;
}

// Compares password against a regex-like pattern (*, #, @)
int regex_compare(char regex[], char str[]) {
    for (int i = 0; i < strlen(regex); i++) {
        if (regex[i] == '*' && !isalpha(str[i])) return 0;
        if (regex[i] == '#' && !isdigit(str[i])) return 0;
        if (regex[i] == '@' && isalnum(str[i])) return 0;
        if (isalnum(regex[i]) && regex[i] != str[i]) return 0;
    }
    return 1;
}

// Tries to find up to 10 passwords that partially match the given part
strnC* matchPartPaswd(char partPaswd[], const char WORDLIST[]) {
    strnC *matches = malloc(10 * sizeof(strnC));
    initStrnCArr(matches, 10);
    int total = 0;

    FILE *wordlist = fopen(WORDLIST, "r");
    if (!wordlist) {
        printf("Error: wordlist not found\n");
        return NULL;
    }

    char paswd[128];

    // Pass 1: Prefix match
    rewind(wordlist);
    while (fgets(paswd, sizeof(paswd), wordlist) && total < 10) {
        paswd[strcspn(paswd, "\n")] = '\0';
        if (checkPrefix(paswd, partPaswd)) {
            strcpy(matches[total].str, paswd);
            matches[total].num = 1;
            total++;
        }
    }

    // Pass 2: Suffix match
    rewind(wordlist);
    while (fgets(paswd, sizeof(paswd), wordlist) && total < 10) {
        paswd[strcspn(paswd, "\n")] = '\0';
        if (checkSuffix(paswd, partPaswd)) {
            strcpy(matches[total].str, paswd);
            matches[total].num = 2;
            total++;
        }
    }

    // Pass 3: Substring match
    rewind(wordlist);
    while (fgets(paswd, sizeof(paswd), wordlist) && total < 10) {
        paswd[strcspn(paswd, "\n")] = '\0';
        if (checkSubstr(paswd, partPaswd)) {
            strcpy(matches[total].str, paswd);
            matches[total].num = 3;
            total++;
        }
    }

    // Pass 4: Scattered subsequence match
    rewind(wordlist);
    while (fgets(paswd, sizeof(paswd), wordlist) && total < 10) {
        paswd[strcspn(paswd, "\n")] = '\0';
        if (checkScattered(paswd, partPaswd)) {
            strcpy(matches[total].str, paswd);
            matches[total].num = 4;
            total++;
        }
    }

    fclose(wordlist);
    return matches;
}

// Check if partPaswd is a prefix of paswd
int checkPrefix(char paswd[], char partPaswd[]) {
    for (int i = 0; i < strlen(partPaswd); i++) {
        if (paswd[i] != partPaswd[i]) return 0;
    }
    return 1;
}

// Check if partPaswd is a suffix of paswd
int checkSuffix(char paswd[], char partPaswd[]) {
    for (int i = 0; i < strlen(partPaswd); i++) {
        if (paswd[strlen(paswd) - 1 - i] != partPaswd[strlen(partPaswd) - 1 - i]) return 0;
    }
    return 1;
}

// Check if partPaswd appears scattered (non-contiguous) in paswd
int checkScattered(char paswd[], char partPaswd[]) {
    int found, k = -1;
    for (int i = 0; i < strlen(partPaswd); i++) {
        found = 0;
        for (int j = k + 1; j < strlen(paswd); j++) {
            if (paswd[j] == partPaswd[i]) {
                found = 1;
                k = j;
                break;
            }
        }
        if (!found) return 0;
    }
    return 1;
}

// Check if part is a contiguous substring of str
int checkSubstr(char str[], char part[]) {
    int lenStr = strlen(str);
    int lenPart = strlen(part);

    if (lenPart > lenStr) return 0;

    for (int i = 0; i <= lenStr - lenPart; i++) {
        int isSub = 1;
        for (int j = 0; j < lenPart; j++) {
            if (str[i + j] != part[j]) {
                isSub = 0;
                break;
            }
        }
        if (isSub) return 1;
    }
    return 0;
}

// Allocate memory for array of strings
void initStrArr(char *strArr[], int numStrings) {
    for (int i = 0; i < numStrings; i++) {
        strArr[i] = malloc(128 * sizeof(char));
    }
}

// Free memory for array of strings
void freeStrArr(char *strArr[], int numStrings) {
    for (int i = 0; i < numStrings; i++) {
        free(strArr[i]);
    }
}

// Print an array of strings
void printStrArr(char *strArr[], int numStrings) {
    for (int i = 0; i < numStrings; i++) {
        printf("%s\n", strArr[i]);
    }
}

// Initialize strnC array to default values (empty strings and 0 type)
void initStrnCArr(strnC arr[], int size) {
    for (int i = 0; i < size; i++) {
        arr[i].str[0] = '\0';
        arr[i].num = 0;
    }
}
