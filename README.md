## GhTrCLI 项目简介
**GhTrCLI** 是一个面向游戏 *GhTr* 的外置命令行工具，通过与游戏进程进行内存交互，来实现对游戏状态（如阳光数量等）的快速修改和操作。该项目目前主要针对《PlantsVsZombies GhTr ~ Perfect Voyage ver.0.17d》进行演示和测试，能够在检测到游戏进程后自动或手动进行附加（Attach）并提供简单的命令行交互功能。

---
[English](README-en.md)

## 功能特性
1. **自动检测并附加游戏进程**  
   - 启动时如检测到 `PlantsVsZombies.exe` 正在运行，可提示是否自动附加到该进程 。  
   - 也可以使用命令 `attach [进程名]` 手动附加到自定义进程。

2. **基础命令行交互**  
   - 内置多个命令，如 `help`、`cls`、`exit`、`quit` 等，用于查看帮助、清空屏幕、退出程序等操作 。

3. **游戏数据修改**  
   - 当前已支持修改游戏内“阳光数量” (`set-sun <数量>`)，在成功附加到目标进程后，可直接通过命令修改。该操作会在玩家存档中标记“帮助模式”。

4. **安全断开**  
   - 若已附加进程，也可通过 `exit` 或 `quit` 命令安全断开与目标进程的连接 。

5. **扩展性**  
   - 使用 `GhTrMemory` 类对游戏中内存偏移及读写进行统一管理 ；  
   - 使用 `ProcessHelper` 类进行进程的查找、打开、读取与写入等操作 ；  
   - 通过添加或修改命令函数，可轻松扩展更多游戏内操作。

---

## 项目结构
项目主要包含以下核心文件（省略部分辅助文件）：
- **GhTrCLI.cpp / GhTrCLI.h**  
  - 实现主命令行逻辑，包括命令注册、处理输入、执行命令等。
- **GhTrMemory.cpp / GhTrMemory.h**  
  - 管理并封装了对游戏内存（如“阳光值”）的读取和写入操作，包含对内存偏移量的计算和定位。
- **ProcessHelper.cpp / ProcessHelper.h**  
  - 提供进程操作的相关接口，如获取进程 ID、读取写入内存、获取模块基址等。
- **Logger.h**  
  - 输出日志和提示信息的工具类，支持文本高亮与颜色打印，便于命令行显示结果。
- **main.cpp**  
  - 项目入口，主要负责初始化控制台环境以及启动 `GhTrCLI` 的 `Run()` 方法。

---

## 编译与运行
1. **克隆仓库**  
   ```bash
   git clone https://github.com/Xcating/GhTrCLI.git
   cd GhTrCLI
   ```

2. **配置与编译**  
   - 使用 Visual Studio 或其他支持 CMake 的环境加载/编译本项目。  
   - 项目使用 Windows API，如需跨平台请自行适配相关进程读写接口。

3. **运行**  
   - 编译完成后，在生成的可执行文件所在目录下打开命令行，直接运行 `GhTrCLI.exe`。  
   - 若检测到 `PlantsVsZombies.exe` 正在运行，则会提示是否要进行进程附加。输入 `y` 或 `Y` 即可附加。

---

## 常用命令介绍
以下命令在程序启动后可直接输入使用：
1. **help**  
   - 查看帮助信息，列出所有内置命令和操作说明 。

2. **cls**  
   - 清空控制台输出，便于保持界面整洁。

3. **exit** 或 **quit**  
   - 退出 CLI 程序。如已附加进程，会在退出前自动断开连接 。

4. **attach**  
   - 尝试附加到 `PlantsVsZombies.exe` 进程，若检测到进程正在运行，会询问是否附加。  
   - 使用 `attach [进程名]` 可附加到指定的其他进程 。

5. **set-sun <阳光数量>**  
   - 修改当前附加游戏的阳光数值，需先用 `attach` 命令成功附加到进程后才可执行 。  
   - 示例：`set-sun 999` 会将阳光数量修改为 999。

---

## 注意事项
1. **版本兼容**  
   - 本工具默认针对《PlantsVsZombies GhTr ~ Perfect Voyage ver.0.17d》进行测试，其他版本可能存在内存偏移量不同而导致无法正常工作。
2. **反外挂或安全风险**  
   - 使用本工具修改游戏内存属于**测试和学习**目的，严禁使用本工具进行作弊。
3. **使用风险自担**  
   - 任何对进程和内存的修改均存在不稳定或崩溃的风险，请务必在确保安全的环境下测试使用。

---

## 贡献与反馈
- 欢迎提交 Issue 或 Pull Request，对本项目提出问题或改进建议。  
- 如果您对其他功能有需求或希望添加更多游戏内存修改选项，欢迎在项目中增加新的命令函数并更新对应的偏移值与逻辑。
- 如想提交 Pull Request 请保留源代码的风格一致和功能稳定。

感谢您使用 **GhTrCLI**！如有疑问或建议，欢迎随时讨论。祝您使用愉快。