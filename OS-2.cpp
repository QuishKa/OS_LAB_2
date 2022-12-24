#include <windows.h>
#include <iostream>
#include <string.h>
#include <bitset>

using namespace std;

void SysInfo();
void MemStatus();
void MemStatusByAddress();
void MemReserve();
void MemReserve2();
void MemWrite();
void MemProtect();
void MemFree();
void Task2();
void Reader();
void Writer();

HANDLE fMapHandle = NULL, fHandle = NULL;

class Menu {
private:
	static int chosen;
	static bool work;
	static void execute(int n) {
		if (!work) return;
		switch (n)
		{
		case 1:
			SysInfo();
			break;
		case 2:
			MemStatus();
			break;
		case 3:
			MemStatusByAddress();
			break;
		case 4:
			MemReserve();
			break;
		case 5:
			MemReserve2();
			break;
		case 6:
			MemWrite();
			break;
		case 7:
			MemProtect();
			break;
		case 8:
			MemFree();
			break;
		case 9:
			Task2();
			break;
		default:
			break;
		}
	}
public:
	static void init() { chosen = 0; work = true; }
	static void PrintList() {
		printf(
			"1 - ��������� ���������� � �������������� �������\n"
			"2 - ����������� ������� ����������� ������\n"
			"3 - ����������� ��������� ����������� ������� ������ �� ��������� � ���������� ������\n"
			"4 - �������������� ������� � �������������� ������ � � ������ ����� ������ ������ �������\n"
			"5 - �������������� ������� � �������� ��� ���������� ������ � �������������� ������ � � ������ ����� ������ ������ �������\n"
			"6 - ������ ������ � ������ ������ �� �������� � ���������� �������\n"
			"7 - ��������� ������ ������� ��� ��������� (� ����������) ������� ������ � �� ��������\n"
			"8 - ������� ���������� ������ � ������������ ������� ��������� ������������ ���������(� ����������) ������� ������\n"
			"9 - ������������� ������������ ������ ��� ������ ������� ����� ����������\n"
			"0 - ����� �� ���������\n");
	}
	static void setChosen(int value) { chosen = value; work = value == 0 ? false : true; execute(value); }
	static bool getWork() { return work; }
};
int Menu::chosen;
bool Menu::work;

int main()
{
	string tmp;
	int val;
	Menu::init();
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	// menu loop
	while (Menu::getWork()) {
		system("cls");
		Menu::PrintList();
		cin >> tmp;
		val = _strtoi64(tmp.c_str(), nullptr, 10);
		Menu::setChosen(val);
		printf("������� ����� ������...");
		getchar();
		getchar();
	}

	if (fHandle != NULL) CloseHandle(fHandle);
	if (fMapHandle != NULL) CloseHandle(fMapHandle);

	return 0;
}

void SysInfo() {
	SYSTEM_INFO lpSystemInfo;
	GetSystemInfo(&lpSystemInfo);
	// open system info using microsoft documentation
	printf("����������� ����������: ");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		printf("x64 (AMD ��� Intel)\n");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)
		printf("ARM\n");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64)
		printf("ARM64\n");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		printf("Intel Itanium\n");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		printf("x86\n");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_UNKNOWN)
		printf("����������\n");
	printf(
		"������ ��������: %i\n"
		"��������� �� ����� ������ ����� ������, ��������� ��� ���������� � ��������� ������������ ���������� (DLL): 0x%p\n"
		"��������� �� ����� ������� ����� ������, ��������� ��� ���������� � ��������� DLL:                          0x%p\n"
		"�����, �������������� ����� �����������, ����������� � �������: \n%s\n"
		"���������� ���������� ����������� � ������� ������: %i\n"
		"������� ����������� ���������� ������, �� �������� ����� �������� ����������� ������: %i\n"
		"������� ����������, ��������� �� �����������: %i\n"
		"�������� ����������, ��������� �� �����������: %i\n",
		(int)lpSystemInfo.dwPageSize,
		(void*)lpSystemInfo.lpMinimumApplicationAddress,
		(void*)lpSystemInfo.lpMaximumApplicationAddress,
		bitset<sizeof(lpSystemInfo.dwActiveProcessorMask) * CHAR_BIT>(lpSystemInfo.dwActiveProcessorMask).to_string().c_str(),
		(int)lpSystemInfo.dwNumberOfProcessors,
		(int)lpSystemInfo.dwAllocationGranularity,
		(int)lpSystemInfo.wProcessorLevel,
		(int)lpSystemInfo.wProcessorRevision);
}

