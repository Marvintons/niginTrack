#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

map<string, vector<string>> openFiles;
vector<string> fileOrder;
string currentFile = "";

// Colors
#define RESET   "\033[0m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

#ifdef _WIN32
void clearScreen() { system("cls"); }
#else
void clearScreen() { system("clear"); }
#endif

void printLogo() {
    cout << "\n";
    cout << "  _   _ _       _        ___              _     \n";
    cout << " | \\ | ()     ()      |_   _|            | |    \n";
    cout << " |  \\| |_  _ _ _ _ _    | |_ _ _ _  _| | __ \n";
    cout << " | . ` | |/ ` | | ' \\   | | '_/ _` |/ _| |/ / \n";
    cout << " | |\\  | | (| | | | | |  | | | | (| | (__|   <  \n";
    cout << " || \\||\\, ||| ||  \\/|  \\_,|\\_||\\\\ \n";
    cout << "           __/ |                                \n";
    cout << "          |_/   Terminal Code Editor           \n\n";
}

bool isJavaFile(const string& filename) {
    return filename.size() >= 5 && filename.substr(filename.size() - 5) == ".java";
}

string highlightJava(const string& line) {
    static vector<string> keywords = {
        "public", "private", "protected", "class", "static", "void",
        "int", "double", "float", "if", "else", "return", "new", "for",
        "while", "String", "boolean", "char", "try", "catch", "throws"
    };

    stringstream ss(line);
    string word, result;

    while (ss >> word) {
        string cleanWord;
        for (char c : word)
            if (isalnum(c)) cleanWord += c;

        if (find(keywords.begin(), keywords.end(), cleanWord) != keywords.end())
            result += YELLOW + word + RESET + " ";
        else
            result += word + " ";
    }
    return result;
}

void showHeader() {
    cout << "+------------------- Open Files --------------------+\n";
    for (size_t i = 0; i < fileOrder.size(); ++i) {
        cout << "  [" << i + 1 << "] " << fileOrder[i];
        if (fileOrder[i] == currentFile) cout << "   <-- active";
        cout << endl;
    }
    cout << "+---------------------------------------------------+\n";
}

void showLines(int centerLine = -1) {
    if (currentFile.empty()) {
        cout << "No file selected.\n";
        return;
    }

    const auto& lines = openFiles[currentFile];
    int total = lines.size();
    int start = 0;
    int end = total;

    if (centerLine > 0) {
        start = max(0, centerLine - 6);
        end = min(total, centerLine + 5);
    }

    cout << "| Showing file: " << currentFile << "\n";
    for (int i = start; i < end; ++i) {
        cout << i + 1 << ": ";
        if (isJavaFile(currentFile)) cout << highlightJava(lines[i]) << endl;
        else cout << lines[i] << endl;
    }
    cout << "+---------------------------------------------------+\n";
}

void showTerminal() {
    cout << "| Terminal - Current Directory: " << fs::current_path() << "\n";
    cout << "| Files: ";
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        cout << entry.path().filename().string() << " ";
    }
    cout << "\n+---------------------------------------------------+\n";
}

void refreshUI(int centerLine = -1) {
    clearScreen();
    showHeader();
    showLines(centerLine);
    showTerminal();
}

void saveFile() {
    if (currentFile.empty()) return;
    ofstream out(currentFile);
    for (const string& line : openFiles[currentFile])
        out << line << endl;
    cout << "Saved " << currentFile << "\n";
}

void addLine(const string& content) {
    openFiles[currentFile].push_back(content);
    refreshUI();
}

void insertLine(int index, const string& content) {
    auto& lines = openFiles[currentFile];
    if (index < 1 || index > lines.size() + 1) {
        cout << "Invalid line number.\n";
        return;
    }
    lines.insert(lines.begin() + index - 1, content);
    refreshUI(index);
}

void replaceLine(int index, const string& content) {
    auto& lines = openFiles[currentFile];
    if (index < 1 || index > lines.size()) {
        cout << "Invalid line number.\n";
        return;
    }
    lines[index - 1] = content;
    refreshUI(index);
}

void loadFile(const string& path) {
    ifstream in(path);
    vector<string> lines;
    string line;
    while (getline(in, line)) lines.push_back(line);
    openFiles[path] = lines;
    fileOrder.push_back(path);
    currentFile = path;
    refreshUI();
}

void touchFile(const string& path) {
    ofstream(path).close();
    openFiles[path] = {};
    fileOrder.push_back(path);
    currentFile = path;
    refreshUI();
}

void switchToFile(const string& identifier) {
    if (isdigit(identifier[0])) {
        int idx = stoi(identifier);
        if (idx > 0 && idx <= fileOrder.size()) {
            currentFile = fileOrder[idx - 1];
            refreshUI();
        } else cout << "Invalid file index.\n";
    } else {
        if (openFiles.count(identifier)) {
            currentFile = identifier;
            refreshUI();
        } else cout << "File not found.\n";
    }
}

