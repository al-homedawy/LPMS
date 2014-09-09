#include <Windows.h>
#include <TlHelp32.h>
#include <Commctrl.h>
#include <sstream>
#include <Windowsx.h>
#include <fstream>

#include "resource.h"

using namespace std;

#pragma comment ( lib, "Urlmon.lib" )

// Global Variables
HWND      updateDlg = NULL;
HWND      mainDlg	= NULL;
HINSTANCE hInstance = NULL;
HANDLE    hInject   = NULL;
HANDLE    hTitle    = NULL;
HANDLE    hMerge    = NULL;

string Target   = "";
string Username = "";
string Version  = "0.91";
string OnlineVers = "";
string FileName = "";
string DownloadTo = "";

char szDirectory [256] = {0};

bool downloadMerger = false;
bool downloadUpdate = false;
bool downloadTutorial = false;
bool downloadFinished = false;

/*
	0 - regular
	1 - driver
*/

int injectionMethod   = 0;

//
class DownloadSoftware : public IBindStatusCallback { 
public: 
    HRESULT __stdcall QueryInterface(const IID &,void **) {  
        return E_NOINTERFACE; 
    } 
    ULONG STDMETHODCALLTYPE AddRef(void) {  
        return 1; 
    } 
    ULONG STDMETHODCALLTYPE Release(void) { 
        return 1; 
    } 
    HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD dwReserved, IBinding *pib) { 
        return E_NOTIMPL; 
    } 
    virtual HRESULT STDMETHODCALLTYPE GetPriority(LONG *pnPriority) { 
        return E_NOTIMPL; 
    } 
    virtual HRESULT STDMETHODCALLTYPE OnLowResource(DWORD reserved) { 
        return S_OK; 
    } 
    virtual HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT hresult, LPCWSTR szError) { 
        return E_NOTIMPL; 
    } 
    virtual HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD *grfBINDF, BINDINFO *pbindinfo) { 
        return E_NOTIMPL; 
    } 
    virtual HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC *pformatetc, STGMEDIUM *pstgmed) { 
        return E_NOTIMPL; 
    }         
    virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable(REFIID riid, IUnknown *punk) { 
        return E_NOTIMPL; 
    } 
 
    virtual HRESULT __stdcall OnProgress (ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText) 
    { 
		downloadFinished = false;

		string Download = "Progress ";		
		stringstream ss;
		ss << ( ulProgress / 1024 );
		Download += ss.str ();
		Download += "\\";
		stringstream ss2;
		ss2 << ( ulProgressMax / 1024 );
		Download += ss2.str ();
		Download += " kB";

		SetWindowText ( GetDlgItem ( updateDlg, IDC_DOWNLOAD ), Download.c_str () );

		if ( ulProgress == ulProgressMax )
			downloadFinished = true;

        return S_OK; 
    } 
};
//

//
void decipherAlgorithm ( string text, string* hWid, string* Username )
{
	int beginAt = 0;

	for ( SIZE_T i = 0; i < text.length (); i ++ )
	{
		char character  = (char) text.at ( i );
		char deciphered = (char) ( (int) character + 15 );

		if ( deciphered == '}' )
			beginAt = i;
	}

	for ( int i = 0; i <= beginAt; i ++ )
		*hWid += (char) ( text.at ( i ) + 15 );

	for ( SIZE_T i = beginAt + 1; i < text.length (); i ++ )
		*Username += (char) ( text.at ( i ) + 15 );
}
//

//
INT_PTR CALLBACK DialogProc ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK UpdateProc ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK LoadingProc ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
//

// Update function
VOID DownloadIt ()
{
	string Site = "http://easygamez.ca/LPMS/";
	Site += FileName;

	DownloadSoftware progress; 
	IBindStatusCallback* callback = (IBindStatusCallback*)&progress; 
	URLDownloadToFile ( NULL, Site.c_str (), DownloadTo.c_str (), 0, callback); 

	ShellExecute ( NULL, "open", "Other.bat", NULL, szDirectory, SW_HIDE );

	if ( !downloadMerger && !downloadTutorial )
		ShellExecute ( NULL, "open", "Install.bat", NULL, szDirectory, SW_HIDE );

	// Close
	EndDialog ( updateDlg, 0 );
}
//