void MemStatus() {
	MEMORYSTATUS lpBuffer;
	GlobalMemoryStatus(&lpBuffer);
	// open MEMORYSTATUS using code-live.ru
	printf(
		"������ ��������� MEMORYSTATUS: %I64d ����\n"
		"������������� ������: %I64d%%\n\n"
		"����� ���������� ������:          %16I64d ����\n"
		"�������� ���������� ������:       %16I64d ����\n\n"
		"����� ������ � ����� ��������:    %16I64d ����\n"
		"�������� ������ � ����� ��������: %16I64d ����\n\n"
		"����� ����������� ������:         %16I64d ����\n"
		"�������� ����������� ������:      %16I64d ����\n",
		lpBuffer.dwLength,
		lpBuffer.dwMemoryLoad,
		lpBuffer.dwTotalPhys,
		lpBuffer.dwAvailPhys,
		lpBuffer.dwTotalPageFile,
		lpBuffer.dwAvailPageFile,
		lpBuffer.dwTotalVirtual,
		lpBuffer.dwAvailVirtual);
}

void MemStatusByAddress() {
	void *address;
	SIZE_T res;
	MEMORY_BASIC_INFORMATION lpBuffer;
	printf("������� ����� ������� ������: 0�");
	cin >> hex >> address;
	res = VirtualQuery(address, &lpBuffer, sizeof(lpBuffer));
	// open MEMORY_BASIC_INFORMATION using microsoft documentation
	if (res) {
		printf("�������� ������� ������ � ������� 0x%p\n", (void*)address);
		printf(
			"��������� �� ������� ����� ������� �������: %p\n"
			"��������� �� ������� ����� ��������� �������, ���������� �������� VirtualAlloc: %p\n",
			(void*)lpBuffer.BaseAddress,
			(void*)lpBuffer.AllocationBase);
		printf("�������� ������ ������ ��� �������������� ��������� �������: \n");
		switch (lpBuffer.Protect)
		{
		case 0:
			printf("	���������� ������ �� ����� �������\n");
			break;
		case PAGE_EXECUTE:
			printf("	��������� ��������� ������ � ��������������� ������� �������\n");
			break;
		case PAGE_EXECUTE_READ:
			printf("	��������� ��������� ��� ������ ��� ������ ������ � ��������������� ������� �������\n");
			break;
		case PAGE_EXECUTE_READWRITE:
			printf("	�������� ����������, ������ ������ ��� ������ ��� ������ � ������ � ��������������� ������� �������\n");
			break;
		case PAGE_EXECUTE_WRITECOPY:
			printf("	�������� ������ �� ����������, ������ ��� ������ ��� ����������� ��� ������ � �������������� ������������� ������� ������������� ������\n");
			break;
		case PAGE_NOACCESS:
			printf("	��������� ���� ������ � ��������������� ������� �������\n");
			break;
		case PAGE_READONLY:
			printf("	��������� ������ ������ ��� ������ � ��������������� ������� �������\n");
			break;
		case PAGE_READWRITE:
			printf("	�������� ������ ������ ��� ������ ��� ������ � ������ � ��������������� ������� �������\n");
			break;
		case PAGE_WRITECOPY:
			printf("	�������� ������ ������ ��� ������ ��� ����������� ��� ������ � �������������� �������������� ������� ������������� ������\n");
			break;
		case PAGE_TARGETS_INVALID:
			printf("	������ ��� ������������ �� ��������� � �������� ������������ ������� �������� ��� CFG\n");
			printf("	�������� � ������� �� ����� ��������� �������� CFG ��� ��������� ������ ��� VirtualProtect\n");
			break;
		default:
			break;
		}
		if (lpBuffer.AllocationProtect & PAGE_GUARD)
			printf("	�������� � ������� ���������� ���������� ������\n");
		if (lpBuffer.AllocationProtect & PAGE_NOCACHE)
			printf("	������ ��� ���� ������� ������ �� �����������\n");
		if (lpBuffer.AllocationProtect & PAGE_WRITECOMBINE)
			printf("	������ ���� ��������� ����� ����������� �������\n");
		printf(
			"������ �������, ������������� � �������� ������, � ������� ��� �������� ����� ���������� �������� � ������: %i\n"
			"��������� ������� � �������: \n", lpBuffer.RegionSize);
		switch (lpBuffer.State)
		{
		case MEM_COMMIT:
			printf("	��������������� ��������, ��� ��� ���� �������� ���������� ��������� ���� � ������, ���� � ����� �������� �� �����\n");
			break;
		case MEM_FREE:
			printf("	��������� �������� ���������� ��� ����������� �������� � �������� ��� ���������\n");
			break;
		case MEM_RESERVE:
			printf("	����������������� ��������, ��� �������� ������������ ��������� ������������ �������� �������������� ��� ��������� ����������� ���������\n");
			break;
		default:
			break;
		}
		printf("��� ������� � �������: \n");
		switch (lpBuffer.Type)
		{
		case MEM_IMAGE:
			printf("	�������� ������ � ������� �������������� � �������������� ������� �����������\n");
			break;
		case MEM_MAPPED:
			printf("	�������� ������ � ������� �������������� � �������������� �������\n");
			break;
		case MEM_PRIVATE:
			printf("	�������� ������ � ������� �������� ��������\n");
			break;
		default:
			break;
		}
	}
	else printf("������ ������ ������� ������\n");
}

