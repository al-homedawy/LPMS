// Global Headers
#include <Windows.h>
#include <Commctrl.h>
#include <Windowsx.h>
#include <typeinfo>
#include <sstream>
#include <Winuser.h>

// Local Headers
#include "memory.h"

using namespace std;

string Title	    = " Licensed To: ";

// Callback declarations
INT_PTR CALLBACK MainDialog  ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK AddrDialog  ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK EditDialog  ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK PtrDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK MemDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK ChgDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK OptDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK DbgDlg		 ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK ModuleDlg   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK fwDialog    ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK regDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK disDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK extraDialog  ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK settingDialog ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK pointerDialog ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK editAllDialog ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ); 
INT_PTR CALLBACK DataDialog    ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

// Global Variables
HINSTANCE hInstance = NULL;

// IDC_DIALOG3
int selectedItem = 0;

char * oldScan;
bool   DataNotDone = false;

// Scanning Variables
LPCTSTR ST   []	 = { "Byte", "2 Bytes", "4 Bytes", "8 Bytes", "Float", "Double" };
LPCTSTR NSN  []  = { "Precise", "Unknown" };
LPCTSTR NSN2 []  = { "Precise", "Greater Than", "Lower Than" };
//

// Column Strings
LPSTR Scan        [] = { "Freeze", "  Address", "     Type", "  Value", "Locked" };
LPSTR MDL         [] = { "Module Name", "Base Address" };
LPSTR Reg         [] = { "Base Address", "State", "Type", "Protect", "Size" };
LPSTR Dbg         [] = { "Count", "Address" };
LPSTR DisList     [] = { "Address", "Opcode", "Instruction" };
LPSTR DataBuf     [] = { "Address", "Value" };
//

// Column Widths
int   MdlWidth    [] = { 100, 80};
int   ScanWidth   [] = { 49, 96, 100, 100, 100 };
int   RegWidth    [] = { 90, 60, 60, 120, 90 };
int   DbgWidth    [] = { 50, 89 };
int   DisWidth    [] = { 75, 230, 233 };
int   DataWidth   [] = { 90, 173 };
//

// Debuggin variables
PVOID pHandler;
//