//
void StartUpdate ()
{
	// Latest Update
	DownloadTo = szDirectory;
	DownloadTo += "\\LPMS2.txt";

	ofstream File ( DownloadTo.c_str () );
	File.clear ();
	File.close ();

	// Download
	FileName = "LPMS.txt";

	CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) DownloadIt, NULL, NULL, NULL );

	string RemoveFinal = "DEL ";
	RemoveFinal += 0x22;
	RemoveFinal += szDirectory;
	RemoveFinal += "\\LPMS.exe";
	RemoveFinal += 0x22;		

	string BatchFile = "DEL ";
	BatchFile += 0x22;
	BatchFile += szDirectory;
	BatchFile += "\\Install.bat";
	BatchFile += 0x22;

	string FinalDir = szDirectory;
	FinalDir += "\\Install.bat";

	string Closer = "taskkill ";
	Closer += 0x2F;
	Closer += "f";
	Closer += " ";
	Closer += 0x2F;
	Closer += "im";
	Closer += " ";
	Closer += "LPMS.exe";

	string Merger = "Start ";
	Merger += 0x22;
	Merger += 0x22;
	Merger += " ";
	Merger += 0x22;
	Merger += szDirectory;
	Merger += "\\LPMS.exe";
	Merger += 0x22;

	// Setup batch file
	ofstream Batch ( FinalDir.c_str () );
	Batch.clear ();			
	Batch << Closer.c_str () << endl;
	Batch << RemoveFinal.c_str () << endl;
	Batch << "CD " << szDirectory << endl;
	Batch << "REN LPMS2.txt LPMS.exe" << endl;
	Batch << Merger.c_str () << endl;
	Batch << BatchFile.c_str () << endl;
	Batch.close ();	
}
int  StartMerge ()
{	
	// Obtain
	DownloadTo = szDirectory;
	DownloadTo += "\\Merge.txt";

	// Create
	ofstream File ( DownloadTo.c_str () );
	File.clear ();
	File.close ();

	// Download
	FileName = "Merge.txt";

	CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) DownloadIt, NULL, NULL, NULL );

	string RemoveFinal = "DEL ";
	RemoveFinal += 0x22;
	RemoveFinal += szDirectory;
	RemoveFinal += "\\Merge.exe";
	RemoveFinal += 0x22;		

	string BatchFile = "DEL ";
	BatchFile += 0x22;
	BatchFile += szDirectory;
	BatchFile += "\\Other.bat";
	BatchFile += 0x22;

	string FinalDir = szDirectory;
	FinalDir += "\\Other.bat";

	ofstream Batch ( FinalDir.c_str () );
	Batch.clear ();			
	Batch << RemoveFinal.c_str () << endl;
	Batch << "CD " << szDirectory << endl;
	Batch << "REN Merge.txt Merge.exe" << endl;
	Batch << BatchFile.c_str () << endl;
	Batch.close ();	

	return 0;
}
int  StartTutorial ()
{
	// Obtain
	DownloadTo = szDirectory;
	DownloadTo += "\\Tutorial.txt";

	// Create
	ofstream File ( DownloadTo.c_str () );
	File.clear ();
	File.close ();

	// Download
	FileName = "Tutorial.txt";

	CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) DownloadIt, NULL, NULL, NULL );

	string RemoveFinal = "DEL ";
	RemoveFinal += 0x22;
	RemoveFinal += szDirectory;
	RemoveFinal += "\\Tutorial.exe";
	RemoveFinal += 0x22;		

	string BatchFile = "DEL ";
	BatchFile += 0x22;
	BatchFile += szDirectory;
	BatchFile += "\\Other.bat";
	BatchFile += 0x22;

	string FinalDir = szDirectory;
	FinalDir += "\\Other.bat";

	string Merger = "Start ";
	Merger += 0x22;
	Merger += 0x22;
	Merger += " ";
	Merger += 0x22;
	Merger += szDirectory;
	Merger += "\\Tutorial.exe";
	Merger += 0x22;

	ofstream Batch ( FinalDir.c_str () );
	Batch.clear ();			
	Batch << RemoveFinal.c_str () << endl;
	Batch << "CD " << szDirectory << endl;
	Batch << "REN Tutorial.txt Tutorial.exe" << endl;
	Batch << Merger << endl;
	Batch << BatchFile.c_str () << endl;
	Batch.close ();	

	return 0;
}
//

//
template <class T> string ToString ( T szValue )
{
	stringstream ss;
	ss << szValue;
	return ss.str ();
}

