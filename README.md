Rationale
=========
Sometimes you want to run a Windows console application with no window from a batch file, or another environment without easy access to CreateProcess.

Usage
=====
1. Rename this executable to [targetfilename].exe
2. The program will attempt to run [targetfilename], [targetfilename].cmd, or [targetfilename].bat
3. Command line paramaters for the target program can be passed directly to this program.

In the latter two cases (.cmd or .bat), the program will actually create a cmd.exe /c process.

In any case, this program will wait for the target to return, and return it's exit code.