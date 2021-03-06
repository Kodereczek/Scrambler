// Filename: class.application.cpp
// Author:   Patryk Połeć

#include "engine.h"

////////////////////////////////////////////////////////////////////////////////////////////////// Constructor and destructor
C_Application::C_Application(C_Log *_objLog, C_Config *_objConfig, C_Converter *_objConverter, C_Scrambler *_objScrambler, HINSTANCE *_hInstance, LPSTR _lpCmdLine): objLog(_objLog), objConfig(_objConfig), objConverter(_objConverter), objScrambler(_objScrambler), instanceApplication(_hInstance), currentFileName(_lpCmdLine)
{
	// Public fields
	stateLoop = RUN;

	// Private fields
	objFile = new C_File;
	
	handleMenu    = NULL;
	handleWindow  = NULL;
	handleTextBox = NULL;
	handleLabel1  = NULL;
	handleLabel2  = NULL;
	handleDialog  = NULL;
	handleLabel3  = NULL;
	handleEdit    = NULL;
	handleButton1 = NULL;
	handleButton2 = NULL;

	windowCaption = "Scrambler";

	savedTextBox = "";

	fileIsSave = true;

	windowWidth  = 800;
	windowHeight = 600;

	// Initialization
	ZeroMemory(&classWindow   , sizeof(WNDCLASSEX));
	classWindow.cbSize        = sizeof(WNDCLASSEX);
	classWindow.style         = NULL;
	classWindow.cbClsExtra    = NULL;
	classWindow.cbWndExtra    = NULL;
	classWindow.lpfnWndProc   = WindowMainProcedure;
	classWindow.hInstance     = *instanceApplication;
	classWindow.lpszClassName = "WINDOW";
	classWindow.lpszMenuName  = NULL;
	classWindow.hCursor       = LoadCursor(NULL, IDC_ARROW);
	classWindow.hIcon         = LoadIcon(*instanceApplication, MAKEINTRESOURCE(ICON_RC));
	classWindow.hIconSm       = LoadIcon(*instanceApplication, MAKEINTRESOURCE(ICON_RC));
	classWindow.hbrBackground = (HBRUSH)(COLOR_WINDOW + 0,5);

	ZeroMemory(&classDialog   , sizeof(WNDCLASSEX));
	classDialog.cbSize        = sizeof(WNDCLASSEX);
	classDialog.style         = NULL;
	classDialog.cbClsExtra    = NULL;
	classDialog.cbWndExtra    = NULL;
	classDialog.lpfnWndProc   = DialogMainProcedure;
	classDialog.hInstance     = *instanceApplication;
	classDialog.lpszClassName = "DIALOG";
	classDialog.lpszMenuName  = NULL;
	classDialog.hCursor       = LoadCursor(NULL, IDC_ARROW);
	classDialog.hIcon         = LoadIcon(*instanceApplication, MAKEINTRESOURCE(ICON_RC));
	classDialog.hIconSm       = LoadIcon(*instanceApplication, MAKEINTRESOURCE(ICON_RC));
	classDialog.hbrBackground = (HBRUSH)(COLOR_WINDOW + 0,5);

	RegisterClassEx(&classWindow);
	RegisterClassEx(&classDialog);

	// Load Controls
	handleMenu    = LoadMenu(*instanceApplication, MAKEINTRESOURCE(MENU_RC));
	handleWindow  = CreateWindowEx(WS_EX_CLIENTEDGE, "WINDOW", windowCaption, WS_OVERLAPPEDWINDOW ^(WS_MAXIMIZEBOX | WS_SIZEBOX) | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, NULL, handleMenu, *instanceApplication, this);
	handleTextBox = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, -1, -1, windowWidth - 17, windowHeight - 85, handleWindow, NULL, *instanceApplication, NULL);
	handleLabel1  = CreateWindowEx(NULL, "STATIC", "Pusty dokument", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, windowHeight - 85, 200, 20, handleWindow, NULL, *instanceApplication, NULL);
	handleLabel2  = CreateWindowEx(NULL, "STATIC", "Znaków: 0", WS_CHILD | WS_VISIBLE | SS_LEFT, windowWidth - 150, windowHeight - 85, 200, 20, handleWindow, NULL, *instanceApplication, NULL);
	handleDialog  = CreateWindowEx(NULL, "DIALOG", windowCaption, WS_OVERLAPPED | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 400, 150, handleWindow, NULL, *instanceApplication, this);
	handleLabel3  = CreateWindowEx(NULL, "STATIC", "Podaj klucz:", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 35, 200, 20, handleDialog, NULL, *instanceApplication, NULL);
	handleEdit    = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", NULL, WS_CHILD | WS_VISIBLE, 20, 55, 200, 22, handleDialog, NULL, *instanceApplication, NULL);
	handleButton1 = CreateWindowEx(NULL, "BUTTON", NULL, WS_CHILD | WS_VISIBLE, 250, 30, 110, 28, handleDialog, NULL, *instanceApplication, NULL);
	handleButton2 = CreateWindowEx(NULL, "BUTTON", "Powrót", WS_CHILD | WS_VISIBLE, 250, 60, 110, 28, handleDialog, NULL, *instanceApplication, NULL);

	// Load Fonts
	handleFontVerdana = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Verdana");
	
	// Send Messages
	SendMessage(handleTextBox, WM_SETFONT, (WPARAM)handleFontVerdana, TRUE);
	SendMessage(handleLabel1 , WM_SETFONT, (WPARAM)handleFontVerdana, TRUE);
	SendMessage(handleLabel2 , WM_SETFONT, (WPARAM)handleFontVerdana, TRUE);
	SendMessage(handleLabel3 , WM_SETFONT, (WPARAM)handleFontVerdana, TRUE);
	SendMessage(handleEdit   , WM_SETFONT, (WPARAM)handleFontVerdana, TRUE);
	SendMessage(handleButton1, WM_SETFONT, (WPARAM)handleFontVerdana, TRUE);
	SendMessage(handleButton2, WM_SETFONT, (WPARAM)handleFontVerdana, TRUE);

	// Load File Input
	if(currentFileName != "")
	{
		string buffer = "";
		for(int l = 1; l < (int)currentFileName.length() - 1; l++) buffer += currentFileName[l];
		currentFileName = buffer;
		objFile->OpenFile(currentFileName, ios::in);
		objFile->LoadData();
		SetWindowText(handleTextBox, objFile->GetData().c_str());
		objFile->CloseFile();

		savedTextBox = GetTextFromWindow(handleTextBox);
		CheckIsChangedText();
	}
}