string NoChar ( string Source, char Target )
{
	string Final = "";

	for ( SIZE_T i = 0; i < Source.length (); i ++ )
	{
		if ( Source.at ( i ) != Target )
			if ( Source.at ( i ) < '0' || Source.at ( i ) > '9' )
				if ( Source.at ( i ) > 'a' || Source.at ( i ) < 'z' )
					Final += Source.at ( i ) - 20;
				else
					Final += Source.at ( i );
			else
				Final += Source.at ( i );
	}

	return Final;
}
string ObtainLicense ()
{
	DWORD SerialNum;
	GetVolumeInformationA ( "C:\\", NULL, NULL, &SerialNum, NULL, NULL, NULL, NULL );

	string License = "{";

	// Setup variables
	string MachineGuid;
	char szMachineGuid [256] = {0};
	DWORD szGuidSize = 256;
	DWORD dwType     = RRF_RT_ANY;

	HKEY hKey;
	RegOpenKeyEx ( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ, &hKey );

	// Obtain GUID
	long Result = RegQueryValueEx ( hKey, "MachineGuid", NULL, &dwType, (BYTE*) &szMachineGuid, &szGuidSize);
	MachineGuid += szMachineGuid;

	// Extract system information 
	SYSTEM_INFO si;
	GetSystemInfo ( &si );

	// Obtain Processor
	string Processor = "";
	
	if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
		Processor = 'A';
	if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
		Processor = "T";
	if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL )
		Processor = "I";
	if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_UNKNOWN )
		Processor = 'U';

	// Add information
	License += Processor;
	License += ToString ( (int) si.wProcessorRevision );
	License += NoChar ( MachineGuid, '-' ).substr ( 0, 5 );
	License += ToString ( (void*) ( (SerialNum * 3) / 5 ) );
	License += "}";

	RegCloseKey ( hKey );

	return License;
}

//

//
string ToLowercase ( string text )
{
	string newString = "";

	for ( SIZE_T i = 0; i < text.length (); i ++ )
	{
		if ( ( text.at (i) >= 65 && text.at (i) <= 90 ) )
			newString += ( text.at (i) + 32 );
		else
			newString += text.at (i);
	}

	return newString;
}
DWORD LookupProcessForProcessId ( string TargetName )
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPALL, 0 );

	PROCESSENTRY32 PE;
	PE.dwSize = sizeof ( PROCESSENTRY32 );

	if ( Process32First ( hSnapshot, &PE ) )
	{
		string processName;
		processName += PE.szExeFile;

		if ( !strcmp ( ToLowercase ( processName ).c_str (), TargetName.c_str () ) )
				return PE.th32ProcessID;

		while ( Process32Next ( hSnapshot, &PE ) )
		{
			processName = "";
			processName += PE.szExeFile;

			if ( !strcmp ( ToLowercase ( processName ).c_str (), TargetName.c_str () ) )
				return PE.th32ProcessID;
		}
	}

	return 0;
}
//

