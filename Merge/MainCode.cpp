#include <Windows.h>
#include <sstream>
#include <fstream>

using namespace std;

// Merging functions
void MergeIt    ( HANDLE hMerge, const char *str )
{	
	// Obtain the file size
	HANDLE hFile     = CreateFile ( (LPCSTR) str, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD dwFileSize = GetFileSize ( hFile, NULL );

		// Read
		BYTE * fileMem = new BYTE [ dwFileSize ];

		DWORD dwBytes;
		::ReadFile ( hFile, fileMem, dwFileSize, &dwBytes, 0 );

		// Write
		::WriteFile ( hMerge, fileMem, dwFileSize, &dwBytes, 0 );
		
		// Close
		CloseHandle ( hFile );

		// Delete file
		DeleteFile ( (LPCSTR) str );

		// Delete buffer
		delete [] fileMem;
	}
}
void MergeFiles ()
{
	char szDirectory [256] = {0};
	GetCurrentDirectory ( 256, szDirectory );

	string CurrentDir = szDirectory;
	
	string Settings = CurrentDir;
	Settings += "\\LPMS.ini";

	// Obtain max threads
	ifstream File ( Settings.c_str () );
	int count = 0;
	int maxThreads = 0;

	while ( !File.eof () )
	{
		string str;
		getline ( File, str );

		if ( count == 5 )
		{
			stringstream ss;
			ss << str;
			ss >> maxThreads;
		}

		++count;
	}

	File.close ();

	// Create our file
	string Memory = CurrentDir;
	Memory += "\\Memory\\Memory.TMP";

	DeleteFile ( Memory.c_str () );
	HANDLE hMerge = CreateFile ( Memory.c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	// Begin merging
	string Directory = CurrentDir;
	Directory += "\\Memory\\Memory";

	for ( int i = 0; i < maxThreads; i ++ )
	{
		string newPath = Directory;

		// Create the path
		stringstream ss;
		ss << i; 
		newPath += ss.str ();
		newPath += ".TMP";

		MergeIt ( hMerge, newPath.c_str () );
	}

	// Write
	ofstream Finished ( "C:\\MergeOver.txt" );
	Finished.clear ();
	Finished << "Done" << endl;
	Finished.close ();

	// Close
	CloseHandle ( hMerge );
}
//

// Entry-point
int CALLBACK WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	MergeFiles ();

	return 0;
}
//