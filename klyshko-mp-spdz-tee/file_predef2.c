//this is the kii file with all env variables hardcoded.
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* addHex(const char* hex1, const char* hex2) {
    // Convert hex strings to unsigned long long integers
    unsigned long long num1 = strtoull(hex1, NULL, 16);
    unsigned long long num2 = strtoull(hex2, NULL, 16);

    // Add the two numbers
    unsigned long long sum = num1 + num2;

    // Allocate memory for the result string (16 characters + 1 for null terminator)
    char* result = (char*)malloc(17);
    if (result == NULL) {
        return NULL; // Handle memory allocation failure
    }

    // Convert the sum back to a hexadecimal string
    snprintf(result, 17, "%016llx", sum);

    return result;
}

void execCommand(char *cmd[], int argCount) {
    // Null-terminate the array of arguments
    //cmd[argCount] = NULL; 
    printf("-------------------------------------%d\n", argCount);
    // Execute the command
    for (int i = 0; i < argCount; ++i) {
        printf("%s ", cmd[i]);  // Print each argument followed by a space
    }
    printf("\n");  // Newline after printing all arguments
    int status = execvp("./Fake-Offline.x", cmd);
    //printf("%s",*cmd);
    if (status == -1) {
        perror("execvp failed");
        exit(EXIT_FAILURE); // Exit if execvp fails
    }
}

void writeFile(const char *filename, const char *text) {
    // Open the file for writing
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(1); // Exit if there's an error opening the file
    }

    // Write the text to the file
    fprintf(file, "%s", text);

    // Close the file
    fclose(file);
}


void createDirectory(const char* path) {
    
    if (mkdir(path, 0755) == -1) {
        perror("Error creating directory");
    }
    return;
}

const char* argsByType(const char *key) {
    const char *keys[] = {
        "BIT_GFP",
        "BIT_GF2N",
        "INPUT_MASK_GFP",
        "INPUT_MASK_GF2N",
        "INVERSE_TUPLE_GFP",
        "INVERSE_TUPLE_GF2N",
        "SQUARE_TUPLE_GFP",
        "SQUARE_TUPLE_GF2N",
        "MULTIPLICATION_TRIPLE_GFP",
        "MULTIPLICATION_TRIPLE_GF2N"
    };

    const char *values[] = {
        "--nbits",
        "--nbits",
        "--ntriples",
        "--ntriples",
        "--ninverses",
        "--ninverses",
        "--nsquares",
        "--nsquares",
        "--ntriples",
        "--ntriples"
    };

    for (int i = 0; i < 10; i++) {
        if (strcmp(keys[i], key) == 0) {
            return values[i]; // Return the corresponding value
        }
    }
    return NULL; // Return NULL if key not found
}

const char* argvalue(const char* key,int n){
    const char *keys[] = {
        "BIT_GFP",
        "BIT_GF2N",
        "INPUT_MASK_GFP",
        "INPUT_MASK_GF2N",
        "INVERSE_TUPLE_GFP",
        "INVERSE_TUPLE_GF2N",
        "SQUARE_TUPLE_GFP",
        "SQUARE_TUPLE_GF2N",
        "MULTIPLICATION_TRIPLE_GFP",
        "MULTIPLICATION_TRIPLE_GF2N"
    };

    const char *values[] = {
        "0,10000",
        "10000,0",
        "0,3333",
        "3333,0",
        "10000",
        "10000",
        "0,10000",
        "10000,0",
        "0,10000",
        "10000,0"
    };

    for (int i = 0; i < 10; i++) {
        if (strcmp(keys[i], key) == 0) {
            return values[i]; // Return the corresponding value
        }
    }
    return NULL; // Return NULL if key not found
}
const char* tupleFileByType(const char *key) {
     const char *keys[] = {
        "BIT_GFP",
        "BIT_GF2N",
        "INPUT_MASK_GFP",
        "INPUT_MASK_GF2N",
        "INVERSE_TUPLE_GFP",
        "INVERSE_TUPLE_GF2N",
        "SQUARE_TUPLE_GFP",
        "SQUARE_TUPLE_GF2N",
        "MULTIPLICATION_TRIPLE_GFP",
        "MULTIPLICATION_TRIPLE_GF2N"
    };

    const char *values[] = {
        "2-p-128/Bits-p-P1",
        "2-2-40/Bits-2-P1",
        "2-p-128/Triples-p-P1",
        "2-2-40/Triples-2-P1",
        "2-p-128/Inverses-p-P1",
        "2-2-40/Inverses-2-P1",
        "2-p-128/Squares-p-P1",
        "2-2-40/Squares-2-P1",
        "2-p-128/Triples-p-P1",
        "2-2-40/Triples-2-P1"
    };

    for (int i = 0; i < 10; i++) {
        
        if (strcmp(keys[i], key) == 0) {
            return values[i]; // Return the corresponding value
        }
    }
    return NULL; // Return NULL if key not found
}