C_Application::~C_Application()
{
	DeleteObject(handleFontVerdana);

	DestroyWindow(handleButton2);
	DestroyWindow(handleButton1);
	DestroyWindow(handleEdit);
	DestroyWindow(handleLabel3);
	DestroyWindow(handleDialog);
	DestroyWindow(handleLabel2);
	DestroyWindow(handleLabel1);
	DestroyWindow(handleTextBox);

	DestroyWindow(handleWindow);
	DestroyMenu(handleMenu);
	
	delete objFile;
}

////////////////////////////////////////////////////////////////////////////////////////////////// Window Procedure
LRESULT CALLBACK C_Application::WindowProcedure(HWND _handleWindow, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	switch(_message)
	{
		case WM_COMMAND:
			     if(_wParam == MENU_NEW_RC)            FileNew();
			else if(_wParam == MENU_OPEN_RC)           FileOpen();
			else if(_wParam == MENU_SAVE_RC)           FileSave();
			else if(_wParam == MENU_SAVE_AS_RC)        FileSaveAs();
			else if(_wParam == MENU_EXIT_RC)           {if(!FileNew()) stateLoop = STOP;}
			else if(_wParam == MENU_CLEAR_RC)          ClearWindow(handleTextBox);
			else if(_wParam == MENU_ENCRYPT_RC)        FileEncrypt();
			else if(_wParam == MENU_DECRYPT_RC)        FileDecrypt();
			else if(_wParam == MENU_CONSOLE_ERRORS_RC) MessageBox(handleWindow, "Konsola nie dostępna!", windowCaption, MB_ICONERROR | MB_OK);

			CheckIsChangedText();
		break;
      
		case WM_KEYDOWN:
			if(_wParam == VK_ESCAPE) if(!FileNew()) stateLoop = STOP;
		break;

		case WM_CLOSE:
			if(!FileNew()) stateLoop = STOP;
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		
		default:
			return DefWindowProc(_handleWindow, _message, _wParam, _lParam);
	}

	return 0;
}

