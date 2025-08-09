# WinIconRefresh
Tiny executable to refresh modern Windows icons caches.

## The Issue
Windows is notorious for not updating/ re-drawing/ refreshing the cache of many things, including desktop/ explorer icons. For example, recently emptied folders will sometimes still display as having content, files whose associations have been changed will still display their icons as the previously associated exe, etc.

* Usually, one could have fixed this by opening the command prompt and executing
```
ie4uinit -show
```
* However, starting from Windows 11, `ie4uinit.exe` seems to have no effect whatsoever.
* ~~In order to _fix_ the issue, I've just added a C++ file to manually call the OS-level function that updates the icons in Windows Explorer. It should work on Windows XP through 11~~.
* __UPDATE__: For the most recent versions of Windows 11, the previous methods will not work due to them being related to _Internet Explorer_ which Microsoft has apparently fully "phased out". From now on, the only way to reliably clear the icons cache & other lingering odd UI issues related to `explorer.exe` seems to be:
  * Navigate to `C:\Users\UserName\AppData\Local\Microsoft\Windows\Explorer\` & make a note of all the files with `.db` extension (We'll have to remove these).
  * Somehow find a way to _terminate the `explorer.exe` process_ & still manage to remove the aforementioned files.
  * Start a new instance of `explorer.exe`.
* The new code does exactly that and comes in the form of a tiny exe application package which allows avoiding odd batch scripts and powershell acrobatics as well as being able to call the program like any other traditional Windows process.

Download the most recent `icref.exe` file from [here](https://github.com/twosixfoursix/WinIconRefresh/releases).

> ~~Rest easy, there are no other calls except 1 OS call to refresh the icons, and the program never connects to the internet. Even a simple hex dump would attest to that~~. Multiple native function calls had to be made to carry out the entire process. The source code is fully available for you to manually build yourself or inspect.

You may compile the [C file](https://github.com/twosixfoursix/WinIconRefresh/blob/main/icref/icref.c) on your own via (via the Developer Command Prompt for VS):
```
cl /nologo /TC /std:c17 /cgthreads8 /Zc:strictStrings /Zc:wchar_t /Zc:inline /EHsc /W3 /D"_CRT_SECURE_NO_WARNINGS" /D"_UNICODE" /D"UNICODE" /GS /O2 /GL /MT /c icref.c
```
and then linking the generated object file
```
link /nologo /LTCG /OPT:REF /MACHINE:X64 /SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup /OUT:app.exe icref.obj user32.lib psapi.lib advapi32.lib
```
