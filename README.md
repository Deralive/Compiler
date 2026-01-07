# Compilers

本仓库为学号 10235101526 的编译原理与技术实践期末项目。

## Test and Verify

进入 [https://deralive.github.io/Compiler](https://deralive.github.io/Compiler) 可以进行四个实验的输出验证（每个实验均自带测试集）。

## Build

项目核心逻辑采用 C++ 编写，通过 **Emscripten** 编译链生成 WebAssembly，实现在浏览器端的本地高性能计算。

> [!WARNING]
验证亮点阶段在前端内即可完成。若需要从零开始复现本前端，请遵循以下在 Windows 下的完整复现流程。

### 环境准备

请确保已安装 [Git](https://git-scm.com/downloads) 与 [Python](https://www.python.org/downloads/)，接下来安装 Emscripten (EMSDK):

```powershell
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
./emsdk_env.bat
```

确保 `emcc -v` 有输出结果，再克隆本仓库，在根目录执行：

```powershell
./wasm-bindgen.bat
```

由于本地浏览器安全策略限制，必须使用 Live WebSever 才能读取到编译生成的 `.wasm` 模块。
1. 可以先安装 VSCode 的 Live Server 插件.
2. 并右击 `index.html` 选择 `Open with Live Server`.

## Testsets

在 `EduCoderSubmission` 下，项目为每一个实验编写了一个 `tester.cpp`，并配备了一键运行脚本，可以对测试用例的正确性进行验证。

### Windows

```bash
./tests.bat
```

### Linux / MacOS

```bash
chmod +x tests.sh
./tests.sh
```

项目中已经带有已编译的 `.exe` 文件，但如果需要亲手构建测试模块，可以使用：

### Windows

```bash
./build.bat
```

### Linux / MacOS

```bash
chmod +x build.sh
./build.sh
```

## 项目架构

```tree
Compiler/
├── .clang-format           # 代码格式化配置
├── README.md               # 项目主文档
├── build.bat               # Windows构建脚本
├── build.sh                # Linux构建脚本
├── tests.bat               # Windows测试脚本
├── tests.sh                # Linux测试脚本
├── EduCoderSubmission/     # EduCoder评测提交目录
│   ├── Lab1/               # 实验1：词法分析
│   │   ├── C_LexAnalysis_mainProcess.cpp # 主程序入口
│   │   ├── C_LexAnalysis_mainProcess.exe # 编译产物
│   │   ├── LexAnalysis.h   # 核心逻辑头文件
│   │   ├── c_keys.txt      # 关键字定义文件
│   │   ├── tester.cpp      # 本地测试单元
│   │   └── tester.exe      # 测试程序产物
│   ├── Lab2/               # 实验2：LL(1)语法分析
│   │   ├── LL1Parser.h     # 核心逻辑头文件
│   │   ├── LLmain.cpp      # 主程序入口
│   │   ├── LLmain.exe      # 编译产物
│   │   ├── tester.cpp      # 本地测试单元
│   │   └── tester.exe      # 测试程序产物
│   ├── Lab3/               # 实验3：LR语法分析
│   │   ├── LRparser.h      # 核心逻辑头文件
│   │   ├── LRparserMain.cpp # 主程序入口
│   │   ├── tester.cpp      # 本地测试单元
│   │   └── tester.exe      # 测试程序产物
│   ├── Lab4/               # 实验4：语义分析
│   │   ├── TranslationSchema.h # 核心逻辑头文件
│   │   ├── TranslationSchemaMain.cpp # 主程序入口
│   │   ├── tester.cpp      # 本地测试单元
│   │   └── tester.exe      # 测试程序产物
│   └── LabDocs/            # 实验报告文档
│       ├── lab1.md         # Lab 1 实验要求
│       ├── lab2.md         # Lab 2 实验要求
│       ├── lab3.md         # Lab 3 实验要求
│       └── lab4.md         # Lab 4 实验要求
└── Wasm/                   # WebAssembly前端构建目录
    ├── LL1Parser.h         # 复用的语法逻辑
    ├── LRParser.h          # 复用的LR逻辑
    ├── LexAnalysis.h       # 复用的词法逻辑
    ├── RegexGraph.h        # 正则可视化组件
    ├── TranslationSchema.h # 复用的语义逻辑
    ├── core.cpp            # Wasm绑定入口源文件
    ├── index.html          # 前端交互主页
    ├── lab.js              # 编译生成的JS胶水代码
    ├── lab.wasm            # 编译生成的二进制文件
    └── wasm-bindgen.bat    # Wasm构建辅助脚本
```