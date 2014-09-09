#include <Windows.h>

// Local Headers
#include "resource.h"
#include "DialogProc.h"

#pragma comment ( lib, "Urlmon.lib" )

// Certification Keys.
BYTE BytesOne [] = { 0x59, 0x6f, 0x75, 0x72, 0x20, 0x6c, 0x69, 0x63, 0x65, 0x6e, 0x73, 0x65, 0x20, 0x69, 0x73, 0x3a, 0x20 };

BYTE BytesTwo [] = { 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58,
					 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58 };

BYTE Certification    [] = { 0x4e, 0x4f, 0x20, 0x43, 0x45, 0x52, 0x54, 0x49, 0x46, 0x49, 
							 0x43, 0x41, 0x54, 0x49, 0x4f, 0x4e, 0x20, 0x4b, 0x45, 0x59, 
							 0x20, 0x41, 0x56, 0x41, 0x49, 0x4c, 0x41, 0x42, 0x4c, 0x45, 
							 0x2e };

BYTE CertificationKey [] = { 0x53, 0x59, 0x4d, 0x42, 0x4f, 0x4c, 0x49, 0x53, 0x4d, 0x20, 
							 0x49, 0x53, 0x20, 0x41, 0x20, 0x50, 0x4f, 0x57, 0x45, 0x52, 
							 0x46, 0x55, 0x4c, 0x20, 0x54, 0x45, 0x43, 0x48, 0x4e, 0x49, 
							 0x51, 0x55, 0x45, 0x20, 0x54, 0x4f, 0x20, 0x45, 0x58, 0x50, 
							 0x52, 0x45, 0x53, 0x53, 0x20, 0x4d, 0x45, 0x53, 0x53, 0x41, 
							 0x47, 0x45, 0x53, 0x2e, 0x20, 0x4e, 0x49, 0x4b, 0x45, 0x20, 
							 0x4c, 0x4f, 0x47, 0x4f, 0x2e };

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

// Initialize the program
void InitializeApp ()
{
	IsWow64Process ( GetCurrentProcess (), &WOWBitPC );

	// Obtain the directory
	ifstream GetDir ( "C:\\CurDir.txt" );
	getline ( GetDir, CurDirectory );
	GetDir.close ();

	// Delete the file
	DeleteFile ( "C:\\CurDir.txt" );

	// Obtain the path
	string DirectoryPath = CurDirectory;
	DirectoryPath += "\\Memory";

	// Delete Values.TMP and Memory.TMP
	string MemoryPath = DirectoryPath;
	MemoryPath += "\\Memory\\Memory.TMP";

	string ValuesPath = DirectoryPath;
	ValuesPath += "\\Memory\\Values.TMP";

	string MemoryPath2 = DirectoryPath;
	MemoryPath2 += "\\Memory\\Memory2.TMP";

	string ValuesPath2 = DirectoryPath;
	ValuesPath2 += "\\Memory\\Values2.TMP";

	DeleteFile ( MemoryPath.c_str () );
	DeleteFile ( ValuesPath.c_str () );
	DeleteFile ( MemoryPath2.c_str () );
	DeleteFile ( ValuesPath2.c_str () );

	// Create the directory
	CreateDirectory ( DirectoryPath.c_str (), NULL );

	// Obtain the settings
	MakeSettings ();

	for ( int i = 0; i < MaxThreads; i ++ )
	{
		szDeletePath ( i );
		szDeletePath ( i + 100 );
	}
	
	// Launch the dialog
	DB ( hInstance, MAKEINTRESOURCE (IDD_DIALOG1), NULL, MainDialog);
}

//

// DLL Entry-point
BOOL WINAPI DllMain ( HINSTANCE hInst, DWORD fdwReason, LPVOID lpReserved )
{
	// Pass the dll's instance
	hInstance = hInst;

	// Initialize
	if ( fdwReason == DLL_PROCESS_ATTACH )
		hMainThread = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) InitializeApp, NULL, NULL, NULL );

	return TRUE;
}
//