LRESULT CALLBACK C_Application::DialogProcedure(HWND _handleWindow, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	switch(_message)
	{
		case WM_COMMAND:
			if(_lParam == (LPARAM)handleButton1)
			{
				if(GetTextFromWindow(handleButton1) == "Szyfruj") SetWindowText(handleTextBox, objScrambler->Encrypt(GetTextFromWindow(handleTextBox), objConverter->ToI(GetTextFromWindow(handleEdit))).c_str());
				else                                              SetWindowText(handleTextBox, objScrambler->Decrypt(GetTextFromWindow(handleTextBox), objConverter->ToI(GetTextFromWindow(handleEdit))).c_str());

				EnableWindow(handleWindow, 1);
				ShowWindow(handleDialog, 0);
			}
			else if(_lParam == (LPARAM)handleButton2)
			{
				EnableWindow(handleWindow, 1);
				ShowWindow(handleDialog, 0);
			}

			CheckIsChangedText();
		break;

		case WM_KEYDOWN:
			if(_wParam == VK_RETURN)
			{
				if(GetTextFromWindow(handleButton1) == "Szyfruj") SetWindowText(handleTextBox, objScrambler->Encrypt(GetTextFromWindow(handleTextBox), objConverter->ToI(GetTextFromWindow(handleEdit))).c_str());
				else                                              SetWindowText(handleTextBox, objScrambler->Decrypt(GetTextFromWindow(handleTextBox), objConverter->ToI(GetTextFromWindow(handleEdit))).c_str());

				EnableWindow(handleWindow, 1);
				ShowWindow(handleDialog, 0);
			}
			if(_wParam == VK_ESCAPE)
			{
				EnableWindow(handleWindow, 1);
				ShowWindow(handleDialog, 0);
			}

			CheckIsChangedText();
		break;

		case WM_CLOSE:
			EnableWindow(handleWindow, 1);
			ShowWindow(handleDialog, 0);
		break;

		default:
			return DefWindowProc(_handleWindow, _message, _wParam, _lParam);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////// Public methods
int C_Application::FileNew()
{
	if(!fileIsSave)
	{
		int result = MessageBox(handleWindow, "Zapisać plik przed zamknięciem?", windowCaption, MB_ICONQUESTION | MB_YESNOCANCEL);

		     if(result == IDYES)    FileSave();
		else if(result == IDCANCEL) return 1;
	}

	ClearWindow(handleTextBox);
	savedTextBox = "";
	currentFileName = "";

	return 0;
}

int C_Application::FileOpen()
{
	if(!fileIsSave)
	{
		int result = MessageBox(handleWindow, "Zapisać plik przed zamknięciem?", windowCaption, MB_ICONQUESTION | MB_YESNOCANCEL);

		     if(result == IDYES)    FileSave();
		else if(result == IDCANCEL) return 1;
	}
	
	if(!GetFileName(handleWindow, currentFileName, "Pliki szyfrowane (*.spp)\0*.spp\0Wszystkie pliki\0*.*\0")) return 1;
	
	objFile->OpenFile(currentFileName, ios::in);
	objFile->LoadData();
	SetWindowText(handleTextBox, objFile->GetData().c_str());
	objFile->CloseFile();

	savedTextBox = GetTextFromWindow(handleTextBox);

	return 0;
}

int C_Application::FileSave()
{
	if(currentFileName == "") if(!SetFileName(handleWindow, currentFileName, "spp", "Pliki szyfrowane (*.spp)\0*.spp\0")) return 1;

	objFile->OpenFile(currentFileName, ios::out | ios::trunc);
	objFile->SetData(GetTextFromWindow(handleTextBox));
	objFile->SaveData();
	objFile->CloseFile();

	savedTextBox = GetTextFromWindow(handleTextBox);

	return 0;
}

int C_Application::FileSaveAs()
{
	if(!SetFileName(handleWindow, currentFileName, "spp", "Pliki szyfrowane (*.spp)\0*.spp\0")) return 1;

	objFile->OpenFile(currentFileName, ios::out | ios::trunc);
	objFile->SetData(GetTextFromWindow(handleTextBox));
	objFile->SaveData();
	objFile->CloseFile();

	savedTextBox = GetTextFromWindow(handleTextBox);

	return 0;
}

int C_Application::FileEncrypt()
{
	EnableWindow(handleWindow, 0);
	SetWindowText(handleButton1, "Szyfruj");
	ShowWindow(handleDialog, 1);

	return 0;
}

int C_Application::FileDecrypt()
{
	EnableWindow(handleWindow, 0);
	SetWindowText(handleButton1, "Deszyfruj");
	ShowWindow(handleDialog, 1);

	return 0;
}

void C_Application::CheckIsChangedText()
{
	if(GetTextFromWindow(handleTextBox) == "")
	{
		SetWindowText(handleLabel1, "Pusty dokument");
	}
	else if(savedTextBox == GetTextFromWindow(handleTextBox))
	{
		fileIsSave = true;
		SetWindowText(handleLabel1, "Dokument zapisany");
	}
	else
	{
		fileIsSave = false;
		SetWindowText(handleLabel1, "Dokument nie zapisany");
	}

	stringstream buffer;
	buffer.str("");
	buffer << "Znaków: " << GetNumberOfCharsFromWindow(handleTextBox);
	SetWindowText(handleLabel2, buffer.str().c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////// Private methods
bool C_Application::SetFileName(HWND _handle, string &_fileName, LPCSTR _extension, LPCSTR _filter)
{
	char filePath[MAX_PATH] = "";
  
	OPENFILENAME setFileName;
	ZeroMemory(&setFileName , sizeof(OPENFILENAME));
	setFileName.lStructSize = sizeof(OPENFILENAME);
	setFileName.hwndOwner   = _handle;
	setFileName.lpstrFilter = _filter;
	setFileName.lpstrFile   = filePath;
	setFileName.nMaxFile    = MAX_PATH;
	setFileName.Flags       = OFN_OVERWRITEPROMPT;
	setFileName.lpstrDefExt = _extension;

	if(GetSaveFileName(&setFileName))
	{
		_fileName = filePath;
		return true;
	}
	
	return false;
}

bool C_Application::GetFileName(HWND _handle, string &_fileName, LPCSTR _filter)
{
	char filePath[MAX_PATH] = "";
  
	OPENFILENAME getFileName;
	ZeroMemory(&getFileName , sizeof(OPENFILENAME));
	getFileName.lStructSize = sizeof(OPENFILENAME);
	getFileName.hwndOwner   = _handle;
	getFileName.lpstrFilter = _filter;
	getFileName.lpstrFile   = filePath;
	getFileName.nMaxFile    = MAX_PATH;
	getFileName.Flags       = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if(GetOpenFileName(&getFileName))
	{
		_fileName = filePath;
		return true;
	}
	
	return false;
}

void C_Application::ClearWindow(HWND _handle)
{
	SetWindowText(_handle, "");
}

int C_Application::GetNumberOfCharsFromWindow(HWND _handle)
{
	return GetWindowTextLength(_handle);
}

void C_Application::GetTextFromWindow(HWND _handle, string &_buffer)
{
	int length = GetNumberOfCharsFromWindow(_handle) + 1;
	if(length - 1 == 0) {_buffer = ""; return;}
	
	char *buffer = new char[length];

	GetWindowText(_handle, buffer, length);
  
	_buffer = buffer;
	delete [] buffer;
}

string C_Application::GetTextFromWindow(HWND _handle)
{
	int length = GetNumberOfCharsFromWindow(_handle) + 1;
	if(length - 1 == 0) return "";

	char *buffer = new char[length];

	GetWindowText(_handle, buffer, length);
  
	string text = buffer;
	delete [] buffer;

	return text;
}