#include <Windows.h>
#include <Windowsx.h>
#include <sstream>

using namespace std;

#include "resource.h"

// Global Variables
HINSTANCE hInst;

HWND hWndOne, 
	 hWndTwo, 
	 hWndThree;

// Step 1
DWORD Ammo;

// Step 2
float XPos = 2000;
float YPos = 2000;

// Step 3
DWORD *Health;
void  *pHealth = &Health;

// Extra Functions
template <class T> string ToString ( T VALUE )
{
	stringstream ss;
	ss << VALUE;
	return ss.str ();
}
//

// Callback
INT_PTR CALLBACK StepOne   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK StepTwo   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK StepThree ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

// Threads
VOID ThreadOne ()
{
	while ( true )
	{
		if ( Ammo == 1000 )
		{
			// Enable the button
			Button_Enable ( GetDlgItem ( hWndOne, IDC_BUTTON2 ), TRUE );

			// Close the thread
			DWORD dwExitCode;
			GetExitCodeThread ( GetCurrentThread (), &dwExitCode );
			TerminateThread   ( GetCurrentThread (), dwExitCode );
		}

		Sleep ( 100 );
	}
}

VOID LaunchTwo ()
{
	DialogBoxA ( hInst, MAKEINTRESOURCE ( IDD_DIALOG2 ), NULL, StepTwo );

	// Close the thread
	DWORD dwExitCode;
	GetExitCodeThread ( GetCurrentThread (), &dwExitCode );
	TerminateThread   ( GetCurrentThread (), dwExitCode );
}
VOID ThreadTwo ()
{
	while ( true )
	{
		if ( XPos == 5000 )
			SetWindowText ( GetDlgItem ( hWndTwo, X_POS ), "Good job!" );
		if ( YPos == 5000 )
			SetWindowText ( GetDlgItem ( hWndTwo, Y_POS ), "Good job!" );

		if ( XPos == 5000 &&
			 YPos == 5000 )
		{
			// Enable the button
			Button_Enable ( GetDlgItem ( hWndTwo, IDC_BUTTON2 ), TRUE );

			// Close the thread
			DWORD dwExitCode;
			GetExitCodeThread ( GetCurrentThread (), &dwExitCode );
			TerminateThread   ( GetCurrentThread (), dwExitCode );
		}

		Sleep ( 100 );
	}
}

VOID LaunchThree ()
{
	DialogBoxA ( hInst, MAKEINTRESOURCE ( IDD_DIALOG3 ), NULL, StepThree );

	// Close the thread
	DWORD dwExitCode;
	GetExitCodeThread ( GetCurrentThread (), &dwExitCode );
	TerminateThread   ( GetCurrentThread (), dwExitCode );
}
//

