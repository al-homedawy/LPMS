// Global Headers
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <Winbase.h>
#include <Psapi.h>
#include <Tlhelp32.h>
#include <bitset>
#include <math.h>

#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL

#include "hopper.h"
#include "BeaEngine.h"

bool     localBrk = false;
BYTE originalByte;

BOOL WOWBitPC = FALSE;

#define READABLE PAGE_EXECUTE_READ || PAGE_EXECUTE_READWRITE || PAGE_READONLY || PAGE_READWRITE

// LPMS Information
string Username;
string Version;
//

#pragma comment ( lib, "Psapi.lib" )

using namespace std;

string CurDirectory = "";

bool Merging = false;

// Structures
template <class T> struct ADDR_INFO
{
	DWORD dwAddr; // Address
	T     Value;  // Value
};
template <class T> struct SCAN_INFO
{
	T     Value;   // Value
};
template <class T> struct BLA_INFO
{
public:
	T      Value;
	DWORD  dwStartScan;
	DWORD  dwStopScan;
	HANDLE hThread;
	bool   finished;
};
struct SCANINFO
{
	int	   regionStart;
	int	   regionStop;
	int    scanNum;
	int   *Regions;
	int    goodReg;
	int    results;
	bool   finished;
	HANDLE hFile;
	HANDLE hNew;
	HANDLE hThread;
};
struct STATIC_INFO
{
	string PointerInfo; 
	BOOL   isStatic;    
};
struct POINTER_INFO
{
	DWORD dwAddress; 
	DWORD dwRange;   
	bool  GoStatic;  
};
struct IMAGE_INFO
{
	DWORD dwBase;
	SIZE_T szBase;
};
struct REGION_INFO
{
	DWORD dwBaseAddr;
	SIZE_T szRegion;
};
struct DBG_INFO
{
	PVOID lpAddr;
	DWORD EAX;
	DWORD EBX;
	DWORD EIP;
	DWORD ESP;
	DWORD EDI;
	DWORD ECX;
	DWORD EBP;
	DWORD EDX;
	DWORD ESI;
};

struct MEM_INFO
{
public:
	DWORD  dwRegBase; // Base Address
	SIZE_T szRegSize; // Region Size
};

struct PTR_INFO
{
	string PointerName;
	int    index;
};
//

// Settings
bool  EpislonOn = false;
float Epislon   = (float) 0.01;
int   MaxThreads = 30;

SIZE_T PreciseSize = PreciseSize;
SIZE_T UnknownSize = 20000000;

bool MemPrivate = false;
bool MemMapped  = false;
bool MemImage   = false;

SCANINFO * si;
int * threadsWorked;

bool siGone		   = false;
int threadsCreated = 0;

PTR_INFO *ptrInfoTwo;
int      ptrCountTwo = 0;

//
DWORD dwMS1;
DWORD dwMS2;
int	  myResult;
//

//
DWORD  dwPtrAddr;
DWORD  dwStartScan;
DWORD  dwStopScan;
POINTER_INFO ptrInfo; 
//

//
DWORD dwFunction;
BYTE  originalBytes [5] = {0};
DWORD dwOldProtect, dwDummy;
//

//
HWND mainDlg	   = NULL;
HWND addrDlg	   = NULL;
HWND editDlg       = NULL;
HWND ptrDlg        = NULL;
HWND memDlg        = NULL;
HWND chgDlg        = NULL;
HWND optDlg        = NULL;
HWND dbgDlg        = NULL;
HWND mdlDlg        = NULL;
HWND fwDlg         = NULL;
HWND regDlg        = NULL;
HWND disDlg        = NULL;
HWND extraDlg      = NULL;
HWND settingDlg    = NULL;
HWND editAllDlg    = NULL;
HWND dataStruct    = NULL;
//

//
HANDLE hScan       = NULL;
HANDLE hAddrDlg    = NULL;
HANDLE hEditDlg    = NULL;
HANDLE hMainThread = NULL;
HANDLE hUpdate     = NULL;
HANDLE hFreeze     = NULL;
HANDLE hPtrDlg     = NULL;
HANDLE hPtrScan    = NULL;
HANDLE hMemDlg     = NULL;
HANDLE hMemScan    = NULL;
HANDLE hChgDlg     = NULL;
HANDLE hPopup      = NULL;
HANDLE hOptDlg     = NULL;
HANDLE hDbgDlg     = NULL;
HANDLE hMdlDlg     = NULL;
HANDLE hArray      = NULL;
HANDLE hNoBug      = NULL;
HANDLE hDmp        = NULL;
HANDLE hDmp2       = NULL;
HANDLE hFuncWatch  = NULL;
HANDLE hRegion     = NULL;
HANDLE hDis        = NULL;
HANDLE hDisassemble = NULL;
HANDLE hExtra       = NULL;
HANDLE hSettings    = NULL;
HANDLE hOpen        = NULL;
HANDLE hSave        = NULL;
HANDLE hPointers    = NULL;
HANDLE hEditAll     = NULL;
HANDLE hDataStruct  = NULL;
HANDLE hHotkeys     = NULL;
//

//
HANDLE hAddress;
HANDLE hValues;
HANDLE hFile;
HANDLE hNew;
//

//
int  typeunknown  = -1;
bool ScanFinished = false;
bool unknownScan  = false;
bool ArrayOfBytes = false;
//

//
int wildcard [] = {0};
int wildcardnum = 0;
//

//
DWORD MS1;
DWORD MS2;
bool Set;
//

//
DWORD *    dwScan;	
DWORD * dwOldScan;
DWORD * dwPtrScan;  
//

//
DBG_INFO *dbgInfo;
void     *Dbged [] = {0};
int       numOfDbg = 0;
//

//
int      scanNum                 = -1;
int      oldResult               = 0;
UINT64   result                  = 0;
int      PointerResult           = 0;
int      finalResult             = 0;
int      newResult               = 0;
int      temp_result             = 0;
bool     dbgOn                   = false;

REGION_INFO * AllRegions;
int         NumRegions    = 0;
//

// Dll name
string DllName = "LPMS.dll";

//
DWORD dwNoStartScan = 0;
DWORD dwNoStopScan  = 0;
//

//
IMAGE_INFO imageInfo []  = {0};
int		   element		 = 0;
//

//
BYTE * pBytes;
SIZE_T szBytes;
int successRegion = 0;
int unknownNumber = 0;
int *SuccessReg;
//

//
string FuncCalled = "";
//

//
void MakeSound  ()
{
	Beep ( 1000, 100 );
}
//

//
BYTE * pRegion;
DWORD dwDbgAddr;
//

// Trimming, Static
string TrimZero ( string str )
{
	// Trim the zero's
	string Output = str.substr ( 6, 2 );

	// Return the final string
	return Output;
}
string RTrimZero ( string str )
{
	string output = str;
	int    index  = 0;

	for ( int i = 0; i < (int) str.length (); i ++ )
	{
		if ( str.at ( i ) != '0' )
			break;
		else
			++index;
	}

	return index < (int) ( str.length () - 1 ) ? output.substr ( index, str.length () ) : "0";
}
STATIC_INFO CS ( DWORD dwAddr )
{
	string PointerInfo;

	// Create our variable
	STATIC_INFO PtrInfo;
	PtrInfo.isStatic = FALSE;

	// Module information
	char  szModule [256] = {0};
	DWORD dwBaseAddr	 = 0;

	// Module entry 32
	MODULEENTRY32 ME;
	ME.dwSize = sizeof ( MODULEENTRY32 );

	// Begin checking the modules
	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPALL, 0 );

	// Start checking
	if ( M32F ( hSnapshot, &ME ) )
	{
		// Check if the address is in the range
		dwBaseAddr       = (DWORD) ME.modBaseAddr;
		DWORD dwLastAddr = (DWORD) ME.modBaseAddr + (DWORD) ME.modBaseSize;

		if ( dwAddr >= dwBaseAddr && 
			 dwAddr <= dwLastAddr )
		{
			// Set the 'isStatic' variable
			PtrInfo.isStatic = TRUE;
		}
		else
		{
			// Check every other module
			while ( M32N ( hSnapshot, &ME ) )
			{
				// Check if the address is in the range
				dwBaseAddr       = (DWORD) ME.modBaseAddr;
				DWORD dwLastAddr = (DWORD) ME.modBaseAddr + (DWORD) ME.modBaseSize;

				if ( dwAddr >= dwBaseAddr && 
					 dwAddr <= dwLastAddr )
				{
					// Set the 'isStatic' variable
					PtrInfo.isStatic = TRUE;

					break;
				}
			}
		}
	}

	// Check
	if ( PtrInfo.isStatic )
	{
		// Obtain the base address
		DWORD dwOffset = 0;

		// Obtain offset
		if ( dwAddr > dwBaseAddr )
			dwOffset = dwAddr - dwBaseAddr;
		else
			dwOffset = dwBaseAddr - dwAddr;

		// Convert offset to string
		stringstream ss;
		ss << (LPVOID) dwOffset;

		// Final string
		PointerInfo += (LPSTR) ME.szModule;
		PointerInfo += " + ";

		string Offset;
		Offset      += ss.str ();
		PointerInfo += RTrimZero ( Offset );

		// Set the string
		PtrInfo.PointerInfo = PointerInfo;
	}
	else {
		// Set the pointer information
		PointerInfo         += "Dynamic Address";
		PtrInfo.PointerInfo = PointerInfo;
	}

	return PtrInfo;
}
//

// Reading/Writing
template <class T> string CTT ( T Value )
{
	// Obtain the value in stringstream format
	if ( !strcmp ( typeid (T).name (), "unsigned char" ) )
	{
		stringstream ss;
		ss << (unsigned int) Value;
		return ss.str ();
	}
	else
	{
		stringstream ss;
		ss << Value;

		// Return the string
		return ss.str ();
	}
}

template <class T> bool   CMP ( DWORD dwAddr, T Value )
{
	__try {
		if ( *(T*) dwAddr == Value )
			return true;
		else 
			return false;
	}
	__except ( true ) {
		return false;
	}
}

template <class T> T      RV ( DWORD dwAddr )
{
	__try {
		return *(T*) dwAddr;
	}
	__except ( true ) {
		return -1;
	}
}

template <class T> void   WV ( DWORD dwAddr, T Value )
{
	__try 
	{
		// Protect the address
		DWORD dwDummy, dwflOldProtect;
		VirtualProtect ( (LPVOID) dwAddr, sizeof (T), PAGE_EXECUTE_READWRITE, &dwflOldProtect );

		// Write
		*(T*) dwAddr = Value;

		// Reprotect
		VirtualProtect ( (LPVOID) dwAddr, sizeof (T), dwflOldProtect, &dwDummy );
	}
	__except ( true ) 
	{
		// Failed to write
	}
}
template <class T> bool   R ( DWORD dwPtr, ADDR_INFO <T> * i )
{
	__try {
		i->dwAddr = dwPtr;
		i->Value  = *(T*) dwPtr;
		return true;
	}
	__except ( true ) {
		return false;
	}
}
bool					  RB ( DWORD dwAddress, BYTE * myBytes )
{
	__try {
		// Read 10 bytes
		memcpy ( myBytes, (void*) dwAddress, 16 );

		return true;
	}
	__except ( true ) {
		// Failed
		return false;
	}
}
bool                      CA ( DWORD dwAddr )
{
	__try 
	{
		if ( !memcmp ( (void*) dwAddr, pBytes, szBytes ) )
			return true;
		else
			return false;
	}
	__except ( true ) {
		return false;
	}
}
//

// Conversion
template <class T> string ConvertToText ( T Value )
{
	stringstream ss;
	ss << Value;
	return ss.str ();
}
template <class T, class R> R ConvertTextTo  ( HWND hWndDlg )
{
	// Obtain the text
	char szBuffer [256] = {0};
	GetWindowText ( hWndDlg, szBuffer, 256 );

	// Convert to stringstream
	stringstream ss;
	ss << szBuffer;

	if ( !strcmp ( typeid ( T ).name (), "unsigned char" ) )
	{
		// R 
		unsigned int pValue = 0;
		ss >> pValue;

		// Return
		return (BYTE) pValue;
	}
	else if ( !strcmp ( typeid ( T ).name (), "void *" ) )
	{		
		if ( !strcmp ( typeid ( R ).name (), "unsigned char" ) )
		{
			LPVOID lpAddr;
			ss >> lpAddr;

			unsigned int uiVal = (unsigned int) lpAddr;
			return (BYTE) lpAddr;
		}
		else if ( !strcmp ( typeid ( R ).name (), "unsigned short" ) )
		{
			LPVOID lpAddr;
			ss >> lpAddr;
			
			return (WORD) lpAddr;
		}
		else if ( !strcmp ( typeid ( R ).name (), "unsigned long" ) )
		{
			LPVOID lpAddr;
			ss >> lpAddr;
			
			return (DWORD) lpAddr;
		}
		else if ( !strcmp ( typeid ( R ).name (), "unsigned __uint64" ) )
		{
			LPVOID lpAddr;
			ss >> lpAddr;
			
			return (UINT64) lpAddr;
		}
	}

	// Pass it to our new value
	R Value = 0;

	// Pass
	ss >> Value;

	// Return 
	return Value;	
}
template <class T, class R> R ConvertTextTo2 ( string ValueStr )
{
	// Convert to stringstream
	stringstream ss;
	ss << ValueStr.c_str ();

	// Pass it to our new value
	R Value = 0;

	if ( !strcmp ( typeid ( T ).name (), "unsigned char" ) )
	{
		// R 
		unsigned int pValue = 0;
		ss >> pValue;

		// Return
		return (BYTE) pValue;
	}
	else if ( !strcmp ( typeid ( T ).name (), "void *" ) )
	{
		LPVOID lpAddr;
		ss >> lpAddr;
		
		return (DWORD) lpAddr;
	}
	else
		ss >> Value;

	// Return value
	return Value;
}
//

