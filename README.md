# GitGetVer
This is small console utility which creates file revision.h and prints there current commit number as C/C++ define.

## Building
I din't put any project files for becase it is really small program. You can do it yourself.

I used VS2013 to build it.

## Running
To run it GitGetVer executable (or workfolder) should be inside of repository folder.
The PATH environment variable should contain path to git.exe to be able get data.


Arguments:
```
-d             enables build date output
-o <file>      output file path. default=revision.h
```

## License
GitGetVer is licensed under the GNU GPL v3 (see LICENSE file).