// Main Procedures
void CloseMerge ()
{
	DWORD dwPid = 0;

	// Create thread handle
	PROCESSENTRY32 PE;
	PE.dwSize = sizeof ( PROCESSENTRY32 );

	// Create snapshot
	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPALL, 0 );

	// Get the pID
	if ( Process32First ( hSnapshot, &PE ) )
	{
		string processName = "";
		processName       += PE.szExeFile;

		if ( !strcmp ( "merge.exe", ToLowercase ( processName ).c_str () ) )
			dwPid = PE.th32ProcessID;
		else
		{
			while ( Process32Next ( hSnapshot, &PE ) )
			{
				processName = "";
				processName += PE.szExeFile;

				if ( !strcmp ( "merge.exe", ToLowercase ( processName ).c_str () ) )
				{
					dwPid = PE.th32ProcessID;
					break;
				}
			}
		}
	}

	HANDLE hProcess = OpenProcess ( PROCESS_TERMINATE, false, dwPid );

	// Get the exit code
	DWORD dwExitCode;
	GetExitCodeProcess ( hProcess, &dwExitCode );
	TerminateProcess   ( hProcess, dwExitCode  );
}
int  Inject     ( string ProcessName )
{	
	// Regular injection
	if ( injectionMethod == 1 )
	{
		// Obtain information
		DWORD targetID = LookupProcessForProcessId ( ProcessName );

		if ( !targetID )
			return 0;

		// Setup addresses
		DWORD dwLoadLib   = (DWORD) GetProcAddress ( GetModuleHandle ( "kernel32.dll" ), "LoadLibraryA" );
		DWORD dwVProtect  = (DWORD) GetProcAddress ( GetModuleHandle ( "kernel32.dll" ), "VirtualProtect" );
		DWORD dwCThread   = (DWORD) GetProcAddress ( GetModuleHandle ( "kernel32.dll" ), "CreateThread" );
		DWORD dwTarget    = (DWORD) GetProcAddress ( GetModuleHandle ( "kernel32.dll" ),  "Sleep" );

		// Process ID
		ofstream File ( "C:\\pID.txt" );
		File.clear ();
		File << targetID << endl;
		File.close ();

		// LoadLibrary
		File.open ( "C:\\LoadLibrary.txt" );
		File.clear ();
		File << (void*) dwLoadLib << endl; 
		File.close ();

		// VirtualProtect
		File.open ( "C:\\VirtualProtect.txt" );
		File.clear ();
		File << (void*) dwVProtect << endl; 
		File.close ();

		// CreateThread
		File.open ( "C:\\CreateThread.txt" );
		File.clear ();
		File << (void*) dwCThread << endl; 
		File.close ();

		// dwTarget
		File.open ( "C:\\dwTarget.txt" );
		File.clear ();
		File << (void*) dwTarget << endl; 
		File.close (); 

		// Declare 
		SERVICE_STATUS ss;
		
		// Open the manager
		SC_HANDLE hscManager = OpenSCManager ( NULL, NULL, SC_MANAGER_ALL_ACCESS );

		if ( hscManager )
		{
			string driverName = "driver";
			string binaryPath = "C:\\Windows\\";
			binaryPath += driverName.c_str ();
			binaryPath += ".sys";

			// Obtain a handle to the service
			SC_HANDLE hscService = OpenService ( hscManager, driverName.c_str (), SERVICE_START | DELETE | SERVICE_STOP );

			// Create the service
			if ( !hscService )
			{
				// Create the service
				hscService = CreateService ( hscManager, driverName.c_str (), driverName.c_str (), SERVICE_START | DELETE | SERVICE_STOP, 
										 SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, binaryPath.c_str (), 
										 NULL, NULL, NULL, NULL, NULL );
			}

			// Check
			if ( hscService )
			{
				// Start the driver
				StartService ( hscService, 0, NULL );

				// Stop the driver
				ControlService ( hscService, SERVICE_CONTROL_STOP, &ss );
				DeleteService  ( hscService );
				CloseServiceHandle ( hscService );
			}
		}

		// Close the manager
		CloseServiceHandle ( hscManager );

		// Get the current directory
		char szDir [256] = {0};
		GetCurrentDirectory ( 256, (LPSTR) szDir );

		// Save the settings
		string Settings;
		Settings += szDir;
		Settings += "\\Settings.ini";

		// Create the file
		ofstream SettingsClear ( Settings.c_str () );
		SettingsClear.clear ();
		SettingsClear << ProcessName << endl;
		SettingsClear.close ();

		Target = ProcessName;
		return -6;
	}
	else
	{
		// Dll Path
		string DllPath = "C:\\Windows\\LPMS.dll";
		DllPath       += '\0';

		// Obtain the process id
		DWORD dwPid = 0;
		
		// Create thread handle
		PROCESSENTRY32 PE;
		PE.dwSize = sizeof ( PROCESSENTRY32 );

		// Create snapshot
		HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPALL, 0 );

		// Add the results
		if ( Process32First ( hSnapshot, &PE ) )
		{
			string processName = "";
			processName       += PE.szExeFile;

			if ( !strcmp ( ProcessName.c_str (), ToLowercase ( processName ).c_str () ) )
				dwPid = PE.th32ProcessID;
			else
			{
				while ( Process32Next ( hSnapshot, &PE ) )
				{
					processName = "";
					processName += PE.szExeFile;

					if ( !strcmp ( ProcessName.c_str (), ToLowercase ( processName ).c_str () ) )
					{
						dwPid = PE.th32ProcessID;
						break;
					}
				}
			}
		}

		// Inject
		if ( !dwPid ) 
			return 0;
		else
		{
			HANDLE hTarget = OpenProcess ( PROCESS_CREATE_THREAD | 
							 PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | 
							 PROCESS_VM_WRITE | PROCESS_VM_READ, false, dwPid );

			HANDLE hThread = NULL;

			if ( !hTarget )
				return -1;	
			else
			{
				LPVOID lpAddr = VirtualAllocEx ( hTarget, NULL, DllPath.length (), MEM_COMMIT, PAGE_EXECUTE_READWRITE );

				if ( !lpAddr )
					return -2;	
				else
				{
					if ( !WriteProcessMemory ( hTarget, lpAddr, DllPath.c_str (), DllPath.length (), NULL ) )
						return -3;
					else
					{
						FARPROC lpLoadLibrary = GetProcAddress ( GetModuleHandle ( "kernel32.dll" ), "LoadLibraryA" );

						if ( !lpLoadLibrary )
							return -4;
						else
						{
							hThread = CreateRemoteThread ( hTarget, NULL, 0, (LPTHREAD_START_ROUTINE) lpLoadLibrary, lpAddr, 0, NULL ); 

							if ( !hThread )
								return -5;
							else
							{
								// Wait
								WaitForSingleObject ( hThread, 10000 );

								// Get the current directory
								char szDir [256] = {0};
								GetCurrentDirectory ( 256, (LPSTR) szDir );

								// Save the settings
								string Settings;
								Settings += szDir;
								Settings += "\\Settings.ini";

								// Create the file
								ofstream File ( Settings.c_str () );
								File.clear ();
								File << ProcessName << endl;
								File.close ();

								// Set
								Target = ProcessName;

								// Free
								VirtualFreeEx ( hTarget, lpAddr, DllPath.length (), MEM_RELEASE );

								// Close
								CloseHandle ( hTarget );
								CloseHandle ( hThread );

								// Return
								return -6;
							}
						}
					}
				}
			}
		}

		return false;
	}
}
void MainThread ()
{
	while ( true )
	{
		bool automatic = SendMessage ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;
		bool driver    = SendMessage ( GetDlgItem ( mainDlg, IDC_CHECKBOX2 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;

		// Set up driver
		if ( driver )
			injectionMethod = 1;
		else
			injectionMethod = 0;

		// Automatic injection
		if ( automatic )
		{
			// Turn off the button
			Button_Enable ( GetDlgItem ( mainDlg, IDC_BUTTON2 ), FALSE );

			// Constantly look for the process
			char szBuffer [256] = {0};
			GetWindowText ( GetDlgItem ( mainDlg, IDC_EDIT1 ), (LPSTR) szBuffer, 256 );

			string text;
			text += szBuffer;

			if ( strcmp ( szBuffer, "" ) != 0 )		
			{
				if ( Inject ( ToLowercase ( text ) ) == -6 )
					break;
			}
		}
		else
			Button_Enable ( GetDlgItem ( mainDlg, IDC_BUTTON2 ), TRUE );

		Sleep ( 100 );
	}

	EndDialog ( mainDlg, 0 );
}
//

// Starting procedures
void Title    ()
{
	while ( true )
	{
		string Title = "  Ready";
		SetWindowText ( mainDlg, Title.c_str () );
		Sleep ( 1000 );
		Title += ".";
		SetWindowText ( mainDlg, Title.c_str () );
		Sleep ( 1000 );
		Title += ".";
		SetWindowText ( mainDlg, Title.c_str () );
		Sleep ( 1000 );
		Title += ".";
		SetWindowText ( mainDlg, Title.c_str () );
		Sleep ( 1000 );
	}
}
void MainProc ()
{
	hTitle = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) Title, NULL, NULL, NULL );

	// Set the version
	SetWindowText ( GetDlgItem ( mainDlg, IDC_VERSION ), Version.c_str () );

	// Set the username
	SetWindowText ( GetDlgItem ( mainDlg, IDC_USERNAME ), Username.c_str () );

	// Get the current directory
	char szDir [256] = {0};
	GetCurrentDirectory ( 256, (LPSTR) szDir );

	// Save the settings
	string Settings;
	Settings += szDir;
	Settings += "\\Settings.ini";

	// Create the file
	ifstream Read ( Settings.c_str () );

	string str;
	getline ( Read, str );

	if ( strcmp ( str.c_str (), "" ) != 0 )
		SetWindowText ( GetDlgItem ( mainDlg, IDC_EDIT1 ), str.c_str () );
	else
		SetWindowText ( GetDlgItem ( mainDlg, IDC_EDIT1 ), "wolfteam.bin" );

	Read.close ();

	// Create the thread
	hInject = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) MainThread, NULL, NULL, NULL );
}
//

