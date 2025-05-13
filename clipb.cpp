#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <libgen.h>

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

int main(int argc, char* argv[]) {
    string progname = basename(argv[0]);

    if (progname == "cl" && argc >= 2) {
        string slot_arg = argv[1];
        static const char* fake_args[4];
        fake_args[0] = "clipb";
        fake_args[1] = "exec";
        fake_args[2] = slot_arg.c_str();
        fake_args[3] = nullptr;

        argc = 3;
        argv = const_cast<char**>(fake_args);
    }

    if (argc < 2 || string(argv[1]) == "--help" || string(argv[1]) == "-h") {
        cout << "clipb v1.0.0 — your personal command clipboard\n\n"
             << "Usage:\n"
             << "  clipb add \"<command>\" <slot>\n"
             << "  clipb exec <slot>\n"
             << "  clipb list\n"
             << "  cl <slot>                (shortcut)\n"
             << "  clipb --version | -v\n"
             << "  clipb --help | -h\n";
        return 0;
    }
    
    if (string(argv[1]) == "--version" || string(argv[1]) == "-v") {
        cout << "clipb version 1.0.0\n";
        return 0;
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
        cerr << "Unknown command: " << action << "\n";
        cerr << "Try 'clipb --help' for usage.\n";
        return 1;
    }

    return 0;
}
