#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>


// Step 1: Define an enumeration for tuple types
typedef enum {
    BIT_GFP,
    BIT_GF2N,
    INPUT_MASK_GFP,
    INPUT_MASK_GF2N,
    INVERSE_TUPLE_GFP,
    INVERSE_TUPLE_GF2N,
    SQUARE_TUPLE_GFP,
    SQUARE_TUPLE_GF2N,
    MULTIPLICATION_TRIPLE_GFP,
    MULTIPLICATION_TRIPLE_GF2N,
    TUPLE_TYPE_COUNT  // Count of types, useful for bounds checking
} TupleType;

// Step 2: Arrays for command arguments and file paths corresponding to the tuple types
const char *arg1ByType[TUPLE_TYPE_COUNT] = {
    "--nbits",    // BIT_GFP
    "--nbits",    // BIT_GF2N
    "--ntriples", // INPUT_MASK_GFP
    "--ntriples", // INPUT_MASK_GF2N
    "--ninverses",// INVERSE_TUPLE_GFP
    "--ninverses",// INVERSE_TUPLE_GF2N
    "--nsquares", // SQUARE_TUPLE_GFP
    "--nsquares", // SQUARE_TUPLE_GF2N
    "--ntriples", // MULTIPLICATION_TRIPLE_GFP
    "--ntriples"  // MULTIPLICATION_TRIPLE_GF2N
};

const char *arg2FormatByType[TUPLE_TYPE_COUNT] = {
    "0,%s",       // BIT_GFP
    "%s,0",       // BIT_GF2N
    "0,%d",       // INPUT_MASK_GFP (where n/3 is used)
    "%d,0",       // INPUT_MASK_GF2N (where n/3 is used)
    "%s",         // INVERSE_TUPLE_GFP
    "%s",         // INVERSE_TUPLE_GF2N
    "0,%s",       // SQUARE_TUPLE_GFP
    "%s,0",       // SQUARE_TUPLE_GF2N
    "0,%s",       // MULTIPLICATION_TRIPLE_GFP
    "%s,0"        // MULTIPLICATION_TRIPLE_GF2N
};

const char *tupleFileByType[TUPLE_TYPE_COUNT] = {
    "%s-p-128/Bits-p-P%s",   // BIT_GFP
    "%s-2-40/Bits-2-P%s",    // BIT_GF2N
    "%s-p-128/Triples-p-P%s",// INPUT_MASK_GFP
    "%s-2-40/Triples-2-P%s", // INPUT_MASK_GF2N
    "%s-p-128/Inverses-p-P%s",// INVERSE_TUPLE_GFP
    "%s-2-40/Inverses-2-P%s", // INVERSE_TUPLE_GF2N
    "%s-p-128/Squares-p-P%s", // SQUARE_TUPLE_GFP
    "%s-2-40/Squares-2-P%s",  // SQUARE_TUPLE_GF2N
    "%s-p-128/Triples-p-P%s", // MULTIPLICATION_TRIPLE_GFP
    "%s-2-40/Triples-2-P%s"   // MULTIPLICATION_TRIPLE_GF2N
};

// Helper function to convert string to TupleType enum
TupleType getTupleType(const char *tuple_type_str) {
    if (strcmp(tuple_type_str, "BIT_GFP") == 0) return BIT_GFP;
    if (strcmp(tuple_type_str, "BIT_GF2N") == 0) return BIT_GF2N;
    if (strcmp(tuple_type_str, "INPUT_MASK_GFP") == 0) return INPUT_MASK_GFP;
    if (strcmp(tuple_type_str, "INPUT_MASK_GF2N") == 0) return INPUT_MASK_GF2N;
    if (strcmp(tuple_type_str, "INVERSE_TUPLE_GFP") == 0) return INVERSE_TUPLE_GFP;
    if (strcmp(tuple_type_str, "INVERSE_TUPLE_GF2N") == 0) return INVERSE_TUPLE_GF2N;
    if (strcmp(tuple_type_str, "SQUARE_TUPLE_GFP") == 0) return SQUARE_TUPLE_GFP;
    if (strcmp(tuple_type_str, "SQUARE_TUPLE_GF2N") == 0) return SQUARE_TUPLE_GF2N;
    if (strcmp(tuple_type_str, "MULTIPLICATION_TRIPLE_GFP") == 0) return MULTIPLICATION_TRIPLE_GFP;
    if (strcmp(tuple_type_str, "MULTIPLICATION_TRIPLE_GF2N") == 0) return MULTIPLICATION_TRIPLE_GF2N;
    
    // Default case, handle unknown types (could be an error handling mechanism)
    return TUPLE_TYPE_COUNT;
}

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