// Set settings
void MakeSettings ()
{
	// Setup the settings
	string Pathway = "";
	Pathway += CurDirectory.c_str ();
	Pathway += "\\LPMS.ini";

	int count = 0;

	// Begin reading
	ifstream SettingsAt ( Pathway.c_str () );

	while ( !SettingsAt.eof () )
	{
		string str;
		getline ( SettingsAt, str );

		switch ( count )
		{
		case 0:
			{
				if ( !strcmp ( str.c_str (), "TRUE" ) )
					MemMapped = true;
				else
					MemMapped = false;
			}
			break;

		case 1:
			{
				if ( !strcmp ( str.c_str (), "TRUE" ) )
					MemImage = true;
				else
					MemImage = false;
			}
			break;

		case 2:
			{
				if ( !strcmp ( str.c_str (), "TRUE" ) )
					MemPrivate = true;
				else
					MemPrivate = false;
			}
			break;

		case 3:
			{
				if ( !strcmp ( str.c_str (), "TRUE" ) )
					Epislon = true;
				else
					Epislon = false;
			}
			break;

		case 4:
			{
				Epislon = ConvertTextTo2 <float, float>  ( str );
			}
			break;

		case 5:
			{
				MaxThreads = ConvertTextTo2 <int, int> ( str );
			}
			break;

		case 6:
			{
				PreciseSize = ConvertTextTo2 <int, int> ( str );
			}
			break;

		case 7:
			{
				UnknownSize = ConvertTextTo2 <int, int> ( str );
			}
			break;
		}

		if ( count > 6 )
			break;

		++ count;

		Sleep (1);
	}
}
//

// Conversion
string CTS ( int value )
{
	stringstream ss;
	ss << value;
	return ss.str ();
}
string CTH ( int decimal )
{
	int number = decimal;
	int result = 1;
	int remainder = 1;
	string hex = "";

	while ( result >= 1 )
	{
		result = ( number / 16 );
		remainder = number % 16;
		
		if ( remainder == 1 )
			hex += CTS ( remainder );
		else if ( remainder < 10 )
			hex += CTS ( remainder );
		else
		{
			if ( remainder == 10 )
				hex += "A";
			else if ( remainder == 11 )
				hex += "B";
			else if ( remainder == 12 )
				hex += "C";
			else if ( remainder == 13 )
				hex += "D";
			else if ( remainder == 14 )
				hex += "E";
			else
				hex += "F";
		}

		number = result;
	}

	string reversedHex = "";

	for ( int i = hex.length () - 1; i > -1; i -- )
		reversedHex += hex [i];

	return reversedHex;
}
string CTB ( string text )
{
	string bytes = "";

	for ( SIZE_T i = 0; i < text.length (); i ++ )
	{
		char character = (char) text [i];
		int  toHex     = (int) character;
		bytes += CTH ( toHex );
		bytes += " ";
	}

	return bytes;
}
//

// Insertion
void IR  ( HWND hWndDlg, DWORD dwAddr )
{
	SMX ( hWndDlg, LB_ADDSTRING, NULL, (LPARAM) CTT <LPVOID> ( (LPVOID) dwAddr ).c_str () );
}
void IR2 ( string PTRINFO )
{
	// Add the message
	SMX ( GetDlgItem ( ptrDlg, IDC_LIST1 ), LB_ADDSTRING, 0, (LPARAM) PTRINFO.c_str () );
}
void IA  ( string Addr, string Type )
{
	// Get Item
	int item = ListView_GetItemCount ( GetDlgItem ( mainDlg, IDC_LIST2 ) );

	// Setup item
	LV_ITEM lv;
	lv.iItem    = item;           
	lv.iSubItem = 0;
	lv.mask     = LVIF_TEXT;       
	lv.pszText  = "";
	lv.cchTextMax = 256; 

	// Insert item
	ListView_InsertItem ( GetDlgItem ( mainDlg, IDC_LIST2 ), &lv );

	// Fix subitems
	for ( int i = 1; i <= 4; i ++ )
	{
		// Setup neccessary values
		lv.iSubItem   = i;
		lv.cchTextMax = 256;

		// Setup text
		string str = "";

		if ( i == 1 )
			str = Addr;
		else if ( i == 2 )
			str = Type;
		else
			str = "0";

		// Add item
		lv.pszText = (LPSTR) str.c_str ();

		// Insert subitem
		ListView_SetItem ( GetDlgItem ( mainDlg, IDC_LIST2 ), &lv );
	}
}
void IP  ( string Addr, string Type, string Pointer )
{
	// Get Item
	int item = ListView_GetItemCount ( GetDlgItem ( mainDlg, IDC_LIST2 ) );

	// Setup item
	LV_ITEM lv;
	lv.iItem    = item;           
	lv.iSubItem = 0;
	lv.mask     = LVIF_TEXT;       
	lv.pszText  = "";
	lv.cchTextMax = 256; 

	// Set pointer up
	PTR_INFO ptrStruct;
	ptrStruct.index       = item;
	ptrStruct.PointerName = Pointer;
	ptrInfoTwo [ ptrCountTwo ]  = ptrStruct;
	++ ptrCountTwo;

	// Insert item
	ListView_InsertItem ( GetDlgItem ( mainDlg, IDC_LIST2 ), &lv );

	// Fix subitems
	for ( int i = 1; i <= 4; i ++ )
	{
		// Setup neccessary values
		lv.iSubItem   = i;
		lv.cchTextMax = 256;

		// Setup text
		string str = "";

		if ( i == 1 )
			str = Addr;
		else if ( i == 2 )
			str = Type;
		else
			str = "0";

		// Add item
		lv.pszText = (LPSTR) str.c_str ();

		// Insert subitem
		ListView_SetItem ( GetDlgItem ( mainDlg, IDC_LIST2 ), &lv );
	}
}
void IM  ( char * MdlName, DWORD dwBase )
{
	// Get Item
	int item = ListView_GetItemCount ( GetDlgItem ( mdlDlg, IDC_LIST1 ) );

	// Setup the item
	LV_ITEM lv;
	lv.iItem    = item;           
	lv.iSubItem = 0;
	lv.mask     = LVIF_TEXT;       
	lv.pszText  = MdlName;
	lv.cchTextMax = 256; 

	// Insert the item
	ListView_InsertItem ( GetDlgItem ( mdlDlg, IDC_LIST1 ), &lv );

	// Convert the string to char
	string strAddr = CTT <LPVOID> ( (LPVOID) dwBase );

	// Setup the item
	lv.cchTextMax = 256;
	lv.pszText = (LPSTR) strAddr.c_str ();
	lv.iSubItem = 1;

	// Insert the item
	ListView_SetItem ( GetDlgItem ( mdlDlg, IDC_LIST1 ), &lv );
}
void ID  ( string Address, char * Instruction, string Opcode )
{
	// Get Item
	int item = ListView_GetItemCount ( GetDlgItem ( disDlg, IDC_LIST1 ) );

	// Setup item
	LV_ITEM lv;
	lv.iItem    = item;           
	lv.iSubItem = 0;
	lv.mask     = LVIF_TEXT;       
	lv.pszText  = "";
	lv.cchTextMax = 256; 

	// Insert item
	ListView_InsertItem ( GetDlgItem ( disDlg, IDC_LIST1 ), &lv );

	// Fix subitems
	for ( int i = 0; i < 3; i ++ )
	{
		// Setup neccessary values
		lv.iSubItem   = i;
		lv.cchTextMax = 256;

		// Setup text
		string str = "";

		if ( i == 0 )
			str = Address;
		else if ( i == 2 )
			str += Instruction;
		else
			str += Opcode;

		// Add item
		lv.pszText = (LPSTR) str.c_str ();

		// Insert subitem
		ListView_SetItem ( GetDlgItem ( disDlg, IDC_LIST1 ), &lv );
	}
}
void test ( string Address, char * Instruction, string Opcode )
{
	ID ( Address, Instruction, Opcode );
}
//

// No scan
void NS ()
{
	MODULEENTRY32 me;
	me.dwSize = sizeof ( MODULEENTRY32 );

	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPALL, 0 );

	if ( Module32First ( hSnapshot, &me ) )
	{
		if ( !strcmp ( DllName.c_str (), me.szModule  ) )
		{
			dwNoStartScan = (DWORD) me.modBaseAddr;
			dwNoStopScan  = (DWORD) me.modBaseAddr + (DWORD) me.modBaseSize;
		}
		else
		{
			while ( Module32Next ( hSnapshot, &me ) )
			{
				if ( !strcmp ( DllName.c_str (), me.szModule  ) )
				{
					dwNoStartScan = (DWORD) me.modBaseAddr;
					dwNoStopScan  = (DWORD) me.modBaseAddr + (DWORD) me.modBaseSize;
				}
			}
		}
	}
}
//

// Region Viewer
void InsertRegion ( void* BaseAddress, DWORD dwState, DWORD dwType, DWORD dwProtect, SIZE_T szReg )
{
	// Get Item
	int item = ListView_GetItemCount ( GetDlgItem ( regDlg, IDC_LIST1 ) );

	// Setup item
	LV_ITEM lv;
	lv.iItem    = item;           
	lv.iSubItem = 0;
	lv.mask     = LVIF_TEXT;       
	lv.pszText  = "";
	lv.cchTextMax = 256; 

	// Insert item
	ListView_InsertItem ( GetDlgItem ( regDlg, IDC_LIST1 ), &lv );

	// Fix subitems
	for ( int i = 0; i < 5; i ++ )
	{
		// Setup neccessary values
		lv.iSubItem   = i;
		lv.cchTextMax = 256;

		// Setup text
		string str = "";

		if ( i == 0 )
			str = CTT <LPVOID> ( BaseAddress );
		else if ( i == 1 )
		{
			switch ( dwState )
			{
			case MEM_COMMIT:
				{
					str = "Commit";
				}
				break;

			case MEM_RESERVE:
				{
					str = "Reserved";
				}
				break;

			case MEM_FREE:
				{
					str = "Free";
				}
				break;
			}
		}
		else if ( i == 2 )
		{
			switch ( dwType )
			{
			case MEM_PRIVATE:
				{
					str = "Private";
				}
				break;

			case MEM_IMAGE:
				{
					str = "Image";
				}
				break;

			case MEM_MAPPED:
				{
					str = "Mapped";
				}
				break;
			}
		}
		else if ( i == 3 )
		{
			switch ( dwProtect )
			{
			case PAGE_EXECUTE:
				{
					str = "PAGE_EXECUTE";
				}
				break;

			case PAGE_EXECUTE_READ:
				{
					str = "PAGE_EXECUTE_READ";
				}
				break;

			case PAGE_EXECUTE_READWRITE:
			{
				str = "PAGE_EXECUTE_READWRITE";
			}
			break;

			case PAGE_EXECUTE_WRITECOPY:
			{
				str = "PAGE_EXECUTE_WRITECOPY";
			}
			break;

			case PAGE_NOACCESS:
			{
				str = "PAGE_NOACCESS";
			}
			break;

			case PAGE_READONLY:
			{
				str = "PAGE_READONLY";
			}
			break;

			case PAGE_READWRITE:
			{
				str = "PAGE_READWRITE";
			}
			break;

			case PAGE_WRITECOPY:
			{
				str = "PAGE_WRITECOPY";
			}
			break;

			}
		}
		else
			str = CTT <LPVOID> ( (void*) szReg );

		// Add item
		lv.pszText = (LPSTR) str.c_str ();

		// Insert subitem
		ListView_SetItem ( GetDlgItem ( regDlg, IDC_LIST1 ), &lv );
	}
}
void ScanRegions ()
{
	// Clear the search 
	ListView_DeleteAllItems ( GetDlgItem ( regDlg, IDC_LIST1 ) ); 

	// Get the system information
	SYSTEM_INFO SI;
	GetSystemInfo ( &SI );

	for ( DWORD dwStart = (DWORD) SI.lpMinimumApplicationAddress; dwStart < (DWORD) SI.lpMaximumApplicationAddress; dwStart ++ )
	{
		// Obtain region information
		MEMORY_BASIC_INFORMATION MBI;
		VQ ( (LPCVOID) dwStart, &MBI, sizeof ( MEMORY_BASIC_INFORMATION ) );

		// Add the information
		InsertRegion ( MBI.BaseAddress, MBI.State, MBI.Type, MBI.Protect, MBI.RegionSize );

		// Go to the next region
		dwStart = (DWORD) MBI.BaseAddress + (DWORD) MBI.RegionSize;
	}
}
//

// Module-Viewer
void GM ()
{
	// Clear the listbox
	SMX ( GetDlgItem ( mdlDlg, IDC_LIST1 ), LVM_DELETEALLITEMS, NULL, NULL );

	// Create the snapshot
	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPALL, 0 );

	// Obtain the module information
	MODULEENTRY32 me;
	me.dwSize = sizeof ( MODULEENTRY32 );

	// Insert the information
	if ( M32F ( hSnapshot, &me ) )
	{
		IM ( me.szModule, (DWORD) me.modBaseAddr );

		while ( M32N ( hSnapshot, &me ) )
			IM ( me.szModule, (DWORD) me.modBaseAddr );
	}
}
//