//
void TerminateOpenProcess ( string TargetName )
{
	DWORD dwPid = 0;

	// Create thread handle
	PROCESSENTRY32 PE;
	PE.dwSize = sizeof ( PROCESSENTRY32 );

	// Create snapshot
	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPALL, 0 );

	// Get the pID
	if ( Process32First ( hSnapshot, &PE ) )
	{
		string processName = "";
		processName       += PE.szExeFile;

		if ( !strcmp ( ToLowercase ( TargetName ).c_str (), ToLowercase ( processName ).c_str () ) )
			dwPid = PE.th32ProcessID;
		else
		{
			while ( Process32Next ( hSnapshot, &PE ) )
			{
				processName = "";
				processName += PE.szExeFile;

				if ( !strcmp ( ToLowercase ( TargetName ).c_str (), ToLowercase ( processName ).c_str () ) )
				{
					dwPid = PE.th32ProcessID;
					break;
				}
			}
		}
	}

	HANDLE hProcess = OpenProcess ( PROCESS_TERMINATE, false, dwPid );

	// Get the exit code
	DWORD dwExitCode;
	GetExitCodeProcess ( hProcess, &dwExitCode );
	TerminateProcess   ( hProcess, dwExitCode  );
}
//

//
void LaunchMerger ()
{
	string MergePath = szDirectory;
	MergePath += "\\Merge.exe";

	if ( fopen ( MergePath.c_str (), "r" ) == NULL  )
	{
		downloadMerger = true;
		downloadUpdate = false;
		DialogBox ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG1 ), NULL, UpdateProc );
	}
}
//

