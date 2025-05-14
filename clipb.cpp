#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <libgen.h>
#include <stdexcept>

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

int safe_stoi(const string& str) {
    try {
        return stoi(str);
    } catch (const invalid_argument& e) {
        cerr << "Error: Invalid slot argument.\n";
        exit(1);
    } catch (const out_of_range& e) {
        cerr << "Error: Slot number out of range.\n";
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    string progname = basename(argv[0]);

    static string slot_arg_str;
    if (progname == "cl" && argc >= 2) {
        slot_arg_str = argv[1];
        static char* fake_args[4];
        fake_args[0] = const_cast<char*>("clipb");
        fake_args[1] = const_cast<char*>("exec");
        fake_args[2] = const_cast<char*>(slot_arg_str.c_str());
        fake_args[3] = nullptr;
        argc = 3;
        argv = fake_args;
    }

    if (argc < 2) {
        cout << "Usage:\n"
             << "  clipb add \"<command>\" <slot>\n"
             << "  clipb exec <slot>\n"
             << "  clipb remove <slot>\n"
             << "  clipb list\n"
             << "  cl <slot>                (shortcut)\n"
             << "  clipb --version | -v\n"
             << "  clipb --help    | -h\n";
        return 0;
    }

    string action = argv[1];

    if (action == "--help" || action == "-h") {
        cout << "clipb v1.0.0 — CLI clipboard\n\n"
             << "Usage:\n"
             << "  clipb add \"<command>\" <slot>\n"
             << "  clipb exec <slot>\n"
             << "  clipb remove <slot>\n"
             << "  clipb list\n"
             << "  cl <slot>                (shortcut)\n"
             << "  clipb --version | -v\n"
             << "  clipb --help    | -h\n";
        return 0;
    }

    if (action == "--version" || action == "-v") {
        cout << "clipb version 1.0.0\n";
        return 0;
    }

    auto commands = load_commands();

    if (action == "add") {
        if (argc < 4) {
            cerr << "Usage: clipb add \"<command>\" <slot>\n";
            return 1;
        }
        string command = argv[2];
        int slot = safe_stoi(argv[3]);

        if (commands.count(slot)) {
            char confirm;
            cout << "Slot " << slot << " already contains: " << commands[slot] << endl;
            cout << "Overwrite? (y/n): ";
            cin >> confirm;
            if (confirm != 'y' && confirm != 'Y') {
                cout << "Cancelled.\n";
                return 0;
            }
        }

        commands[slot] = command;
        save_commands(commands);
        cout << "Added to slot " << slot << ": " << command << endl;
    }
    else if (action == "exec") {
        if (argc < 3) {
            cerr << "Usage: clipb exec <slot>\n";
            return 1;
        }
        int slot = safe_stoi(argv[2]);
        if (commands.count(slot)) {
            cout << "Executing: " << commands[slot] << endl;
            system(commands[slot].c_str());
        } else {
            cerr << "No command in slot " << slot << endl;
        }
    }
    else if (action == "remove") {
        if (argc < 3) {
            cerr << "Usage: clipb remove <slot>\n";
            return 1;
        }
        int slot = safe_stoi(argv[2]);
        if (commands.erase(slot)) {
            save_commands(commands);
            cout << "Removed slot " << slot << endl;
        } else {
            cout << "No command found in slot " << slot << endl;
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