// Locate all regions
void LocateAllRegions ( DWORD dwStartAddr, DWORD dwStopAddr )
{
	// Remove 
	if ( NumRegions != 0 )
		delete [] AllRegions;

	// Initialize
	AllRegions = new REGION_INFO [ 10000 ];
	NumRegions = 0;

	// Scan
	for ( DWORD begin = dwStartAddr;
			    begin < dwStopAddr;
				begin ++ )
	{
		// Obtain 
		MEMORY_BASIC_INFORMATION MBI;
		VQ ( (LPCVOID) begin, &MBI, sizeof ( MEMORY_BASIC_INFORMATION ) );

		// Check 
		if (                           (DWORD) MBI.BaseAddress  < dwNoStartScan || 
		   ( (DWORD) MBI.BaseAddress + (DWORD) MBI.RegionSize ) > dwNoStopScan )
		{
			if ( MBI.Type == ( MemPrivate == true ? MEM_PRIVATE : -1 ) ||
				 MBI.Type == ( MemImage   == true ? MEM_IMAGE   : -1 ) ||
				 MBI.Type == ( MemMapped  == true ? MEM_MAPPED  : -1 ) )
			{
				if ( MBI.RegionSize > UnknownSize )
				{
					SIZE_T allocationSize = 0;
					SIZE_T szDummy = MBI.RegionSize;
					DWORD  dwBase  = (DWORD) MBI.BaseAddress;
					int    Offset  = ( szDummy / UnknownSize ) + 1;

					for ( int x = 0 ; x < Offset; x ++ )
					{
						// Obtain allocation size
						if ( x == Offset - 1 )
							allocationSize = szDummy;
						else
							allocationSize = UnknownSize;

						// Set
						REGION_INFO regionInfo;
						regionInfo.dwBaseAddr = dwBase;
						regionInfo.szRegion   = allocationSize;
						dwBase               += allocationSize;

						// Add 
						AllRegions [ NumRegions ] = regionInfo;				
						NumRegions++;

						// Decrease
						szDummy -= allocationSize;
					}
				}
				else
				{
					// Set 
					REGION_INFO RegionInfo;
					RegionInfo.dwBaseAddr = (DWORD) MBI.BaseAddress;
					RegionInfo.szRegion   = MBI.RegionSize;		

					// Add 
					AllRegions [ NumRegions ] = RegionInfo;				
					NumRegions++;
				}
			}
		}

		// Next
		begin = (DWORD) MBI.BaseAddress + (DWORD) MBI.RegionSize;
	}
}

//

// File operations
void   WriteToFile   ( HANDLE hFile, LPCVOID Buffer, SIZE_T szSize )
{
	DWORD dwBytes;

	__try 
	{
		if ( !WriteFil ( hFile, Buffer, szSize, &dwBytes, 0 ) )
			la ( 0 );
	}
	__except ( true ) 
	{
		// Exception
	}
}
HANDLE szCreateFile  ( int scanNum )
{
	// Create the path
	string Path = CurDirectory;
	Path       += "\\Memory\\";
	Path       += "Memory";
	Path       += CTT ( scanNum );
	Path       += ".TMP";

	// Delete the file
	DeleteFile ( Path.c_str () );

	// Delete then create
	return CreateFile ( Path.c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
}

HANDLE szCreateFile2 ( string Path )
{
	// Create the path
	string NewPath = CurDirectory;
	NewPath       += "\\Memory\\";
	NewPath       += Path.c_str ();

	// Delete the file
	DeleteFile ( NewPath.c_str () );

	// Obtain the handle
	return CreateFile ( NewPath.c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
}
HANDLE szCreateFile3 ( string Path )
{
	// Delete the file
	DeleteFile ( Path.c_str () );

	// Obtain the handle
	return CreateFile ( Path.c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
}
HANDLE szObtainFile  ( int scanNum )
{
	// Create the path
	string Path = CurDirectory;
	Path       += "\\Memory\\";
	Path       += "Memory";
	Path       += CTT ( scanNum );
	Path       += ".TMP";

	return CreateFile ( Path.c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
}
string GetPathway    ( string Path )
{
	// Create the path
	string NewPath = CurDirectory;
	NewPath       += "\\Memory\\";
	NewPath       += Path.c_str ();

	return NewPath;
}
void   szDeletePath  ( int scanNum )
{
	// Create the path
	string Path = CurDirectory;
	Path       += "\\Memory\\";
	Path       += "Memory";
	Path       += CTT ( scanNum );
	Path       += ".TMP";

	DeleteFile ( Path.c_str () );
}
//

// 
void Setup ()
{
	finalResult = 0;
	temp_result = (int) result;

	// Create the handles
	hFile = CreateFile ( GetPathway ( "Memory.TMP" ).c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	hNew  = CreateFile ( GetPathway ( "Values.TMP" ).c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	// New files
	hAddress = szCreateFile2 ( "Memory2.TMP" );
	hValues  = szCreateFile2 ( "Values2.TMP" );

	// Initialize
	int endScan;

	if ( !unknownNumber )
		endScan = successRegion;
	else
		endScan = ( (int) result / UnknownSize ) + 1;

	// Setup progress bar
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_SHOWNORMAL );
	SMX        ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETPOS, (WPARAM) 0, 0 );
	SMX		   ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETRANGE, 0, MAKELPARAM ( 0, endScan ) );
	SMX		   ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETSTEP, (WPARAM) 1, 0 );
}
void Progress ()
{
	SMX ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_STEPIT, 0, 0 );
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_SHOWNORMAL );
}
void FinishNS ()
{
	++unknownNumber;

	// Setup final result
	result = finalResult;

	// Close the handles
	CloseHandle ( hFile );
	CloseHandle ( hNew );
	
	CloseHandle ( hAddress );
	CloseHandle ( hValues );

	// Delete the old buffer and rename
	DeleteFile ( GetPathway ( "OldMemory.TMP" ).c_str () );
	DeleteFile ( GetPathway ( "OldValues.TMP" ).c_str () );

	MoveFileEx ( GetPathway ( "Memory.TMP" ).c_str (), GetPathway ( "OldMemory.TMP" ).c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED );
	MoveFileEx ( GetPathway ( "Values.TMP" ).c_str (), GetPathway ( "OldValues.TMP" ).c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED );

	MoveFileEx ( GetPathway ( "Memory2.TMP" ).c_str (), GetPathway ( "Memory.TMP" ).c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED );
	MoveFileEx ( GetPathway ( "Values2.TMP" ).c_str (), GetPathway ( "Values.TMP" ).c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED );

	// Hide progress bar
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_HIDE );
}
//

// Unknown Scan
bool SetupRegions ()
{
	__try {
		SuccessReg = new int [10000];
		int element = 0;

		for ( int i = 0; i < MaxThreads; i ++ )
		{
			for ( int x = 0; x < si [i].goodReg; x ++ )
			{
				if ( AllRegions [ si [i].Regions [x] ].dwBaseAddr != 0 && 
					 AllRegions [ si [i].Regions [x] ].szRegion   != 0 )
				{
					if (            element <= NumRegions &&
						 si [i].Regions [x] <= NumRegions )
					{
						SuccessReg [element] = si [i].Regions [x];
						++element;
					}
				}
			}

			successRegion += si [ i ].goodReg;
		}

		return true;
	}
	__except ( true )
	{
		MessageBoxA ( mainDlg, "An error has occured.", "Error", MB_ICONINFORMATION );
		SMX         ( mainDlg, WM_COMMAND, IDC_BUTTON8, NULL );

		return false;
	}
}
//

// Memory Scanning Routines
int					   US  ( LPVOID lpParam )
{
	// Scan the memory
	for ( int i = si [ (int) lpParam ].regionStart; i <= si [ (int) lpParam ].regionStop; i ++ )
	{
		MEMORY_BASIC_INFORMATION MBI;
		VirtualQuery ( (LPCVOID) AllRegions [i].dwBaseAddr, &MBI, sizeof ( MEMORY_BASIC_INFORMATION ) );

		if ( MBI.Protect == READABLE )
		{
			// Write information
			DWORD dwBytes;

			if ( WriteFil ( si [ (int) lpParam ].hFile, (void*) AllRegions [i].dwBaseAddr, AllRegions [i].szRegion, &dwBytes, 0 ) )
			{
				// Increment
				si [ (int) lpParam ].results += AllRegions [i].szRegion;
				si [ (int) lpParam ].Regions [ si [ (int) lpParam ].goodReg ] = i;
				si [ (int) lpParam ].goodReg ++;	
			}
		}
	}

	// Notify the scan has finished
	si [ (int) lpParam ].finished = true;

	return 0;
}
template <class T> int FS  ( LPVOID lpParam )
{
	// See if the value is hex
	bool hex = SMX ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;

	// Obtain the value
	T Value;
	
	if ( hex )
		Value = ConvertTextTo <LPVOID, T> ( GetDlgItem ( mainDlg, IDC_EDIT1 ) );
	else
		Value = ConvertTextTo <T, T>	  ( GetDlgItem ( mainDlg, IDC_EDIT1 ) );

	__try 
	{
		for ( int i = si [ (int) lpParam ].regionStart; i <= si [ (int) lpParam ].regionStop; i ++ )
		{
			MEMORY_BASIC_INFORMATION MBI;
			VirtualQuery ( (LPCVOID) AllRegions [i].dwBaseAddr, &MBI, sizeof ( MEMORY_BASIC_INFORMATION ) );

			if ( MBI.Protect == READABLE )
			{
				// Variables
				DWORD  *dwBuffer;
				DWORD   dwBytes;

				int temp_result = 0;

				// Scan the region
				__try 
				{
					temp_result = 0;

					// Obtain buffer info
					dwBuffer = new DWORD [ AllRegions [i].szRegion ];

					// Begin reading the region
					for ( DWORD begin = (DWORD) AllRegions [i].dwBaseAddr;
								begin < (DWORD) AllRegions [i].dwBaseAddr + (DWORD) AllRegions [i].szRegion - 1 - sizeof (T);
								begin ++ )
					{
						__try
						{
							if ( fabs ( (float) ( *(float*) begin - Value ) ) <= (float) Epislon ) 
							{
								dwBuffer [temp_result] = begin;
								++ temp_result;
							}
						}
						__except ( true ) 
						{
							break;
						}
					}

					// Write results into buffer
					::WriteFil ( si [ (int) lpParam ].hFile, dwBuffer, ( temp_result * 4 ), &dwBytes, 0 );

					// Increment results
					si [ (int) lpParam ].results += temp_result;

					// Delete the buffer
					delete [] dwBuffer;
				}
				__except ( true ) 
				{
					// Delete the buffer
					delete [] dwBuffer;
				}
			}
		}
	}
	__except ( true ) 
	{
		// Exception
	}

	// Notify the scan has finished
	si [ (int) lpParam ].finished = true;

	return 0;
}
template <class T> int DS  ( LPVOID lpParam )
{
	// See if the value is hex
	bool hex = SMX ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;

	// Obtain the value
	T Value;
	
	if ( hex )
		Value = ConvertTextTo <LPVOID, T> ( GetDlgItem ( mainDlg, IDC_EDIT1 ) );
	else
		Value = ConvertTextTo <T, T>	  ( GetDlgItem ( mainDlg, IDC_EDIT1 ) );

	__try 
	{
		for ( int i = si [ (int) lpParam ].regionStart; i <= si [ (int) lpParam ].regionStop; i ++ )
		{
			MEMORY_BASIC_INFORMATION MBI;
			VirtualQuery ( (void*) AllRegions [i].dwBaseAddr, &MBI, sizeof ( MEMORY_BASIC_INFORMATION ) );

			if ( MBI.Protect == READABLE )
			{
				// Variables	
				DWORD  *dwBuffer;
				DWORD   dwBytes;

				int temp_result = 0;

				// Scan the region
				__try 
				{
					temp_result = 0;

					// Obtain buffer info
					dwBuffer = new DWORD [ AllRegions [i].szRegion ];

					// Begin reading the region
					for ( DWORD begin = (DWORD) AllRegions [i].dwBaseAddr;
								begin < (DWORD) AllRegions [i].dwBaseAddr + (DWORD) AllRegions [i].szRegion - 1 - sizeof (T);
								begin ++ )
					{
						__try
						{
							if ( fabs ( (double) ( *(double*) begin - Value ) ) <= (double) Epislon ) 
							{
								dwBuffer [temp_result] = begin;
								++ temp_result;
							}
						}
						__except ( true ) 
						{
							break;
						}
					}

					// Write results into buffer
					::WriteFil ( si [ (int) lpParam ].hFile, dwBuffer, ( temp_result * 4 ), &dwBytes, 0 );

					// Increment results
					si [ (int) lpParam ].results += temp_result;

					// Delete the buffer
					delete [] dwBuffer;
				}
				__except ( true )
				{
					// Delete the buffer
					delete [] dwBuffer;
				}
			}
		}
	}
	__except ( true ) 
	{
		// Exception
	}

	// Notify the scan has finished
	si [ (int) lpParam ].finished = true;

	return 0;
}
template <class T> int ES  ( LPVOID lpParam )
{
	// See if the value is hex
	bool hex = SMX ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;

	// Obtain the value
	T Value;
	
	if ( hex )
		Value = ConvertTextTo <LPVOID, T> ( GetDlgItem ( mainDlg, IDC_EDIT1 ) );
	else
		Value = ConvertTextTo <T, T>	  ( GetDlgItem ( mainDlg, IDC_EDIT1 ) );

	__try 
	{
		for ( int i = si [ (int) lpParam ].regionStart; i <= si [ (int) lpParam ].regionStop; i ++ )
		{
			MEMORY_BASIC_INFORMATION MBI;
			VirtualQuery ( (LPCVOID) AllRegions [i].dwBaseAddr, &MBI, sizeof ( MEMORY_BASIC_INFORMATION ) );

			if ( MBI.Protect == READABLE )
			{
				// Variables	
				DWORD  *dwBuffer;
				DWORD   dwBytes;
				int temp_result = 0;

				// Scan the region
				__try 
				{				
					temp_result = 0;

					// Obtain buffer info
					dwBuffer = new DWORD [ AllRegions [i].szRegion ];
					
					// Begin reading the region
					for ( DWORD begin = (DWORD) AllRegions [i].dwBaseAddr;
								begin < (DWORD) AllRegions [i].dwBaseAddr + (DWORD) AllRegions [i].szRegion - 1 - sizeof (T);
								begin ++ )
					{
						__try
						{
							if ( *(T*) begin == Value )
							{
								dwBuffer [ temp_result ] = begin;
								++temp_result;
							}
						}
						__except ( true )
						{
							break;
						}
					}
					
					// Write results into buffer
					::WriteFile ( si [ (int) lpParam ].hFile, dwBuffer, ( temp_result * 4 ), &dwBytes, 0 );

					// Increment results
					si [ (int) lpParam ].results += temp_result;

					// Delete the buffer
					delete [] dwBuffer;
				}
				__except ( true ) 
				{
					// Delete the buffer
					delete [] dwBuffer;
				}
			}
		}
	}
	__except ( true ) 
	{
		// Exception
	}

	// Notify the scan has finished
	si [ (int) lpParam ].finished = true;

	return 0;
}