// Dialog functions/options
void Hotkeys ()
{
	while ( true )
	{
		// Debugging Hotkeys
		if ( ( GKEY ( VK_MENU ) & 0xFF80 ) &&
			 ( GKEY ( 0x31 ) & 0xFF80 ) )
			 SMX ( dbgDlg, WM_COMMAND, (WPARAM) IDC_BUTTON1, NULL );
		if ( ( GKEY ( VK_MENU ) & 0xFF80 ) &&
			 ( GKEY ( 0x32 ) & 0xFF80 ) )
			 SMX ( dbgDlg, WM_COMMAND, (WPARAM) IDC_BUTTON2, NULL );
		if ( ( GKEY ( VK_MENU ) & 0xFF80 ) &&
			 ( GKEY ( 0x33 ) & 0xFF80 ) )
			 SMX ( dbgDlg, WM_COMMAND, (WPARAM) IDC_BUTTON6, NULL );
		if ( ( GKEY ( VK_MENU ) & 0xFF80 ) &&
			 ( GKEY ( 0x34 ) & 0xFF80 ) )
			 SMX ( dbgDlg, WM_COMMAND, (WPARAM) IDC_BUTTON4, NULL );

		// Scanning Hotkeys
		if ( ( GKEY ( VK_MENU ) & 0xFF80 ) &&
			 ( GKEY ( 'S' ) & 0xFF80 ) )
		{
			if ( scanNum == -1 ) {
				ScanFinished = false;
				SMX ( mainDlg, WM_COMMAND, (WPARAM) IDC_BUTTON1, NULL );
			}
			else {
				SMX ( mainDlg, WM_COMMAND, (WPARAM) IDC_BUTTON2, NULL );
			}
			
			while ( !ScanFinished ) {
				Sleep ( 100 );
			}

			Sleep ( 500 );
		}

		if ( unknownScan )
		{
			if ( ( GKEY ( VK_MENU ) & 0xFF80 ) &&
				 ( GKEY ( 'I' ) & 0xFF80 ) )
			{
				// Increased
				SetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), "Increased" );

				if ( scanNum != -1 ) 
					SMX ( mainDlg, WM_COMMAND, (WPARAM) IDC_BUTTON2, NULL );
				
				while ( !ScanFinished ) {
					Sleep ( 100 );
				}

				Sleep ( 500 );
			}

			if ( ( GKEY ( VK_MENU ) & 0xFF80 ) &&
				 ( GKEY ( 'D' ) & 0xFF80 ) )
			{
				// Decreased
				SetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), "Decreased" );

				if ( scanNum != -1 ) 
					SMX ( mainDlg, WM_COMMAND, (WPARAM) IDC_BUTTON2, NULL );
				
				while ( !ScanFinished ) {
					Sleep ( 100 );
				}

				Sleep ( 500 );
			}

			if ( ( GKEY ( VK_MENU ) & 0xFF80 ) &&
				 ( GKEY ( 'C' ) & 0xFF80 ) )
			{
				// Changed
				SetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), "Changed" );

				if ( scanNum != -1 ) 
					SMX ( mainDlg, WM_COMMAND, (WPARAM) IDC_BUTTON2, NULL );
				
				while ( !ScanFinished ) {
					Sleep ( 100 );
				}

				Sleep ( 500 );
			}

			if ( ( GKEY ( VK_MENU ) & 0xFF80 ) &&
				 ( GKEY ( 'U' ) & 0xFF80 ) )
			{
				// Unchanged
				SetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), "Unchanged" );

				if ( scanNum != -1 ) 
					SMX ( mainDlg, WM_COMMAND, (WPARAM) IDC_BUTTON2, NULL );
				
				while ( !ScanFinished ) {
					Sleep ( 100 );
				}

				Sleep ( 500 );
			}
		}

		Sleep ( 100 );
	}
}
void NoBug ()
{
	while ( true )
	{
		char szScan [256] = {0};
		GetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), (LPSTR) szScan, 256 );

		if ( !strcmp ( szScan, "Unknown" ) )
		{
			// Disable edit control
			Edit_Enable   ( GetDlgItem ( mainDlg, IDC_EDIT1		), FALSE );
			Button_Enable ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), FALSE );
			Button_Enable ( GetDlgItem ( mainDlg, IDC_COMBO1 ),    TRUE );
		}
		else if ( !strcmp ( szScan, "Array of Bytes" ) )
		{
			SMX ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), BM_SETCHECK, (WPARAM) BST_UNCHECKED, NULL );

			// Enable controls
			Edit_Enable   ( GetDlgItem ( mainDlg, IDC_EDIT1  ), TRUE );

			// Disable edit control
			Button_Enable ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), FALSE );
			Button_Enable ( GetDlgItem ( mainDlg, IDC_COMBO1 ),    FALSE );
		}
		else if ( !strcmp ( szScan, "Text" ) )
		{
			SMX ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), BM_SETCHECK, (WPARAM) BST_UNCHECKED, NULL );

			// Enable controls
			Edit_Enable   ( GetDlgItem ( mainDlg, IDC_EDIT1  ), TRUE );

			// Disable edit control
			Button_Enable ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), FALSE );
			Button_Enable ( GetDlgItem ( mainDlg, IDC_COMBO1 ),    FALSE );
		}
		else if ( !strcmp ( szScan, "Changed" ) || !strcmp ( szScan, "Unchanged" ) || !strcmp ( szScan, "Increased" ) || !strcmp ( szScan, "Decreased" ) )
		{
			// Disable edit control
			Edit_Enable   ( GetDlgItem ( mainDlg, IDC_EDIT1		), FALSE );
			Button_Enable ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), FALSE );
			Button_Enable ( GetDlgItem ( mainDlg, IDC_COMBO1 ),    TRUE );
		}
		else
		{
			// Default
			Edit_Enable   ( GetDlgItem ( mainDlg, IDC_EDIT1  ),	   TRUE );
			Button_Enable ( GetDlgItem ( mainDlg, IDC_COMBO1 ),    TRUE );
			Button_Enable ( GetDlgItem ( mainDlg, IDC_CHECKBOX1 ), TRUE );
		}

		// Sleep 
		Sleep (100);
	}
}
void Popup ()
{
	while ( true )
	{
		if ( GKEY ( VK_F7 ) & 0xFF80 ) 
		{
			// Make sure the debugger is on
			if ( dbgOn )
			{
				// Send the message and stop the debugger
				SMX ( dbgDlg, WM_COMMAND, IDC_BUTTON4, NULL );
				MakeSound ();
			}
		}

		// Hotkeys
		if ( GKEY ( VK_F5 ) & 0xFF80 ) {
			// Obtain the dimensions
			RECT myRect;
			GetWindowRect ( mainDlg, &myRect );

			int width  = myRect.right - myRect.left;
			int height = myRect.bottom - myRect.top;
			SetWindowPos ( mainDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( addrDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( addrDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( editDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( editDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( ptrDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( ptrDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( memDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( memDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( chgDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( chgDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( optDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( optDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( dbgDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( dbgDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( mdlDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( mdlDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( fwDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( fwDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( regDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( regDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( disDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( disDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//
			GetWindowRect ( extraDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( extraDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_SHOWWINDOW );
			//

			// Beep
			MakeSound ();

			Sleep ( 500 );
		}

		if ( GKEY ( VK_F6 ) & 0xFF80 ) {
			// Obtain the dimensions
			RECT myRect;
			GetWindowRect ( mainDlg, &myRect );

			int width  = myRect.right - myRect.left;
			int height = myRect.bottom - myRect.top;
			SetWindowPos ( mainDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( addrDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( addrDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( editDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( editDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( ptrDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( ptrDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( memDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( memDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( chgDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( chgDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( optDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( optDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( dbgDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( dbgDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( mdlDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( mdlDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( fwDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( fwDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( regDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( regDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( disDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( disDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//
			GetWindowRect ( extraDlg, &myRect );

			width  = myRect.right - myRect.left;
			height = myRect.bottom - myRect.top;

			SetWindowPos ( extraDlg, HWND_TOPMOST, myRect.left, myRect.top, width, height, SWP_HIDEWINDOW );
			//

			// Beep
			MakeSound ();

			Sleep ( 500 );
		}

		Sleep (100);
	}
}

//

// Update values
void UpdatePointers ()
{
	while ( true )
	{
		for ( int query = 0; query < ptrCountTwo; query ++ )
		{
			if ( ptrInfoTwo [query].index != -1 )
			{
				string Text = ptrInfoTwo [query].PointerName;
				string NoMexicans;

				// Obtain variables
				DWORD dwValue   = 0;
				DWORD dwPointer = 0;
				DWORD dwAddress = 0;
				DWORD dwPrev    = 0;

				int check   = 1;
				int LeftOff = 0;
				int Length  = 0;

				// Get rid of spaces
				for ( SIZE_T i = 0; i < Text.length (); i ++ )
				{
					if ( Text.at ( i ) != ' ' )
						NoMexicans += Text.at ( i ) ;
					if ( Text.at ( i ) == '+' )
						++Length;
				}

				// Go through the loop
				for ( SIZE_T i = 0; i < NoMexicans.length (); i ++ )
				{
					if ( NoMexicans.at ( i ) == '+' || i == ( NoMexicans.length () - 1 ) )
					{
						string Number = NoMexicans.substr ( LeftOff, i );
						stringstream ss;
						ss << Number.c_str ();

						LPVOID lpValue;
						ss >> lpValue;
						dwValue = (DWORD) lpValue; 

						if ( check == 1 )
							dwPointer = RV <DWORD> ( dwValue );	

						// Extract the values
						if ( ( Length != 1 ) )
						{										
							if ( check > 1 )
							{
								dwAddress = RV <DWORD> ( dwPointer + dwValue );
								dwPointer = dwAddress;

								--Length;
							}
						}
						else if ( check != 1 )
							dwPointer += dwValue;

						++ check;
						LeftOff = i + 1;
					}
				}

				// Pointer
				string AddressStr = "P->";
				AddressStr       += CTT <LPVOID> ( (void*) dwPointer );

				// Set the address
				ListView_SetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), ptrInfoTwo [query].index, 1, (LPSTR) AddressStr.c_str () );
			}
		}

		Sleep ( 250 );
	}
}
void UpdateValues ()
{
	// Loop until dialog is closed
	while ( true )
	{
		// Obtain item count
		int item = ListView_GetItemCount ( GetDlgItem ( mainDlg, IDC_LIST2 ) );

		// Go through every address
		for ( int i = 0; i < item; i ++ )
		{
			// Obtain the address
			char szBuffer [256] = {0};
			ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 1, szBuffer, 256 );

			// Obtain the ST
			char szST [256] = {0};
			ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 2, szST, 256 );

			// Convert the address to string
			string addr;
			addr += szBuffer;

			// Address
			LPVOID lpAddr = 0;
			
			if ( addr.at (0) != 'P' )
			{
				// Convert the string to LPVOID
				stringstream ss;
				ss << addr;
				ss >> lpAddr;
			}
			else
			{
				// Convert the string to LPVOID
				stringstream ss;
				ss << addr.substr ( 3, 10 );
				ss >> lpAddr;
			}

			// Byte
			if ( !strcmp ( szST, "Byte" ) )
			{
				// R value
				BYTE Value = RV <BYTE> ( (DWORD) lpAddr );

				// Set text
				string str = "";

				if ( Value == -1 )
					str = "?";
				else
					str = CTT <unsigned int> ( (unsigned int) Value );

				ListView_SetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 3, (LPSTR) str.c_str () );
			}
			// 2 Bytes
			if ( !strcmp ( szST, "2 Bytes" ) )
			{
				// R value
				WORD Value = RV <WORD> ( (DWORD) lpAddr );

				// Set text
				string str = "";

				if ( Value == -1 )
					str = "?";
				else
					str = CTT <WORD> ( Value );

				ListView_SetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 3, (LPSTR) str.c_str () );
			}
			// 4 Bytes
			if ( !strcmp ( szST, "4 Bytes" ) )
			{
				// R value
				DWORD Value = RV <DWORD> ( (DWORD) lpAddr );

				// Set text
				string str = "";

				if ( Value == -1 )
					str = "?";
				else
					str = CTT <DWORD> ( Value );

				ListView_SetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 3, (LPSTR) str.c_str () );
			}
			// 8 Bytes
			if ( !strcmp ( szST, "8 Bytes" ) )
			{
				// R value
				UINT64 Value = RV <UINT64> ( (DWORD) lpAddr );

				// Set text
				string str = "";

				if ( Value == -1 )
					str = "?";
				else
					str = CTT <UINT64> ( Value );

				ListView_SetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 3, (LPSTR) str.c_str () );
			}
			// Float
			if ( !strcmp ( szST, "Float" ) )
			{
				// R value
				float Value = RV <float> ( (DWORD) lpAddr );

				// Set text
				string str = "";

				if ( Value == -1 )
					str = "?";
				else
					str = CTT <float> ( (float) Value );

				ListView_SetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 3, (LPSTR) str.c_str () );
			}
			// Double
			if ( !strcmp ( szST, "Double" ) )
			{
				// R value
				double Value = RV <double> ( (DWORD) lpAddr );

				// Set text
				string str = "";

				if ( Value == -1 )
					str = "?";
				else
					str = CTT <double> ( Value );

				ListView_SetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 3, (LPSTR) str.c_str () );
			}
			// Text
			if ( !strcmp ( szST, "Text" ) )
			{
				// Text
			}
		}

		// Sleep
		Sleep ( 500 );
	}
}
void FreezeValues ()
{
	// Loop until dialog is closed
	while ( true )
	{
		// Get the # of items
		int items = ListView_GetItemCount ( GetDlgItem ( mainDlg, IDC_LIST2 ) );

		// Go through every item
		for ( int i = 0; i < items; i ++ )
		{
			// Check if the address is frozen
			bool freeze = ListView_GetCheckState ( GetDlgItem ( mainDlg, IDC_LIST2 ), i ) == 1 ? true : false;

			// Write the values now
			if ( freeze )
			{
				// Obtain the address
				char szBuffer [256] = {0};
				ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 1, szBuffer, 256 );

				// Obtain the ST
				char szST [256] = {0};
				ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 2, szST, 256 );

				// Obtain the value
				char szValue [256] = {0};
				ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 4, szValue, 256 );

				// Convert to string
				string str = "";
				str       += szValue;

				// Convert the address to string
				string addr;
				addr += szBuffer;

				// Address
				LPVOID lpAddr;
				
				if ( addr.at (0) != 'P' )
				{
					// Convert the string to LPVOID
					stringstream ss;
					ss << addr;
					ss >> lpAddr;
				}
				else
				{
					// Convert the string to LPVOID
					stringstream ss;
					ss << addr.substr ( 3, 10 );
					ss >> lpAddr;
				}

				// Byte
				if ( !strcmp ( szST, "Byte" ) )
				{
					BYTE Value = ConvertTextTo2 <BYTE, BYTE> ( str );
					WV     ( (DWORD) lpAddr, Value );
				}
				// 2 Bytes
				if ( !strcmp ( szST, "2 Bytes" ) )
				{
					WORD Value = ConvertTextTo2 <WORD, WORD> ( str );
					WV     ( (DWORD) lpAddr, Value );
				}
				// 4 Bytes
				if ( !strcmp ( szST, "4 Bytes" ) )
				{
					DWORD Value = ConvertTextTo2 <DWORD, DWORD> ( str );
					WV     ( (DWORD) lpAddr, Value );
				}
				// 8 Bytes
				if ( !strcmp ( szST, "8 Bytes" ) )
				{
					UINT64 Value = ConvertTextTo2 <UINT64, UINT64> ( str );
					WV     ( (DWORD) lpAddr, Value );
				}
				// Float
				if ( !strcmp ( szST, "Float" ) )
				{
					float Value = ConvertTextTo2 <float, float> ( str );
					WV     ( (DWORD) lpAddr, Value );
				}
				// Double
				if ( !strcmp ( szST, "Double" ) )
				{
					double Value = ConvertTextTo2 <double, double> ( str );
					WV     ( (DWORD) lpAddr, Value );
				}
			}
		}

		// Sleep
		Sleep ( 100 );
	}
}
//

// Initialize dialogs
void initAddrDlg ()
{
	DB ( hInstance, MAKEINTRESOURCE (IDD_DIALOG2), 0, AddrDialog );
}
void initEditDlg ()
{
	DB ( hInstance, MAKEINTRESOURCE (IDD_DIALOG3), 0, EditDialog );
}
void initPtrDlg  ()
{
	DB ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG4 ), 0, PtrDialog );
}
void initMemDlg  ()
{
	// Open the dialog
	DB ( hInstance, MAKEINTRESOURCE (IDD_DIALOG5), NULL, MemDialog );
}
void initChgDlg  ()
{
	DB ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG6 ), NULL, ChgDialog );
}
void initOptDlg  ()
{
	// Call the dialog
	DB ( hInstance, MAKEINTRESOURCE (IDD_DIALOG7), NULL, OptDialog );
}
void initDbgDlg  ()
{
	DB ( hInstance, MAKEINTRESOURCE (IDD_DIALOG8), NULL, DbgDlg );
}
void initMdlDlg  ()
{
	DB ( hInstance, MAKEINTRESOURCE (IDD_DIALOG9), NULL, ModuleDlg );
}
void initFuncWatch ()
{
	DB ( hInstance, MAKEINTRESOURCE (IDD_DIALOG10), NULL, fwDialog );
}
void initExtra     ()
{
	DialogBox ( hInstance, MAKEINTRESOURCE (IDD_DIALOG13), NULL, extraDialog );
}
void initRegDlg    ()
{
	DB ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG11 ), NULL, regDialog );
}
void initDisDlg    ()
{
	DialogBox ( hInstance, MAKEINTRESOURCE (IDD_DIALOG12), NULL, disDialog );
}
void initSettings  ()
{
	DialogBox ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG14 ), NULL, settingDialog );
}
void initPointer   ()
{
	DialogBox ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG15 ), NULL, pointerDialog );
}
void initAll       ()
{
	DialogBox ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG17 ), NULL, editAllDialog );
}
void initDataStruct ()
{
	DialogBox ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG16 ), NULL, DataDialog );
}
//

char szCurrentValue [256] = {0};

// Setup Procedures 
void MainProc ()
{
	// Make LPMS appear on top
	RECT rect;
	GetWindowRect ( mainDlg, &rect );

	int width  = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// Set LPMS to topmost
	SetWindowPos ( mainDlg, HWND_TOPMOST, rect.left, rect.top, width, height, SW_NORMAL );

	// Setup information
	Title += 0x22;
	Title += Username.c_str ();
	Title += 0x22;

	SetWindowText ( mainDlg, Title.c_str () );
	SetWindowText ( GetDlgItem ( mainDlg, IDC_USERNAME ), Username.c_str () );
	SetWindowText ( GetDlgItem ( mainDlg, IDC_VERSION ), Version.c_str () );

	// Obtain information
	NS ();
	MS1 = 0x10000;
	MS2 = 0x10010;
	dwMS1 = 0x10000;
	dwMS2 = 0x10010;
	Set   = false;

	// Create buffers
	dwPtrScan  = new DWORD [ 100000 ];
	ptrInfoTwo = new PTR_INFO [256];

	// Set the process name
	char myProcess [256] = {0};
	GetModuleBaseName ( GetCurrentProcess (), NULL, (LPSTR) myProcess, 256 );

	SetWindowText ( GetDlgItem ( mainDlg, IDC_FILE ), (LPCSTR) myProcess );

	// Make sure no bug occurs
	hNoBug   = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) NoBug, NULL, NULL, NULL );

	// Call the popup function
	hPopup = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) Popup, NULL, NULL, NULL );

	// Main listview functions
	hUpdate     = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) UpdateValues, NULL, NULL, NULL );
	hFreeze     = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) FreezeValues, NULL, NULL, NULL );
	hPointers   = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) UpdatePointers, NULL, NULL, NULL );

	// Hotkeys
	hHotkeys     = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) Hotkeys, NULL, NULL, NULL );

	// Obtain hWnd's
	HWND hList1 = GetDlgItem (mainDlg, IDC_LIST1);
	HWND hList2 = GetDlgItem (mainDlg, IDC_LIST2);

	// Hide progress bar
	ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_HIDE );

	// Obtain system, information
	SYSTEM_INFO si;
	GetSystemInfo ( &si );

	// Setup dialogs
	SetWindowText ( GetDlgItem ( mainDlg, IDC_EDIT1 ), "0" );
	SetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO1 ), "4 Bytes" );
	SetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO2 ), "Precise" );
	SetWindowText ( GetDlgItem ( mainDlg, IDC_EDIT2 ), CTT <LPVOID> ( si.lpMinimumApplicationAddress ).c_str () );
	SetWindowText ( GetDlgItem ( mainDlg, IDC_EDIT3 ), CTT <LPVOID> ( si.lpMaximumApplicationAddress ).c_str () );
	
	// Add items to combo
	for ( int index = 0; index < _countof ( ST ); index ++ )
		SMX ( GetDlgItem ( mainDlg, IDC_COMBO1 ), CB_ADDSTRING, 0, (LPARAM) ST [index] );

	for ( int index = 0; index < _countof ( NSN ); index ++ )
		SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, 0, (LPARAM) NSN [index] );

	SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, 0, (LPARAM) "Array of Bytes" );
	SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, 0, (LPARAM) "Text" );

	// Setup columns for List2
	for ( int i = 0; i < _countof ( Scan ); i ++ )
	{
		// Setup the column
		LV_COLUMN lv;
		lv.mask       = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lv.cx         = ScanWidth [i];
		lv.cchTextMax = 256;
		lv.pszText    = Scan [i];

		// Insert column
		ListView_InsertColumn ( hList2, i, &lv );
	}

	// Set the listview style 
	ListView_SetExtendedListViewStyle ( GetDlgItem (mainDlg, IDC_LIST2), LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	ListView_SetTextColor             ( GetDlgItem (mainDlg, IDC_LIST2), RGB ( 255, 0, 0 ) );

	// Disable the Next Scan button
	Button_Enable ( GetDlgItem ( mainDlg, IDC_BUTTON2 ), FALSE );

	// Disable the Undo Scan button
	Button_Enable ( GetDlgItem ( mainDlg, IDC_BUTTON8 ), FALSE );

	// Hide the New Scan button
	ShowWindow ( GetDlgItem ( mainDlg, IDC_BUTTON5 ), SW_HIDE );
}

void AddrProc ()
{
	SetWindowText ( addrDlg, Title.c_str () );

	// Set the default address
	SetWindowText ( GetDlgItem ( addrDlg, IDC_EDIT1 ), "00010000" );
	SetWindowText ( GetDlgItem ( addrDlg, IDC_COMBO1 ), "4 Bytes" );

	// Check checkbox
	SMX ( GetDlgItem ( addrDlg, IDC_CHECKBOX1 ), BM_SETCHECK, (WPARAM) BST_CHECKED, 0 );

	// Add the items
	for ( int i = 0; i < _countof ( ST ); i ++ )
		SMX ( GetDlgItem ( addrDlg, IDC_COMBO1 ), CB_ADDSTRING, 0, (LPARAM) ST [i] );
}
void EditProc ()
{
	SetWindowText ( editDlg, Title.c_str () );

	// Listview variables
	char szBuffer [ 256 ] = { 0 };
	char szST   [ 256 ] = { 0 };
	char szValue  [ 256 ] = { 0 };
	char szLocked [ 256 ] = { 0 };

	// Obtain the selected item
	selectedItem = SMX ( GetDlgItem ( mainDlg, IDC_LIST2 ), LVM_GETNEXTITEM, -1, LVNI_SELECTED );

	// Obtain the address
	ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), selectedItem, 1, szBuffer, 256 );		

	// Setup address
	SetWindowText ( GetDlgItem ( editDlg, IDC_EDIT1 ), szBuffer );

	// Obtain the ST
	ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), selectedItem, 2, szST, 256 );		
	
	// Setup ST
	SetWindowText ( GetDlgItem ( editDlg, IDC_COMBO1 ), szST );

	// Obtain the Value
	ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), selectedItem, 3, szValue, 256 );	

	// Obtain the Locked Value
	ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), selectedItem, 4, szLocked, 256 );	

	// Setup value, locked
	SetWindowText ( GetDlgItem ( editDlg, IDC_EDIT2 ), szValue  );
	SetWindowText ( GetDlgItem ( editDlg, IDC_EDIT3 ), szLocked );

	// Make sure the value hasn't changed
	GetWindowText ( GetDlgItem ( editDlg, IDC_EDIT2 ), (LPSTR) szCurrentValue, 256 );

	// Add items to ST
	for ( int i = 0; i < _countof ( ST ); i ++ )
		SMX ( GetDlgItem ( editDlg, IDC_COMBO1 ), CB_ADDSTRING, 0, (LPARAM) ST [i] );

	// Setup checkboxes
	SMX ( GetDlgItem ( editDlg, IDC_CHECKBOX1 ), BM_SETCHECK, (WPARAM) BST_CHECKED, NULL );

	// Obtain the string value
	string addr;
	addr += szBuffer;

	// Convert address to DWORD
	LPVOID lpAddr;
	
	if ( addr.at (0) != 'P' )
	{
		// Convert the string to LPVOID
		stringstream ss;
		ss << addr;
		ss >> lpAddr;
	}
	else
	{
		// Convert the string to LPVOID
		stringstream ss;
		ss << addr.substr ( 3, 10 );
		ss >> lpAddr;
	}

	dwPtrAddr = (DWORD) lpAddr;

	// Preform the scan
	SetWindowText ( GetDlgItem ( editDlg, IDC_EDIT4 ), CS ( dwPtrAddr ).PointerInfo.c_str () ); 
}
void PtrProc  ()
{
	SetWindowText ( ptrDlg, Title.c_str () );

	// Setup window names
	SetWindowText ( GetDlgItem ( ptrDlg, IDC_ADDRESS ), CTT <LPVOID> ( (LPVOID) dwPtrAddr ).c_str () );
	SetWindowText ( GetDlgItem ( ptrDlg, IDC_EDIT1 ), "800" );

	// Setup checkboxes
	SMX ( GetDlgItem ( ptrDlg, IDC_CHECKBOX2 ), BM_SETCHECK, (WPARAM) BST_CHECKED, 0 );
	SMX ( GetDlgItem ( ptrDlg, IDC_CHECKBOX1 ), BM_SETCHECK, (WPARAM) BST_CHECKED, 0 );

	// Setup ListBox
	SMX ( GetDlgItem ( ptrDlg, IDC_LIST1 ), LB_SETHORIZONTALEXTENT, (WPARAM) 256, 0 );
}
void MemProc  ()
{
	SetWindowText ( memDlg, Title.c_str () );

	// Setup window titles
	SetWindowText ( GetDlgItem ( memDlg, IDC_EDIT1 ), CTT <LPVOID> ( (LPVOID) MS1 ).c_str () );
	SetWindowText ( GetDlgItem ( memDlg, IDC_EDIT2 ), CTT <LPVOID> ( (LPVOID) MS2  ).c_str () );	

	Set = false;

	// Setup ListBox's
	SMX ( GetDlgItem ( memDlg, IDC_LIST1 ), LB_SETHORIZONTALEXTENT, (WPARAM) 350, 0 );
	SMX ( GetDlgItem ( memDlg, IDC_LIST2 ), LB_SETHORIZONTALEXTENT, (WPARAM) 150, 0 );
}
void ChgProc  ()
{
	SetWindowText ( chgDlg, Title.c_str () );

	// Obtain the selected index 
	int item = SMX ( GetDlgItem ( memDlg, IDC_LIST1 ), LB_GETCURSEL, NULL, NULL );

	if ( item == -1 ) 
		MessageBoxA ( memDlg, "Please select an item", "Error", MB_ICONINFORMATION );
	else
	{
		// Get the text
		char szBuffer [256] = {0};
		SMX ( GetDlgItem ( memDlg, IDC_LIST1 ), LB_GETTEXT, (WPARAM) item, (LPARAM) szBuffer );

		// Convert the text to string
		string strAddr;
		strAddr += szBuffer;

		// String to dword
		DWORD dwAddr = ConvertTextTo2 <LPVOID, DWORD> ( strAddr.substr ( strAddr.find_first_of (".") + 2, strAddr.find_first_of (".") + 8) );

		// Set information
		SetWindowText ( GetDlgItem ( chgDlg, IDC_EDIT1 ), strAddr.substr (strAddr.find_first_of (".") + 2, strAddr.find_first_of (".") + 8 ).c_str () );
		SetWindowText ( GetDlgItem ( chgDlg, IDC_EDIT2 ), strAddr.substr (strAddr.find_first_of ("-") + 2, 63).c_str () );
	}
}
void DbgProc  ()
{
	SetWindowText ( dbgDlg, Title.c_str () );

	// Set window titles
	SetWindowText ( GetDlgItem ( dbgDlg, IDC_EDIT1 ), CTT <LPVOID> ( (void*) MS1 ).c_str () );

	HWND hList = GetDlgItem ( dbgDlg, IDC_LIST1 );

	// Add the columns
	for ( int i = 0; i < _countof ( Dbg ); i ++ )
	{
		// Setup the column
		LV_COLUMN lv;
		lv.mask       = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lv.cx         = DbgWidth [i];
		lv.cchTextMax = 256;
		lv.pszText    = Dbg [i];

		// Insert column
		ListView_InsertColumn ( hList, i, &lv );
	}

	// Set the listview style 
	ListView_SetExtendedListViewStyle (GetDlgItem (dbgDlg, IDC_LIST1), LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
}
void MdlProc  ()
{
	SetWindowText ( mdlDlg, Title.c_str () );

	HWND hList = GetDlgItem ( mdlDlg, IDC_LIST1 );

	for ( int i = 0; i < 2; i ++ )
	{
		// Setup the column
		LV_COLUMN lv;
		lv.mask       = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lv.cx         = MdlWidth [i];
		lv.cchTextMax = 256;
		lv.pszText    = MDL      [i];

		// Insert column
		ListView_InsertColumn ( hList, i, &lv );
	}
}
void FuncProc ()
{
	SetWindowText ( fwDlg, Title.c_str () );

	// Set Window Titles
	SetWindowText ( GetDlgItem ( fwDlg, IDC_EDIT1 ), "kernel32.dll" );
	SetWindowText ( GetDlgItem ( fwDlg, IDC_EDIT2 ), "Sleep" );
}
void RegProc  ()
{
	SetWindowText ( regDlg, Title.c_str () );

	HWND hList = GetDlgItem ( regDlg, IDC_LIST1 );

	// Setup columns for the listview
	for ( int i = 0; i < _countof ( Reg ); i ++ )
	{
		// Setup the column
		LV_COLUMN lv;
		lv.mask       = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lv.cx         = RegWidth [i];
		lv.cchTextMax = 256;
		lv.pszText    = Reg [i];

		// Insert column
		ListView_InsertColumn ( hList, i, &lv );
	}

	// Set the listview style 
	ListView_SetExtendedListViewStyle (GetDlgItem (regDlg, IDC_LIST1), LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	ScanRegions ();
}
void DisProc  ()
{
	HWND hList = GetDlgItem  (disDlg, IDC_LIST1);

	// Setup columns for List2
	for ( int i = 0; i < _countof ( DisList ); i ++ )
	{
		// Setup the column
		LV_COLUMN lv;
		lv.mask       = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lv.cx         = DisWidth [i];
		lv.cchTextMax = 256;
		lv.pszText    = DisList [i];

		// Insert column
		ListView_InsertColumn ( hList, i, &lv );
	}

	// Set the listview style 
	ListView_SetExtendedListViewStyle (hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	SetWindowText ( disDlg, Title.c_str () );

	// Set the title
	SetWindowText ( GetDlgItem ( disDlg, IDC_EDIT1 ), CTT <LPVOID> ( (void*) MS1 ).c_str () ); 
}
void ExtraProc ()
{
	SMX ( GetDlgItem ( extraDlg, IDC_LIST1 ), LB_SETHORIZONTALEXTENT, (WPARAM) 256, NULL );

	// Obtain the selected item
	int Item = SMX ( GetDlgItem ( dbgDlg, IDC_LIST1 ), LVM_GETNEXTITEM, -1, LVNI_SELECTED );
	int Sel  = 0;

	// Obtain the text buffer
	char szBuffer [256] = {0};
	ListView_GetItemText ( GetDlgItem ( dbgDlg, IDC_LIST1 ), Item, 1, szBuffer, 256 );

	// Obtain system info
	SYSTEM_INFO SI;
	GetSystemInfo ( &SI );

	// Show the disassembly
	DWORD dwAddr = (DWORD) dbgInfo [Item].lpAddr;

	// Create the variable
	DISASM myDisasm;

	// Display the disassembly
	for ( int i = 0; i < 5; i ++ )
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

	// Display the next 10 addresses
	for ( int i = 0; i < 10; i ++ )
	{
		if ( dwAddr == (DWORD) dbgInfo [Item].lpAddr )
			Sel = i;

		// Get to the last 2 addresses
		DISASM myDisasm;
		myDisasm.EIP    = dwAddr;
		DWORD  dwLength = (DWORD) Disasm ( &myDisasm );

		while ( dwLength == 0xFFFFFFFF )
		{
			dwAddr += 0x1;
			myDisasm.EIP = dwAddr;
			dwLength = (DWORD) Disasm ( &myDisasm );
		}

		// Add it to the string
		string myStr = CTT <LPVOID> ( (void*) dwAddr );
		myStr        += " - ";
		myStr	     += myDisasm.CompleteInstr;

		// Insert the address
		SMX ( GetDlgItem ( extraDlg, IDC_LIST1 ), LB_ADDSTRING, NULL, (LPARAM) myStr.c_str () );

		// Increment
		dwAddr	     += dwLength;
	}

	// Set the selection
	SMX ( GetDlgItem ( extraDlg, IDC_LIST1 ), LB_SETCURSEL, (WPARAM) Sel, NULL );

	// Set the registers
	SetWindowText ( GetDlgItem ( extraDlg, IDC_EAX ), CTT < LPVOID > ( (void*) dbgInfo [Item].EAX ).c_str () );
	SetWindowText ( GetDlgItem ( extraDlg, IDC_EBX ), CTT < LPVOID > ( (void*) dbgInfo [Item].EBX ).c_str () );
	SetWindowText ( GetDlgItem ( extraDlg, IDC_ECX ), CTT < LPVOID > ( (void*) dbgInfo [Item].ECX ).c_str () );
	SetWindowText ( GetDlgItem ( extraDlg, IDC_EDX ), CTT < LPVOID > ( (void*) dbgInfo [Item].EDX ).c_str () );
	SetWindowText ( GetDlgItem ( extraDlg, IDC_ESI ), CTT < LPVOID > ( (void*) dbgInfo [Item].ESI ).c_str () );
	SetWindowText ( GetDlgItem ( extraDlg, IDC_EDI ), CTT < LPVOID > ( (void*) dbgInfo [Item].EDI ).c_str () );
	SetWindowText ( GetDlgItem ( extraDlg, IDC_EBP ), CTT < LPVOID > ( (void*) dbgInfo [Item].EBP ).c_str () );
	SetWindowText ( GetDlgItem ( extraDlg, IDC_ESP ), CTT < LPVOID > ( (void*) dbgInfo [Item].ESP ).c_str () );
	SetWindowText ( GetDlgItem ( extraDlg, IDC_EIP ), CTT < LPVOID > ( (void*) dbgInfo [Item].EIP ).c_str () );
}
void SettingProc ()
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
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX3 ), BM_SETCHECK, (WPARAM) BST_CHECKED, NULL );
				else
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX3 ), BM_SETCHECK, (WPARAM) BST_UNCHECKED, NULL );
			}
			break;

		case 1:
			{
				if ( !strcmp ( str.c_str (), "TRUE" ) )
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX2 ), BM_SETCHECK, (WPARAM) BST_CHECKED, NULL );
				else
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX2 ), BM_SETCHECK, (WPARAM) BST_UNCHECKED, NULL );
			}
			break;

		case 2:
			{
				if ( !strcmp ( str.c_str (), "TRUE" ) )
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX1 ), BM_SETCHECK, (WPARAM) BST_CHECKED, NULL );
				else
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX1 ), BM_SETCHECK, (WPARAM) BST_UNCHECKED, NULL );
			}
			break;

		case 3:
			{
				if ( !strcmp ( str.c_str (), "TRUE" ) )
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX4 ), BM_SETCHECK, (WPARAM) BST_CHECKED, NULL );
				else
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX4 ), BM_SETCHECK, (WPARAM) BST_UNCHECKED, NULL );
			}
			break;

		case 4:
			{
				SetWindowText ( GetDlgItem ( settingDlg, IDC_EDIT1 ), str.c_str () );
			}
			break;

		case 5:
			{
				SetWindowText ( GetDlgItem ( settingDlg, IDC_EDIT2 ), str.c_str () );
			}
			break;

		case 6:
			{
				SetWindowText ( GetDlgItem ( settingDlg, IDC_EDIT3 ), str.c_str () );
			}
			break;

		case 7:
			{
				SetWindowText ( GetDlgItem ( settingDlg, IDC_EDIT4 ), str.c_str () );
			}
			break;
		}

		if ( count > 6 )
			break;

		++ count;
	}

	SettingsAt.close ();
}
void EditAllProc ()
{
	// Change text 
	SetWindowText ( GetDlgItem ( editAllDlg, IDC_EDIT1 ), "0" );
	SetWindowText ( GetDlgItem ( editAllDlg, IDC_EDIT2 ), "0" );
}
void DataProc ()
{
	// Setup ListView
	for ( int i = 0; i < _countof ( DataBuf ); i ++ )
	{
		// Setup the column
		LV_COLUMN lv;
		lv.mask       = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lv.cx         = DataWidth [i];
		lv.cchTextMax = 256;
		lv.pszText    = DataBuf [i];

		// Insert column
		ListView_InsertColumn ( GetDlgItem (dataStruct, IDC_LIST1), i, &lv );
	}

	// Set the listview style 
	ListView_SetExtendedListViewStyle ( GetDlgItem (dataStruct, IDC_LIST1), LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	// Setup windows
	SetWindowText ( GetDlgItem ( dataStruct, IDC_EDIT1 ), "00010000" );
	SetWindowText ( GetDlgItem ( dataStruct, IDC_EDIT2 ), "1024" );
	SetWindowText ( GetDlgItem ( dataStruct, IDC_COMBO1 ), "4 Bytes" );

	// Add types
	for ( int index = 0; index < _countof ( ST ); index ++ )
		SMX ( GetDlgItem ( dataStruct, IDC_COMBO1 ), CB_ADDSTRING, 0, (LPARAM) ST [index] );
	
	// Setup checkboxes
	SMX ( GetDlgItem ( dataStruct, IDC_CHECKBOX1 ), BM_SETCHECK, BST_CHECKED, 0 );
	SMX ( GetDlgItem ( dataStruct, IDC_CHECKBOX2 ), BM_SETCHECK, BST_CHECKED, 0 );
}
//

//
void DataFunc ()
{
	HWND hWndDlg = dataStruct;

	// Clear the items
	ListView_DeleteAllItems ( GetDlgItem ( hWndDlg, IDC_LIST1 ) );

	// Read
	bool addrHex = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX2 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;
	bool sizeHex = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;

	// Obtain the address
	char szBuffer [256] = {0};
	GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szBuffer, 256 );

	// Obtain the size
	char szSize [256] = {0};
	GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT2 ), (LPSTR) szSize, 256 );

	// Obtain the type
	char szType [256] = {0};
	GetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO1 ), (LPSTR) szType, 256 );

	// Variables
	DWORD dwAddr = 0;
	DWORD dwSize = 0;

	// Get values
	stringstream ss;
	ss << szSize;

	stringstream ss1;
	ss1 << szBuffer;

	// Determine values
	if ( sizeHex )
	{
		LPVOID lpSize;
		ss >> lpSize;
		dwSize = (DWORD) lpSize;
	}
	else
		ss >> dwSize;

	// Determine values
	if ( addrHex )
	{
		LPVOID lpAddr;
		ss1 >> lpAddr;
		dwAddr = (DWORD) lpAddr;
	}
	else
		ss1 >> dwAddr;

	// Start
	DWORD dwEnd = ( dwAddr + dwSize );

	for ( dwAddr; dwAddr < dwEnd; dwAddr ++ )
	{
		// Byte
		if ( !strcmp ( szType, "Byte" ) )
		{
			BYTE Value = RV <BYTE> ( (DWORD) dwAddr );

			// Get Item
			int item = ListView_GetItemCount ( GetDlgItem ( hWndDlg, IDC_LIST1 ) );

			// Setup item
			LV_ITEM lv;
			lv.iItem    = item;           
			lv.iSubItem = 0;
			lv.mask     = LVIF_TEXT;       
			lv.pszText  = "";
			lv.cchTextMax = 256; 

			// Insert item
			ListView_InsertItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );

			// Fix subitems
			for ( int i = 0; i <= 1; i ++ )
			{
				// Setup neccessary values
				lv.iSubItem   = i;
				lv.cchTextMax = 256;

				// Setup text
				string str = "";

				if ( i == 0 )
					str = CTT <LPVOID> ( (void*) dwAddr );
				else if ( i == 1 )
					str = CTT <unsigned int> ( (unsigned int) Value );

				// Add item
				lv.pszText = (LPSTR) str.c_str ();

				// Insert subitem
				ListView_SetItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );
			}
		}
		// 2 Bytes
		if ( !strcmp ( szType, "2 Bytes" ) )
		{
			// Read Value
			WORD Value = RV <WORD> ( dwAddr );

			// Get Item
			int item = ListView_GetItemCount ( GetDlgItem ( hWndDlg, IDC_LIST1 ) );

			// Setup item
			LV_ITEM lv;
			lv.iItem    = item;           
			lv.iSubItem = 0;
			lv.mask     = LVIF_TEXT;       
			lv.pszText  = "";
			lv.cchTextMax = 256; 

			// Insert item
			ListView_InsertItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );

			// Fix subitems
			for ( int i = 0; i <= 1; i ++ )
			{
				// Setup neccessary values
				lv.iSubItem   = i;
				lv.cchTextMax = 256;

				// Setup text
				string str = "";

				if ( i == 0 )
					str = CTT <LPVOID> ( (void*) dwAddr );
				else if ( i == 1 )
					str = CTT <WORD> ( Value );

				// Add item
				lv.pszText = (LPSTR) str.c_str ();

				// Insert subitem
				ListView_SetItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );
			}
		}
		// 4 Bytes
		if ( !strcmp ( szType, "4 Bytes" ) )
		{
			// R value
			DWORD Value = RV <DWORD> ( dwAddr );

			// Get Item
			int item = ListView_GetItemCount ( GetDlgItem ( hWndDlg, IDC_LIST1 ) );

			// Setup item
			LV_ITEM lv;
			lv.iItem    = item;           
			lv.iSubItem = 0;
			lv.mask     = LVIF_TEXT;       
			lv.pszText  = "";
			lv.cchTextMax = 256; 

			// Insert item
			ListView_InsertItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );

			// Fix subitems
			for ( int i = 0; i <= 1; i ++ )
			{
				// Setup neccessary values
				lv.iSubItem   = i;
				lv.cchTextMax = 256;

				// Setup text
				string str = "";

				if ( i == 0 )
					str = CTT <LPVOID> ( (void*) dwAddr );
				else if ( i == 1 )
					str = CTT <DWORD> ( Value );

				// Add item
				lv.pszText = (LPSTR) str.c_str ();

				// Insert subitem
				ListView_SetItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );
			}
		}
		// 8 Bytes
		if ( !strcmp ( szType, "8 Bytes" ) )
		{
			// R value
			UINT64 Value = RV <UINT64> ( dwAddr );

			// Get Item
			int item = ListView_GetItemCount ( GetDlgItem ( hWndDlg, IDC_LIST1 ) );

			// Setup item
			LV_ITEM lv;
			lv.iItem    = item;           
			lv.iSubItem = 0;
			lv.mask     = LVIF_TEXT;       
			lv.pszText  = "";
			lv.cchTextMax = 256; 

			// Insert item
			ListView_InsertItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );

			// Fix subitems
			for ( int i = 0; i <= 1; i ++ )
			{
				// Setup neccessary values
				lv.iSubItem   = i;
				lv.cchTextMax = 256;

				// Setup text
				string str = "";

				if ( i == 0 )
					str = CTT <LPVOID> ( (void*) dwAddr );
				else if ( i == 1 )
					str = CTT <UINT64> ( Value );

				// Add item
				lv.pszText = (LPSTR) str.c_str ();

				// Insert subitem
				ListView_SetItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );
			}
		}
		// Float
		if ( !strcmp ( szType, "Float" ) )
		{
			// R value
			float Value = RV <float> ( dwAddr );

			// Get Item
			int item = ListView_GetItemCount ( GetDlgItem ( hWndDlg, IDC_LIST1 ) );

			// Setup item
			LV_ITEM lv;
			lv.iItem    = item;           
			lv.iSubItem = 0;
			lv.mask     = LVIF_TEXT;       
			lv.pszText  = "";
			lv.cchTextMax = 256; 

			// Insert item
			ListView_InsertItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );

			// Fix subitems
			for ( int i = 0; i <= 1; i ++ )
			{
				// Setup neccessary values
				lv.iSubItem   = i;
				lv.cchTextMax = 256;

				// Setup text
				string str = "";

				if ( i == 0 )
					str = CTT <LPVOID> ( (void*) dwAddr );
				else if ( i == 1 )
					str = CTT <float> ( Value );

				// Add item
				lv.pszText = (LPSTR) str.c_str ();

				// Insert subitem
				ListView_SetItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );
			}
		}
		// Double
		if ( !strcmp ( szType, "Double" ) )
		{
			// R value
			double Value = RV <double> ( dwAddr );

			// Get Item
			int item = ListView_GetItemCount ( GetDlgItem ( hWndDlg, IDC_LIST1 ) );

			// Setup item
			LV_ITEM lv;
			lv.iItem    = item;           
			lv.iSubItem = 0;
			lv.mask     = LVIF_TEXT;       
			lv.pszText  = "";
			lv.cchTextMax = 256; 

			// Insert item
			ListView_InsertItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );

			// Fix subitems
			for ( int i = 0; i <= 1; i ++ )
			{
				// Setup neccessary values
				lv.iSubItem   = i;
				lv.cchTextMax = 256;

				// Setup text
				string str = "";

				if ( i == 0 )
					str = CTT <LPVOID> ( (void*) dwAddr );
				else if ( i == 1 )
					str = CTT <double> ( Value );

				// Add item
				lv.pszText = (LPSTR) str.c_str ();

				// Insert subitem
				ListView_SetItem ( GetDlgItem ( hWndDlg, IDC_LIST1 ), &lv );
			}
		}
	}

	DataNotDone = false;
}
//

