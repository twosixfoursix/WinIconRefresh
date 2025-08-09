#include <windows.h>
#include <stdio.h>
#include <handleapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <psapi.h>
#include <wchar.h>
#include <winnt.h>
#include <winuser.h>

#define FETCH 15

int fetcexplp(void)
{
  DWORD pidarray[512] = {0}, pidlen = sizeof(pidarray), magnitude, pcount;
  if(!EnumProcesses(pidarray, pidlen, &magnitude))
  {
    MessageBoxW(NULL, L"Fault during process enumeration.", L"WinIconRefresh", MB_OK | MB_ICONWARNING);
    return -1;
  }
  pcount = magnitude / sizeof(DWORD);
  HANDLE p;
  HMODULE pmod;
  DWORD pmodlen = sizeof(HMODULE);
  wchar_t pname[FETCH + 1];
  wchar_t *target = L"explorer.exe\0";
  wchar_t *tchar, *cchar;
  for(DWORD u = 0; u < pcount; ++u)
  {
    if(pidarray[u])
    {
      p = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pidarray[u]);
      if(p)
      {
        if(EnumProcessModules(p, &pmod, pmodlen, &magnitude))
        {
          if(GetModuleBaseNameW(p, pmod, pname, FETCH) == 12)
          {
            pname[12] = 0;
            tchar = target;
            cchar = pname;
            while((towlower(*cchar++) == *tchar++));
            if(*cchar == L'\0')
            {
              CloseHandle(p);
              p = OpenProcess(PROCESS_TERMINATE, FALSE, pidarray[u]);
              if(!p)
              {
                MessageBoxW(NULL, L"Could not fetch target process.", L"WinIconRefresh", MB_OK | MB_ICONWARNING);
                return 0;
              }
              TerminateProcess(p, 0);
              CloseHandle(p);
              return 1;
            }
          }
        }
        CloseHandle(p);
      }
    }
  }
  // printf("%d\n", pcount);
  return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE appinst, _In_opt_ HINSTANCE prevInst, _In_ LPWSTR warg, _In_ int cmdview)
{
  HWND bottom;
  if((bottom = FindWindowW(L"Shell_TrayWnd", NULL))) // Try fetch explorer process using fast method first.
  {
    DWORD pid;
    GetWindowThreadProcessId(bottom, &pid);
    if(pid)
    {
      HANDLE p = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
      if(p)
      {
        TerminateProcess(p, 0); // Terminate explorer & get ready for old icon data removal.
        CloseHandle(p);
      }
      else
      {
        MessageBoxW(NULL, L"Could not fetch target process.", L"WinIconRefresh", MB_OK | MB_ICONWARNING);
        return -1;
      }
    }
    else if(!fetcexplp()) // Try fetch explorer process the long way.
    {
      MessageBoxW(NULL, L"Fault: <fetcexplp>.", L"WinIconRefresh", MB_OK | MB_ICONWARNING);
      return -1;
    }
  }
  else if(!fetcexplp()) // Try fetch explorer process the long way.
  {
    MessageBoxW(NULL, L"Fault: <fetcexplp>.", L"WinIconRefresh", MB_OK | MB_ICONWARNING);
    return -1;
  }

  // Removal phase.
  wchar_t uname[128];
  DWORD unamelen = sizeof(uname);
  if(!GetUserNameW(uname, &unamelen)) // Need to fetch current user name.
  {
    MessageBoxW(NULL, L"Could not fetch user name of this PC.", L"WinIconRefresh", MB_OK | MB_ICONWARNING);
    return -1;
  }
  // Prepare target path holding the icon files.
  wchar_t targetpath[MAX_PATH] = L"C:\\Users\\", *postfix = L"\\AppData\\Local\\Microsoft\\Windows\\Explorer\\*.db";
  wchar_t *u = uname, *t = targetpath, *p = postfix;
  size_t winduplen = 5;
  while(*++t);
  while((*t++ = *u++));
  --t;
  while((*t++ = *p++));
  // wprintf(L"%s\n", targetpath);

  // Try & remove all .db files at target path.
  WIN32_FIND_DATAW fprop;
  HANDLE hFind = FindFirstFileW(targetpath, &fprop);
  if(hFind != INVALID_HANDLE_VALUE)
  {
    do {
      t -= winduplen;
      u = fprop.cFileName;
      winduplen = wcsnlen_s(u, MAX_PATH) + 1;
      while((*t++ = *u++));
      // wprintf(L"%s\n", targetpath);
      DeleteFileW(targetpath);
    } while(FindNextFileW(hFind, &fprop));
    FindClose(hFind);
  }

  STARTUPINFOW uinfo;
  PROCESS_INFORMATION pinfo;
  ZeroMemory(&uinfo, sizeof(uinfo));
  uinfo.cb = sizeof(uinfo);
  ZeroMemory(&pinfo, sizeof(pinfo));
  Sleep(500); // Avoid some Windows issues of race conditions...
  CreateProcessW(L"C:\\Windows\\explorer.exe", L"", NULL, NULL, FALSE, 0, NULL, NULL, &uinfo, &pinfo);
  WaitForSingleObject(pinfo.hProcess, 0); // Try 'restart' explorer process.
  CloseHandle(pinfo.hProcess);
  CloseHandle(pinfo.hThread);

  return 0;
}