int main(){
    /**in carbynestack the base image is used to create the binary,
     over here we assume we already have .x */

    // env variables
    char* KII_TUPLE_TYPE = "BIT_GFP";
    int KII_TUPLES_PER_JOB = 10000;
    int KII_PLAYER_NUMBER  = 1;
    int KII_PLAYER_COUNT = 2;
    
    //seed is given as env var (seed = hex1 + hex2)
    const char* hex1 = "1a2b3c4d5e6f7081";
    const char* hex2 = "ffffffffffffffff";

    /**prime read from file originally
    string prime = readFile("etc/kii/params/prime");*/
    const char* prime = "198766463529478683931867765928436695041";

    //mac_key shares
    //p = 0, 2 = 1
    const char* arr[2][2];

    arr[0][0] = "-88222337191559387830816715872691188861";
    arr[0][1] = "f0cf6099e629fd0bda2de3f9515ab72b";
    arr[1][0] = "1113507028231509545156335486838233835";
    arr[1][1] = "c347ce3d9e165e4e85221f9da7591d98";

    int n = KII_TUPLES_PER_JOB;
    int pn = KII_PLAYER_NUMBER;
    int pc = KII_PLAYER_COUNT;
    
    //setting up the mapping between file and var
  
    const char* fields[] = {"p", "2"};

    //check is mac key share being read
    const char* homedir = "./";
    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); ++i) {
        const char* f = fields[i];
        const char* bit_width = (strcmp(f, "p") == 0) ? "128" : "40";
        
        char folder[256];
        snprintf(folder, sizeof(folder), "%sPlayer-Data/", homedir);
        createDirectory(folder);

        char folderPath[256];
        snprintf(folderPath, sizeof(folderPath), "%s%d-%s-%s", folder, pc, f, bit_width);
        createDirectory(folder);
        
        printf("Providing parameters for field %s-%s in folder %s\n", f, bit_width, folder);
        //data = exec(cat $varname)
        // Write MAC key shares for all players
        for (int playerNumber = 0; playerNumber < pc; ++playerNumber) {
            char macKeyShareFile[256];
            snprintf(macKeyShareFile, sizeof(macKeyShareFile), "%s/Player-MAC-Keys-%s-P%d", folderPath, f, playerNumber);

            const char* macKeyShare;

            if (playerNumber == pn) {
                //macKeyShare = readFile("/etc/kii/secret-params/mac_key_share_" + f);
                if(f == "p"){
                    macKeyShare = arr[pn][0];
                }
                else{
                    macKeyShare = arr[pn][1];
                }
                printf("%s\n", macKeyShare);
            } else {
                //macKeyShare = readFile("/etc/kii/extra-params/" + to_string(playerNumber) + "_mac_key_share_" + f);
                macKeyShare = " ";
                printf("%s\n", macKeyShare);
            }

            char dataToWrite[256];
            snprintf(dataToWrite, sizeof(dataToWrite), "%d %s", pc, macKeyShare);
            writeFile(macKeyShareFile, dataToWrite);

            printf("MAC key share for player %d written to %s\n", playerNumber, macKeyShareFile);
            
        }
    }

    char* seed = addHex(hex1, hex2);
    //printf("Sum: %s\n", result);

    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%d", pc);
    char *playerNo = buffer;
    
    /**converting pointer to actual value
    const char* z = argsByType(KII_TUPLE_TYPE);
    const char a = z;
    char b = &playerNo;
    */

    //execute .x file
    char *cmd[] = {
        "./Fake-Offline.x",
        playerNo,
        //playerNo,  
        "-d",
        "0",
        "--prime",
        (char* )prime,
        "--prngseed",
        strndup(seed, 16), // Use strndup to copy first 16 characters of seed
        argsByType(KII_TUPLE_TYPE), 
        argvalue(KII_TUPLE_TYPE,n),
        //"0,0",
        NULL
        //(char *)malloc(12) // Placeholder for player count as string
    };
    
    
     // Convert player count to string
    //snprintf(cmd[8], 12, "%d", pc);

    // Call the execCommand function with the command
    execCommand(cmd, 9); 
    free(seed);

}