void get_random_hex(char *hex_str, int length) {
    const char hex_chars[] = "0123456789abcdef";  // Use lowercase letters
    unsigned char random_bytes[length];

    // Get truly random bytes from the system
    if (getrandom(random_bytes, length, 0) == -1) {
        perror("getrandom");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < length; i++) {
        hex_str[i] = hex_chars[random_bytes[i] % 16];  // Convert to hex
    }

    hex_str[length] = '\0';  // Null-terminate the string
}
void writeFile(const char *filename, const char *text) {
    // Open the file for writing
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error openin file while writing");
        exit(1); // Exit if there's an error opening the file
    }

    // Write the text to the file
    fprintf(file, "%s", text);

    // Close the file
    fclose(file);
}

char* readFile(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error opening file while reading");
        return NULL;
    }

    // Allocate a buffer to hold the file contents
    fseek(file, 0, SEEK_END);  // Move to the end of the file
    long file_size = ftell(file);  // Get the current file pointer position (file size)
    fseek(file, 0, SEEK_SET);  // Move back to the start of the file

    char *content = malloc(file_size + 1);  // Allocate memory for file contents
    if (content == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    fread(content, 1, file_size, file);  // Read file contents into buffer
    content[file_size] = '\0';  // Null-terminate the string

    fclose(file);
    return content;  // Return the buffer
}

void createDirectory(const char* path) {
    
    if (mkdir(path, 0755) == -1) {
        printf("Error creating directory");
    }
    return;
}

void create_mac_key_shares(int pc, int pn) {
    
    const char* arr[2][2];

    arr[0][0] = "-88222337191559387830816715872691188861";
    arr[0][1] = "f0cf6099e629fd0bda2de3f9515ab72b";
    arr[1][0] = "1113507028231509545156335486838233835";
    arr[1][1] = "c347ce3d9e165e4e85221f9da7591d98";

    const char* fields[] = {"p", "2"};

    //check is mac key share being read
    //const char* homedir = "./";
    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); ++i) {
        const char* f = fields[i];
        const char* bit_width = (strcmp(f, "p") == 0) ? "128" : "40";
        
        //char folder[256];
        //snprintf(folder, sizeof(folder), "%sPlayer-Data/", homedir);
       
        char* folder = "Player-Data/";
        //createDirectory(folder);

        char folderPath[256];
        snprintf(folderPath, sizeof(folderPath), "%s%d-%s-%s", folder, pc, f, bit_width);
        createDirectory(folderPath);
        
        printf("Providing parameters for field %s-%s in folder %s\n", f, bit_width, folder);
        //data = exec(cat $varname)
        // Write MAC key shares for all players
        for (int playerNumber = 0; playerNumber < pc; ++playerNumber) {
            char macKeyShareFile[256];
            snprintf(macKeyShareFile, sizeof(macKeyShareFile), "%s/Player-MAC-Keys-%s-P%d", folderPath, f, playerNumber);

            char* macKeyShare;

            if (playerNumber == pn) {
                char file_path[256];
                sprintf(file_path, "etc/kii/secret-params/mac_key_share_%s",f);
                //macKeyShare = readFile(file_path);
                
                if(f == "p"){
                    macKeyShare = arr[pn][0];
                }
                else{
                    macKeyShare = arr[pn][1];
                }
                
                printf("%s\n", macKeyShare);
                
            } 
            else {
                char file_path[256];
                sprintf(file_path, "etc/kii/extra-params/%d_mac_key_share_%s", playerNumber, f);
                //macKeyShare = readFile(file_path);
                macKeyShare = "2";
                printf("%s\n", macKeyShare);
            }
            

            char dataToWrite[256];

            printf("----- TRYING TO WRITE for MAC key share for player %d written to %s\n", playerNumber, macKeyShareFile);
            snprintf(dataToWrite, sizeof(dataToWrite), "%d %s", pc, macKeyShare);
            writeFile(macKeyShareFile, dataToWrite);

            printf("MAC key share for player %d written to %s\n", playerNumber, macKeyShareFile);
            
        }
    }
}