// Main Callback
INT_PTR CALLBACK UpdateProc  ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	updateDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		{
			if ( downloadMerger )
				StartMerge ();
			else if ( downloadTutorial )
				StartTutorial ();
			else
				StartUpdate ();

			// Set merger to false
			downloadMerger = false;

			return TRUE;
		}
		
	default:
		return FALSE;
	}
}
INT_PTR CALLBACK DialogProc  ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	mainDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					// About message
					ShellExecute ( NULL, "open", "http://www.easygamez.ca/", NULL, NULL, SW_SHOWNORMAL );
				}
				break;

			case IDC_BUTTON2:
				{
					// Obtain the process 
					char szBuffer [256] = {0};
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szBuffer, 256 );

					string text = "";
					text += szBuffer;
					
					// Inject
					if ( Inject ( ToLowercase ( text ) ) == -6 )
						EndDialog ( hWndDlg, 0 );
				}
				break;
			}
			return TRUE;
		}

	case WM_CLOSE:
		{
			// Close threads
			DWORD dwExitCode;
			GetExitCodeThread ( hInject, &dwExitCode );
			TerminateThread   ( hInject, dwExitCode );
			GetExitCodeThread ( hTitle, &dwExitCode );
			TerminateThread   ( hTitle, dwExitCode );

			EndDialog ( hWndDlg, 0 );

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			// Invoke main procedures
			MainProc ();

			return TRUE;
		}

	default:
		return FALSE;
	}
}
INT_PTR CALLBACK LicenseProc ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:
		{
			// Set the product id
			SetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), ObtainLicense ().c_str () );

			return TRUE;
		}

	case WM_CLOSE:
		{
			EndDialog ( hWndDlg, 0 );
			return TRUE;
		}

	default:
		return FALSE;
	}
}

//