// Dialog functions
void TryOpen ()
{
	// Create variables
	OPENFILENAME ofn;
	char szBuffer [260];

	// Initialize OPENFILENAME
	ZeroMemory ( &ofn, sizeof(ofn) );

	ofn.lStructSize = sizeof ( ofn );
	ofn.hwndOwner = mainDlg;
	ofn.lpstrFile = szBuffer;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof ( szBuffer );
	ofn.lpstrFilter = ".LPMS File\0*.LPMS*\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = " Select your table ";
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = "C:\\";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Open the dialog
		if ( GetOpenFileName (&ofn) == TRUE )
		{
			// Set the text
			if ( strcmp ( szBuffer, "" ) != 0 )
			{
				ifstream File ( szBuffer );
				
				while ( !File.eof () )
				{
					string Table;
					string Address;
					string Type;

					getline ( File, Table );

					if ( strcmp ( Table.c_str (), "" ) )
					{
						if ( Table.at (0) == 'P' ) 
							Address = Table.substr ( 0, 11 );
						else
							Address = Table.substr ( 0, 8 );

						Type    = Table.substr ( Table.find_first_of ( " " ) + 1, Table.length () );

						IA ( Address, Type );
					}
				}

				File.close ();
			}
		}
	

	DWORD dwExitCode;
	GetExitCodeThread ( GetCurrentThread (), &dwExitCode );
	TerminateThread   ( GetCurrentThread (), dwExitCode  );
}
void TrySave ()
{
	// Create variables
	OPENFILENAME ofn;
	char szBuffer [260];

	// Initialize OPENFILENAME
	ZeroMemory ( &ofn, sizeof(ofn) );
	ofn.lStructSize = sizeof ( ofn );
	ofn.hwndOwner = mainDlg;
	ofn.lpstrFile = szBuffer;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof ( szBuffer );
	ofn.lpstrFilter = ".LPMS File\0*.LPMS*\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = " Select your table ";
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = "C:\\";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Open the dialog
	
		if ( GetSaveFileName (&ofn) == TRUE )
		{
			// Set the text
			if ( strcmp ( szBuffer, "" ) != 0 )
			{		
				string Path;
				Path += szBuffer;
				Path += ".lpms";

				// Setup the file
				ofstream File ( Path.c_str () );
				File.clear ();

				int max = ListView_GetItemCount ( GetDlgItem ( mainDlg, IDC_LIST2 ) );

				for ( int i = 0 ; i < max; i ++ )
				{
					char szAddress [256] = {0};
					char szType    [256] = {0};

					ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), selectedItem, 1, szAddress, 256 );
					ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), selectedItem, 2, szType, 256 );

					File << szAddress << " " << szType << endl;
				}

				// Close the file
				File.close ();
			}
		}

	DWORD dwExitCode;
	GetExitCodeThread ( GetCurrentThread (), &dwExitCode );
	TerminateThread   ( GetCurrentThread (), dwExitCode  );
}
//

