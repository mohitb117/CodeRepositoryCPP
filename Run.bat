@echo off

cd ClientGUI\bin\x86\Debug
start ClientGUI.exe

cd ..
cd ..
cd ..

cd ..
cd Debug
start RepositoryServer.exe 4117
pause