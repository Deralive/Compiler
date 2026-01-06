@echo off

echo ========== Lab 1 ==========
pushd EduCoderSubmission\Lab1
g++ -o tester.exe tester.cpp
tester.exe
popd
echo.

echo ========== Lab 2 ==========
pushd EduCoderSubmission\Lab2
g++ -o tester.exe tester.cpp
tester.exe
popd
echo.

echo ========== Lab 3 ==========
pushd EduCoderSubmission\Lab3
g++ -o tester.exe tester.cpp
tester.exe
popd
echo.

echo ========== Lab 4 ==========
pushd EduCoderSubmission\Lab4
g++ -o tester.exe tester.cpp
tester.exe
popd
echo.

echo All Done.
pause