// Ending procedures
void CloseThreads ()
{
	__try 
	{
		// Close the threads
		DWORD dwExitCode;
		GetExitCodeThread ( hScan, &dwExitCode );
		TerminateThread   ( hScan, dwExitCode  );
		GetExitCodeThread ( hArray, &dwExitCode );
		TerminateThread   ( hArray, dwExitCode  );

		// Close handles to the files
		CloseHandle ( hAddress );
		CloseHandle ( hValues );
		CloseHandle ( hFile );
		CloseHandle ( hNew );

		// Check if the threads haven't already closed
		if ( !siGone )
		{
			// Terminate the handles to the threads
			for ( int i = 0; i < threadsCreated; i ++ )
			{
				// Remove the thread
				DWORD dwExitCode;
				GetExitCodeThread ( si [ threadsWorked [i] ].hThread, &dwExitCode );
				TerminateThread   ( si [ threadsWorked [i] ].hThread, dwExitCode  );

				// Close the files
				CloseHandle ( si [ threadsWorked [i] ].hFile );
			}

			delete [] threadsWorked;
			delete [] si;

			siGone = true;
		}

		// Delete files
		DeleteFile ( "C:\\Memory.TMP" );
		DeleteFile ( "C:\\Values.TMP" );
		DeleteFile ( "C:\\Memory2.TMP" );
		DeleteFile ( "C:\\Values2.TMP" );
		DeleteFile ( "C:\\OldMemory.TMP" );
		DeleteFile ( "C:\\OldValues.TMP" );
		DeleteFile ( "C:\\Merge2.bat" );		

		delete [] SuccessReg;
	}
	__except ( true ) 
	{
		// Exception hit
	}
}
void EndMain      ()
{
	CloseThreads ();

	// Delete files
	DeleteFile ( "C:\\MemoryScan.iScan" );
	DeleteFile ( "C:\\MemoryText.iScan" );
	DeleteFile ( "C:\\PointerInfo.iScan" );

	// Close other dialogs
	EndDialog ( addrDlg, 0 );
	EndDialog ( editDlg, 0 );
	EndDialog ( ptrDlg, 0 );
	EndDialog ( memDlg, 0 );
	EndDialog ( chgDlg, 0 );
	EndDialog ( optDlg, 0 );
	EndDialog ( dbgDlg, 0 );
	EndDialog ( mdlDlg, 0 );
	EndDialog ( fwDlg, 0 );
	EndDialog ( regDlg, 0 );
	EndDialog ( disDlg, 0 );
	EndDialog ( extraDlg, 0 );
	EndDialog ( settingDlg, 0 );
	EndDialog ( editAllDlg, 0 );
	EndDialog ( dataStruct, 0 );

	// Close other threads
	CloseHandle ( hAddrDlg );
	CloseHandle ( hUpdate  );
	CloseHandle ( hFreeze  );
	CloseHandle ( hEditDlg );
	CloseHandle ( hPtrDlg );
	CloseHandle ( hMemDlg );
	CloseHandle ( hEditAll );
	CloseHandle ( hDataStruct );
	CloseHandle ( hHotkeys );

	// Close the dialog
	EndDialog ( mainDlg, 0 );

	// Close the threads
	DWORD dwExitCode;
	GetExitCodeThread ( hMemScan, &dwExitCode	 );
	TerminateThread   ( hMemScan, dwExitCode	 );
	GetExitCodeThread ( hMemDlg, &dwExitCode	 );
	TerminateThread   ( hMemDlg, dwExitCode		 );
	GetExitCodeThread ( hPtrScan, &dwExitCode	 );
	TerminateThread   ( hPtrScan, dwExitCode	 );
	GetExitCodeThread ( hPtrDlg, &dwExitCode	 );
	TerminateThread   ( hPtrDlg, dwExitCode		 );
	GetExitCodeThread ( hEditDlg, &dwExitCode	 );
	TerminateThread   ( hEditDlg, dwExitCode	 );
	GetExitCodeThread ( hAddrDlg, &dwExitCode	 );
	TerminateThread   ( hAddrDlg, dwExitCode	 );
	GetExitCodeThread ( hChgDlg, &dwExitCode	 );
	TerminateThread   ( hChgDlg, dwExitCode		 );
	GetExitCodeThread ( hPopup, &dwExitCode		 );
	TerminateThread   ( hPopup, dwExitCode		 );
	GetExitCodeThread ( hNoBug, &dwExitCode		 );
	TerminateThread   ( hNoBug, dwExitCode		 );
	GetExitCodeThread ( hUpdate, &dwExitCode	 );
	TerminateThread   ( hUpdate, dwExitCode		 );
	GetExitCodeThread ( hFreeze, &dwExitCode	 );
	TerminateThread   ( hFreeze, dwExitCode      );
	GetExitCodeThread ( hOptDlg, &dwExitCode	 );
	TerminateThread   ( hOptDlg, dwExitCode      );
	GetExitCodeThread ( hDbgDlg, &dwExitCode	 );
	TerminateThread   ( hDbgDlg, dwExitCode      );
	GetExitCodeThread ( hMdlDlg, &dwExitCode	 );
	TerminateThread   ( hMdlDlg, dwExitCode      );
	GetExitCodeThread ( hDmp, &dwExitCode		 );
	TerminateThread   ( hDmp, dwExitCode		 );
	GetExitCodeThread ( hDmp2, &dwExitCode		 );
	TerminateThread   ( hDmp2, dwExitCode		 );
	GetExitCodeThread ( hFuncWatch, &dwExitCode	 );
	TerminateThread   ( hFuncWatch, dwExitCode   );
	GetExitCodeThread ( hRegion, &dwExitCode	 );
	TerminateThread   ( hRegion, dwExitCode		 );
	GetExitCodeThread ( hDis, &dwExitCode	     );
	TerminateThread   ( hDis, dwExitCode		 );
	GetExitCodeThread ( hDisassemble, &dwExitCode );
	TerminateThread   ( hDisassemble, dwExitCode  );
	GetExitCodeThread ( hExtra,		  &dwExitCode );
	TerminateThread   ( hExtra,		  dwExitCode  );
	GetExitCodeThread ( hPointers,	  &dwExitCode );
	TerminateThread   ( hPointers,	  dwExitCode  );
	GetExitCodeThread ( hEditAll,	  &dwExitCode );
	TerminateThread   ( hEditAll,	  dwExitCode  );
	GetExitCodeThread ( hDataStruct,  &dwExitCode );
	TerminateThread   ( hDataStruct,  dwExitCode  );
	GetExitCodeThread ( hHotkeys,  &dwExitCode );
	TerminateThread   ( hHotkeys,  dwExitCode  );
	GetExitCodeThread ( hMainThread, &dwExitCode );
	TerminateThread   ( hMainThread, dwExitCode  );
}
//