// Callback Procedures
INT_PTR CALLBACK StepOne   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	hWndOne = hWndDlg;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		{
			Ammo = 30;

			// Create the thread
			CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) ThreadOne, NULL, NULL, NULL );

			// Set window titles
			SetWindowText ( GetDlgItem ( hWndDlg, IDC_AMMO ), "30" );

			return TRUE;
		}

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					DWORD randomNum = rand () % 5 + 1;

					// Decrease the Ammo
					Ammo -= randomNum;

					if ( randomNum > Ammo )
					{
						MessageBoxA ( hWndDlg, "Reloading...", "You ran out!", MB_ICONINFORMATION );
						Ammo = 30;
					}

					// Display the new Ammo
					stringstream ss;
					ss << Ammo;

					string ammoStr;
					ammoStr += ss.str ();

					// Set Title
					SetWindowText ( GetDlgItem ( hWndDlg, IDC_AMMO ), ammoStr.c_str () );
				}
				break;

			case IDC_BUTTON2:
				{
					// Create the thread
					CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) LaunchTwo, NULL, NULL, NULL );

					// Close the window
					ShowWindow ( hWndDlg, SW_HIDE );
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
INT_PTR CALLBACK StepTwo   ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	hWndTwo = hWndDlg;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		{
			CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) ThreadTwo, NULL, NULL, NULL );

			// Set Window Text
			SetWindowText ( GetDlgItem ( hWndDlg, X_POS ), "2000.00" );
			SetWindowText ( GetDlgItem ( hWndDlg, Y_POS ), "2000.00" );

			return TRUE;
		}

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					int decreasedX = rand () % 10 + 1;
					int decreasedY = rand () % 10 + 1;

					// Create the text
					string Orig = "Decreased by ";

					string XStr = Orig;
					XStr += ToString <int> ( decreasedX );

					string YStr = Orig;
					YStr += ToString <int> ( decreasedY );					
					
					// Set the text
					if ( XPos != 5000 )
						SetWindowText ( GetDlgItem ( hWndDlg, X_POS ), XStr.c_str () );
					if ( YPos != 5000 )
						SetWindowText ( GetDlgItem ( hWndDlg, Y_POS ), YStr.c_str () );

					if ( XPos != 5000 )
						XPos -= decreasedX;
					if ( YPos != 5000 )
						YPos -= decreasedY;

					if ( XPos < decreasedX )
						XPos = 2000;
					if ( YPos < decreasedY )
						YPos = 2000;
				}
				break;

			case IDC_BUTTON2:
				{
					// Create
					CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) LaunchThree, NULL, NULL, NULL );

					// Close
					EndDialog ( hWndDlg, 0 );
				}
				break;
			}

			return TRUE;
		}

	case WM_CLOSE:
		{
			EndDialog ( hWndDlg, 0 );
			EndDialog ( hWndOne, 0 );

			return TRUE;
		}

	default:
		return FALSE;
	}
}
INT_PTR CALLBACK StepThree ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	hWndThree = hWndDlg;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		{
			// Set variables
			Health  = new DWORD;
			*Health = 1000;

			return TRUE;
		}

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDC_BUTTON1:
				{
					DWORD randNum = rand () % 50 + 1;

					// Subtract
					*(DWORD*) ( *(DWORD*) pHealth ) -= randNum;

					if ( *Health < randNum )
						*Health = 1000;

					SetWindowText ( GetDlgItem ( hWndDlg, IDC_HEALTH ), ToString <DWORD> ( *Health ).c_str () );
				}
				break;

			case IDC_BUTTON2:
				{
					// Change health
					Health  = new DWORD;
					*Health = 1000;

					int second = 3;
					ShowWindow ( GetDlgItem ( hWndDlg, IDC_SEC ), SW_SHOW );

					while ( second != 0 )
					{
						string str = "You have ";
						str += ToString ( second );
						str += " seconds left.";

						SetWindowText ( GetDlgItem ( hWndDlg, IDC_SEC ), str.c_str () );

						-- second;
						Sleep ( 1000 );
					}

					ShowWindow ( GetDlgItem ( hWndDlg, IDC_SEC ), SW_HIDE );

					if ( *Health == 5000 )
						Button_Enable ( GetDlgItem ( hWndDlg, IDC_BUTTON3 ), TRUE );
				}
				break;

			case IDC_BUTTON3:
				{
					MessageBoxA ( hWndDlg, "Congragulations\n  Looks like you're ready.", "LPMS Tutorial", MB_ICONINFORMATION );

					EndDialog ( hWndDlg, 0 );
					EndDialog ( hWndOne, 0 );
				}
				break;
			}

			return TRUE;
		}

	case WM_CLOSE:
		{
			EndDialog ( hWndDlg, 0 );
			EndDialog ( hWndOne, 0 );

			return TRUE;
		}

	default:
		return FALSE;
	}
}

//

// Main Entrypoint
int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	hInst = hInstance;

	DialogBoxA ( hInstance, MAKEINTRESOURCE ( IDD_DIALOG1 ), NULL, StepOne );

	return 0;
}
//