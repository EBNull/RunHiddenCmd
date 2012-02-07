#include <windows.h>
#include <stdio.h>

//#define CONSOLE_DEBUG

BOOL file_exists(LPWSTR lpFileName) {
	HANDLE f = CreateFileW(
		lpFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (f == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	CloseHandle(f);
	return TRUE;
}

void CreateConsole() {
	AllocConsole();
	freopen("conin$","r",stdin);
	freopen("conout$","w",stdout);
	freopen("conout$","w",stderr);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	LPWSTR *szArglist;
	int nArgs;

	int i=0;
	int ti = 0;
	BOOL needcmd = FALSE;

	int curdir_len=128;
	LPWSTR curdir = NULL;
	LPWSTR params = NULL;

	int cp_res = 0;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	LPWSTR cmdline = GetCommandLineW();

#ifdef CONSOLE_DEBUG
	CreateConsole();
#endif


	szArglist = CommandLineToArgvW(cmdline, &nArgs);
	if(NULL == szArglist) {
		CreateConsole();
		wprintf(L"CommandLineToArgvW failed\n");
		Sleep(5000);
		return 1;
	}

	ti = wcslen(szArglist[0]);
	while (1) {
		szArglist[0][ti-3]=L'c';
		szArglist[0][ti-2]=L'm';
		szArglist[0][ti-1]=L'd';
		if (file_exists(szArglist[0])) {
			needcmd = TRUE;
			break;
		}
		szArglist[0][ti-3]=L'b';
		szArglist[0][ti-2]=L'a';
		szArglist[0][ti-1]=L't';
		if (file_exists(szArglist[0])) {
			needcmd = TRUE;
			break;
		}
		szArglist[0][ti-4]=L'\0';
		szArglist[0][ti-3]=L'\0';
		szArglist[0][ti-2]=L'\0';
		szArglist[0][ti-1]=L'\0';
		if (file_exists(szArglist[0])) {
			needcmd = FALSE;
			break;
		}
		break;
	}

	if (!file_exists(szArglist[0])) {
		CreateConsole();
		printf("Could not find %ws", szArglist[0]);
		MessageBoxW(NULL, L"The specified script could not be found", L"Could not start script", MB_ICONERROR);
		return 2;
	}

	for(i=0; i<nArgs; i++) {
		printf("%d: %ws\n", i, szArglist[i]);
	}


	while(1) {
		curdir = malloc(sizeof(WCHAR) * (curdir_len+1));
		ti=GetCurrentDirectoryW(curdir_len, curdir);
		if (ti == 0) {
			return 3;
		}
		if (ti > curdir_len) {
			free(curdir);
			curdir_len = ti;
		} else {
			break;
		}
	}
	

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	if (needcmd) {
		ti = (wcslen(cmdline)+128)*sizeof(WCHAR);
		params = malloc(ti);
		ZeroMemory(params, ti);
		wcscat(params, L"cmd.exe /c \"");
		wcscat(params, szArglist[0]);
		wcscat(params, L"\" ");

		for(i=1; i<nArgs; i++) {
			wcscat(params, szArglist[i]);
			wcscat(params, L" ");
		}

		printf("params=%ws\n", params);

		cp_res = CreateProcessW(
			NULL, //L"cmd.exe",
			params,
			NULL,
			NULL,
			FALSE,
			CREATE_NO_WINDOW,
			NULL,
			curdir,
			&si,
			&pi
		);
		if (cp_res) {
			ti = GetLastError();
		} else {
			ti = 0;
		}
	} else {
		//target is an exe
		ti = (wcslen(cmdline)+128)*sizeof(WCHAR);
		params = malloc(ti);
		ZeroMemory(params, ti);

		for(i=0; i<nArgs; i++) {
			wcscat(params, szArglist[i]);
			wcscat(params, L" ");
		}

		printf("params=%ws\n", params);

		cp_res = CreateProcessW(
			szArglist[0],
			params,
			NULL,
			NULL,
			FALSE,
			CREATE_NO_WINDOW,
			NULL,
			curdir,
			&si,
			&pi
		);
		if (cp_res) {
			ti = GetLastError();
		} else {
			ti = 0;
		}
	}
	if (!cp_res) {
		CreateConsole();
		printf("params=%ws", params);
		printf("CreateProcess failed: Error %d", ti);
		Sleep(5000);
		return ti;
	} else {
		//success!
		CloseHandle(pi.hThread);
		printf("Waiting for process %d to close...\n", pi.dwProcessId);
		WaitForSingleObject(pi.hProcess, INFINITE);
		ti = 0;
		GetExitCodeProcess(pi.hProcess, &ti);
		printf("Process %d returned %d.\n", pi.dwProcessId, ti);
		CloseHandle(pi.hProcess);
		return ti;
	}
};