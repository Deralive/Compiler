@echo off

echo Building Lab 1 Tester
pushd EduCoderSubmission\Lab1
g++ -o tester.exe tester.cpp
popd
echo.

echo Building Lab 2 Tester
pushd EduCoderSubmission\Lab2
g++ -o tester.exe tester.cpp
popd
echo.

echo Building Lab 3 Tester
pushd EduCoderSubmission\Lab3
g++ -o tester.exe tester.cpp
popd
echo.

echo Building Lab 4 Tester
pushd EduCoderSubmission\Lab4
g++ -o tester.exe tester.cpp
popd
echo.

echo All Done.
pause