void MemReserve() {
	int val;
	void* addr;
	LPVOID result;
	SYSTEM_INFO lpSystemInfo;
	GetSystemInfo(&lpSystemInfo);
	printf(
		"����� ������� �������� ������?\n"
		"1 - �������������\n"
		"2 - � ������ ���������� �������\n");
	cin >> val;
	switch (val)
	{
	case 1:
		result = VirtualAlloc(NULL, lpSystemInfo.dwPageSize, MEM_RESERVE, PAGE_READWRITE);
		if (result) printf("������ ������� ��������, ����� ������ �������: 0x%p\n"
			"������ �������: %i ����\n", result, lpSystemInfo.dwPageSize);
		else printf("������ ��������� ������: %i", GetLastError());
		break;
	case 2:
		printf("������� ����� ���������� �������: 0x");
		cin >> hex >> addr;
		cin >> dec;
		result = VirtualAlloc(addr, lpSystemInfo.dwPageSize, MEM_RESERVE, PAGE_READWRITE);
		if (result) printf("������ ������� ��������, ����� ������ �������: 0x%p\n"
			"������ �������: %i ����\n", result, lpSystemInfo.dwPageSize);
		else printf("������ ��������� ������: %i", GetLastError());
		break;
	default:
		break;
	}
}

void MemReserve2() {
	int val;
	void* addr;
	LPVOID result;
	SYSTEM_INFO lpSystemInfo;
	GetSystemInfo(&lpSystemInfo);
	printf(
		"����� ������� �������� ������?\n"
		"1 - �������������\n"
		"2 - � ������ ���������� �������\n");
	cin >> val;
	switch (val)
	{
	case 1:
		result = VirtualAlloc(NULL, lpSystemInfo.dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (result) printf("������ ������� �������� � ��������, ����� ������ �������: 0x%p\n"
			"������ �������: %i ����\n", (void*)result, lpSystemInfo.dwPageSize);
		else printf("������ ��������� ������: %i\n", GetLastError());
		break;
	case 2:
		printf("������� ����� ���������� �������: 0x");
		cin >> hex >> addr;
		cin >> dec;
		result = VirtualAlloc(addr, lpSystemInfo.dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (result) printf("������ ������� �������� � ��������, ����� ������ �������: 0x%p\n"
			"������ �������: %i ����\n", result, lpSystemInfo.dwPageSize);
		else printf("������ ��������� ������: %i\n", GetLastError());
		break;
	default:
		break;
	}
}

void MemWrite() {
	void* addr;
	char* strAddr;
	SIZE_T res;
	string str;
	MEMORY_BASIC_INFORMATION lpBuffer;
	printf("������� ����� ������� ������: 0�");
	cin >> hex >> addr;
	cin >> dec;
	printf("������� ������ ������: ");
	cin >> str;
	res = VirtualQuery(addr, &lpBuffer, sizeof(lpBuffer));
	if (res) {
		if (!((lpBuffer.Protect & (PAGE_EXECUTE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_READWRITE | PAGE_WRITECOPY)) && (lpBuffer.State == MEM_COMMIT))) {
			printf("������: ������ �� ��������\n");
			return;
		}
		if (!(lpBuffer.RegionSize >= str.length() * sizeof(char))) {
			printf("������� ������� ������\n");
			return;
		}
		strAddr = (char*)addr;
		CopyMemory(strAddr, str.c_str(), str.length() * sizeof(char));
		printf("��������� ����� ������� ������: 0x%p\n", (void*)strAddr);
		printf("���������� ����������: ");
		for (int i = 0; i < str.length(); ++i)
			printf("%c", strAddr[i]);
		printf("\n");
	}
	else printf("������ �������� ������: %i\n", GetLastError());
}

void MemProtect() {
	void* addr;
	DWORD input, oldProtect;
	SIZE_T res;
	MEMORY_BASIC_INFORMATION lpBuffer;
	printf("������� ����� ������� ������: 0�");
	cin >> hex >> addr;
	cin >> dec;
	printf("�������� ������� ������:\n"
		"1 - PAGE_EXECUTE\n"
		"2 - PAGE_EXECUTE_READ\n"
		"3 - PAGE_EXECUTE_READWRITE\n"
		"4 - PAGE_EXECUTE_WRITECOPY\n"
		"5 - PAGE_NOACCESS\n"
		"6 - PAGE_READONLY\n"
		"7 - PAGE_READWRITE\n"
		"8 - PAGE_WRITECOPY\n"
		"9 - PAGE_TARGETS_INVALID\n");
	cin >> input;
	switch (input) {
	case 1:
		input = PAGE_EXECUTE;
		break;
	case 2:
		input = PAGE_EXECUTE_READ;
		break;
	case 3:
		input = PAGE_EXECUTE_READWRITE;
		break;
	case 4:
		input = PAGE_EXECUTE_WRITECOPY;
		break;
	case 5:
		input = PAGE_NOACCESS;
		break;
	case 6:
		input = PAGE_READONLY;
		break;
	case 7:
		input = PAGE_READWRITE;
		break;
	case 8:
		input = PAGE_WRITECOPY;
		break;
	case 9:
		input = PAGE_TARGETS_INVALID;
		break;
	}
	if (VirtualProtect(addr, sizeof(DWORD), input, &oldProtect)) {
		printf("���������� ������� ������: %i\n"
			   "����� ������� ������:      %i\n", oldProtect, input);
	}
	else printf("������ ������������ ������ ������: %i\n", GetLastError());
	res = VirtualQuery(addr, &lpBuffer, sizeof(lpBuffer));
	printf("��������: ������������� ������� ������: %i\n", lpBuffer.Protect);
}

void MemFree() {
	void* addr;
	DWORD input, oldProtect;
	MEMORY_BASIC_INFORMATION lpBuffer;
	printf("������� ����� ������� ������: 0�");
	cin >> hex >> addr;
	cin >> dec;
	if (VirtualFree(addr, 0, MEM_RELEASE)) {
		printf("������ ������ ��������\n");
	}
	else printf("������ ������������ ������� ������: %i", GetLastError());
}

void Task2() {
	int input;
	printf("����� ���������� ���������:\n"
		"1 - Writer\n"
		"2 - Reader\n");
	cin >> input;
	switch (input)
	{
	case 1:
		Writer();
		break;
	case 2:
		Reader();
		break;
	default:
		printf("�������� ����\n");
		break;
	}
}

void Writer() {
	void* mapAddr;
	string fileName, viewName, input;
	wstring wfileName, wviewName;
	LPCWSTR lpFileName, lpViewName;
	system("cls");
	printf("���������� Writer.\n");
	if (fHandle == NULL) {
		printf("������� ������ ��� ����� (C:\\folder\\file.ext): ");
		cin >> fileName;
		wfileName = wstring(fileName.begin(), fileName.end());
		lpFileName = wfileName.c_str();
		fHandle = CreateFile(lpFileName, GENERIC_ALL, NULL, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);
	}
	else printf("���� ��� ������������� ������, ���������� ������ �����������:\n");

	if (fHandle) {
		printf("������� ��� ����������� �����: ");
		cin >> viewName;
		wviewName = wstring(viewName.begin(), viewName.end());
		lpViewName = wviewName.c_str();
		fMapHandle = CreateFileMapping(fHandle, NULL, PAGE_READWRITE, 0, 256, lpViewName);

		if (fMapHandle) {
			mapAddr = MapViewOfFile(fMapHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

			if (mapAddr) {
				printf("���� ������������\n"
					"������� ������ ��� ������:\n");
				cin >> input;
				CopyMemory(mapAddr, input.c_str(), sizeof(char) * input.length());
				printf("\n������ ��������, �� ����� ������� � ��������� Reader.\n"
					"������ ��������� � ����������� � ������: %s\n", viewName.c_str());
				UnmapViewOfFile(mapAddr);
			}
			else printf("������ ������������� �����: %i\n", GetLastError());
		}
		else printf("������ ������������� �����: %i\n", GetLastError());
	}
	else printf("������ �������� �����: %i\n", GetLastError());
}

void Reader() {
	void* mapAddr;
	string viewName;
	wstring wviewName;
	LPCWSTR lpViewName;
	HANDLE fOpenHandle;
	system("cls");
	printf("���������� Reader.\n"
		"������� ��� ����������� �����: ");
	cin >> viewName;
	wviewName = wstring(viewName.begin(), viewName.end());
	lpViewName = wviewName.c_str();
	fOpenHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, lpViewName);

	if (fOpenHandle) {
		mapAddr = MapViewOfFile(fOpenHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		if (mapAddr) {
			printf("���������� ������: %s\n", (char*)mapAddr);
			UnmapViewOfFile(mapAddr);
		}
		else printf("������ �������� �������������: %i\n", GetLastError());
		CloseHandle(fOpenHandle);
	}
	else printf("������ �������� �������������: %i\n", GetLastError());
}