template <class T> int SFB ( LPVOID lpParam )
{
	__try 
	{
		for ( int i = si [ (int) lpParam ].regionStart; i <= si [ (int) lpParam ].regionStop; i ++ )
		{
			MEMORY_BASIC_INFORMATION MBI;
			VirtualQuery ( (LPCVOID) AllRegions [i].dwBaseAddr, &MBI, sizeof ( MEMORY_BASIC_INFORMATION ) );

			if ( MBI.Protect == READABLE )
			{
				// Variables	
				DWORD  *dwBuffer;
				DWORD   dwBytes;

				int temp_result = 0;

				// Scan the region
				__try 
				{
					temp_result = 0;

					// Obtain buffer info
					dwBuffer = new DWORD [ AllRegions [i].szRegion ];

					// Begin reading the region
					for ( DWORD begin = (DWORD) AllRegions [i].dwBaseAddr;
								begin < (DWORD) AllRegions [i].dwBaseAddr + (DWORD) AllRegions [i].szRegion - 1 - sizeof (T);
								begin ++ )
					{
						if ( !memcmp ( (void*) begin, pBytes, szBytes ) ) 
						{
							dwBuffer [temp_result] = begin;
							++temp_result;
						}			
					}

					// Write results into buffer
					::WriteFil ( si [ (int) lpParam ].hFile, dwBuffer, ( temp_result * 4 ), &dwBytes, 0 );

					// Increment results
					si [ (int) lpParam ].results += temp_result;

					// Delete the buffer
					delete [] dwBuffer;
				}
				__except ( true ) {
					// Delete the buffer
					delete [] dwBuffer;
				}
			}
		}
	}
	__except ( true ) 
	{
		// Notify the scan has finished
		si [ (int) lpParam ].finished = true;

		return 0;
	}

	// Notify the scan has finished
	si [ (int) lpParam ].finished = true;

	return 0;
}

//

// Sophisticated Thread System (STS)
template <class T> void STS ( int NumberOfThreads, DWORD dwStart, DWORD dwStop )
{
	// Reset windows
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS ), SW_HIDE );

	// Reset variables
	siGone = false;

	// Delete files
	DeleteFile ( GetPathway ( "Memory.TMP" ).c_str () );
	DeleteFile ( GetPathway ( "OldMemory.TMP" ).c_str () );
	DeleteFile ( GetPathway ( "Values.TMP" ).c_str () );
	DeleteFile ( GetPathway ( "OldValues.TMP" ).c_str () );
	DeleteFile ( GetPathway ( "Merge.bat" ).c_str () );
	DeleteFile ( GetPathway ( "Memory2.TMP" ).c_str () );
	DeleteFile ( GetPathway ( "Values2.TMP" ).c_str () );

	// Create threads
	result         = 0;
	threadsCreated = 0;

	// Setup variables
	int offset = NumRegions / NumberOfThreads;

	// Setup variables
	si			  = new SCANINFO [ NumberOfThreads ];
	threadsWorked = new int      [ NumberOfThreads ];

	// Create threads
	for ( int i = 0; i < NumberOfThreads; i ++ )
	{
		// Setup our si variable
		si [i].finished = false;
		si [i].results  = 0;

		// Setup the beginning scan of the regions
		if ( !i )
			si [i].regionStart = 0;
		else
			si [i].regionStart = si [ i - 1 ].regionStop + 1;

		// Setup the ending scan of the regions
		if ( NumberOfThreads == 1 )
			si [i].regionStop = NumRegions;
		else if ( i == ( NumberOfThreads - 1 ) )
			si [i].regionStop = NumRegions;
		else
			si [i].regionStop = si [i].regionStart + offset;

		if ( si [i].regionStop > NumRegions )
			si [i].regionStop = NumRegions;

		// Setup 'si'
		si [i].goodReg = 0;
		si [i].scanNum = i;

		// Setup file
		si [i].hFile = szCreateFile ( i );

		// Array of bytes
		if ( ArrayOfBytes )
			si [i].hThread = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) SFB <T>, (void*) i, NULL, NULL );
		else if ( !unknownScan  )
		{
			if      ( !strcmp ( typeid (T).name (), "float" ) )
				si [i].hThread = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) FS <T>, (void*) i, NULL, NULL );
			else if ( !strcmp ( typeid (T).name (), "double" ) )
				si [i].hThread = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) DS <T>, (void*) i, NULL, NULL );
			else
				si [i].hThread = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) ES <T>, (void*) i, NULL, NULL );
		}
		else if ( unknownScan )
		{
			si [i].Regions = new int [1000];

			// Setup 'si'
			si [i].hThread = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) US, (void*) i, NULL, NULL );
		}

		// Check if the thread passed
		if ( si [i].hThread != NULL )
		{
			threadsWorked [ threadsCreated ] = i;
			++threadsCreated;
		}
	}

	// Setup progress bar
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_SHOW );
	SMX        ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETPOS, (WPARAM) 0, 0 );
	SMX		   ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETRANGE, 0, MAKELPARAM ( 0, threadsCreated ) );
	SMX		   ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETSTEP, (WPARAM) 1, 0 );

	// Create our finished variable
	int finished;
		
	if ( threadsCreated == 1 )
		finished = -1;
	else
		finished = 0;

	// Check if the threads finished
	while ( true )
	{
		for ( int i = 0; i < threadsCreated; i ++ )
		{
			if ( si [ threadsWorked [i] ].finished == true )
			{
				// Increment finished
				result += si [ threadsWorked [i] ].results; 
				++finished;

				// Increment progress bar
				SMX ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_STEPIT, 0, 0 );
				ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_SHOWNORMAL );

				// Remove the thread
				DWORD dwExitCode;
				GetExitCodeThread ( si [ threadsWorked [i] ].hThread, &dwExitCode );
				TerminateThread   ( si [ threadsWorked [i] ].hThread, dwExitCode  );

				// Close the files
				CloseHandle ( si [ threadsWorked [i] ].hFile );

				si [ threadsWorked [i] ].finished = false;
			}
		}

		if ( NumberOfThreads == 1 )
		{
			if ( finished == threadsCreated - 1 )
				break;
		}
		else if ( finished == threadsCreated )
			break;		

		// Sleep 
		Sleep ( 100 );
	}

	// Setup successful regions
	if ( unknownScan )
		SetupRegions ();

	// Delete variables
	delete [] threadsWorked;
	delete [] si;
	siGone = true;

	// Notify 
	Merging = true;
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_HIDE );
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS ), SW_NORMAL );
	SetWindowText ( GetDlgItem ( mainDlg, IDC_PROGRESS ), "Merging.." );

	// Notify
	char Finished [] = "Ready";

	HANDLE hFile = szCreateFile3 ( "C:\\MergeOver.txt" );

	DWORD bytesWritten;
	WriteFile ( hFile, &Finished, strlen ( "Ready" ), &bytesWritten, NULL );

	CloseHandle ( hFile );

	// Wait
	while ( Merging )
	{
		string Data;
		ifstream ReadMerge ( "C:\\MergeOver.txt" );

		while ( !ReadMerge.eof () )
		{
			getline ( ReadMerge, Data );

			if ( !strcmp ( Data.c_str (), "Done" ) )
			{
				ReadMerge.close ();

				Merging = false;
				break;
			}
		}
		
		ReadMerge.close ();
		Sleep ( 100 );
	}

	// Delete merge
	DeleteFile ( "C:\\MergeOver.txt" );
}
//

// First Scan
template <class T> void GE ()
{
	// Reset variables
	Merging = false;
	result  = 0;

	// Obtain scanning type
	char szScan [256] = {0};
	GetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), szScan, 256 );	

	// Call the first scan
	if ( ArrayOfBytes )
	{
		// Setup variables
		string test;
		char szBuffer [256] = {0};
		GetWindowText ( GetDlgItem ( mainDlg, IDC_EDIT1 ), szBuffer, 256 );

		// Obtain the value
		if ( !strcmp ( szScan, "Array of Bytes" ) ) {
			test += szBuffer;
		}
		else {
			string buffer;
			buffer += szBuffer;		
			test += CTB ( buffer );
		}

		szBytes = 0;

		// Obtain the number of bytes
		for ( SIZE_T i = 0; i < test.length (); i ++ )
		{
			// Avoid an exception
			if ( i + 1 < test.length () )
			{
				// Find the number of bytes 
				if ( test.at (i) != ' ' )
				{
					// If you can form a two letter word
					if ( test.at ( i + 1 ) != ' ' )
					{
						++ szBytes;
					}
				}
			}
		}

		// Setup the variable
		pBytes = new BYTE [ szBytes ];
		SIZE_T Dummy = 0;
		
		// Create the bytes
		for ( SIZE_T i = 0; i < test.length (); i ++ )
		{
			// Avoid an exception
			if ( i + 1 < test.length () )
			{
				// Write the bytes
				if ( test.at (i) != ' ' )
				{
					// If you can form a two letter word
					if ( test.at ( i + 1 ) != ' ' )
					{
						// Obtain the string format
						string str;
						str += test.at (i);
						str += test.at (i + 1);

						// Convert it to LPVOID
						stringstream ss;
						ss << str;

						LPVOID lpVal;
						ss >> lpVal;

						// Check
						pBytes [Dummy] = (unsigned int) lpVal;
						
						++ Dummy;
						++ i;
					}
				}
			}
		}

		scanNum = 0;
		unknownScan = false;
	}
	else
	{
		if ( !strcmp ( szScan, "Unknown" ) )
		{	
			successRegion = 0;
			unknownNumber = 0;

			// Set settings
			scanNum     = -2;
			unknownScan = true;
		}
		else if ( !strcmp ( szScan, "Precise" ) )
		{
			// Set settings
			scanNum     = 0;
			unknownScan = false;		
		}
	}

	// Locate all regions
	LocateAllRegions ( dwStartScan, dwStopScan );

	// Begin scanning
	STS <T> ( MaxThreads, dwStartScan, dwStopScan );	
	
	// Reset scanning options
	ComboBox_ResetContent ( GetDlgItem ( mainDlg, IDC_COMBO2 ) );

	// Add new options
	if ( !ArrayOfBytes )
	{
		for ( int i = 0; i < _countof ( NSN2 ); i ++ )
			ComboBox_AddString ( GetDlgItem ( mainDlg, IDC_COMBO2 ), NSN2 [i] );

		// Unknown Initial Scan options
		if ( unknownScan )
		{
			SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, NULL, (LPARAM) "Increased by" );
			SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, NULL, (LPARAM) "Decreased by" );
			SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, NULL, (LPARAM) "Increased" );
			SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, NULL, (LPARAM) "Decreased" );
			SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, NULL, (LPARAM) "Changed" );
			SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, NULL, (LPARAM) "Unchanged" );
		}
	}

	// Set the new text
	if ( ArrayOfBytes )
		SetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), oldScan );
	else
		SetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), "Precise" );

	// Display results
	SetWindowText ( GetDlgItem ( mainDlg, IDC_RESULT ), CTT ( result ).c_str () );

	// Notify 
	SetWindowText ( GetDlgItem ( mainDlg, IDC_PROGRESS ), "" );
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS ), SW_HIDE );

	// Enable next scan
	Button_Enable ( GetDlgItem ( mainDlg, IDC_BUTTON2 ), TRUE );

	// Enable scan finished
	ScanFinished = true;

	// Beep
	MakeSound ();

	// Close the handle
	DWORD dwExitCode;
	GetExitCodeThread ( GetCurrentThread (), &dwExitCode );
	TerminateThread   ( GetCurrentThread (), dwExitCode  );
}
//

