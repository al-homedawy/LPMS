#include <Windows.h>
#include <sstream>

using namespace std;

void la ( LPVOID lpAddr )
{
	stringstream ss;
	ss << lpAddr;

	string str;
	str += ss.str ();

	MessageBoxA ( 0, str.c_str (), 0, 0 );
}

//

// SendMessageA
LRESULT WINAPI SMX (
  __in  HWND hWnd,
  __in  UINT Msg,
  __in  WPARAM wParam,
  __in  LPARAM lParam
  )
{
	return SendMessageA ( hWnd, Msg, wParam, lParam );
	/*
	DWORD dwSend = (DWORD) GetProcAddress ( GetModuleHandle ( "user32.dll" ), "SendMessageA" ) + 5;

	__asm {
		mov edi, edi
		push ebp
		mov esp, ebp
		jmp [dwSend]
	}*/
}

// VirtualQuery
SIZE_T WINAPI VQ (
  __in_opt  LPCVOID lpAddress,
  __out     PMEMORY_BASIC_INFORMATION lpBuffer,
  __in      SIZE_T dwLength
  )
{
	return VirtualQuery ( lpAddress, lpBuffer, dwLength );
	/*
	DWORD dwQuery = (DWORD) GetProcAddress ( GetModuleHandle ( "kernel32.dll" ), "VirtualQuery" ) + 5;

	__asm {
		mov edi, edi
		push ebp
		mov esp, ebp
		jmp [dwQuery]
	}
	*/
}
// Module32First
BOOL WINAPI M32F (
  __in     HANDLE hSnapshot,
  __inout  LPMODULEENTRY32 lpme
)
{
	return Module32First ( hSnapshot, lpme );

	/*
	DWORD dwM32F = (DWORD) GetProcAddress ( GetModuleHandle ( "kernel32.dll" ), "Module32First" ) + 5;

	__asm {
		mov edi, edi
		push ebp
		mov esp, ebp
		jmp [dwM32F]
	}
	*/
}
// Module32Next
BOOL WINAPI M32N (
  __in   HANDLE hSnapshot,
  __out  LPMODULEENTRY32 lpme
)
{
	return Module32Next ( hSnapshot, lpme );

	/*
	DWORD dwM32N = (DWORD) GetProcAddress ( GetModuleHandle ( "kernel32.dll" ), "Module32Next" ) + 5;

	__asm {
		mov edi, edi
		push ebp
		mov esp, ebp
		jmp [dwM32N]
	}
	*/
}
// DialogBoxA
INT_PTR WINAPI DB (
  __in_opt  HINSTANCE hInstance,
  __in      LPCTSTR lpTemplate,
  __in_opt  HWND hWndParent,
  __in_opt  DLGPROC lpDialogFunc
)
{
	return DialogBox ( hInstance, lpTemplate, hWndParent, lpDialogFunc );
}
// GetAsyncKeyState 
SHORT WINAPI GKEY (
  __in  int vKey
)
{
	return GetKeyState ( vKey );
	/*
	DWORD dwKey = (DWORD) GetProcAddress ( GetModuleHandle ( "user32.dll" ), "GetAsyncKeyState" ) + 5;

	__asm {
		mov edi, edi
		push ebp
		mov esp, ebp
		jmp [dwKey]
	}
	*/
}
// GetThreadContext
BOOL WINAPI GetContext(
  __in     HANDLE hThread,
  __inout  LPCONTEXT lpContext
)
{
	return GetThreadContext ( hThread, lpContext );
	/*
	DWORD dwHopTo = (DWORD) GetProcAddress ( GetModuleHandle ( "kernel32.dll" ), "GetThreadContext" ) + 5;

	__asm {
		mov edi, edi
		push ebp
		mov esp, ebp
		jmp [dwHopTo]
	}
	*/
}
// SetThreadContext
BOOL WINAPI SetContext(
  __in  HANDLE hThread,
  __in  const CONTEXT *lpContext
)
{
	return SetThreadContext ( hThread, lpContext );

	/*
	DWORD dwHopTo = (DWORD) GetProcAddress ( GetModuleHandle ( "kernel32.dll" ), "SetThreadContext" ) + 5;

	__asm {
		mov edi, edi
		push ebp
		mov esp, ebp
		jmp [dwHopTo]
	}
	*/
}
// WriteFile
BOOL WINAPI WriteFil (
  __in         HANDLE hFile,
  __in         LPCVOID lpBuffer,
  __in         DWORD nNumberOfBytesToWrite,
  __out_opt    LPDWORD lpNumberOfBytesWritten,
  __inout_opt  LPOVERLAPPED lpOverlapped
)
{
	return WriteFile ( hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped );
	/*
	DWORD dwHopTo = (DWORD) GetProcAddress ( GetModuleHandle ( "kernel32.dll" ), "WriteFile" ) + 5;

	__asm {
		mov edi, edi
		push ebp
		mov esp, ebp
		jmp [dwHopTo]
	}
	*/
}