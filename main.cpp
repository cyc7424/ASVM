#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdlib>
using namespace std;
namespace fs = std::filesystem;

// Wrap a string in single quotes so the shell treats it literally.
static string shellQuote(const string &s) {
    string out = "'";
    for (char c : s) {
        if (c == '\'') out += "'\\''";
        else out += c;
    }
    out += "'";
    return out;
}

// Remove backslash escapes before spaces ("My\ VM" -> "My VM") so input in the
// old escaped style still works. Plain paths with spaces work as well.
static string unescapeSpaces(const string &s) {
    string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size() && s[i + 1] == ' ') continue;
        out += s[i];
    }
    return out;
}

static string trim(const string &s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

// Read one line; exit cleanly if input is closed (Ctrl+D).
static string readLine(const string &prompt) {
    cout << prompt << flush;
    string line;
    if (!getline(cin, line)) {
        cout << "\nInput closed, aborting." << endl;
        exit(1);
    }
    return trim(line);
}

// Ask a 1/2 question until a valid answer is given. Returns true for 1 (yes).
static bool askYesNo(const string &question) {
    while (true) {
        string ans = readLine(question + "\n1.yes 2.no (input 1 or 2): ");
        if (ans == "1") return true;
        if (ans == "2") return false;
        cout << "Invalid input, please enter 1 or 2." << endl;
    }
}

int main() {
    const string line(90, '=');
    const string dash(90, '-');
    string asciiArt = R"(
      ___           ___           ___           ___
     /\  \         /\  \         /\__\         /\__\
    /::\  \       /::\  \       /:/  /        /::|  |
   /:/\:\  \     /:/\ \  \     /:/  /        /:|:|  |
  /::\~\:\  \   _\:\~\ \  \   /:/__/  ___   /:/|:|__|__
 /:/\:\ \:\__\ /\ \:\ \ \__\  |:|  | /\__\ /:/ |::::\__\
 \/__\:\/:/  / \:\ \:\ \/__/  |:|  |/:/  / \/__/~~/:/  /
      \::/  /   \:\ \:\__\    |:|__/:/  /        /:/  /
      /:/  /     \:\/:/  /     \::::/__/        /:/  /
     /:/  /       \::/  /       ~~~~           /:/  /
     \/__/         \/__/                       \/__/
    )";
    cout << asciiArt << endl;
    cout << line << endl;
    cout << "Welcome to ASVM, a tool that starts a VMware virtual machine automatically at boot." << endl;
    cout << line << endl;

    // 1. VM directory
    string path;
    while (true) {
        path = unescapeSpaces(readLine(
            "Input the directory that contains your .vmx file\n"
            "e.g. /home/user/vmware/Windows XP Professional/\n> "));
        if (path.empty()) { cout << "Path cannot be empty." << endl; continue; }
        if (!fs::is_directory(path)) { cout << "Directory not found: " << path << endl; continue; }
        break;
    }

    // 2. .vmx file name
    cout << dash << endl;
    string vmxname;
    while (true) {
        vmxname = unescapeSpaces(readLine(
            "Input your .vmx file name\n"
            "e.g. Windows XP Professional.vmx\n> "));
        if (vmxname.empty()) { cout << "File name cannot be empty." << endl; continue; }
        if (!fs::is_regular_file(fs::path(path) / vmxname)) {
            cout << "File not found: " << (fs::path(path) / vmxname).string() << endl;
            continue;
        }
        break;
    }

    // 3. Fullscreen?
    cout << dash << endl;
    bool fullscreen = askYesNo("Do you want your VM to start in fullscreen?");

    // Generate asvm.sh in the current directory (absolute path used for cron).
    fs::path scriptPath = fs::absolute("asvm.sh");
    {
        ofstream outfile(scriptPath);
        if (!outfile) {
            cerr << "Error: cannot write " << scriptPath << endl;
            return 1;
        }
        outfile << "#!/bin/sh\n";
        outfile << "cd " << shellQuote(path) << " || exit 1\n";
        outfile << "vmware " << shellQuote(vmxname) << (fullscreen ? " -X" : " -x") << "\n";
    }
    error_code ec;
    fs::permissions(scriptPath,
                    fs::perms::owner_all | fs::perms::group_read | fs::perms::group_exec |
                        fs::perms::others_read | fs::perms::others_exec,
                    fs::perm_options::replace, ec);
    if (ec) {
        cerr << "Warning: could not make the script executable: " << ec.message() << endl;
    }
    cout << "Created " << scriptPath.string() << endl;

    // 4. crontab
    cout << dash << endl;
    if (!askYesNo("Do you want to add this script to your crontab for automatic startup?")) {
        cout << "The script has been created, but it will not start automatically. "
                "You can run it manually." << endl;
        return 0;
    }

    // '%' has a special meaning in crontab and must be escaped.
    string cronPath = shellQuote(scriptPath.string());
    string escaped;
    for (char c : cronPath) {
        if (c == '%') escaped += "\\%";
        else escaped += c;
    }
    string cronLine = "@reboot " + escaped;

    // Skip if the exact entry already exists.
    string checkCmd = "crontab -l 2>/dev/null | grep -qxF " + shellQuote(cronLine);
    if (system(checkCmd.c_str()) == 0) {
        cout << "This script is already in your crontab. Nothing changed." << endl;
        return 0;
    }

    string cronCommand = "(crontab -l 2>/dev/null; echo " + shellQuote(cronLine) + ") | crontab -";
    if (system(cronCommand.c_str()) == 0) {
        cout << "Successfully added 'asvm.sh' to crontab. The virtual machine will now start automatically at boot." << endl;
    } else {
        cout << "Error: Failed to add script to crontab. Please check your permissions or try adding it manually." << endl;
        return 1;
    }
    return 0;
}