// Next Scan
void                    NS3 ()
{
	int newResult   = 0;
	int finalResult = 0;
	int temp_result = (int) result;

	// Create the handle to the file
	hFile = CreateFile ( GetPathway ( "Memory.TMP" ).c_str (),  GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	hNew  = CreateFile ( GetPathway ( "Memory2.TMP" ).c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );

	// Obtain the scan type
	char szScan [256] = {0};
	GetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), szScan, 256 );

	// Create the offset
	int offset = ( (int) result / PreciseSize) + 1;

	// Set up progress bar
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_SHOWNORMAL );
	SMX        ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETPOS, (WPARAM) 0, 0 );
	SMX		   ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETRANGE, 0, MAKELPARAM ( 0, offset ) );
	SMX		   ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETSTEP, (WPARAM) 1, 0 );

	// Begin
	for ( int readLoop = 0; readLoop < offset; readLoop ++ )
	{
		// Reset new result
		newResult = 0;

		// Obtain allocation size
		SIZE_T allocationSize;

		if ( readLoop == ( offset - 1 ) )
			allocationSize = (SIZE_T) result;
		else
			allocationSize = PreciseSize;

		// Initialize the variable
		dwScan = new DWORD [ allocationSize ];

		// Read the file
		DWORD dwBytes;
		::ReadFile ( hFile, dwScan, (allocationSize * 4), &dwBytes, 0 );

		// Begin the scan
		for ( SIZE_T i = 0; i < allocationSize; i ++ )
		{
			if ( CA ( dwScan [i] ) ) 
			{
				dwScan [newResult] = dwScan [i];
				++newResult;
			}
		}

		// Write the results
		DWORD dwWrite;
		::WriteFil ( hNew, dwScan, ( newResult * 4 ), &dwWrite, 0 );

		// Subtract from result
		result      -= allocationSize;
		finalResult += newResult;		

		// Delete the buffer		
		delete [] dwScan;

		// Increment progress bar
		SMX ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_STEPIT, 0, 0 );
		ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_SHOWNORMAL );
	}

	// Setup final result
	result = finalResult;
	++scanNum;

	// Close the handles
	CloseHandle ( hFile );
	CloseHandle ( hNew  );

	// Delete the old buffer and rename
	DeleteFile ( GetPathway ( "OldMemory.TMP" ).c_str () );
	MoveFileEx ( GetPathway ( "Memory.TMP" ).c_str (),  GetPathway ( "OldMemory.TMP" ).c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED );
	MoveFileEx ( GetPathway ( "Memory2.TMP" ).c_str (), GetPathway ( "Memory.TMP" ).c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED );

	// Hide progress bar
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_HIDE );
}
template <class T> void NS2 ( T Value )
{
	// Declare
	DWORD dwEndAddr = NULL;

	// Initialize scan
	Setup ();

	// Obtain the scan type
	char szScan [256] = {0};
	GetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), szScan, 256 );

	// Begin
	if ( !unknownNumber )
	{
		// Begin
		for ( int x = 0; x < successRegion; x ++ )
		{
			// Reset
			newResult = 0;

			// Declare
			DWORD * Pointers;
			BYTE  * Values;

			// Attempt
			__try 
			{
				// Create
				dwEndAddr = AllRegions [ SuccessReg [x] ].dwBaseAddr + (DWORD) ( AllRegions [ SuccessReg [x] ].szRegion - 1 - sizeof (T) );

				// Initialize the variable
				Pointers = new DWORD [ AllRegions [ SuccessReg [x] ].szRegion ];
				Values   = new BYTE  [ AllRegions [ SuccessReg [x] ].szRegion * sizeof (T) ];

				// Obtain memory info
				DWORD dwBytes;
				::ReadFile ( hFile, Values, AllRegions [ SuccessReg [x] ].szRegion, &dwBytes, 0 );

				// Scanning Options
				if ( !strcmp ( szScan, "Precise" ) )
				{
					// Go through the results				
					for ( DWORD Addr = AllRegions [ SuccessReg [x] ].dwBaseAddr; Addr < dwEndAddr; Addr ++ )
					{					
						// Read the value
						T check = RV <T> ( Addr );

						if ( check == Value )
						{
							// Set information
							*(T*) &Values  [newResult * sizeof (T)] = check;
							Pointers [newResult] = Addr;

							// Increment results
							++ newResult;
						}
					}
				}
				else if ( !strcmp ( szScan, "Greater Than" ) )
				{
					for ( DWORD Addr = AllRegions [ SuccessReg [x] ].dwBaseAddr; Addr < dwEndAddr; Addr ++ )
					{
						T check = *(T*) Addr;

						if ( check > Value )
						{
							// Set information
							*(T*) (DWORD) &Values  [newResult * sizeof (T)] = check;
							Pointers [newResult] = Addr;

							// Increment results
							++ newResult;
						}
					}
				}
				else if ( !strcmp ( szScan, "Lower Than" ) )
				{
					for ( DWORD Addr = AllRegions [ SuccessReg [x] ].dwBaseAddr; Addr < dwEndAddr; Addr ++ )
					{
						T check = *(T*) Addr;

						if ( check < Value )
						{
							// Set information
							*(T*) (DWORD) &Values  [newResult * sizeof (T)] = check;
							Pointers [newResult] = Addr;

							// Increment results
							++ newResult;
						}
					}
				}
				else if ( !strcmp ( szScan, "Increased by" ) )
				{
					for ( DWORD Addr = AllRegions [ SuccessReg [x] ].dwBaseAddr; Addr < dwEndAddr; Addr ++ )
					{
						T check = *(T*) Addr;

						if ( check - *(T*) &Values [ (Addr - AllRegions [ SuccessReg [x] ].dwBaseAddr) * sizeof (T)] == Value )
						{
							// Set information
							*(T*) (DWORD) &Values  [newResult * sizeof (T)] = check;
							Pointers [newResult] = Addr;

							// Increment results
							++ newResult;
						}
					}
				}
				else if ( !strcmp ( szScan, "Decreased by" ) )
				{
					for ( DWORD Addr = AllRegions [ SuccessReg [x] ].dwBaseAddr; Addr < dwEndAddr; Addr ++ )
					{
						T check = *(T*) Addr;

						if ( *(T*) (DWORD) &Values [ (Addr - AllRegions [ SuccessReg [x] ].dwBaseAddr) ] - check == Value )
						{
							// Set information
							*(T*) (DWORD) &Values  [newResult * sizeof (T)] = check;
							Pointers [newResult] = Addr;

							// Increment results
							++ newResult;
						}
					}
				}
				else if ( !strcmp ( szScan, "Increased" ) )
				{
					for ( DWORD Addr = AllRegions [ SuccessReg [x] ].dwBaseAddr; Addr < dwEndAddr; Addr ++ )
					{
						T check = *(T*) Addr;

						if ( check > *(T*) &Values [ (Addr - AllRegions [ SuccessReg [x] ].dwBaseAddr) ] )
						{
							// Set information
							*(T*) (DWORD) &Values  [newResult * sizeof (T)] = check;
							Pointers [newResult] = Addr;

							// Increment results
							++ newResult;
						}
					}
				}
				else if ( !strcmp ( szScan, "Decreased" ) )
				{
					for ( DWORD Addr = AllRegions [ SuccessReg [x] ].dwBaseAddr; Addr < dwEndAddr; Addr ++ )
					{
						T check = *(T*) Addr;

						if ( check < *(T*) &Values [ (Addr - AllRegions [ SuccessReg [x] ].dwBaseAddr) ] )
						{
							// Set information
							*(T*) (DWORD) &Values  [newResult * sizeof (T)] = check;
							Pointers [newResult] = Addr;

							// Increment results
							++ newResult;
						}
					}
				}
				else if ( !strcmp ( szScan, "Changed" ) )
				{
					for ( DWORD Addr = AllRegions [ SuccessReg [x] ].dwBaseAddr; Addr < dwEndAddr; Addr ++ )
					{
						T check = *(T*) Addr;

						if ( check != *(T*) &Values [ (Addr - AllRegions [ SuccessReg [x] ].dwBaseAddr) ] )
						{
							// Set information
							*(T*) (DWORD) &Values  [newResult * sizeof (T)] = check;
							Pointers [newResult] = Addr;

							// Increment results
							++ newResult;
						}
					}
				}
				else if ( !strcmp ( szScan, "Unchanged" ) )
				{
					for ( DWORD Addr = AllRegions [ SuccessReg [x] ].dwBaseAddr; Addr < dwEndAddr; Addr ++ )
					{
						T check = *(T*) Addr;

						if ( check == *(T*) &Values [ (Addr - AllRegions [ SuccessReg [x] ].dwBaseAddr) ] )
						{
							// Set information
							*(T*) (DWORD) &Values  [newResult * sizeof (T)] = check;
							Pointers [newResult] = Addr;

							// Increment results
							++ newResult;
						}
					}
				}

				// Write 
				WriteToFile ( hAddress, Pointers, newResult * 4 );
				WriteToFile ( hValues,    Values, newResult * sizeof (T) );

				// Subtract from result
				finalResult += newResult;

				// Delete the buffer		
				delete [] Pointers;
				delete [] Values;

				Progress ();
			}
			__except ( true )
			{
				// Delete the buffer		
				delete [] Pointers;
				delete [] Values;

				Progress ();
			}
		}
	}
	else 
	{
		SIZE_T offset = ( (SIZE_T) result / UnknownSize ) + 1;

		for ( SIZE_T readLoop = 0; readLoop < offset; readLoop ++ )
		{
			// Reset new result
			newResult = 0;

			// Obtain allocation size
			SIZE_T allocationSize;

			if ( readLoop == ( offset - 1 ) )
				allocationSize = (SIZE_T) result;
			else
				allocationSize = UnknownSize;

			// Initialize the variable
			DWORD * dwAddress = new DWORD [ allocationSize ];
			BYTE  * Values    = new BYTE  [ allocationSize * sizeof (T) ];

			// Obtain the information
			DWORD dwBytes;
			::ReadFile ( hFile, dwAddress, allocationSize * sizeof (DWORD), &dwBytes, 0 );
			::ReadFile ( hNew,     Values, allocationSize * sizeof (T), &dwBytes, 0 );

			// Scan
			if ( !strcmp ( szScan, "Precise" ) )
			{
				for ( SIZE_T i = 0; i < allocationSize; i ++ )
				{
					T check = RV <T> ( dwAddress [i] );

					if ( check == Value )
					{
						*(T*) &Values  [newResult * sizeof (T)] = check;
						dwAddress      [newResult] = dwAddress [i];
						++ newResult;
					}
				}
			}
			else if ( !strcmp ( szScan, "Greater Than" ) )
			{
				for ( SIZE_T i = 0; i < allocationSize; i ++ )
				{
					T check = RV <T> ( dwAddress [i] );

					if ( check > Value )
					{
						*(T*) &Values  [newResult * sizeof (T)] = check;
						dwAddress      [newResult] = dwAddress [i];
						++ newResult;
					}
				}
			}
			else if ( !strcmp ( szScan, "Lower Than" ) )
			{
				for ( SIZE_T i = 0; i < allocationSize; i ++ )
				{
					T check = RV <T> ( dwAddress [i] );

					if ( check < Value )
					{
						*(T*) &Values  [newResult * sizeof (T)] = check;
						dwAddress      [newResult] = dwAddress [i];
						++ newResult;
					}
				}
			}
			else if ( !strcmp ( szScan, "Increased by" ) )
			{
				for ( SIZE_T i = 0; i < allocationSize; i ++ )
				{
					T check = RV <T> ( dwAddress [i] );

					if ( check - *(T*) &Values [i * sizeof (T)] == Value )
					{
						*(T*) &Values  [newResult * sizeof (T)] = check;
						dwAddress      [newResult] = dwAddress [i];
						++ newResult;
					}
				}
			}
			else if ( !strcmp ( szScan, "Decreased by" ) )
			{
				for ( SIZE_T i = 0; i < allocationSize; i ++ )
				{
					T check = RV <T> ( dwAddress [i] );

					if ( *(T*) &Values [i * sizeof (T)] - check == Value )
					{
						*(T*) &Values  [newResult * sizeof (T)] = check;
						dwAddress      [newResult] = dwAddress [i];
						++ newResult;
					}
				}
			}
			else if ( !strcmp ( szScan, "Increased" ) )
			{
				for ( SIZE_T i = 0; i < allocationSize; i ++ )
				{
					T check = RV <T> ( dwAddress [i] );

					if ( check > *(T*) &Values [i * sizeof (T)] )
					{
						*(T*) &Values  [newResult * sizeof (T)] = check;
						dwAddress      [newResult] = dwAddress [i];
						++ newResult;
					}
				}
			}
			else if ( !strcmp ( szScan, "Decreased" ) )
			{
				for ( SIZE_T i = 0; i < allocationSize; i ++ )
				{
					T check = RV <T> ( dwAddress [i] );

					if ( check < *(T*) &Values [i * sizeof (T)] )
					{
						*(T*) &Values  [newResult * sizeof (T)] = check;
						dwAddress      [newResult] = dwAddress [i];
						++ newResult;
					}
				}
			}
			else if ( !strcmp ( szScan, "Changed" ) )
			{
				for ( SIZE_T i = 0; i < allocationSize; i ++ )
				{
					T check = RV <T> ( dwAddress [i] );

					if ( check != *(T*) &Values [i * sizeof (T)] )
					{
						*(T*) &Values  [newResult * sizeof (T)] = check;
						dwAddress      [newResult] = dwAddress [i];
						++ newResult;
					}
				}
			}
			else if ( !strcmp ( szScan, "Unchanged" ) )
			{
				for ( SIZE_T i = 0; i < allocationSize; i ++ )
				{
					T check = RV <T> ( dwAddress [i] );

					if ( check == *(T*) &Values [i * sizeof (T)] )
					{
						*(T*) &Values  [newResult * sizeof (T)] = check;
						dwAddress      [newResult] = dwAddress [i];
						++ newResult;
					}
				}
			}

			// Write the new results
			DWORD dwWrite;
			::WriteFil ( hAddress, dwAddress, newResult * 4, &dwWrite, 0 );
			::WriteFil ( hValues,     Values, newResult * sizeof (T), &dwWrite, 0 );

			// Subtract from result
			result      -= allocationSize;
			finalResult += newResult;

			// Delete the buffer		
			delete [] dwAddress;
			delete [] Values;

			// Increment progress bar
			SMX ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_STEPIT, 0, 0 );
			ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_SHOWNORMAL );
		}
	}

	// Finish the scan
	FinishNS ();
}
template <class T> void NS1 ( T Value )
{
	int newResult   = 0;
	int finalResult = 0;
	int temp_result = (int) result;

	// Create the handle to the file
	HANDLE hFile = CreateFile ( GetPathway ( "Memory.TMP" ).c_str (),  GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	HANDLE hNew  = CreateFile ( GetPathway ( "Memory2.TMP" ).c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );

	// Obtain the scan type
	char szScan [256] = {0};
	GetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), szScan, 256 );

	// Create the offset
	int offset = ( (int) result / PreciseSize) + 1;

	// Set up progress bar
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_SHOWNORMAL );
	SMX        ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETPOS, (WPARAM) 0, 0 );
	SMX		   ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETRANGE, 0, MAKELPARAM ( 0, offset ) );
	SMX		   ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_SETSTEP, (WPARAM) 1, 0 );

	// Begin
	for ( int readLoop = 0; readLoop < offset; readLoop ++ )
	{
		// Reset new result
		newResult = 0;

		// Obtain allocation size
		SIZE_T allocationSize;

		if ( readLoop == ( offset - 1 ) )
			allocationSize = (SIZE_T) result;
		else
			allocationSize = PreciseSize;

		// Initialize the variable
		dwScan = new DWORD [ allocationSize ];

		// Read the file
		DWORD dwBytes;
		::ReadFile ( hFile, dwScan, (allocationSize * 4), &dwBytes, 0 );

		// Begin the scan
		if ( !strcmp ( szScan, "Precise" ) )
		{
			for ( SIZE_T i = 0; i < allocationSize; i ++ )
			{
				if ( !strcmp ( typeid (T).name (), "float" ) )
				{
					// Check values
					if ( fabs ( (float) ( RV <float> ( dwScan [i] ) - Value ) ) <= (float) Epislon ) 
					{	
						dwScan [newResult] = dwScan [i];
						++newResult;
					}
				}
				else if ( !strcmp ( typeid (T).name (), "double" ) )
				{
					// Check values
					if ( fabs ( (double) ( RV <double> ( dwScan [i] ) - Value ) ) <= (double) Epislon ) 
					{	
						dwScan [newResult] = dwScan [i];
						++newResult;
					}
				}
				else
				{
					if ( CMP <T> ( dwScan [i], Value ) ) 
					{		
						dwScan [newResult] = dwScan [i];
						++newResult;
					}
				}
			}
		}
		// Greater than
		if ( !strcmp ( szScan, "Greater Than" ) )
		{
			for ( SIZE_T i = 0; i < allocationSize; i ++ )
			{
				if ( RV <T> ( dwScan [i] ) > Value )
				{
					dwScan [newResult] = dwScan [i];
					++newResult;
				}
			}
		}
		// Lower than
		if ( !strcmp ( szScan, "Lower Than" ) )
		{
			for ( SIZE_T i = 0; i < allocationSize; i ++ )
			{
				if ( RV <T> ( dwScan [i] ) < Value )
				{
					dwScan [newResult] = dwScan [i];
					++newResult;
				}
			}
		}	

		// Write the results
		DWORD dwWrite;
		::WriteFil ( hNew, dwScan, ( newResult * 4 ), &dwWrite, 0 );

		// Subtract from result
		result      -= allocationSize;
		finalResult += newResult;

		// Delete the buffer		
		delete [] dwScan;

		// Increment progress bar
		SMX ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), PBM_STEPIT, 0, 0 );
		ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_SHOWNORMAL );
	}

	// Setup final result
	result = finalResult;
	++scanNum;

	// Close the handles
	CloseHandle ( hFile );
	CloseHandle ( hNew  );

	// Delete the old buffer and rename
	DeleteFile ( GetPathway ( "OldMemory.TMP" ).c_str () );
	MoveFileEx ( GetPathway ( "Memory.TMP" ).c_str (),  GetPathway ( "OldMemory.TMP" ).c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED );
	MoveFileEx ( GetPathway ( "Memory2.TMP" ).c_str (), GetPathway ( "Memory.TMP" ).c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED );

	// Hide progress bar
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_HIDE );
}
template <class T> void GN  ()
{
	oldResult = (int) result;

	// See if the value is hex
	bool hex = SMX ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;

	// Obtain the value
	T Value;
	
	if ( hex )
		Value = ConvertTextTo <LPVOID, T> ( GetDlgItem ( mainDlg, IDC_EDIT1 ) );
	else
		Value = ConvertTextTo <T, T>	  ( GetDlgItem ( mainDlg, IDC_EDIT1 ) );

	// Clear the listbox
	SMX ( GetDlgItem ( mainDlg, IDC_LIST1 ), LB_RESETCONTENT, 0, 0 );
	
	// Obtain the scan type
	char szScan [256] = {0};
	GetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), szScan, 256 );

	// Begin the scan
	if      ( ArrayOfBytes )
	{
		// Setup variables
		string test;
		char szBuffer [256] = {0};
		GetWindowText ( GetDlgItem ( mainDlg, IDC_EDIT1 ), szBuffer, 256 );

		// Obtain the value
		if ( !strcmp ( szScan, "Array of Bytes" ) ) {
			test += szBuffer;
		}
		else {
			string buffer;
			buffer += szBuffer;		
			test += CTB ( buffer );
		}

		szBytes = 0;

		// Obtain the number of bytes
		for ( SIZE_T i = 0; i < test.length (); i ++ )
		{
			// Avoid an exception
			if ( i + 1 < test.length () )
			{
				// Find the number of bytes 
				if ( test.at (i) != ' ' )
				{
					// If you can form a two letter word
					if ( test.at ( i + 1 ) != ' ' )
					{
						++ szBytes;
					}
				}
			}
		}

		// Setup the variable
		delete [] pBytes;
		pBytes = new BYTE [ szBytes ];
		SIZE_T Dummy = 0;
		
		// Create the bytes
		for ( SIZE_T i = 0; i < test.length (); i ++ )
		{
			// Avoid an exception
			if ( i + 1 < test.length () )
			{
				// Write the bytes
				if ( test.at (i) != ' ' )
				{
					// If you can form a two letter word
					if ( test.at ( i + 1 ) != ' ' )
					{
						// Obtain the string format
						string str;
						str += test.at (i);
						str += test.at (i + 1);

						// Convert it to LPVOID
						stringstream ss;
						ss << str;

						LPVOID lpVal;
						ss >> lpVal;

						// Check
						pBytes [Dummy] = (unsigned int) lpVal;
						
						++ Dummy;
						++ i;
					}
				}
			}
		}

		NS3 ();
	}
	else if ( !unknownScan ) 
		NS1  <T> ( Value );
	else 
	{
		NS2  <T> ( Value );
	}
	
	// Notify 
	SetWindowText ( GetDlgItem ( mainDlg, IDC_PROGRESS ), "" );
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS ), SW_HIDE );

	// Display results
	SetWindowText ( GetDlgItem ( mainDlg, IDC_RESULT ), CTT ( result ).c_str () );

	// Notify the user the scan has finished
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_HIDE );

	// Enable 'New Scan'
	Button_Enable ( GetDlgItem ( mainDlg, IDC_BUTTON2 ), TRUE );

	// Enable 'Undo Scan'
	Button_Enable ( GetDlgItem ( mainDlg, IDC_BUTTON8 ), TRUE );

	ScanFinished = true;

	// Beep
	MakeSound ();

	// Close the handle
	DWORD dwExitCode;
	GetExitCodeThread ( GetCurrentThread (), &dwExitCode );
	ExitThread		  ( dwExitCode );
}
//

