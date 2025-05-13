#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include<libgen.h>
using namespace std;
namespace fs = filesystem;

const string CONFIG_DIR = string(getenv("HOME")) + "/.config/clipb/";
const string DB_FILE = CONFIG_DIR + "clipb_store.txt";

map<int, string> load_commands() {
    map<int, string> commands;
    ifstream file(DB_FILE);
    if (!file.is_open()) return commands;

    int key;
    string cmd;
    while (file >> key) {
        file.ignore(); 
        getline(file, cmd);
        commands[key] = cmd;
    }
    return commands;
}

void save_commands(const map<int, string>& commands) {
    fs::create_directories(CONFIG_DIR);
    ofstream file(DB_FILE);
    for (const auto& [key, cmd] : commands) {
        file << key << " " << cmd << endl;
    }
}

int main(int argc, char* argv[]) 
{
    char* base = basename(argv[0]);
string progname = base;

if (progname == "cl" && argc >= 2) 
{
    string slot_arg = argv[1];
    const char* fake_argv[] = {"clipb", "exec", slot_arg.c_str()};
    argc = 3;
    argv = const_cast<char**>(fake_argv);  // cast to modify argv
}

    if (argc < 2) {
        cout << "Usage:\n"
             << "  clipb add \"<command>\" <slot>\n"
             << "  clipb exec <slot>\n"
             << "  clipb list\n";
        return 1;
    }

    string action = argv[1];
    auto commands = load_commands();

    if (action == "add") {
        if (argc < 4) {
            cerr << "Usage: clipb add \"<command>\" <slot>\n";
            return 1;
        }
        string command = argv[2];
        int slot = stoi(argv[3]);
        commands[slot] = command;
        save_commands(commands);
        cout << "Added to slot " << slot << ": " << command << endl;
    }
    else if (action == "exec") {
        if (argc < 3) {
            cerr << "Usage: clipb exec <slot>\n";
            return 1;
        }
        int slot = stoi(argv[2]);
        if (commands.count(slot)) {
            cout << "Executing: " << commands[slot] << endl;
            system(commands[slot].c_str());
        } else {
            cerr << "No command in slot " << slot << endl;
        }
    }
    else if (action == "list") {
        if (commands.empty()) {
            cout << "No saved commands.\n";
        } else {
            cout << "Stored commands:\n";
            for (const auto& [slot, cmd] : commands) {
                cout << slot << ": " << cmd << endl;
            }
        }
    }
    else {
        cerr << "Unknown command.\n";
    }

    return 0;
}