// Initialize
void InitializeApp  ()
{
	string hwid    = "";
	bool bContinue = false;

	// Create the file
	ofstream myFile ( "C:\\Windows\\asdhasd.ce" );
	myFile.clear ();
	myFile.close ();

	// Setup the URL
	string URL = "http://easygamez.ca/LPMS/CheckLicense.php?hwid=";
	URL       += ObtainLicense ().c_str ();

	// Download the file
	URLDownloadToFile ( NULL, URL.c_str (), "C:\\Windows\\asdhasd.ce", 0, NULL );

	// Read the file
	ifstream Check ( "C:\\Windows\\asdhasd.ce" );
	
	// Obtain the version
	ofstream myVers ( "C:\\Version.txt" );
	myVers.clear ();
	myVers.close ();

	URLDownloadToFile ( NULL, "http://easygamez.ca/LPMS/Version.txt", "C:\\Version.txt", 0, NULL );

	// Obtain the version
	ifstream VersIs ( "C:\\Version.txt" );
	getline ( VersIs, OnlineVers );
	VersIs.close ();

	while ( !Check.eof () )
	{
		// Obtain text
		string str;
		getline ( Check, str );

		if ( str.length () > 1 )
		{
			// Decipher the algorithm
			decipherAlgorithm ( str, &hwid, &Username );

			if ( !strcmp ( ObtainLicense ().c_str (), hwid.c_str () ) )
			{
				// Add directory
				ofstream CurDir ( "C:\\CurDir.txt" );
				CurDir << szDirectory;
				CurDir.close ();

				// Setup username
				string DllPath = "C:\\Windows\\LPMS.dll";

				// Create the file
				ofstream myLib ( DllPath.c_str () );
				myLib.clear ();
				myLib.close ();

				// Download the file
				URLDownloadToFile ( NULL, "http://easygamez.ca/LPMS/LPMS.dll", DllPath.c_str (), 0, NULL );
				bContinue = true;

				// Create the driver
				string DriverPath = "C:\\Windows\\driver.sys";
				ofstream Driver ( DriverPath.c_str () );
				Driver.clear ();
				Driver.close ();

				// Download the driver
				URLDownloadToFile ( NULL, "http://easygamez.ca/LPMS/driver.sys", DriverPath.c_str (), NULL, NULL );
				break;
			}
		}
	}

	// Close and delete
	Check.close ();
	DeleteFile ( "C:\\Windows\\asdhasd.ce" );

	if ( !strcmp ( OnlineVers.c_str (), Version.c_str () ) )
	{
		// Call the splash screen
		if ( bContinue )
		{
			// Download the latest Merger.exe
			LaunchMerger ();

			// Save the settings
			string Settings;
			Settings += szDirectory;
			Settings += "\\Settings.ini";

			// Create the file
			ifstream Read ( Settings.c_str () );

			string str;
			getline ( Read, str );

			if ( !strcmp ( str.c_str (), "" ) )
			{
				if ( MessageBoxA ( mainDlg, "Would you like to try out the Tutorial?", "LPMS", MB_YESNO ) == IDYES )
				{
					bool ContinueTut = false;

					if ( (long) ShellExecute ( NULL, "open", "Tutorial.exe", NULL, szDirectory, SW_NORMAL ) == ERROR_FILE_NOT_FOUND )
					{
						if ( MessageBoxA ( mainDlg, "LPMS has detected that Tutorial.exe does not exist.\n     Would you like Tutorial.exe to be downloaded?", "LPMS", MB_YESNO ) == IDYES )
						{
							ContinueTut      = true;
							downloadMerger   = false;
							downloadTutorial = true;

							DialogBox ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG1 ), NULL, UpdateProc ); 							
						}
						else
							DialogBox ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG2 ), NULL, DialogProc );
					}

					if ( ContinueTut )
					{
						// Open Tutorial.exe
						ShellExecute ( NULL, "open", "Tutorial.exe", NULL, szDirectory, SW_NORMAL );

						// Inject
						int check = Inject ( ToLowercase ( "Tutorial.exe" ) );

						while ( check != -6 )
							check = Inject ( ToLowercase ( "Tutorial.exe" ) ); 
					}
				}
				else
					DialogBox ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG2 ), NULL, DialogProc );
			}
			else
				DialogBox ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG2 ), NULL, DialogProc );

			Read.close ();
		}
		else
			DialogBox ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG3 ), NULL, LicenseProc );
	}
	else
	{
		MessageBoxA ( 0, "An update has been found.", "LPMS", MB_ICONINFORMATION );		

		downloadMerger = false;
		downloadTutorial = false;

		DialogBox ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG1 ), NULL, UpdateProc );		
	}
}
//