// Pointer Scanner
void PS ( DWORD dwAddress, DWORD Range, bool GoStatic )
{
	// Setup minimum value
	DWORD dwMinimum = dwAddress - Range;

	// Keep track of results
	PointerResult = 0;

	// Scan the memory
	for ( DWORD i = dwStartScan; i < dwStopScan; i ++ )
	{
		MEMORY_BASIC_INFORMATION MBI;
		VQ ( (LPCVOID) i, &MBI, sizeof ( MEMORY_BASIC_INFORMATION ) );

		// Check the range
		if ( MBI.State == MEM_COMMIT )
		{
			// Check the static information
			HMODULE hModule;

			if ( GoStatic == true )
				GetModuleHandleEx ( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR) (DWORD) MBI.BaseAddress, &hModule );

			// Check if its a static region
			if ( ( GoStatic == true && hModule != NULL ) ||
				 ( GoStatic == false ) )
			{
				// Last address
				DWORD dwLastAddr = (DWORD) MBI.BaseAddress + (DWORD) MBI.RegionSize;
				dwLastAddr = dwLastAddr - 1 - sizeof ( DWORD );

				// Read the region
				for ( DWORD ptr = (DWORD) MBI.BaseAddress; ptr < dwLastAddr && PointerResult < 100000; ptr ++ )
				{
					// Obtain the value
					DWORD dwVal = RV <DWORD> ( ptr );	

					// Check if the value is in the range
					if ( dwVal >= dwMinimum && dwVal <= dwAddress ) 
					{			
						// Add the result
						dwPtrScan [PointerResult] = ptr;
						++ PointerResult;
					}
				}
			}
		}	

		i = (DWORD) MBI.BaseAddress + (DWORD) MBI.RegionSize;
	}
}
void GP ()
{
	// Clear the list
	SMX ( GetDlgItem ( ptrDlg, IDC_LIST1 ), LB_RESETCONTENT, 0, 0 );

	// Call the function
	PS ( ptrInfo.dwAddress, ptrInfo.dwRange, ptrInfo.GoStatic );

	// Adding Results
	SetWindowText ( GetDlgItem ( ptrDlg, IDC_RESULT2 ), "Adding Results" );

	// Add the results
	for ( int i = 0; i < PointerResult; i ++ )
	{
		string Pointer = "[ ";

		// Check pointer information
		STATIC_INFO staticInfo = CS ( dwPtrScan [ i ] );

		Pointer += staticInfo.PointerInfo;
		Pointer += " ] + ";

		// Obtain value
		DWORD dwValue = RV <DWORD> ( dwPtrScan [ i ] );

		Pointer += RTrimZero ( ConvertToText <void*> ( (void*) ( dwValue > ptrInfo.dwAddress ? dwValue - ptrInfo.dwAddress : ptrInfo.dwAddress - dwValue ) ) );

		// Add result
		IR2 ( Pointer );
	}

	// Set the results
	SetWindowText ( GetDlgItem ( ptrDlg, IDC_RESULT2 ), CTT ( PointerResult ).c_str () );

	// Beep
	MakeSound ();

	// Enable scan button
	Button_Enable ( GetDlgItem ( ptrDlg, IDC_BUTTON1 ), TRUE );

	// Close the handle
	DWORD dwExitCode;
	GetExitCodeThread ( hPtrScan, &dwExitCode );
	TerminateThread   ( hPtrScan, dwExitCode  );
}
//

// Memory Viewer
void MV ( DWORD dwStart, DWORD dwStop )
{
	// Keep track of results
	myResult = 0;

	// Setup file
	ofstream File ( "C:\\MemoryScan.iScan" );
	File.clear ();

	ofstream Hex  ( "C:\\MemoryText.iScan" );
	Hex.clear ();

	// Begin reading
	for ( DWORD i = dwStart; i <= dwStop; i += 0x10 )
	{
		// Setup bytes
		BYTE * myBytes;
		myBytes = new BYTE [16];

		// Read the bytes
		if ( RB ( i, myBytes ) )
		{
			++myResult;

			// Initialize the string
			string Text   = CTT ( myResult );
			string Output = CTT ( myResult );	

			Text         += ". ";
			Output       += ". ";

			// Fix output
			Output       += CTT <LPVOID> ( (LPVOID) i );
			Output       += " - ";

			// Add all the bytes
			for ( int index = 0; index < 16; index ++ ) {
				Output += TrimZero ( CTT <LPVOID> ( (LPVOID) (unsigned int) myBytes [index] ) );
				Output += " ";

				char myChar = (char) myBytes [index];

				// Skip NULL and new line
				if ( !myChar || myChar == 0x0A )
					Text += ".";
				else
					Text += myChar;
			}

			// Add the file
			File << Output << endl;
			Hex  << Text   << endl;
		}
	}

	// Close the file
	File.close ();
	Hex.close ();
}
void GM2 ()
{
	// Clear the listbox
	SMX ( GetDlgItem ( memDlg, IDC_LIST1 ), LB_RESETCONTENT, 0, 0 );
	SMX ( GetDlgItem ( memDlg, IDC_LIST2 ), LB_RESETCONTENT, 0, 0 );

	// Set the status
	SetWindowText ( GetDlgItem ( memDlg, IDC_RESULT3 ), "Scanning!" );

	// Call MV () func
	MV ( dwMS1, dwMS2 );

	// Set the status
	SetWindowText ( GetDlgItem ( memDlg, IDC_RESULT3 ), "Adding Results" );

	// Setup the file
	ifstream File ( "C:\\MemoryScan.iScan" );
	ifstream Hex  ( "C:\\MemoryText.iScan" );

	// Read and add results
	while ( !File.eof () )
	{
		// Obtain the string
		string myLine, myHex;
		getline ( File, myLine );
		getline ( Hex, myHex );

		// Insert the result
		if ( strcmp ( myLine.c_str (), "" ) != 0 && strcmp ( myHex.c_str (), "" ) != 0 ) {
			SMX ( GetDlgItem ( memDlg, IDC_LIST1 ), LB_ADDSTRING, 0, (LPARAM) myLine.c_str () );
			SMX ( GetDlgItem ( memDlg, IDC_LIST2 ), LB_ADDSTRING, 0, (LPARAM) myHex.c_str ()  );
		}
	}

	// Close the files
	File.close ();
	Hex.close  ();

	// Set the status
	SetWindowText ( GetDlgItem ( memDlg, IDC_RESULT3 ), "" );

	// Beep
	MakeSound ();

	// Close the handle
	DWORD dwExitCode;
	GetExitCodeThread ( hMemScan, &dwExitCode );
	TerminateThread   ( GetCurrentThread (), dwExitCode  );
}
//

// Function Watcher
__declspec ( naked ) void HookTo ()
{
	__asm 
	{
		push ebp
		push ebx
		push edx
		push esp
		push ecx
		push eax
		push esp
		push esi
		push edi
	}

	SMX ( GetDlgItem ( fwDlg, IDC_LIST1 ), LB_ADDSTRING, 0, (LPARAM) FuncCalled.c_str () );

	// Protect the memory
	VirtualProtect ( (void*) dwFunction, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect );

	// Rewrite the bytes
	memcpy ( (void*) dwFunction, &originalBytes, 5 );	

	// Remove hook
	VirtualProtect ( (void*) dwFunction, 5, PAGE_EXECUTE_READWRITE, &dwDummy );

	// JMP to original func
	__asm 
	{
		pop edi
		pop esi
		pop esp
		pop eax
		pop ecx
		pop esp
		pop edx
		pop ebx
		pop ebp

		jmp [ dwFunction ];
	}
}

