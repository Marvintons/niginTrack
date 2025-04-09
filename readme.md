# Nigin Track - Terminal Code Editor

🎯 A lightweight terminal-based code editor written in C++.  
Supports multi-file tab switching, line editing, and basic syntax highlighting for `.java` files — all inside your terminal!

---

## 🔧 Features

- File creation, editing, and saving
- Syntax highlighting for Java
- Multiline editing mode
- Tab-like file switching
- Simple command interface
- Built-in terminal-like file browser
- Cross-platform screen clear support

---

## 📦 How to Run

1. Clone the repo:
  
   git clone https://github.com/Marvintons/niginTrack.git
   cd niginTrack

2. Compile:
   g++ -std=c++17 -o niginTrack nt.cpp

3. Run:
    ./niginTrack

---
## 🧠 Commands Reference

open <file>        - Open existing file
touch <file>       - Create new file
switch <name/#>    - Switch between opened files
close <name/#>     - Close opened file
add <text>         - Add line to end
add .              - Enter multiline add mode (:q to quit)
insert <n> <text>  - Insert at line number
insert . <n>       - Multiline insert at line
replace <n> <txt>  - Replace specific line
save               - Save current file
show               - Show all lines
show <n>           - Focus view around line number
about              - Show app info
exit               - Exit the editor

---
## 👤 Author
Marvin
Version: Final 1.0
Nigin Track — Ninja Type Editor 💻🌀

