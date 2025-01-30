## GhTrCLI Project Overview
**GhTrCLI** is an external command-line tool for the game _GhTr_, which allows for quick modifications and operations on game states (such as the number of suns) by interacting with the game's memory. Currently, the project is mainly demonstrated and tested on the version "PlantsVsZombies GhTr ~ Perfect Voyage ver.0.17d", and provides the ability to automatically or manually attach (Attach) to the game process once detected, offering simple command-line interactions.

---
[Chinese](README.md)
## Features
1. **Automatic Detection and Process Attachment**  
   - On startup, if `PlantsVsZombies.exe` is detected as running, it will prompt whether to automatically attach to the process.  
   - You can also manually attach to a custom process using the command `attach [process name]`.
2. **Basic Command-Line Interaction**  
   - Built-in commands like `help`, `cls`, `exit`, and `quit` for viewing help, clearing the screen, exiting the program, etc.
3. **Game Data Modification**  
   - Currently supports modifying the "sun count" (`set-sun <quantity>`). After successfully attaching to the target process, you can directly modify this value via the command. This operation will mark the player’s save file with "help mode".
4. **Safe Disconnection**  
   - If a process is attached, you can safely disconnect from the target process using the `exit` or `quit` commands.
5. **Extensibility**  
   - Uses the `GhTrMemory` class to manage game memory offsets and reading/writing operations.  
   - Uses the `ProcessHelper` class for operations like finding, opening, reading, and writing to processes.  
   - Additional game operations can easily be added by modifying or adding command functions.

---
## Project Structure
The project mainly includes the following core files (with some auxiliary files omitted):
- **GhTrCLI.cpp / GhTrCLI.h**  
  - Implements the main command-line logic, including command registration, input handling, and command execution.
- **GhTrMemory.cpp / GhTrMemory.h**  
  - Manages and encapsulates the reading and writing of game memory (e.g., "sun count"), including calculating and locating memory offsets.
- **ProcessHelper.cpp / ProcessHelper.h**  
  - Provides interfaces for process operations, such as obtaining process ID, reading and writing memory, and obtaining module base addresses.
- **Logger.h**  
  - A utility class for outputting logs and messages, supporting text highlighting and color printing for better command-line display.
- **main.cpp**  
  - The project entry point, mainly responsible for initializing the console environment and starting the `Run()` method of `GhTrCLI`.

---
## Compilation and Execution
1. **Clone the Repository**  
   ```bash
   git clone https://github.com/Xcating/GhTrCLI.git
   cd GhTrCLI
   ```
2. **Configuration and Compilation**  
   - Use Visual Studio or other CMake-supported environments to load/compile the project.  
   - The project uses the Windows API, so cross-platform compatibility requires adapting the process read/write interfaces.
3. **Running**  
   - After compilation, open a command-line terminal in the directory where the executable is located and run `GhTrCLI.exe`.  
   - If `PlantsVsZombies.exe` is running, it will prompt whether to attach. Input `y` or `Y` to proceed.

---
## Common Commands
The following commands can be directly entered after the program starts:
1. **help**  
   - View help information, listing all built-in commands and their descriptions.
2. **cls**  
   - Clear the console output for a cleaner interface.
3. **exit** or **quit**  
   - Exit the CLI program. If a process is attached, it will safely disconnect before exiting.
4. **attach**  
   - Attempt to attach to the `PlantsVsZombies.exe` process. If the process is detected, it will ask whether to attach.  
   - Use `attach [process name]` to attach to a specific process.
5. **set-sun <sun count>**  
   - Modify the current attached game's sun count. This command can only be executed after successfully attaching to the process using the `attach` command.  
   - Example: `set-sun 999` will change the sun count to 999.

---
## Notes
1. **Version Compatibility**  
   - This tool is tested specifically on `PlantsVsZombies GhTr ~ Perfect Voyage ver.0.17d`. Other versions may have different memory offsets, causing the tool to malfunction.
2. **Anti-Cheat or Security Risks**  
   - Modifying game memory using this tool is for **testing and learning** purposes only. Cheating using this tool is strictly prohibited.
3. **Use at Your Own Risk**  
   - Any modification to processes and memory carries the risk of instability or crashes. Please test and use in a safe environment.

---
## Contributing and Feedback
- Feel free to submit Issues or Pull Requests, report problems, or suggest improvements for the project.  
- If you need additional functionality or wish to add more game memory modification options, feel free to add new command functions and update the relevant offsets and logic in the project.
- When submitting a Pull Request, please maintain the consistency of the original code style and functionality.
Thank you for using **GhTrCLI**! If you have any questions or suggestions, feel free to discuss them anytime. Enjoy your experience!