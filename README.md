# RogerLux (C shell)

RogerLux (`rsh`) is a mini Unix shell implemented in C. It reads user commands, parses input, and executes them using POSIX APIs. It supports both built-in commands and external programs, with features such as sequential and conditional execution, I/O redirection, and background processes.

## Table of Contents

- [Features](#features)
- [Getting Starded](#getting-started)
- [Installation](#installation)
- [Usage](#usage)
- [Examples](#examples)
- [Implementation Details](#implementation-details)
- [Acknowledgements](#acknowledgments)

## Features

- **Built-in commands**
    - `cd (directory)`: change the current working directory.
 (~ for home, no arguments for home as well)
    - `exit`: exit the shell with the according status code.
    - `history`: display previously executed commands.
- **Command history**
    - Persistent across sessions via `.rshell_history` in the user's home directory.
    - Navigate history using the up and down arrow keys.
- **Execution flow**
    - Sequential commands with `;`.
    - Conditional execution using `&&` execute next if previous succeeded and `||` execute next if previous failed.
- **External commands**
     - Executed via `execvp`. Supports full path execution.
- **I/O redirection**
    - `>` to overwrite output to a file.
    - `>>` to append output to a file.
    - `<` to use file content as input.
- **Background processes**
     - `&` at the end of a command runs it in the background.
     - Shell tracks background PIDs and prints a notification when they terminate.
- **User input handling**
     - Tokenizes input considering spaces and special characters.
     - Supports `~` expansion for home directory.

## Getting-Started

### Prerequisites

- Ubuntu 18.04+ or any Linux-based OS.
- GCC compiler.
- Git installed to clone the repository.

```bash
sudo apt-get install git gcc
```

### Installation

1. Clone the repository:

```bash
git clone https://github.com/Blgr-Fish/Cshell.git

cd Cshell
```

2. Compile the source code using the Makefile:

```bash
make
```

3. Run the shell:

```bash
./bin/main.out
```

## Usage

- Interactive mode: type commands directly.
- Commands are executed immediately or according to the separators `;`, `&&`, `||`.
- Use `&` at the end of a command to run it in the background.
- Access history using up/down arrows.

## Examples

Sequential and conditional commands

```bash
╭─(/home/linux/Cshell [ ✓ ])
╰─>ls -l /nonexistent || echo "Failed!"
ls: cannot access '/nonexistent': No such file or directory
"Failed!"

╭─(/home/linux/Cshell [ ✓ ])
╰─>echo "Hello" && echo "World"
"Hello"
"World"

╭─(/home/linux/Cshell [ ✓ ])
╰─>ls ; echo "Done"
LICENSE  Makefile  README.md  bin  src
"Done"
```

Background processes

```bash
╭─(/home/linux/Cshell [ ✓ ])
╰─>sleep 2 &
[-] 2360 in background

╭─(/home/linux/Cshell [ ✓ ])
╰─>[-] 2360 terminated
```

I/O redirection

```bash
╭─(/tmp/ex [ ✓ ])
╰─>echo "Hello World" > output.txt

╭─(/tmp/ex [ ✓ ])
╰─>cat output.txt
"Hello World"

╭─(/tmp/ex [ ✓ ])
╰─>echo "Again" >> output.txt

╭─(/tmp/ex [ ✓ ])
╰─>cat output.txt
"Hello World"
"Again"
```

## Implementation-Details

- **Processes**
    - Forks a child for each external command.
    - Foreground processes: shell waits using `waitpid`.
    - Background processes: terminated processes cleaned with SIGCHLD handler to prevent zombie processes.
- **Parsing**
     - Input is tokenized with `strtok`.
    - Logical separators `;`, `&&`, `||` and redirection `>`, `>>` are handled during parsing.
     - Background flag set if `&` detected.
- **Memory management**
     - Commands and arguments dynamically allocated.
    - Proper freeing after execution to avoid leaks.
- **History**
    - Stored in memory and persistent file.
    - Implemented as dynamic array, navigable with arrow keys.


## Version

- 1.0.0

## Authors

- **Ziad Ijja** - https://github.com/Blgr-Fish

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Danish Prake for his article about how to write a shell in C (https://danishpraka.sh/posts/write-a-shell)
- Stephen Brennan for his tutorial (https://brennan.io/2015/01/16/write-a-shell-in-c)
- Stack Overflow for handling memory management and signals.
- Holberton School tutorials on shell projects.