// Dialog Procedures
INT_PTR CALLBACK MainDialog  ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	mainDlg = hWndDlg;

	// Handle messages
	switch ( uMsg )
	{
	// ListView Handler
	case WM_NOTIFY:
		{
			switch ( ( (LPNMHDR) lParam )->code )
			{
			case NM_DBLCLK:
				{
					selectedItem = SMX ( GetDlgItem ( mainDlg, IDC_LIST2 ), LVM_GETNEXTITEM, -1, LVNI_SELECTED );

					char szBuffer [256] = {0};
					ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), selectedItem, 1, szBuffer, 256 );

					if ( strcmp ( szBuffer, "" ) != 0 )
						hEditDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initEditDlg, NULL, NULL, NULL );
				}
				break;
			}
		}
		return TRUE;

	case WM_INITDIALOG:
		{
			// Setup dialog settings
			MainProc ();
		}
		return TRUE;
		
	case WM_COMMAND:
		{
			// ListBox Handler
			switch ( HIWORD (wParam) )
			{
			case LBN_DBLCLK:
				{
					switch (LOWORD (wParam))
					{
					case IDC_LIST1:
						{
							// Obtain the address
							int i = SMX ( GetDlgItem ( hWndDlg, IDC_LIST1 ), LB_GETCURSEL, 0, 0 );

							char szBuffer [256] = {0};
							SMX ( GetDlgItem ( hWndDlg, IDC_LIST1 ), LB_GETTEXT, (WPARAM) i, (LPARAM) (LPTSTR) szBuffer );

							if ( strcmp ( szBuffer, "" ) != 0 )
							{
								// Obtain the type
								char szType [256] = {0};
								GetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO1 ), (LPSTR) szType, 256 );

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
						break;
					}
				}
				break;
			}

			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					if ( !MaxThreads )
						MessageBoxA ( hWndDlg, "Please check your settings.", "Error", MB_ICONINFORMATION );
					else
					{
						// See if the value is hex
						bool hex    = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;

						// Get text
						char szUnknown [256] = {0};
						GetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO2 ), (LPSTR) szUnknown, 256 );
						
						// Set 'unknownScan'
						unknownScan = !strcmp ( szUnknown, "Unknown" ) ? true : false;

						// Obtain the ST
						char szST [256] = {0};
						GetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO1 ), szST, 256 );

						// Check if the user has entered a value
						char szValue [256] = {0};
						GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szValue, 256 );

						if ( ( hex ) && 
							 ( ( !strcmp ( szST, "Float"  ) ) || 
							   ( !strcmp ( szST, "Double" ) ) ) )
						{
								MessageBoxA ( hWndDlg, "Please enter decimal numbers for float and double.", "Error", MB_ICONINFORMATION );
								SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX1 ), BM_SETCHECK, (WPARAM) BST_UNCHECKED, NULL );
						}
						else
						{
							if ( !strcmp ( szValue, "" ) && unknownScan == false )
								MessageBoxA (hWndDlg, "Please enter a valid value.", "Error", MB_ICONINFORMATION);
							else {
								scanNum      = -1;
								result		 = 0;
								oldResult	 = 0;
								unknownScan  = false;
								ArrayOfBytes = false;

								// Disable the Edit Control
								Edit_Enable ( GetDlgItem ( hWndDlg, IDC_EDIT2 ), FALSE );
								Edit_Enable ( GetDlgItem ( hWndDlg, IDC_EDIT3 ), FALSE );

								// Disable next scan
								Button_Enable ( GetDlgItem ( mainDlg, IDC_BUTTON2 ), FALSE );

								// Obtain the range of the scan
								dwStartScan = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );
								dwStopScan  = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT3 ) );		

								// Obtain the scan type
								char szScan [256] = {0};
								GetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO2 ), szScan, 256 );

								oldScan = new char [256];
								GetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO2 ), oldScan, 256 ); 

								if ( !strcmp ( szScan, "Array of Bytes" ) || !strcmp ( szScan, "Text" ) )
								{
									// Set array of bytes to true
									ArrayOfBytes = true;

									// Create the thread
									hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GE <BYTE>,   NULL, NULL, NULL );
								}
								else if ( !strcmp ( szST, "Byte" ) )
									hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GE <BYTE>,   NULL, NULL, NULL );
								else if ( !strcmp ( szST, "2 Bytes" ) )
									hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GE <WORD>,   NULL, NULL, NULL );
								else if ( !strcmp ( szST, "4 Bytes" ) )
									hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GE <DWORD>,  NULL, NULL, NULL );
								else if ( !strcmp ( szST, "8 Bytes" ) )
									hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GE <UINT64>, NULL, NULL, NULL );
								else if ( !strcmp ( szST, "Float" ) )
									hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GE <float>,  NULL, NULL, NULL );
								else if ( !strcmp ( szST, "Double" ) )
									hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GE <double>, NULL, NULL, NULL );

								// Show the New Scan button, hide First Scan
								ShowWindow ( GetDlgItem ( hWndDlg, IDC_BUTTON5 ), SW_SHOW );
								ShowWindow ( GetDlgItem ( hWndDlg, IDC_BUTTON1 ), SW_HIDE );

								// Enable New Scan
								Button_Enable ( GetDlgItem ( hWndDlg, IDC_BUTTON5 ), TRUE );

								// Disable changing scan type
								ComboBox_Enable ( GetDlgItem ( hWndDlg, IDC_COMBO1 ), FALSE );
							}
						}
					}
					break;

				case IDC_BUTTON2:
					{
						// Check if the user has entered a value
						char szValue [256] = {0};
						GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szValue, 256 );

						if ( !strcmp ( szValue, "" ) )
							MessageBoxA (hWndDlg, "Please enter a valid value.", "Error", MB_ICONINFORMATION);
						else {
							// Disable buttons to prevent crash
							Button_Enable ( GetDlgItem ( hWndDlg, IDC_BUTTON5 ), TRUE );
							Button_Enable ( GetDlgItem ( hWndDlg, IDC_BUTTON2 ), FALSE );
							Button_Enable ( GetDlgItem ( hWndDlg, IDC_BUTTON8 ), FALSE );

							// Obtain the type
							char szST [256] = {0};
							GetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO1 ), szST, 256 );

							// Obtain the scan type
							char szScan [256] = {0};
							GetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO2 ), szScan, 256 );

							// See if the value is hex
							bool hex = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;

							if ( !strcmp ( szScan, "Array of Bytes" ) || !strcmp ( szScan, "Text" ) )
							{
								ArrayOfBytes = true;
								hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GN <BYTE>, NULL, NULL, NULL );
							}
							else if ( !strcmp ( szST, "Byte" ) )
								hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GN <BYTE>, NULL, NULL, NULL );
							else if ( !strcmp ( szST, "2 Bytes" ) )			
								hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GN <WORD>, NULL, NULL, NULL );	
							else if ( !strcmp ( szST, "4 Bytes" ) )
								hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GN <DWORD>, NULL, NULL, NULL );
							else if ( !strcmp ( szST, "8 Bytes" ) )
								hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GN <UINT64>, NULL, NULL, NULL );
							else if ( !strcmp ( szST, "Float" ) )
							{					
								if ( !hex ) 
								{
									hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GN <float>, NULL, NULL, NULL );
								}
								else
									MessageBoxA ( 0, "Hex values of float and double are not supported!", "Error", MB_ICONINFORMATION );
							}
							else if ( !strcmp ( szST, "Double" ) )
							{								
								if ( !hex ) 
								{		
									// Create SCAN_INFO
									hScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GN <double>, NULL, NULL, NULL );
								}
								else
									MessageBoxA ( 0, "Hex values of float and double are not supported!", "Error", MB_ICONINFORMATION );
							}
						}
					}					
				}
				break;

			case IDC_BUTTON3:
				{
					// Initialize the dialog
					hMemDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initMemDlg, NULL, NULL, NULL );
				}
				break;

			case IDC_BUTTON4:
				{
					// Clear the items
					ListView_DeleteAllItems ( GetDlgItem ( mainDlg, IDC_LIST2 ) );

					ptrCountTwo = 0;
				}
				break;

			case IDC_BUTTON5:
				{
					if ( !Merging )
					{
						CloseThreads ();

						// Notify the user that we're done merging the files
						SetWindowText ( GetDlgItem ( mainDlg, IDC_PROGRESS ), "" );
						ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS ), SW_HIDE );

						// Hide progress bar
						ShowWindow ( GetDlgItem ( mainDlg, IDC_PROGRESS1 ), SW_HIDE );

						// Clear the listbox
						SMX ( GetDlgItem ( mainDlg, IDC_LIST1 ), LB_RESETCONTENT, 0, 0 );

						// Reset array of bytes
						ArrayOfBytes = false;
						siGone       = true;
						unknownNumber = 0;

						// Obtain the type
						char szST [256] = {0};
						GetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO1 ), szST, 256 );

						// Reset items
						SMX ( GetDlgItem ( hWndDlg, IDC_COMBO2 ), CB_RESETCONTENT, 0, 0 );

						// Add items to combo
						for ( int index = 0; index < _countof ( NSN ); index ++ )
							SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, 0, (LPARAM) NSN [index] );

						SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, 0, (LPARAM) "Array of Bytes" );
						SMX ( GetDlgItem ( mainDlg, IDC_COMBO2 ), CB_ADDSTRING, 0, (LPARAM) "Text" );

						// Set Window Name
						SetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO2 ), oldScan );
						delete [] oldScan;

						// Reset variables
						scanNum     = -1;
						unknownScan = false;
						result      = 0;

						// Enable changing scan type
						ComboBox_Enable ( GetDlgItem ( hWndDlg, IDC_COMBO1 ), TRUE );

						// Disable next scan
						Button_Enable ( GetDlgItem ( hWndDlg, IDC_BUTTON2 ), FALSE );

						// Disable undo scan
						Button_Enable ( GetDlgItem ( hWndDlg, IDC_BUTTON8 ), FALSE );

						// Show First Scan
						ShowWindow ( GetDlgItem ( hWndDlg, IDC_BUTTON1 ), SW_SHOW );

						// Hide New Scan
						ShowWindow ( GetDlgItem ( hWndDlg, IDC_BUTTON5 ), SW_HIDE );

						// Clear the search 
						ListView_DeleteAllItems ( GetDlgItem ( hWndDlg, IDC_LIST1 ) ); 

						// Enable the Edit Control
						Edit_Enable ( GetDlgItem ( hWndDlg, IDC_EDIT2 ), TRUE );
						Edit_Enable ( GetDlgItem ( hWndDlg, IDC_EDIT3 ), TRUE );

						// Set results back to 0
						SetWindowText ( GetDlgItem ( hWndDlg, IDC_RESULT ), "0" );
					}
					else
						MessageBoxA ( hWndDlg, "Please wait for merging to finish:\n\n- Some scans require a long merging duration\n- If merging is taking too long, visit the site", "Read", MB_ICONINFORMATION );
				}
				break;

			case IDC_BUTTON6:
				{
					// Setup handle for thR
					hAddrDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initAddrDlg, NULL, NULL, NULL );
				}
				break;

			case IDC_BUTTON7:
				{
					if ( result > 50000 )
						MessageBoxA ( mainDlg, "Can't display results larger than 50,000", "Error", MB_ICONERROR );
					else
					{
						HWND hWnd = GetDlgItem ( hWndDlg, IDC_LIST1 );
						SMX ( GetDlgItem ( hWndDlg, IDC_LIST1 ), LB_RESETCONTENT, NULL, NULL );

						// Display all results
						if ( scanNum == -1 )
							MessageBoxA ( hWndDlg, "Please preform a scan.", "Error", MB_ICONINFORMATION );
						else
						{
							// Obtain results
							HANDLE hFile = CreateFile ( GetPathway ( "Memory.TMP" ).c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
							dwScan	     = new DWORD [ (DWORD) result * 4 ];

							// Read the reuslts
							DWORD dwBytes;
							::ReadFile ( hFile, dwScan, (DWORD) result * 4, &dwBytes, 0 );

							for ( DWORD i = 0; i < result; i ++ )
								IR ( hWnd, dwScan [i] );

							// Close the handle
							CloseHandle ( hFile );

							// Delete the buffer
							delete [] dwScan;
						}
					}
				}
				break;

			case IDC_BUTTON8:
				{
					// Set old result #
					SetWindowText ( GetDlgItem ( hWndDlg, IDC_RESULT ), CTT ( oldResult ).c_str () );
					result = oldResult;

					// Swap buffers
					DeleteFile ( GetPathway ( "Memory.TMP" ).c_str () );
					MoveFileEx ( GetPathway ( "OldMemory.TMP" ).c_str (), GetPathway ( "Memory.TMP" ).c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED );

					DeleteFile ( GetPathway ( "Values.TMP" ).c_str () );
					MoveFileEx (GetPathway ( "OldValues.TMP" ).c_str (), GetPathway ( "Values.TMP" ).c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED );

					// Clear the list
					SMX ( GetDlgItem ( mainDlg, IDC_LIST1 ), LB_RESETCONTENT, NULL, NULL );

					// Reset 'scanNum'
					if ( scanNum == 1 )
					{
						scanNum = 0;
						Button_Enable ( GetDlgItem ( mainDlg, IDC_BUTTON8 ), FALSE );
					}
					else
						scanNum --;

					unknownNumber --;

					// Beep
					MakeSound ();
				}
				break;

				case IDC_BUTTON9:
				{
					// Obtain the selected item
					selectedItem = SMX ( GetDlgItem ( mainDlg, IDC_LIST2 ), LVM_GETNEXTITEM, -1, LVNI_SELECTED );
					
					int Target = 0;

					for ( int i = 0; i < ptrCountTwo; i ++ )
						if ( ptrInfoTwo [i].index == selectedItem )
							ptrInfoTwo [i].index = -1;

					// Delete it
					ListView_DeleteItem ( GetDlgItem ( mainDlg, IDC_LIST2 ), selectedItem );
				}
				break;

				case IDC_BUTTON10:
				{
					hSettings = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initSettings, NULL, NULL, NULL );
				}
				break;

				case IDC_BUTTON11:
				{
					CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) TryOpen, NULL, NULL, NULL );
				}
				break;

				case IDC_BUTTON12:
				{
					CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) TrySave, NULL, NULL, NULL );
				}
				break;

				case IDC_BUTTON13:
				{
					hMemDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initPointer, NULL, NULL, NULL );
				}
				break;

				case IDC_BUTTON14:
				{
					AddAllItems ( false );
				}
				break;

				case IDC_BUTTON15:
				{
					AddAllItems ( true );
				}
				break;

				case IDC_BUTTON16:
				{
					hEditAll = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initAll, NULL, NULL, NULL );
				}
				break;
			}			
		}
		return TRUE;

	case WM_CLOSE:
		{
			// Call the ending procedures
			EndMain ();
		}
		return TRUE;

	default:
		return FALSE;
	}
}
INT_PTR CALLBACK AddrDialog  ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Setup hWnd
	addrDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					// Obtain the string
					char szBuffer [256] = {0};
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szBuffer, 256 );
					string Address;
					Address += szBuffer;

					if ( Address.find_first_of ( "+" ) == -1 )
					{
						// Check if hex
						bool Hex = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX1 ), BM_GETSTATE, 0, 0 ) == BST_CHECKED ? true : false;

						// Obtain the address
						DWORD dwValue = 0;

						if ( Hex )
							dwValue = ConvertTextTo2 <LPVOID, DWORD> ( Address );
						else
							dwValue = ConvertTextTo2 <DWORD, DWORD>  ( Address );

						// Obtain the address
						Address = CTT <LPVOID> ( (LPVOID) dwValue );
					}
					else
					{
						int plus = Address.find_first_of ( "+" );
						string Base = "";

						string test = "";
						test += Address.at ( plus - 1 );

						if ( !strcmp ( test.c_str (), " " ) )
							Base = Address.substr ( 0, plus - 1 );
						else if ( Address.at ( plus - 1 ) == '"' )
							Base = Address.substr ( 1, plus - 2 );
						else
							Base = Address.substr ( 0, plus );

						test  = "";
						test += Address.at ( plus + 1 );

						stringstream ss;

						if ( !strcmp ( test.c_str (), " " ) )
							ss << Address.substr ( plus + 2, Address.length () );
						else
							ss << Address.substr ( plus + 1, Address.length () );

						LPVOID lpAddr;
						ss >> lpAddr;

						HANDLE hModule = GetModuleHandle ( Base.c_str () );

						LPVOID finalAddr = (void*) ( (DWORD) hModule + (DWORD) lpAddr );

						stringstream ss2;
						ss2 << finalAddr;
						Address = "";
						Address += ss2.str ();
					}

					if ( SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX2 ), BM_GETSTATE, 0, 0 ) == BST_CHECKED )
					{
						string dummy = Address;
						Address = "P->";
						Address += dummy;
					}

					// Obtain the type
					char szType [256] = {0};
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO1 ), (LPSTR) szType, 256 );

					// Convert it to string
					string ST;
					ST += szType;

					// Insert the items
					IA ( Address, ST );

					// Close the window
					EndDialog ( hWndDlg, 0 );
				}
				break;
			}

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			AddrProc ();

			return TRUE;
		}

	case WM_CLOSE:
		{
			CloseHandle ( hAddrDlg );
			EndDialog   ( hWndDlg, 0 );
			return TRUE;
		}

	default:
		return FALSE;
	}
}
INT_PTR CALLBACK EditDialog  ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Setup hWnd
	editDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					// Obtain type
					char szBuffer [256] = {0};
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO1 ), (LPSTR) szBuffer, 256 );

					// Check if the value changed
					char szNewValue [256] = {0};
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT2 ), (LPSTR) szNewValue, 256 );

					// Obtain hex value option
					bool hex = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX2 ), BM_GETCHECK, 0, 0 ) == 1 ? true : false;

					if ( ( hex ) && ( ( !strcmp ( szBuffer, "Float" ) ) || ( !strcmp ( szBuffer, "Double" ) ) ) )
						MessageBoxA ( hWndDlg, "Hex-value support for float and double is not supported!", "Error", MB_ICONINFORMATION );
					else
					{
						// Neccessary variables
						DWORD dwLocked;

						// Obtain locked value
						hex = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX3 ), BM_GETCHECK, 0, 0 ) == 1 ? true : false;

						if ( hex )
							dwLocked = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT3 ) );
						else
							dwLocked = ConvertTextTo <DWORD, DWORD>  ( GetDlgItem ( hWndDlg, IDC_EDIT3 ) );

						// Check if hex option for value is valid
						hex = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX2 ), BM_GETCHECK, 0, 0 ) == 1 ? true : false;		

						if ( strcmp ( szCurrentValue, szNewValue  ) != 0 )
						{
							// Byte
							if ( !strcmp ( szBuffer, "Byte" ) )
							{
								BYTE Value = 0;

								// Obtain the hex value
								if ( hex )
									Value =	ConvertTextTo <LPVOID, BYTE> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );
								else
									Value = ConvertTextTo <BYTE, BYTE> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );

								WV <BYTE> (dwPtrAddr, Value );
							}
							// 2 Bytes
							if ( !strcmp ( szBuffer, "2 Bytes" ) )
							{
								WORD Value = 0;

								// Obtain the hex value
								if ( hex )
									Value =	ConvertTextTo <LPVOID, WORD> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );
								else
									Value = ConvertTextTo <WORD, WORD> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );

								WV <WORD> (dwPtrAddr, Value );
							}
							// 4 Bytes
							if ( !strcmp ( szBuffer, "4 Bytes" ) )
							{
								DWORD Value = 0;

								// Obtain the hex value
								if ( hex )
									Value =	ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );
								else
									Value = ConvertTextTo <DWORD, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );

								WV <DWORD> (dwPtrAddr, Value );
							}
							// 8 Bytes
							if ( !strcmp ( szBuffer, "8 Bytes" ) )
							{
								UINT64 Value = 0;

								// Obtain the hex value
								if ( hex )
									Value =	ConvertTextTo <LPVOID, UINT64> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );
								else
									Value = ConvertTextTo <UINT64, UINT64> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );

								WV <UINT64> (dwPtrAddr, Value );
							}
							// Float
							if ( !strcmp ( szBuffer, "Float" ) )
							{
								float Value = ConvertTextTo <float, float> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );
								WV <float> (dwPtrAddr, Value );
							}
							// Double
							if ( !strcmp ( szBuffer, "Double" ) )
							{
								double Value = ConvertTextTo <double, double> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );
								WV <double> (dwPtrAddr, Value );
							}
							// Text
							if ( !strcmp ( szBuffer, "Text" ) )
							{
								// Text
							}
						}

						// Setup string
						string addr;
						bool hexOn = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;

						// Obtain the address
						char szAddr [256] = {0};
						GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szAddr, 256 );

						if ( !hexOn ) {
							DWORD dwVal = ConvertTextTo <DWORD, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT1 ) );
							addr += CTT <LPVOID> ( (LPVOID) dwVal );
						}
						else {
							addr += szAddr; 
						}

						// Set the address
						ListView_SetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), selectedItem, 1, (LPSTR) addr.c_str () );

						// Set the locked
						string locked = CTT ( dwLocked );

						// Set the locked
						ListView_SetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), selectedItem, 4, (LPSTR) locked.c_str () );

						// Set the type
						ListView_SetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), selectedItem, 2, szBuffer );

						// Close
						EndDialog ( hWndDlg, 0 );
					}
				}
				break;

			case IDC_BUTTON4:
				{
					Set = true;

					// Check if the address is in hex format
					bool hex = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;

					if ( !hex )
					{
						MS1 = ConvertTextTo <DWORD, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT1 ) );
						MS2 = ConvertTextTo <DWORD, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT1 ) );
					}
					else {
						MS1 = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT1 ) );
						MS2 = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT1 ) );
					}

					// Create the thread
					hMemDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initMemDlg, NULL, NULL, NULL );

					EndDialog ( hWndDlg, 0 );
				}
				break;
			}

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			// Call main procedures
			EditProc ();

			return TRUE;
		}

	case WM_CLOSE:
		{
			CloseHandle ( hEditDlg );
			EndDialog   ( hWndDlg, 0 );
			return TRUE;
		}

	default:
		return FALSE;
	}
}
INT_PTR CALLBACK PtrDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Setup window
	ptrDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			// ListBox Handler
			switch ( HIWORD (wParam) )
			{
			case LBN_DBLCLK:
				{
					switch (LOWORD (wParam))
					{
					case IDC_LIST1:
						{
							// Obtain the address
							int i = SMX ( GetDlgItem ( ptrDlg, IDC_LIST1 ), LB_GETCURSEL, 0, 0 );

							char szBuffer [256] = {0};
							SMX ( GetDlgItem ( ptrDlg, IDC_LIST1 ), LB_GETTEXT, (WPARAM) i, (LPARAM) (LPTSTR) szBuffer );

							if ( strcmp ( szBuffer, "" ) != 0 )
							{
								// To string
								string ptr;
								ptr += (LPSTR) szBuffer;

								string type = "4 Bytes";

								// Insert
								IA ( ptr, type );
							}
						}
						break;
					}
				}
				break;
			}

			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					// Obtain the range
					char szBuffer [256] = {0};
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szBuffer, 256 );

					if ( !strcmp ( szBuffer, "" ) )
						MessageBoxA ( hWndDlg, "Invalid range value", "Error", MB_ICONINFORMATION );
					else
					{
						// Disable the scan button
						Button_Enable ( GetDlgItem ( ptrDlg, IDC_BUTTON1 ), FALSE );

						// Obtain the range of the scan
						dwStartScan = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( mainDlg, IDC_EDIT2 ) );
						dwStopScan  = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( mainDlg, IDC_EDIT3 ) );	

						// Set the scanning option
						SetWindowText ( GetDlgItem ( hWndDlg, IDC_RESULT2 ), "Scanning!" );

						// Obtain range
						DWORD dwRange = 0;

						// Check if hex is valid
						bool Hex = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == 1 ? true : false;

						if ( Hex )
							dwRange = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT1 ) );
						else
							dwRange = ConvertTextTo <DWORD, DWORD>  ( GetDlgItem ( hWndDlg, IDC_EDIT1 ) );

						// Obtain static option
						bool Static = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX2 ), BM_GETCHECK, 0, 0 ) == 1 ? true : false;

						// Setup information
						ptrInfo.dwAddress = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( hWndDlg, IDC_ADDRESS ) );
						ptrInfo.dwRange   = dwRange;
						ptrInfo.GoStatic  = Static;

						// Call thread
						hPtrScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GP, NULL, NULL, NULL );
					}
				}
				break;

			case IDC_BUTTON2:
				{
					// Close the handle
					DWORD dwExitCode;
					GetExitCodeThread ( hPtrScan, &dwExitCode );
					TerminateThread   ( hPtrScan, dwExitCode  );

					// Set the results
					SetWindowText ( GetDlgItem ( ptrDlg, IDC_RESULT2 ), CTT ( PointerResult ).c_str () );

					// Beep
					MakeSound ();

					// Enable scan button
					Button_Enable ( GetDlgItem ( ptrDlg, IDC_BUTTON1 ), TRUE );
				}
				break;
			}

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			// Call setup procedures
			PtrProc ();

			return TRUE;
		}

	case WM_CLOSE:
		{

			// Close the dialog
			EndDialog ( hWndDlg, 0 );

			return TRUE;
		}
		
	default:
		return FALSE;
	}
}
INT_PTR CALLBACK MemDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Set window handle
	memDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( HIWORD ( wParam ) )
			{
			case LBN_DBLCLK:
				{
					switch ( LOWORD ( wParam ) )
					{
					case IDC_LIST1:
						{
							// Create the thread
							hChgDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initChgDlg, NULL, NULL, NULL );
						}
						break;
					}
				}
				break;
			}

			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					// Get the regions
					char szStart [256] = {0};
					char szStop  [256] = {0};

					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szStart, 256 );
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT2 ), (LPSTR) szStop, 256  );

					// Start
					if ( !strcmp ( szStart, "" ) || !strcmp ( szStop, "" ) )
						MessageBoxA ( hWndDlg, "You must enter addresses to scan!", "Error", MB_ICONINFORMATION );
					else
					{
						// Setup string values
						string str1, str2;
						str1 += szStart;
						str2 += szStop;

						// Setup the addresses
						dwMS1 = ConvertTextTo2 <LPVOID, DWORD> ( str1 );
						dwMS2 = ConvertTextTo2 <LPVOID, DWORD> ( str2 );

						// Start the scan
						hMemScan = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) GM2, NULL, NULL, NULL );
					}
				}
				break;

			case IDC_BUTTON2:
				{
					// Create the thread
					hChgDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initChgDlg, NULL, NULL, NULL );
				}
				break;

			case IDC_BUTTON4:
				{
					// Create the thread
					hOptDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initOptDlg, NULL, NULL, NULL );

					// End the dialog
					EndDialog ( hWndDlg, 0 );
				}
				break;

			case IDC_BUTTON5:
				{
					// Create the thread
					hDbgDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initDbgDlg, NULL, NULL, NULL );

					// End the dialog
					EndDialog ( hWndDlg, 0 );
				}
				break;
			}

			return TRUE;
		}

	case WM_INITDIALOG: 
		{
			// Call main procedures
			MemProc ();

			return TRUE;
		}

	case WM_CLOSE:
		{
			// Close the handle
			DWORD dwExitCode;
			GetExitCodeThread ( hMemScan, &dwExitCode );
			TerminateThread   ( hMemScan, dwExitCode  );

			// End the dialog
			EndDialog ( hWndDlg, 0 );

			return TRUE;
		}

	default:
		return FALSE;
	}
}
INT_PTR CALLBACK ChgDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Set the hWnd
	chgDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					// Obtain the address
					char szAddress [256] = {0};
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szAddress, 256 );

					// Obtain the array
					char szBuffer [256] = {0};
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT2 ), (LPSTR) szBuffer, 256 );
					
					// Check if values are okay
					if ( !strcmp ( szAddress, "" ) || !strcmp ( szBuffer, "" ) )
						MessageBoxA ( hWndDlg, "Please fill in the blank(s)", "Error", MB_ICONINFORMATION );
					else
					{	
						// Check if text is enabled
						bool text = SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false;

						// Create our variable
						string test;

						if ( !text ) {
							test += szBuffer;
						}
						else {
							string buffer;
							buffer += szBuffer;		
							test   += CTB ( buffer );
						}

						BYTE * myBytes;
						SIZE_T numOfBytes = 0;

						// Obtain the number of bytes
						for ( SIZE_T i = 0; i < test.length (); i ++ )
						{
							// Avoid an exception
							if ( i + 1 < test.length () )
							{
								// If its not a space
								if ( test.at (i) != ' ' )
								{
									// If you can form a two letter word
									if ( test.at ( i + 1 ) != ' ' )
									{
										++ numOfBytes;
									}
								}
							}
						}

						// Setup the variable
						myBytes      = new BYTE [ numOfBytes ];
						SIZE_T Dummy = 0;
						
						// Create the bytes
						for ( SIZE_T i = 0; i < test.length (); i ++ )
						{
							// Avoid an exception
							if ( i + 1 < test.length () )
							{
								// If its not a space
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
										myBytes [Dummy] = (unsigned int) lpVal;
										
										++ Dummy;
										++ i;
									}
								}
							}
						}

						// Obtain our address
						LPVOID lpAddress;

						// Convert
						stringstream ss2;
						ss2 << szAddress;

						// Set
						ss2 >> lpAddress;

						// Convert
						DWORD dwAddress = (DWORD) lpAddress;

						// Set the bytes
						for ( SIZE_T i = 0; i < numOfBytes; i ++ )
						{
							// Set the byte
							WV <BYTE> ( dwAddress, myBytes [i] );

							// Increment
							++ dwAddress;
						}
					}

					// Close
					EndDialog ( hWndDlg, 0 );
				}
			}

			return TRUE;
		}
		
	case WM_CLOSE:
		{
			// Close the window
			EndDialog ( hWndDlg, 0 );

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			// Call starting procedures
			ChgProc ();

			return TRUE;
		}
		
	default:
		return FALSE;
	}
}
INT_PTR CALLBACK OptDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Set the window
	optDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					// Obtain the file path
					char szBuffer [256] = {0};
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szBuffer, 256 );

					// Load the library
					LoadLibrary ( szBuffer );

					// Close the dialog
					EndDialog ( hWndDlg, 0 );
				}
				break;

			case IDC_BUTTON2:
				{
					// Create variables
					OPENFILENAME ofn = {0};
					char szBuffer [256] = {0};
				
					// Setup variable
					ofn.lStructSize		= sizeof ( ofn );
					ofn.hwndOwner		= hWndDlg;
					ofn.hInstance       = hInstance;
					ofn.lpstrFile       = szBuffer;
					ofn.nMaxFile        = sizeof ( szBuffer );
					ofn.lpstrFilter     = ".DLL File\0*.dll*\0\0";
					ofn.lpstrFileTitle  = " Select DLL to Inject ";
					ofn.Flags			= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					// Open the dialog
					GetOpenFileName ( &ofn );

					// Set the text
					if ( strcmp ( szBuffer, "" ) != 0 )
						SetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szBuffer );
				}
				break;

			case IDC_BUTTON3:
				{
					// Call the thread
					hMdlDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initMdlDlg, NULL, NULL, NULL );

					EndDialog ( hWndDlg, 0 );
				}
				break;

			case IDC_BUTTON4:
				{
					// Call the thread
					hFuncWatch = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initFuncWatch, NULL, NULL, NULL );

					EndDialog ( hWndDlg, 0 );
				}
				break;

			case IDC_BUTTON5:
				{
					// Call the thread
					hRegion = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initRegDlg, NULL, NULL, NULL );
					
					EndDialog ( hWndDlg, 0 );
				}
				break;

			case IDC_BUTTON6:
				{
					// Call the thread
					hDisassemble = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initDisDlg, NULL, NULL, NULL );

					EndDialog ( hWndDlg, 0 );
				}
				break;

			case IDC_BUTTON7:
				{
					// Call the pointer scanner
					hPtrDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initPtrDlg, NULL, NULL, NULL );

					// End the dialog
					EndDialog ( hWndDlg, 0 );
				}
				break;

			case IDC_BUTTON8:
				{
					// Call the pointer scanner
					hDataStruct = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initDataStruct, NULL, NULL, NULL );

					// End the dialog
					EndDialog ( hWndDlg, 0 );
				}
				break;
			}

			return TRUE;
		}

	case WM_CLOSE:
		{
			// Close
			EndDialog ( hWndDlg, 0 );

			return TRUE;
		}

	default: 
			return FALSE;
		
	}
}
INT_PTR CALLBACK DbgDlg      ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Set the hWnd
	dbgDlg = hWndDlg;

	switch ( uMsg )
	{
	// ListView Handler
	case WM_NOTIFY:
		{
			switch ( ( (LPNMHDR) lParam )->code )
			{
			case NM_DBLCLK:
				{
					int Item = SMX ( GetDlgItem ( dbgDlg, IDC_LIST1 ), LVM_GETNEXTITEM, -1, LVNI_SELECTED );

					char szBuffer [256] = {0};
					ListView_GetItemText ( GetDlgItem ( dbgDlg, IDC_LIST1 ), Item, 1, szBuffer, 256 );

					if ( strcmp ( szBuffer, "" ) != 0 )
					{
						string Addr;
						Addr += (LPSTR) szBuffer;

						IA ( Addr, "4 Bytes" );
					}
				}
				break;
			}
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					if ( localBrk )
						DelHandler2 ( pHandler );

					if ( dbgOn )
					{
						// Remove the handler
						DelHandler ( pHandler );
					}

					// Convert it to DWORD
					DWORD dwAddr = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT1 ) );

					// Start debugging
					pHandler = AddHandler ();
					SetBreakpoint ( dwAddr, 0 );

					CreateThread ( 0, 0, (LPTHREAD_START_ROUTINE) DoBreakpoint, NULL, NULL, NULL );
				}
				break;

			case IDC_BUTTON2:
				{
					if ( localBrk )
						DelHandler2 ( pHandler );

					if ( dbgOn )
					{
						// Remove the handler
						DelHandler ( pHandler );
					}

					// Convert it to DWORD
					DWORD dwAddr = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT1 ) );

					// Start debugging
					pHandler = AddHandler ();
					SetBreakpoint ( dwAddr, 1 );
				}
				break;

			case IDC_BUTTON6:
				{
					if ( localBrk )
						DelHandler2 ( pHandler );

					if ( dbgOn )
					{
						// Remove the handler
						DelHandler ( pHandler );
					}

					// Convert it to DWORD
					DWORD dwAddr = ConvertTextTo <LPVOID, DWORD> ( GetDlgItem ( hWndDlg, IDC_EDIT1 ) );

					// Gather information
					dwDbgAddr = dwAddr;
					localBrk  = true;
					dbgOn     = false;

					// Take original byte
					originalByte = RV <BYTE> ( dwAddr );

					// Add Handler
					pHandler = AddHandler ();

					// Set the Int3
					WV <BYTE> ( dwDbgAddr, 0xCC );
				}
				break;

			case IDC_BUTTON3:
				{
					// Remove the handler
					DelHandler ( pHandler );

					// Obtain the selected item
					selectedItem = SMX ( GetDlgItem ( hWndDlg, IDC_LIST1 ), LVM_GETNEXTITEM, -1, LVNI_SELECTED );

					char szBuffer [256] = {0};
					ListView_GetItemText ( GetDlgItem ( hWndDlg, IDC_LIST1 ), selectedItem, 1, szBuffer, 256 );

					if ( !strcmp ( szBuffer, "" ) )
						MessageBoxA ( hWndDlg, "Please select an item.", "Error", MB_ICONINFORMATION );
					else
					{
						delete [] dbgInfo;

						string Address;
						Address += szBuffer;

						// Set MS1, MS2
						MS1 = ConvertTextTo2 <LPVOID, DWORD> ( Address );
						MS2 = ConvertTextTo2 <LPVOID, DWORD> ( Address );

						// Call the thread
						hDisassemble = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initDisDlg, NULL, NULL, NULL );

						EndDialog ( hWndDlg, 0 );
					}					
				}
				break;

			case IDC_BUTTON4:
				{
					if ( localBrk )
						DelHandler2 ( pHandler );

					if ( dbgOn )
						DelHandler ( pHandler );
				}
				break;

			case IDC_BUTTON5:
				{
					int Item = SMX ( GetDlgItem ( dbgDlg, IDC_LIST1 ), LVM_GETNEXTITEM, -1, LVNI_SELECTED );

					if ( Item == -1 )
						MessageBoxA ( hWndDlg, "Please select an item.", "Error", MB_ICONINFORMATION );
					else 
						hExtra = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initExtra, NULL, NULL, NULL );
				}
				break;
			}

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			// Call starting procedures
			DbgProc ();

			return TRUE;
		}

	case WM_CLOSE:
		{
			if ( dbgOn )
			{
				// Remove the handler
				DelHandler ( pHandler );
			}

			delete [] dbgInfo;

			// Close the dialog
			EndDialog ( hWndDlg, 0 );

			return TRUE;
		}

	default:
		return FALSE;
	}
}
INT_PTR CALLBACK ModuleDlg   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Set the hWnd
	mdlDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					GM ();
				}
				break;
			}

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			// Setup procedures
			MdlProc ();

			// Obtain the modules
			GM ();

			return TRUE;
		}

	case WM_CLOSE:
		{
			// Close the dialog
			EndDialog ( hWndDlg, 0 );

			return TRUE;
		}

	default:
		return FALSE;
	}
}
INT_PTR CALLBACK fwDialog    ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	fwDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					// Obtain the information
					char szLib [256] = {0}, szFunc [256] = {0};
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), szLib, 256  );
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT2 ), szFunc, 256 );

					// Set the information to string
					string Library, Function;
					Library  += szLib;
					Function += szFunc;

					// Call the function
					SetHook ( Library, Function );
				}
				break;
			}

			return TRUE;
		}

	case WM_CLOSE:
		{
			EndDialog ( hWndDlg, 0 );

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			FuncProc ();

			return TRUE;
		}

	default:
			return FALSE;
	}
}
INT_PTR CALLBACK regDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	regDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					ScanRegions ();
				}
				break;
			}

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			RegProc ();

			return TRUE;
		}

	case WM_CLOSE:
		{
			EndDialog ( hWndDlg, 0 );
			return TRUE;
		}

	default:
		{
			return FALSE;
		}
	}
}
INT_PTR CALLBACK disDialog   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	disDlg = hWndDlg;

	switch ( uMsg )
	{
	// ListView Handler
	case WM_NOTIFY:
		{
			switch ( ( (LPNMHDR) lParam )->code )
			{
			case NM_DBLCLK:
				{
					selectedItem = SMX ( GetDlgItem ( hWndDlg, IDC_LIST1 ), LVM_GETNEXTITEM, -1, LVNI_SELECTED );

					char szBuffer [256] = {0};
					ListView_GetItemText ( GetDlgItem ( hWndDlg, IDC_LIST1 ), selectedItem, 0, szBuffer, 256 );

					if ( strcmp ( szBuffer, "" ) != 0 )
					{
						string Addr;
						Addr += szBuffer;

						IA ( Addr, "4 Bytes" );
					}
				}
				break;
			}
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					// Call the thread
					hDisassemble = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) Dis, NULL, NULL, NULL );
				}
				break;

			case IDC_BUTTON2:
				{
					int Item = SMX ( GetDlgItem ( hWndDlg, IDC_LIST1 ), LVM_GETNEXTITEM, -1, LVNI_SELECTED );

					if ( Item != -1 )
					{
						char szText [256] = {0};
						ListView_GetItemText ( GetDlgItem ( hWndDlg, IDC_LIST1 ), Item, 0, szText, 256 );

						// Add it to our string
						string Text;
						Text += szText;

						// Grab the address
						stringstream ss;
						ss << Text;
						
						LPVOID lpAddr;
						ss >> lpAddr;

						// Nop it
						DISASM myDisasm;
						myDisasm.EIP = (UIntPtr) lpAddr;

						int length = Disasm ( &myDisasm );

						for ( int i = 0; i < length; i ++ )
							WV <BYTE> ( (DWORD) lpAddr + i, 144 );
					}
				}
				break;

			case IDC_BUTTON3:
				{
					selectedItem = SMX ( GetDlgItem ( hWndDlg, IDC_LIST1 ), LVM_GETNEXTITEM, -1, LVNI_SELECTED );

					char szBuffer [256] = {0};
					ListView_GetItemText ( GetDlgItem ( hWndDlg, IDC_LIST1 ), selectedItem, 0, szBuffer, 256 );

					if ( !strcmp ( szBuffer, "" ) )
						MessageBoxA ( hWndDlg, "Please select an item.", "Error", MB_ICONINFORMATION );
					else
					{
						string Address;
						Address += szBuffer;

						// Set MS1, MS2
						MS1 = ConvertTextTo2 <LPVOID, DWORD> ( Address );
						MS2 = ConvertTextTo2 <LPVOID, DWORD> ( Address );

						// Create the thread
						hMemDlg = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) initMemDlg, NULL, NULL, NULL );
					}
				}
				break;
			}

			return TRUE;
		}

	case WM_CLOSE:
		{
			EndDialog ( hWndDlg, 0 );
			return TRUE;
		}

	case WM_INITDIALOG:
		{
			// Call the main procedures
			DisProc ();

			return TRUE;
		}

	default:
		return FALSE;
	}
}
INT_PTR CALLBACK extraDialog  ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	extraDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		{
			ExtraProc ();

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
INT_PTR CALLBACK settingDialog ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	settingDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					// Setup the settings
					string Pathway = "";
					Pathway += CurDirectory.c_str ();
					Pathway += "\\LPMS.ini";

					int count = 0;

					// Begin reading
					ofstream SettingsAt ( Pathway.c_str () );

					while ( !SettingsAt.eof () )
					{
						switch ( count )
						{
						case 0:
							{
								if ( SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX3 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
									SettingsAt << "TRUE" << endl;
								else
									SettingsAt << "FALSE" << endl;
							}
							break;

						case 1:
							{
								if ( SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX2 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
									SettingsAt << "TRUE" << endl;
								else
									SettingsAt << "FALSE" << endl;
							}
							break;

						case 2:
							{
								if ( SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX1 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
									SettingsAt << "TRUE" << endl;
								else
									SettingsAt << "FALSE" << endl;
							}
							break;

						case 3:
							{
								if ( SMX ( GetDlgItem ( hWndDlg, IDC_CHECKBOX4 ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
									SettingsAt << "TRUE" << endl;
								else
									SettingsAt << "FALSE" << endl;
							}
							break;

						case 4:
							{
								Epislon = ConvertTextTo <float, float> ( GetDlgItem ( hWndDlg, IDC_EDIT1 ) );
								SettingsAt << Epislon << endl;
							}
							break;

						case 5:
							{
								MaxThreads = ConvertTextTo <int, int> ( GetDlgItem ( hWndDlg, IDC_EDIT2 ) );
								SettingsAt << MaxThreads << endl;
							}
							break;

						case 6:
							{
								PreciseSize = ConvertTextTo <int, int> ( GetDlgItem ( hWndDlg, IDC_EDIT3 ) );
								SettingsAt << PreciseSize << endl;
							}
							break;

						case 7:
							{
								UnknownSize = ConvertTextTo <int, int> ( GetDlgItem ( hWndDlg, IDC_EDIT4 ) );
								SettingsAt << UnknownSize << endl;
							}
							break;
						}

						if ( count > 6 )
							break;

						++ count;						
					}

					// Store the values
					SettingsAt.close ();

					// Store the settings
					MakeSettings ();

					EndDialog ( hWndDlg, 0 );
				}
				break;

			case IDC_BUTTON3:
				{
					// Create the pathway
					string Pathway = CurDirectory;
					Pathway       += "\\LPMS.ini";

					// Create the settings
					ofstream File ( Pathway.c_str () );
					File.clear ();
					File << "TRUE\nTRUE\nTRUE\nTRUE\n0.01\n30\n10000000\n20000000" << endl;
					File.close ();

					// Reset settings
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX1 ), BM_SETCHECK, (WPARAM) BST_CHECKED, NULL );
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX2 ), BM_SETCHECK, (WPARAM) BST_CHECKED, NULL );
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX3 ), BM_SETCHECK, (WPARAM) BST_CHECKED, NULL );
					SMX ( GetDlgItem ( settingDlg, IDC_CHECKBOX4 ), BM_SETCHECK, (WPARAM) BST_CHECKED, NULL );
					SetWindowText ( GetDlgItem ( settingDlg, IDC_EDIT1 ), "0.01" );
					SetWindowText ( GetDlgItem ( settingDlg, IDC_EDIT2 ), "30" );
					SetWindowText ( GetDlgItem ( settingDlg, IDC_EDIT3 ), "10000000" );
					SetWindowText ( GetDlgItem ( settingDlg, IDC_EDIT4 ), "20000000" );

					// Make the settings
					MakeSettings ();

					EndDialog ( hWndDlg, 0 );
				}
				break;
			}

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			SettingProc ();

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
INT_PTR CALLBACK pointerDialog ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), "10000+3A" );
			return TRUE;
		}
	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					// Obtain the text
					char szBuffer [256] = {0};
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szBuffer, 256 );

					// Create the string
					string Text;
					Text += (LPSTR) szBuffer;

					string NoMexicans;

					// Obtain variables
					DWORD dwValue   = 0;
					DWORD dwPointer = 0;
					DWORD dwAddress = 0;
					DWORD dwPrev    = 0;

					int check   = 1;
					int LeftOff = 0;
					int Length  = 0;

					// Get rid of spaces
					for ( SIZE_T i = 0; i < Text.length (); i ++ )
					{
						if ( Text.at ( i ) != ' ' )
							NoMexicans += Text.at ( i ) ;
						if ( Text.at ( i ) == '+' )
							++Length;
					}

					// Go through the loop
					for ( SIZE_T i = 0; i < NoMexicans.length (); i ++ )
					{
						if ( NoMexicans.at ( i ) == '+' || i == ( NoMexicans.length () - 1 ) )
						{
							string Number = NoMexicans.substr ( LeftOff, i );
							stringstream ss;
							ss << Number.c_str ();

							LPVOID lpValue;
							ss >> lpValue;
							dwValue = (DWORD) lpValue; 

							if ( check == 1 )
								dwPointer = RV <DWORD> ( dwValue );	

							// Extract the values
							if ( ( Length != 1 ) )
							{										
								if ( check > 1 )
								{
									dwAddress = RV <DWORD> ( dwPointer + dwValue );
									dwPointer = dwAddress;

									--Length;
								}
							}
							else if ( check != 1 )
								dwPointer += dwValue;

							++ check;
							LeftOff = i + 1;
						}
					}

					// Pointer
					string AddressStr = "P->";
					AddressStr       += CTT <LPVOID> ( (void*) dwPointer );

					// Obtain the type
					char szType [256] = {0};
					GetWindowText ( GetDlgItem ( mainDlg, IDC_COMBO1 ), (LPSTR) szType, 256 );

					// Convert it to string
					string ST;
					ST += szType;

					// Insert the items
					IP ( AddressStr, ST, Text );

					// Close the window
					EndDialog ( hWndDlg, 0 );
				}
				break;
			}

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
INT_PTR CALLBACK editAllDialog ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	editAllDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		{
			EditAllProc ();
			return TRUE;
		}

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					int maxItems = ListView_GetItemCount ( GetDlgItem ( mainDlg, IDC_LIST2 ) );

					for ( int i = 0; i < maxItems; i ++ )
						ListView_SetCheckState ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, true );

					EndDialog ( hWndDlg, 0 );
				}
				break;

			case IDC_BUTTON2:
				{
					char szValue [256];
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT1 ), (LPSTR) szValue, 256 );

					// Setup strings
					string str = "";
					str       += szValue;

					int maxItems = ListView_GetItemCount ( GetDlgItem ( mainDlg, IDC_LIST2 ) );

					for ( int i = 0; i < maxItems; i ++ )
					{
						// Buffers
						char szBuffer [256] = {0};
						char szType   [256] = {0};

						// Get Item Text
						ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 1, szBuffer, 256 );

						// Get Item Type
						ListView_GetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 2, szType, 256 );				

						string addr;
						addr += szBuffer;

						// Address
						LPVOID lpAddr;
						
						if ( addr.at (0) != 'P' )
						{
							// Convert the string to LPVOID
							stringstream ss;
							ss << addr;
							ss >> lpAddr;
						}
						else
						{
							// Convert the string to LPVOID
							stringstream ss;
							ss << addr.substr ( 3, 10 );
							ss >> lpAddr;
						}

						// Byte
						if ( !strcmp ( szType, "Byte" ) )
						{
							BYTE Value = ConvertTextTo2 <BYTE, BYTE> ( str );
							WV     ( (DWORD) lpAddr, Value );
						}
						// 2 Bytes
						if ( !strcmp ( szType, "2 Bytes" ) )
						{
							WORD Value = ConvertTextTo2 <WORD, WORD> ( str );
							WV     ( (DWORD) lpAddr, Value );
						}
						// 4 Bytes
						if ( !strcmp ( szType, "4 Bytes" ) )
						{
							DWORD Value = ConvertTextTo2 <DWORD, DWORD> ( str );
							WV     ( (DWORD) lpAddr, Value );
						}
						// 8 Bytes
						if ( !strcmp ( szType, "8 Bytes" ) )
						{
							UINT64 Value = ConvertTextTo2 <UINT64, UINT64> ( str );
							WV     ( (DWORD) lpAddr, Value );
						}
						// Float
						if ( !strcmp ( szType, "Float" ) )
						{
							float Value = ConvertTextTo2 <float, float> ( str );
							WV     ( (DWORD) lpAddr, Value );
						}
						// Double
						if ( !strcmp ( szType, "Double" ) )
						{
							double Value = ConvertTextTo2 <double, double> ( str );
							WV     ( (DWORD) lpAddr, Value );
						}
					}

					EndDialog ( hWndDlg, 0 );
				}
				break;

			case IDC_BUTTON3:
				{
					char szValue [256];
					GetWindowText ( GetDlgItem ( hWndDlg, IDC_EDIT2 ), (LPSTR) szValue, 256 );

					int maxItems = ListView_GetItemCount ( GetDlgItem ( mainDlg, IDC_LIST2 ) );

					for ( int i = 0; i < maxItems; i ++ )
						ListView_SetItemText ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, 4, szValue );

					EndDialog ( hWndDlg, 0 );
				}
				break;

			case IDC_BUTTON4:
				{
					int maxItems = ListView_GetItemCount ( GetDlgItem ( mainDlg, IDC_LIST2 ) );

					for ( int i = 0; i < maxItems; i ++ )
						ListView_SetCheckState ( GetDlgItem ( mainDlg, IDC_LIST2 ), i, false );

					EndDialog ( hWndDlg, 0 );
				}
				break;
			}

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
INT_PTR CALLBACK DataDialog    ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	dataStruct = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					if ( DataNotDone == false )
					{
						CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) DataFunc, NULL, NULL, NULL );
						DataNotDone = true;
					}
					else
						MessageBoxA ( hWndDlg, "Please wait for the scan to finish", "Patience", MB_ICONINFORMATION );
				}
				break;
			}

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			DataProc ();

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