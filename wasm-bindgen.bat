@echo off
echo 正在编译 core.cpp ...

emcc core.cpp -o lab.js -O3 --bind -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -s FORCE_FILESYSTEM=1 -std=c++17

if %errorlevel% equ 0 (
    echo.
    echo [编译成功] output: lab.js
) else (
    echo.
    echo [编译失败] 请检查错误信息
)

pause