void closeFile(const string& identifier) {
    string target;
    if (isdigit(identifier[0])) {
        int idx = stoi(identifier);
        if (idx > 0 && idx <= fileOrder.size())
            target = fileOrder[idx - 1];
    } else target = identifier;

    auto it = find(fileOrder.begin(), fileOrder.end(), target);
    if (it != fileOrder.end()) {
        fileOrder.erase(it);
        openFiles.erase(target);
        if (currentFile == target) currentFile = fileOrder.empty() ? "" : fileOrder[0];
        refreshUI();
    } else {
        cout << "File not found.\n";
    }
}

void about() {
    cout << "\n--- About ---\n";
    cout << "Author: Marvin\nVersion: Final 1.0\n";
    cout << "Nigin Track - Terminal Code Editor\n";
}

void enterMultilineAddMode() {
    if (currentFile.empty()) {
        cout << "No file open.\n";
        return;
    }

    cout << "Multiline mode. Type ':q' to exit.\n";
    string line;
    while (true) {
        cout << "> ";
        getline(cin, line);
        if (line == ":q") break;
        openFiles[currentFile].push_back(line);
    }
    refreshUI();
}

void enterMultilineInsertMode(int index) {
    if (currentFile.empty()) {
        cout << "No file open.\n";
        return;
    }

    cout << "Multiline insert mode at line " << index << ". Type ':q' to exit.\n";
    string line;
    vector<string> newLines;

    while (true) {
        cout << "> ";
        getline(cin, line);
        if (line == ":q") break;
        newLines.push_back(line);
    }

    auto& lines = openFiles[currentFile];
    if (index < 1 || index > lines.size() + 1) {
        cout << "Invalid line number.\n";
        return;
    }

    lines.insert(lines.begin() + index - 1, newLines.begin(), newLines.end());
    refreshUI(index);
}

int main() {
    clearScreen();
    printLogo();
    cout << "Welcome to Ninja Type Editor!\n";
    cout << "Type help for command list.\n";

    string command;
    while (true) {
        cout << "\n(" << (currentFile.empty() ? "no file" : currentFile) << ") > ";
        getline(cin, command);
        stringstream ss(command);
        string cmd;
        ss >> cmd;

        if (cmd == "add") {
            string content;
            getline(ss, content);
            if (!content.empty() && content[0] == ' ') content.erase(0, 1);
            if (content == ".") enterMultilineAddMode();
            else addLine(content);
        } else if (cmd == "insert") {
            string first;
            ss >> first;
            if (first == ".") {
                int lineNum;
                ss >> lineNum;
                enterMultilineInsertMode(lineNum);
            } else {
                int lineNum = stoi(first);
                string content;
                getline(ss, content);
                if (!content.empty() && content[0] == ' ') content.erase(0, 1);
                insertLine(lineNum, content);
            }
        } else if (cmd == "replace") {
            int lineNum; ss >> lineNum;
            string content; getline(ss, content);
            if (!content.empty() && content[0] == ' ') content.erase(0, 1);
            replaceLine(lineNum, content);
        } else if (cmd == "save") saveFile();
        else if (cmd == "show") {
            string lineStr; ss >> lineStr;
            if (lineStr.empty()) refreshUI();
            else {
                int centerLine = stoi(lineStr);
                refreshUI(centerLine);
            }
        } else if (cmd == "about") about();
        else if (cmd == "exit") break;
        else if (cmd == "open") {
            string filename; ss >> filename;
            if (!filename.empty()) loadFile(filename);
        } else if (cmd == "touch") {
            string filename; ss >> filename;
            if (!filename.empty()) touchFile(filename);
        } else if (cmd == "switch") {
            string target; ss >> target;
            if (!target.empty()) switchToFile(target);
        } else if (cmd == "close") {
            string target; ss >> target;
            if (!target.empty()) closeFile(target);
        } else if (cmd == "help") {
            printLogo();
            cout << "\n+---------------- HELP MENU ----------------+\n";
            cout << "| Current File: " << (currentFile.empty() ? "[No file open]" : currentFile) << "\n";
            cout << "+------------------------------------------+\n";
            cout << "  open <path>       - Open existing file\n";
            cout << "  touch <file>      - Create and open file\n";
            cout << "  switch <name/#>   - Switch file tab\n";
            cout << "  close <name/#>    - Close a file tab\n";
            cout << "  add <text>        - Add line to file\n";
            cout << "  add .             - Enter multiline mode (:q to exit)\n";
            cout << "  insert <n> <text> - Insert at line n\n";
            cout << "  insert . <n>      - Multiline insert at line n (:q to exit)\n";
            cout << "  replace <n> <txt> - Replace line n\n";
            cout << "  save              - Save current file\n";
            cout << "  show              - Show all lines\n";
            cout << "  show <n>          - Show lines around line n\n";
            cout << "  about             - App info\n";
            cout << "  exit              - Exit editor\n";
            cout << "+------------------------------------------+\n";
        } else {
            cout << "Unknown command.\n";
        }
    }

    return 0;
}