// Main Entry-point
int CALLBACK WinMain (__in  HINSTANCE hInst, __in  HINSTANCE hPrevInstance, __in  LPSTR lpCmdLine, __in  int nCmdShow)
{
	// Open Process Token
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
	   LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
	   tkp.PrivilegeCount = 1;
	   tkp.Privileges[0].Attributes  = SE_PRIVILEGE_ENABLED;
	   AdjustTokenPrivileges(hToken, 0, &tkp, sizeof(tkp), NULL, NULL);
	}

	// Get current directory
	GetCurrentDirectory ( 256, szDirectory );
	string CurrentPathway = szDirectory;
	CurrentPathway += "\\";

	// Set the hInstance
	hInstance = hInst;

	// Create the pathways
	string hwidPath = szDirectory;
	hwidPath += "\\Memory\\HWID.exe";

	string lpmsPath = szDirectory;
	lpmsPath += "\\LPMS.dll";	

	// Delete the file
	DeleteFile ( "C:\\Windows\\Message.ce" );
	DeleteFile ( "C:\\Version.txt" );

	// Delete the files
	DeleteFile ( hwidPath.c_str () );
	DeleteFile ( lpmsPath.c_str () );
	DeleteFile ( "C:\\MergeOver.txt" );

	// Begin
	InitializeApp ();

	// Wait for the target to close
	if ( strcmp ( Target.c_str (), "" ) != 0 )
	{
		HANDLE hSnapshot = NULL;

		// Initialize our structure
		PROCESSENTRY32 PE;
		PE.dwSize = sizeof ( PROCESSENTRY32 );

		bool Found = false;		

		string MemoryDir = CurrentPathway;
		MemoryDir += "\\Memory\\";

		// Check if the target exists
		while ( true ) 
		{
			// Create the /Memory/ Directory just incase its gone
			CreateDirectory ( MemoryDir.c_str (), NULL );

			// Merge the files
			string str = "";

			ifstream Merge ( "C:\\MergeOver.txt" );
			getline ( Merge, str );

			if ( !strcmp ( str.c_str (), "Ready" ) )
			{
				// Execute
				ShellExecute ( NULL, "open", "Merge.exe", NULL, NULL, SW_HIDE );

				// Close
				Merge.close ();

				// Clear
				ofstream MergeFile ( "C:\\MergeOver.txt" );
				MergeFile.clear ();
				MergeFile.close ();
			}

			Merge.close ();

			Found = false;	

			// Create the snapshot
			hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPALL, 0 );		

			// Begin
			if ( Process32First ( hSnapshot, &PE ) )
			{
				string processName = "";
				processName       += PE.szExeFile;

				if ( !strcmp ( ToLowercase ( Target ).c_str (), ToLowercase ( processName ).c_str () ) )
					Found = true;
				else
				{
					while ( Process32Next ( hSnapshot, &PE ) )
					{
						processName = "";
						processName += PE.szExeFile;

						if ( !strcmp ( ToLowercase ( Target ).c_str (), ToLowercase ( processName ).c_str () ) )
							Found = true;
					}
				}
			}

			if ( !Found ) 
				break;

			Sleep ( 100 );
		}
	}

	// Delete remaining files
	DeleteFile ( "C:\\Windows\\LPMS.dll" );
	DeleteFile ( "C:\\Windows\\aadffs.dll" );
	DeleteFile ( "C:\\Windows\\MemoryScan.iScan" );
	DeleteFile ( "C:\\Windows\\MemoryText.iScan" );
	DeleteFile ( "C:\\Windows\\MemoryText.iScan" );

	string Settings = szDirectory;
	Settings += "\\LPMS.ini";

	string Memory = szDirectory;
	Memory += "\\Memory\\Memory.TMP";

	string Values = szDirectory;
	Values += "\\Memory\\Values.TMP";

	string OldValues = szDirectory;
	OldValues += "\\Memory\\OldValues.TMP";

	string OldMemory = szDirectory;
	OldMemory += "\\Memory\\OldMemory.TMP";

	string MemoryTwo = szDirectory;
	MemoryTwo += "\\Memory\\Memory2.TMP";

	string ValuesTwo = szDirectory;
	ValuesTwo += "\\Memory\\Values2.TMP";

	string Directory = szDirectory;
	Directory += "\\Memory\\";	

	ifstream File ( Settings.c_str () );
	int Count = 0;
	string Threads;

	while ( !File.eof () )
	{
		File >> Threads;

		if ( Count == 4 )
		{
			File >> Threads;
			break;
		}

		++ Count;
	}

	File.close ();
	

	// Delete the .TMP files
	int numThreads = 0;

	stringstream ss;

	if ( !strcmp ( Threads.c_str (), "" ) )
		numThreads = 0;
	else
	{
		ss << Threads;
		ss >> numThreads;
	}

	for ( int i = 0; i < numThreads; i ++ )
	{
		// Delete the files in /Memory/
		string MemoryFile = szDirectory;
		MemoryFile += "\\Memory\\Memory";
		string ValueFile = MemoryFile;
		stringstream ss2;
		ss2 << i;
		MemoryFile += ss2.str ();
		MemoryFile += ".TMP";
		stringstream ss3;
		ss3 << i + 100;
		ValueFile += ss3.str ();
		ValueFile += ".TMP";
		DeleteFile ( ValueFile.c_str () );
		DeleteFile ( MemoryFile.c_str () );
	}

	DeleteFile ( Memory.c_str () );
	DeleteFile ( OldMemory.c_str () );
	DeleteFile ( OldValues.c_str () );
	DeleteFile ( MemoryTwo.c_str () );
	DeleteFile ( ValuesTwo.c_str () );
	DeleteFile ( Values.c_str () );

	// Close Merge
	CloseMerge ();

	// Delete Merge.exe
	string mergePath;
	mergePath += szDirectory;
	mergePath += "\\Merge.exe";
	DeleteFile ( mergePath.c_str () );

	// Delete the folder
	RemoveDirectory ( Directory.c_str () );

	return 0;
}
//