# WinIconRefresh
Tiny executable to refresh modern Windows icons caches.

## The Issue
Windows is notorious for not updating/ re-drawing/ refreshing the cache of many things, including desktop/ explorer icons. For example, recently emptied folders will sometimes still display as having content, files whose associations have been changed will still display their icons as the previously associated exe, etc.

* Usually, one could have fixed this by opening the command prompt and executing
```
ie4uinit -show
```
* However, starting from Windows 11, ```ie4uinit.exe``` seems to have no effect whatsoever.
* In order to _fix_ the issue, I've just added a C++ file to manually call the OS-level function that updates the icons in Windows Explorer. It should work on Windows XP through 11.

Download the most recent ```icref.exe``` file from [here](https://github.com/twosixfoursix/WinIconRefresh/releases).

> Rest easy, there are no other calls except 1 OS call to refresh the icons, and the program never connects to the internet. Even a simple hex dump would attest to that.

Or if you're still on the fence, you may compile the [C++ file](https://github.com/twosixfoursix/WinIconRefresh/blob/main/icref/icref.cxx) on your own via:
```
cl /nologo /EHsc /c icref.cxx
```
and then linking the generated object file
```
link /nologo /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup icref.obj shell32.lib /OUT:icref.exe
```