void SetHook (string Library, string Function)
{
	SMX ( GetDlgItem ( fwDlg, IDC_LIST1 ), LB_RESETCONTENT, 0, 0 );

	// Set string
	FuncCalled = "-> ";
	FuncCalled += Function;
	FuncCalled += " () in library ";
	FuncCalled += Library.c_str ();
	FuncCalled += " has been called.";

	// Declare variables
	DWORD dwNewProtect;

	// Obtain Function addr
	dwFunction = (DWORD) GetProcAddress ( GetModuleHandle (Library.c_str ()), Function.c_str () );

	if ( dwFunction != 0 )
	{
		// Protect the memory
		VirtualProtect ( (void*) dwFunction, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect );

		// Copy the first five bytes
		memcpy ( &originalBytes, (void*) dwFunction, 5 );

		// Set the hook
		*(BYTE*)    dwFunction       = 0xE9;
		*(DWORD*) ( dwFunction + 1 ) = ( (DWORD) &HookTo - ( dwFunction + 0x5) );

		VirtualProtect ( (void*) dwFunction, 5, dwOldProtect, &dwNewProtect );

		// Beep
		MakeSound ();
	}
	else
	{
		FuncCalled = "The function ";
		FuncCalled += Function.c_str ();
		FuncCalled += " can't be located.";

		SMX ( GetDlgItem ( fwDlg, IDC_LIST1 ), LB_ADDSTRING, 0, (LPARAM) FuncCalled.c_str () );
	}
}
//

// Handling the results
void InsertResult ( void* lpAddr )
{
	// Get Item
	int item = ListView_GetItemCount ( GetDlgItem ( dbgDlg, IDC_LIST1 ) );

	// Setup item
	LV_ITEM lv;
	lv.iItem    = item;           
	lv.iSubItem = 0;
	lv.mask     = LVIF_TEXT;       
	lv.pszText  = "";
	lv.cchTextMax = 256; 

	// Insert item
	ListView_InsertItem ( GetDlgItem ( dbgDlg, IDC_LIST1 ), &lv );

	// Fix subitems
	for ( int i = 0; i < 2; i ++ )
	{
		// Setup neccessary values
		lv.iSubItem   = i;
		lv.cchTextMax = 256;

		// Setup text
		string str = "";

		if ( !i )
			str = "1";
		else
			str = CTT <LPVOID> ( lpAddr );

		// Add item
		if ( !strcmp ( str.c_str (), "" ) )
			str = "Error";

		lv.pszText = (LPSTR) str.c_str ();

		// Insert subitem
		ListView_SetItem ( GetDlgItem ( dbgDlg, IDC_LIST1 ), &lv );
	}
}
void CheckResult  ( void* Exception )
{	
	// Obtain system information
	SYSTEM_INFO SI;
	GetSystemInfo ( &SI );

	// 
	DWORD  dwAddr = (DWORD) Exception;
	DISASM myDisasm;

	// Find the last address
	for ( int i = 0; i < 10; i++ )
	{
		myDisasm.EIP = dwAddr;
		DWORD length = (DWORD) Disasm ( &myDisasm );
		DWORD dwPossibility = 0;

		if ( length != 0xFFFFFFFF )
			dwPossibility = dwAddr - length;
		else
			break;

		if ( dwPossibility >= (DWORD) SI.lpMinimumApplicationAddress &&
			 dwPossibility <= (DWORD) SI.lpMaximumApplicationAddress ) 
			dwAddr = dwPossibility;
		else
			break;
	}

	dwAddr -= 0x1;

	DWORD dwPrevAddr = dwAddr;

	// Get disassembler information
	void* ExceptionAddr = 0;

	for ( int i = 0; i < 20; i ++ )
	{
		if ( dwAddr == (DWORD) Exception )
			break;
		
		myDisasm.EIP = dwAddr;
		DWORD Length = (DWORD) Disasm ( &myDisasm );

		while ( Length == 0xFFFFFFFF )
		{
			dwAddr += 0x1;
			myDisasm.EIP = dwAddr;
			Length = (DWORD) Disasm ( &myDisasm );
		}

		dwPrevAddr = dwAddr;
		dwAddr    += Length;
	}

	ExceptionAddr = (void*) dwPrevAddr;

	bool NoAdd = false;

	// Check if the item exists
	int max = ListView_GetItemCount ( GetDlgItem ( dbgDlg, IDC_LIST1 ) );

	if ( !max )
	{
		InsertResult ( ExceptionAddr );
		Dbged [numOfDbg] = ExceptionAddr;
		++numOfDbg;
	}
	else
	{
		int i = 0;

		for ( i = 0; i < numOfDbg; i ++ )
		{
			if ( Dbged [i] == ExceptionAddr )
			{
				NoAdd = true;
				break;
			}
		}

		if ( !NoAdd && ExceptionAddr != 0 )
		{
			InsertResult ( ExceptionAddr );
			Dbged [numOfDbg] = ExceptionAddr;
			++numOfDbg;
		}
		else
		{
			// Increment the count
			char szCount [256] = {0};
			ListView_GetItemText ( GetDlgItem ( dbgDlg, IDC_LIST1 ), i, 0, szCount, 256 );

			stringstream ss2;
			ss2 << szCount;

			int Count = 0;
			ss2 >> Count;
			++Count;

			// Convert count to string
			string str = CTT <int> ( Count );

			// Add the information
			LV_ITEM lv;
			lv.iItem    = i;           
			lv.iSubItem = 0;
			lv.mask     = LVIF_TEXT;       
			lv.pszText  = (LPSTR) str.c_str ();
			lv.cchTextMax = 256; 

			// Insert subitem
			ListView_SetItem ( GetDlgItem ( dbgDlg, IDC_LIST1 ), &lv );
		}
	}

	// Close the handle
	DWORD dwExitCode;
	GetExitCodeThread ( GetCurrentThread (), &dwExitCode );
	TerminateThread   ( GetCurrentThread (), dwExitCode  );
}
//

// Set bits
DWORD GetDr7 ( int BreakType, int Length, bool GE, bool LE, bool Local, bool Global, bool GD )
{
	bitset <32> myBitset;	
	
	// Set the break on condition
	if ( BreakType == 00 )
	{
		myBitset.set ( 16, 0 );
		myBitset.set ( 17, 0 );
		myBitset.set ( 20, 0 );
		myBitset.set ( 21, 0 );
		myBitset.set ( 24, 0 );
		myBitset.set ( 25, 0 );
		myBitset.set ( 28, 0 );
		myBitset.set ( 29, 0 );	
	}
	if ( BreakType == 01 )
	{
		myBitset.set ( 16, 0 );
		myBitset.set ( 17, 1 );
		myBitset.set ( 20, 0 );
		myBitset.set ( 21, 1 );
		myBitset.set ( 24, 0 );
		myBitset.set ( 25, 1 );
		myBitset.set ( 28, 0 );
		myBitset.set ( 29, 1 );	
	}
	if ( BreakType == 10 )
	{
		myBitset.set ( 16, 1 );
		myBitset.set ( 17, 0 );
		myBitset.set ( 20, 1 );
		myBitset.set ( 21, 0 );
		myBitset.set ( 24, 1 );
		myBitset.set ( 25, 0 );
		myBitset.set ( 28, 1 );
		myBitset.set ( 29, 0 );	
	}
	else
	{
		myBitset.set ( 16, 1 );
		myBitset.set ( 17, 1 );
		myBitset.set ( 20, 1 );
		myBitset.set ( 21, 1 );
		myBitset.set ( 24, 1 );
		myBitset.set ( 25, 1 );
		myBitset.set ( 28, 1 );
		myBitset.set ( 29, 1 );	
	}

	// Set the length
	if ( Length == 00 )
	{
		myBitset.set ( 18, 0 );
		myBitset.set ( 19, 0 );
		myBitset.set ( 22, 0 );
		myBitset.set ( 23, 0 );
		myBitset.set ( 26, 0 );
		myBitset.set ( 27, 0 );
		myBitset.set ( 30, 0 );
		myBitset.set ( 31, 0 );	
	}
	if ( Length == 01 )
	{
		myBitset.set ( 18, 0 );
		myBitset.set ( 19, 1 );
		myBitset.set ( 22, 0 );
		myBitset.set ( 23, 1 );
		myBitset.set ( 26, 0 );
		myBitset.set ( 27, 1 );
		myBitset.set ( 30, 0 );
		myBitset.set ( 31, 1 );		
	}
	if ( Length == 10 )
	{
		myBitset.set ( 18, 1 );
		myBitset.set ( 19, 0 );
		myBitset.set ( 22, 1 );
		myBitset.set ( 23, 0 );
		myBitset.set ( 26, 1 );
		myBitset.set ( 27, 0 );
		myBitset.set ( 30, 1 );
		myBitset.set ( 31, 0 );	
	}
	else
	{
		myBitset.set ( 18, 1 );
		myBitset.set ( 19, 1 );
		myBitset.set ( 22, 1 );
		myBitset.set ( 23, 1 );
		myBitset.set ( 26, 1 );
		myBitset.set ( 27, 1 );
		myBitset.set ( 30, 1 );
		myBitset.set ( 31, 1 );	
	}	

	if ( GE )
		myBitset.set ( 9, 1 );
	else
		myBitset.set ( 9, 1 );

	if ( LE )
		myBitset.set ( 8, 1 );
	else
		myBitset.set ( 8, 1 );

	if ( Local )
	{
		for ( int i = 0; i <= 6; i += 2 )
			myBitset.set ( i, 1 );
	}
	else
	{
		for ( int i = 0; i <= 6; i += 2 )
			myBitset.set ( i, 0 );
	}

	if ( Global )
	{
		for ( int i = 1; i <= 7; i += 2 )
			myBitset.set ( i, 1 );
	}
	else
	{
		for ( int i = 1; i <= 7; i += 2 )
			myBitset.set ( i, 0 );
	}

	if ( GD )
		myBitset.set ( 13, 1 );
	else
		myBitset.set ( 13, 0 );

	return (DWORD) myBitset.to_ulong ();
}

//

//
ofstream File ( "C:\\breakpoint.txt" );

void CheckBreakpoint ()
{
	// Initialize our variable
	THREADENTRY32 te;
	te.dwSize = sizeof ( THREADENTRY32 );

	// Create the snapshot
	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPTHREAD, 0 );

	if ( Thread32First ( hSnapshot, &te ) )
	{
		// Check the thread
		if ( te.th32OwnerProcessID == GetCurrentProcessId () && te.th32ThreadID != GetCurrentThreadId () )
		{
			// Open the thread
			HANDLE hThread = OpenThread ( THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT | THREAD_GET_CONTEXT, false, te.th32ThreadID );

			// Suspend the thread
			SuspendThread ( hThread );

			// Obtain the thread information
			CONTEXT Context;
			Context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

			// Obtain the thread information
			GetThreadContext ( hThread, &Context );

			// Display 
			File << "Dr0: " << (void*) Context.Dr0 << endl;
			File << "Dr1: " << (void*) Context.Dr1 << endl;
			File << "Dr2: " << (void*) Context.Dr2 << endl;
			File << "Dr3: " << (void*) Context.Dr3 << endl;
			File << endl;
			
			// Resume the thread
			ResumeThread ( hThread );
			CloseHandle  ( hThread );
		}

		while ( Thread32Next ( hSnapshot, &te ) )
		{
			// Check the thread
			if ( te.th32OwnerProcessID == GetCurrentProcessId () && te.th32ThreadID != GetCurrentThreadId () )
			{
				// Open the thread
				HANDLE hThread = OpenThread ( THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT | THREAD_GET_CONTEXT, false, te.th32ThreadID );

				// Suspend the thread
				SuspendThread ( hThread );

				// Obtain the thread information
				CONTEXT Context;
				Context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

				// Obtain the thread information
				GetThreadContext ( hThread, &Context );

				// Display 
				File << "Dr0: " << (void*) Context.Dr0 << endl;
				File << "Dr1: " << (void*) Context.Dr1 << endl;
				File << "Dr2: " << (void*) Context.Dr2 << endl;
				File << "Dr3: " << (void*) Context.Dr3 << endl;
				File << endl;

				// Resume the thread
				ResumeThread ( hThread );
				CloseHandle  ( hThread );
			}
		}
	}
}
void DoBreakpoint ()
{
	while ( true )
	{
		CheckBreakpoint ();
		Sleep ( 5000 );
	}
}
//