void write_test_to_file() {
    // Directory and file paths
    const char *directory = "Player-Data";
    const char *file_path = "Player-Data/test";

    // Ensure the directory exists
    struct stat st = {0};
    /*
    if (stat(directory, &st) == -1) {
        if (mkdir(directory, 0755) != 0) {
            perror("Error creating directory");
            return;
        }
    }*/
    printf("LINE 253");
    // Open the file for writing (creates it if it doesn't exist)
    int fd = open(file_path, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("line 257 Error opening file");
        return;
    }
    printf("LINE 260");

    // Initialize a file lock structure
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;     // Set write lock
    lock.l_whence = SEEK_SET;  // Start from the beginning of the file

    // Apply the file lock
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        close(fd);
        return;
    }

    // Write "test" to the file using file descriptor
    if (write(fd, "test", 4) == -1) {
        perror("Error writing to file");
    }

    // Release the lock
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Error unlocking file");
    }

    // Close the file
    close(fd);
}

void main() {
    // Step 1: Declare these variables at the start
    printf("Program starts");
    char *n, *pn, *pc, *tuple_type_str, *prime, *job_id, *tuple_file;
    printf("step 1 complete");
    // write_test_to_file();
    // Step 2: Check command line input
    int input_value = 0;
    if (input_value == 0) {
        // Hardcoded values if input_value is 0
        n = "10000";
        pn = "0";
        pc = "2";
        tuple_type_str = "BIT_GFP";
        //prime = "198766463529478683931867765928436695041";
        prime = readFile("etc/kii/params/prime");
        job_id = "123456";
        tuple_file = "file.txt";
    } else {
        // Fetch from environment variables if input_value != 0
        n = getenv("KII_TUPLES_PER_JOB");
        pn = getenv("KII_PLAYER_NUMBER");
        pc = getenv("KII_PLAYER_COUNT");
        tuple_type_str = getenv("KII_TUPLE_TYPE");
        //prime = getenv("PRIME");
        prime = readFile("etc/kii/params/prime");
        job_id = getenv("KII_JOB_ID");
        tuple_file = getenv("KII_TUPLE_FILE");
    }
    printf("step 2 complete");

    // Step 3: Convert tuple type string to enum
    TupleType tuple_type = getTupleType(tuple_type_str);
    if (tuple_type == TUPLE_TYPE_COUNT) {
        fprintf(stderr, "Unknown tuple type: %s\n", tuple_type_str);
        return 1;
    }
    printf("step 3 complete");

    // Step 4: Prepare the second argument based on tuple type
    char arg2[256] = {0};
    if (strstr(arg2FormatByType[tuple_type], "%d") != NULL) {
        snprintf(arg2, sizeof(arg2), arg2FormatByType[tuple_type], atoi(n) / 3);
    } else {
        snprintf(arg2, sizeof(arg2), arg2FormatByType[tuple_type], n);
    }
    printf("step 4 complete");

    int player_count = atoi(pc);
    int player_number = atoi(pn);
    //create_mac_key_shares(player_count, player_number);
    printf("mac key created");

    // Step 5: Generate seed 

    char hex_str[17];
    /**get_random_hex(hex_str, 16);
    //we will get a seed from other TEE and add that 
    const char* hex2 = "1a2b3c4d5e6f7081";
    char* seed = addHex(hex_str, hex2);
    */
    char* seed = "1a2b3c4d5e6f7081";
    printf("step 5 complete:");

    // Step 6: Prepare arguments for execvp
    char *args[] = {
        "./Fake-Offline.x",
        "-d", "0",
        "--prime", prime,
        "--prngseed", seed,
        arg1ByType[tuple_type],  // The argument part 1 (e.g., --nbits)
        arg2,                    // The argument part 2 (e.g., 0,1000)
        pc,                      // Player count
        NULL                     // Terminate with NULL
    };
    printf("step 6 complete");

    // Debug print
    for (int i = 0; args[i] != NULL; ++i) {
        printf("%s ", args[i]);  // Print each argument followed by a space
    }
    printf("\n");

    // Step 7: Execute ./Fake-Offline.x using execvp
    execvp(args[0], args);
    printf("step 7 complete");

    // If execvp fails:
    perror("execvp failed");
    
}
