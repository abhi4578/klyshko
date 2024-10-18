#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <iomanip>
#include <unistd.h>
using namespace std;

string addHex(const string& hex1, const string& hex2) {
    // Convert hex strings to unsigned long long integers
    unsigned long long num1, num2;
    stringstream ss1, ss2;

    ss1 << hex << hex1;
    ss1 >> num1;

    ss2 << hex << hex2;
    ss2 >> num2;

    // Add the two numbers
    unsigned long long sum = num1 + num2;

    // Convert the sum back to a hexadecimal string
    stringstream resultStream;
    resultStream << hex << setw(16) << setfill('0') << sum;

    return resultStream.str();
}

string readFile(const string& path) {
    ifstream file(path);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void execCommand(const vector<string>& cmd) {
    vector<char*> argv;
    for (const auto& arg : cmd) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr); // Null-terminate the array

    pid_t pid = fork();
    if (pid == 0) { // Child process
        execvp(argv[0], argv.data());
        perror("execvp failed"); // Execvp only returns on error
        exit(1);
    } else if (pid > 0) { // Parent process
        int status;
        //waitpid(pid, &status, 0); // Wait for child process to finish
    } else {
        perror("fork failed");
    }
}


void writeFile(const string& path, const string& content) {
    ofstream file(path);
    file << content;
}

void createDirectory(const string& path) {
    
    if (mkdir(path.c_str(), 0777) == -1)
		cerr << "Error : " << strerror(errno) << endl;

	else
		cout << "Directory created";
}

int main(){
    /**in carbynestack the base image is used to create the binary,
     over here we assume we already have .x */

    // env variables
    string KII_TUPLE_TYPE = "BIT_GFP";
    int KII_TUPLES_PER_JOB = 10000;
    int KII_PLAYER_NUMBER  = 1;
    int KII_PLAYER_COUNT = 2;
    
    int n = KII_TUPLES_PER_JOB;
    int pn = KII_PLAYER_NUMBER;
    int pc = KII_PLAYER_COUNT;
    
    //setting up the mapping between file and var
    unordered_map<string, string> argsByType = {
        {"BIT_GFP", "--nbits 0," + to_string(n)},
        {"BIT_GF2N", "--nbits " + to_string(n) + ",0"},
        {"INPUT_MASK_GFP", "--ntriples 0," + to_string(n/3)},
        {"INPUT_MASK_GF2N", "--ntriples " + to_string(n/3) + ",0"},
        {"INVERSE_TUPLE_GFP", "--ninverses " + to_string(n)},
        {"INVERSE_TUPLE_GF2N", "--ninverses " + to_string(n)},
        {"SQUARE_TUPLE_GFP", "--nsquares 0," + to_string(n)},
        {"SQUARE_TUPLE_GF2N", "--nsquares " + to_string(n) + ",0"},
        {"MULTIPLICATION_TRIPLE_GFP", "--ntriples 0," + to_string(n)},
        {"MULTIPLICATION_TRIPLE_GF2N", "--ntriples " + to_string(n) + ",0"}
    };

    unordered_map<string, string> tupleFileByType = {
        {"BIT_GFP", to_string(pc) + "-p-128/Bits-p-P" + to_string(pn)},
        {"BIT_GF2N", to_string(pc) + "-2-40/Bits-2-P" + to_string(pn)},
        {"INPUT_MASK_GFP", to_string(pc) + "-p-128/Triples-p-P" + to_string(pn)},
        {"INPUT_MASK_GF2N", to_string(pc) + "-2-40/Triples-2-P" + to_string(pn)},
        {"INVERSE_TUPLE_GFP", to_string(pc) + "-p-128/Inverses-p-P" + to_string(pn)},
        {"INVERSE_TUPLE_GF2N", to_string(pc) + "-2-40/Inverses-2-P" + to_string(pn)},
        {"SQUARE_TUPLE_GFP", to_string(pc) + "-p-128/Squares-p-P" + to_string(pn)},
        {"SQUARE_TUPLE_GF2N", to_string(pc) + "-2-40/Squares-2-P" + to_string(pn)},
        {"MULTIPLICATION_TRIPLE_GFP", to_string(pc) + "-p-128/Triples-p-P" + to_string(pn)},
        {"MULTIPLICATION_TRIPLE_GF2N", to_string(pc) + "-2-40/Triples-2-P" + to_string(pn)}
    };
    
    /** checking dictionary 
    for(auto x: argsByType)
    {
       cout << x.first << "->" <<
               x.second <<endl;
    }
    */
    
    //prime needs to be read from file
    string prime = readFile("etc/kii/params/prime");
    vector<string> fields = {"p", "2"};

    //check is mac key share being read
    for (const auto& f : fields) {
        string bit_width = (f == "p") ? "128" : "40";
        string folder = "/home/ankit/kii_c/Player-Data/" + to_string(pc) + "-" + f + "-" + bit_width;
        createDirectory(folder);
        cout << "Providing parameters for field " << f << "-" << bit_width << " in folder " << folder << endl;

        // Write MAC key shares for all players
        for (int playerNumber = 0; playerNumber < pc; ++playerNumber) {
            string macKeyShareFile = folder + "/Player-MAC-Keys-" + f + "-P" + to_string(playerNumber);
            string macKeyShare;

            if (playerNumber == pn) {
                macKeyShare = readFile("/etc/kii/secret-params/mac_key_share_" + f);
                cout << macKeyShare << endl;
            } else {
                macKeyShare = readFile("/etc/kii/extra-params/" + to_string(playerNumber) + "_mac_key_share_" + f);
                cout << macKeyShare << endl;
            }

            writeFile(macKeyShareFile, to_string(pc) + " " + macKeyShare);
            cout << "MAC key share for player " << to_string(playerNumber) << " written to " << macKeyShareFile << endl;
            
        }
    }

    //seed is given as env var 
    string hex1 = "1a2b3c4d5e6f7081";
    string hex2 = "ffffffffffffffff";

    string seed = hex1 + hex2;

    //execute .x file
    vector<string> cmd = {
        "./Fake-Offline.x",
        "-d",
        "0",
        "--prime",
        prime,
        "--prngseed",
        seed.substr(0, 16),
        KII_TUPLE_TYPE,
        to_string(pc)
    };

    execCommand(cmd);

}