// Handling/Setting the breakpoint
LONG CALLBACK VectoredHandler ( PEXCEPTION_POINTERS ExceptionInfo )
{
	if ( dbgOn )
	{
		if ( ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP )
		{
			Sleep ( 100 );

			if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress < dwNoStartScan ||
				 (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress > dwNoStopScan )
			{
				bool add = true;

				for ( int i = 0; i < numOfDbg; i ++ ) {
					if ( dbgInfo [i].lpAddr == ExceptionInfo->ExceptionRecord->ExceptionAddress ) {
						add = false;
						break;
					}
				}

				if ( add ) 
				{
					dbgInfo [numOfDbg].lpAddr = ExceptionInfo->ExceptionRecord->ExceptionAddress;
					dbgInfo [numOfDbg].EAX    = ExceptionInfo->ContextRecord->Eax;
					dbgInfo [numOfDbg].EBP    = ExceptionInfo->ContextRecord->Ebp;
					dbgInfo [numOfDbg].EBX    = ExceptionInfo->ContextRecord->Ebx;
					dbgInfo [numOfDbg].ECX    = ExceptionInfo->ContextRecord->Ecx;
					dbgInfo [numOfDbg].EDI    = ExceptionInfo->ContextRecord->Edi;
					dbgInfo [numOfDbg].EDX    = ExceptionInfo->ContextRecord->Edx;
					dbgInfo [numOfDbg].EIP    = ExceptionInfo->ContextRecord->Eip;
					dbgInfo [numOfDbg].ESI    = ExceptionInfo->ContextRecord->Esi;
					dbgInfo [numOfDbg].ESP    = ExceptionInfo->ContextRecord->Esp;
				}

				CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) CheckResult, ExceptionInfo->ExceptionRecord->ExceptionAddress, NULL, NULL );
			}

			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	if ( localBrk )
	{
		if ( ( ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION ) ||
		     ( ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT    ) )
		{
			Sleep ( 100 );

			// Add result
			bool add = true;

			for ( int i = 0; i < numOfDbg; i ++ ) {
				if ( dbgInfo [i].lpAddr == (void*) RV <DWORD> ( ExceptionInfo->ContextRecord->Esp ) ) {
					add = false;
					break;
				}
			}

			if ( add ) 
			{
				dbgInfo [numOfDbg].lpAddr = (void*) RV <DWORD> ( ExceptionInfo->ContextRecord->Esp );
				dbgInfo [numOfDbg].EAX    = ExceptionInfo->ContextRecord->Eax;
				dbgInfo [numOfDbg].EBP    = ExceptionInfo->ContextRecord->Ebp;
				dbgInfo [numOfDbg].EBX    = ExceptionInfo->ContextRecord->Ebx;
				dbgInfo [numOfDbg].ECX    = ExceptionInfo->ContextRecord->Ecx;
				dbgInfo [numOfDbg].EDI    = ExceptionInfo->ContextRecord->Edi;
				dbgInfo [numOfDbg].EDX    = ExceptionInfo->ContextRecord->Edx;
				dbgInfo [numOfDbg].EIP    = ExceptionInfo->ContextRecord->Eip;
				dbgInfo [numOfDbg].ESI    = ExceptionInfo->ContextRecord->Esi;
				dbgInfo [numOfDbg].ESP    = ExceptionInfo->ContextRecord->Esp;
			}

			CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) CheckResult, (void*) RV <DWORD> ( ExceptionInfo->ContextRecord->Esp ), NULL, NULL );

			// Continue
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}
bool            SetBreakpoint ( DWORD dwAddr, int Type )
{
	if ( dbgOn )
	{
		dwDbgAddr = dwAddr;

		// Initialize our variable
		THREADENTRY32 te;
		te.dwSize = sizeof ( THREADENTRY32 );

		// Create the snapshot
		HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPTHREAD, 0 );

		if ( Thread32First ( hSnapshot, &te ) )
		{
			// Check the thread
			if ( te.th32OwnerProcessID == GetCurrentProcessId () && te.th32ThreadID != GetCurrentThreadId () )
			{
				// Open the thread
				HANDLE hThread = OpenThread ( THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT | THREAD_GET_CONTEXT, false, te.th32ThreadID );

				// Suspend the thread
				SuspendThread ( hThread );

				// Obtain the thread information
				CONTEXT Context;
				Context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

				// Obtain the thread information
				GetThreadContext ( hThread, &Context );

				// Find an available regiser and set it
				//Context.Dr0 = dwAddr;
				Context.Dr1 = dwAddr;
				Context.Dr2 = dwAddr;
				Context.Dr3 = dwAddr;

				// Set the Dr7 register
				if ( !Type )
					Context.Dr7 = GetDr7 ( 11, 11, true, true, true, true, true);
				else if ( Type == 1 )
					Context.Dr7 = GetDr7 ( 10, 11, true, true, true, true, true);
				else if ( Type == 2 )
					Context.Dr7 = GetDr7 ( 00, 11, true, true, true, true, true );

				// Set the thread information
				SetThreadContext ( hThread, &Context );

				// Resume the thread
				ResumeThread ( hThread );
				CloseHandle  ( hThread );
			}

			while ( Thread32Next ( hSnapshot, &te ) )
			{
				// Check the thread
				if ( te.th32OwnerProcessID == GetCurrentProcessId () && te.th32ThreadID != GetCurrentThreadId () )
				{
					// Open the thread
					HANDLE hThread = OpenThread ( THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT | THREAD_GET_CONTEXT, false, te.th32ThreadID );

					// Suspend the thread
					SuspendThread ( hThread );

					// Obtain the thread information
					CONTEXT Context;
					Context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

					// Obtain the thread information
					GetThreadContext ( hThread, &Context );

					// Find an available regiser and set it
					//Context.Dr0 = dwAddr;
					Context.Dr1 = dwAddr;
					Context.Dr2 = dwAddr;
					Context.Dr3 = dwAddr;

					// Set the Dr7 register
					if ( !Type )
						Context.Dr7 = GetDr7 ( 11, 11, true, true, true, true, true);
					else if ( Type == 1 )
						Context.Dr7 = GetDr7 ( 10, 11, true, true, true, true, true);
					else if ( Type == 2 )
						Context.Dr7 = GetDr7 ( 00, 11, true, true, true, true, true );

					// Set the thread information
					SetThreadContext ( hThread, &Context );

					// Resume the thread
					ResumeThread ( hThread );
					CloseHandle  ( hThread );
				}
			}
		}
	}

	return true;
}

//

// Adding/Removing the handler
PVOID AddHandler ()
{
	dbgInfo = new DBG_INFO [100];
	numOfDbg = 0;
	PVOID pHandler = AddVectoredExceptionHandler ( 0, VectoredHandler );

	if ( !pHandler )
	{
		dbgOn = false;
		MessageBoxA ( 0, "Failed to add the handler!", "Error", MB_ICONINFORMATION );
	}
	else
	{
		if ( !localBrk )
			dbgOn = true;

		ListView_DeleteAllItems ( GetDlgItem ( dbgDlg, IDC_LIST1 ) );
	}

	return pHandler;
}

void  DelHandler  ( PVOID pHandler )
{
	dbgOn = false;

	// Remove the breakpoint
	THREADENTRY32 te;
	te.dwSize = sizeof ( THREADENTRY32 );

	// Create the snapshot
	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPTHREAD, 0 );

	if ( Thread32First ( hSnapshot, &te ) )
	{
		// Check the thread
		if ( te.th32OwnerProcessID == GetCurrentProcessId () && te.th32ThreadID != GetCurrentThreadId () )
		{
			// Open the thread
			HANDLE hThread = OpenThread ( THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT | THREAD_GET_CONTEXT, false, te.th32ThreadID );

			// Suspend the thread
			SuspendThread ( hThread );

			// Obtain the thread information
			CONTEXT Context;
			Context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

			// Obtain the thread information
			GetContext ( hThread, &Context );

			// Find an available regiser and set it
			int i = 0;

			for ( i = 0; i < 4; i ++ )
			{
				if ( *(DWORD*) ( ((DWORD) &Context.Dr0) + (4 * i) ) == dwDbgAddr )
				{
					*(DWORD*) ( ((DWORD) &Context.Dr0) + (4 * i) ) = 0;
					break;
				}
			}

			// Set the Dr7 register
			Context.Dr7 = 0;

			// Set the thread information
			SetContext ( hThread, &Context );

			// Resume the thread
			ResumeThread ( hThread );

			// Close Handle
			CloseHandle ( hThread );
		}

		while ( Thread32Next ( hSnapshot, &te ) )
		{
			// Check the thread
			if ( te.th32OwnerProcessID == GetCurrentProcessId () && te.th32ThreadID != GetCurrentThreadId () )
			{
				// Open the thread
				HANDLE hThread = OpenThread ( THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT | THREAD_GET_CONTEXT, false, te.th32ThreadID );

				// Suspend the thread
				SuspendThread ( hThread );

				// Obtain the thread information
				CONTEXT Context;
				Context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

				// Obtain the thread information
				GetContext ( hThread, &Context );

				// Find an available regiser and set it
				Context.Dr0 = 0;
				Context.Dr1 = 0;
				Context.Dr2 = 0;
				Context.Dr3 = 0;

				// Set the Dr7 register
				Context.Dr7 = 0;

				// Set the thread information
				SetContext ( hThread, &Context );

				// Resume the thread
				ResumeThread ( hThread );

				// Close Handle
				CloseHandle ( hThread );
			}
		}
	}

	// Remove our exception handler
	RemoveVectoredExceptionHandler ( pHandler );
}
void  DelHandler2 ( PVOID pHandler )
{
	localBrk = false;

	// Remove our exception handler
	RemoveVectoredExceptionHandler ( pHandler );

	// Restore original byte
	WV <BYTE> ( dwDbgAddr, originalByte );
}
//

// Disassembler
string GetBytesInStr ( DWORD dwAddr, DWORD dwLength )
{
	// Create the variable
	BYTE * opcodes;

	// Set the length
	opcodes = new BYTE [ dwLength ];
	memcpy ( opcodes, (void*) dwAddr, (SIZE_T) dwLength );

	// Our byte variable
	string OpcodesStr;

	// Convert our bytes to lpvoid
	for ( SIZE_T i = 0; i < (SIZE_T) dwLength; i ++ )
	{
		stringstream bla;
		bla << (LPVOID) (unsigned int) opcodes [i];

		// Substr and then add it
		string hexa;
		hexa += bla.str ();

		OpcodesStr += hexa.substr ( 6, 8 );			
		OpcodesStr += " ";
	}

	delete [] opcodes;
	return OpcodesStr;
}

void Dis ()
{
	// Clear the listbox
	ListView_DeleteAllItems ( GetDlgItem ( disDlg, IDC_LIST1 ) );

	// Get system information
	SYSTEM_INFO SI;
	GetSystemInfo ( &SI );

	// Begin debugging
	DISASM myDisasm;
	DWORD dwTrgt = (DWORD) ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( disDlg, IDC_EDIT1 ) );
	DWORD dwAddr = dwTrgt;

	int myItem = 0;

	// Find the last address
	for ( int i = 0; i < 10; i++ )
	{
		if ( dwAddr > (DWORD) SI.lpMinimumApplicationAddress &&
			 dwAddr < (DWORD) SI.lpMaximumApplicationAddress ) 
		{
			myDisasm.EIP = dwAddr;
			DWORD length = (DWORD) Disasm ( &myDisasm );

			if ( length != 0xFFFFFFFF )
				dwAddr -= length;
			else
				break;
		}
		else
			break;
	}

	if ( dwAddr > (DWORD) SI.lpMinimumApplicationAddress )
		dwAddr -= 0x1;	

	// Display the next 20 instructions
	for ( int i = 0; i < 20; i ++ )
	{
		if ( dwAddr == dwTrgt )
			myItem = i;

		if ( dwAddr >= (DWORD) SI.lpMinimumApplicationAddress &&
			 dwAddr <= (DWORD) SI.lpMaximumApplicationAddress ) 
		{
			myDisasm.EIP = dwAddr;
			DWORD length = (DWORD) Disasm ( &myDisasm );

			while ( length == 0xFFFFFFFF )
			{
				dwAddr ++;
				myDisasm.EIP = dwAddr;
				length = (DWORD) Disasm ( &myDisasm );
			}

			// Add then increment				
			ID ( CTT <LPVOID> ( (void*) dwAddr ), myDisasm.CompleteInstr, GetBytesInStr ( dwAddr, length ) );
			dwAddr += length;
		}
		else
			break;
	}

	SendMessage           (GetDlgItem ( disDlg, IDC_LIST1 ), LVM_ENSUREVISIBLE, (WPARAM) myItem, FALSE); 
	ListView_SetItemState (GetDlgItem ( disDlg, IDC_LIST1 ), myItem, LVIS_SELECTED, LVIS_SELECTED); 
	ListView_SetItemState (GetDlgItem ( disDlg, IDC_LIST1 ), myItem, LVIS_FOCUSED, LVIS_FOCUSED);
}
//

// Add all items
void AddAllItems ( bool GoStatic )
{
	int maxItems = SendMessage ( GetDlgItem ( mainDlg, IDC_LIST1 ), LB_GETCOUNT, 0, 0 );

	if ( result > 0 && maxItems != 0 )
	{
		for ( int i = 0; i < maxItems; i ++ )
		{
			char szBuffer [256] = {0};
			SMX ( GetDlgItem ( mainDlg, IDC_LIST1 ), LB_GETTEXT, (WPARAM) i, (LPARAM) (LPTSTR) szBuffer );

			HMODULE hModule;

			if ( GoStatic )
				GetModuleHandleEx ( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR) szBuffer, &hModule );

			if ( ( strcmp ( szBuffer, "" ) != 0 ) &&
				 ( ( ( GoStatic == true ) && hModule != NULL ) ) ||
				   ( ( GoStatic == false ) ) )
			{
				// Obtain the type
				char szType [256] = {0};
				GetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO1 ), (LPSTR) szType, 256 );

				// To string
				string ptr;
				ptr += (LPSTR) szBuffer;

				string type;

				if ( !strcmp ( szType, "Array of Bytes" ) )
					type += "Byte";
				else
					type += (LPSTR) szType;

				// Insert
				IA ( ptr, type );
			}
		}
	}
	else
	{
		if ( result > 50000 )
			MessageBoxA ( mainDlg, "Can't display results larger than 50,000", "Error", MB_ICONERROR );
		else
		{
			// Obtain results
			HANDLE hFile = CreateFile ( GetPathway ( "Memory.TMP" ).c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
			dwScan	     = new DWORD [ (DWORD) result * 4 ];

			// Read the reuslts
			DWORD dwBytes;
			::ReadFile ( hFile, dwScan, (DWORD) result * 4, &dwBytes, 0 );

			for ( int i = 0; i < result; i ++ )
			{
				HMODULE hModule;

				if ( GoStatic )
					GetModuleHandleEx ( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR) dwScan [i], &hModule );

				if ( ( ( ( GoStatic == true ) && hModule != NULL ) ) ||
					   ( ( GoStatic == false ) ) )
				{
					stringstream ss;
					ss << (void*) dwScan [i];

					// Obtain the type
					char szType [256] = {0};
					GetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO1 ), (LPSTR) szType, 256 );

					// Convert address to str
					string ptr;
					ptr += ss.str ();

					// Obtain type
					string type;

					if ( !strcmp ( szType, "Array of Bytes" ) )
						type += "Byte";
					else
						type += (LPSTR) szType;

					// Insert
					IA ( ptr, type );
				}
			}

			// Close the handle
			CloseHandle ( hFile );

			// Delete the buffer
			delete [] dwScan;
		}
	}
}
//