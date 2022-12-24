#include <windows.h>
#include <iostream>
#include <string>
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
			"1 - получение информации о вычислительной системе\n"
			"2 - определение статуса виртуальной памяти\n"
			"3 - определение состояния конкретного участка памяти по заданному с клавиатуры адресу\n"
			"4 - резервирование региона в автоматическом режиме и в режиме ввода адреса начала региона\n"
			"5 - резервирование региона и передача ему физической памяти в автоматическом режиме и в режиме ввода адреса начала региона\n"
			"6 - запись данных в ячейки памяти по заданным с клавиатуры адресам\n"
			"7 - установку защиты доступа для заданного (с клавиатуры) региона памяти и ее проверку\n"
			"8 - возврат физической памяти и освобождение региона адресного пространства заданного(с клавиатуры) региона памяти\n"
			"9 - Использование проецируемых файлов для обмена данными между процессами\n"
			"0 - выход из программы\n");
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
		printf("Нажмите любую кнопку...");
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
	printf("Ахритектура процессора: ");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		printf("x64 (AMD или Intel)\n");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)
		printf("ARM\n");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64)
		printf("ARM64\n");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		printf("Intel Itanium\n");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		printf("x86\n");
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_UNKNOWN)
		printf("неизвестно\n");
	printf(
		"Размер страницы: %i\n"
		"Указатель на самый низкий адрес памяти, доступный для приложений и библиотек динамической компоновки (DLL): 0x%p\n"
		"Указатель на самый высокий адрес памяти, доступный для приложений и БИБЛИОТЕК DLL:                          0x%p\n"
		"Маска, представляющая набор процессоров, настроенных в системе: \n%s\n"
		"Количество логических процессоров в текущей группе: %i\n"
		"Степень детализации начального адреса, по которому можно выделить виртуальную память: %i\n"
		"Уровень процессора, зависящей от архитектуры: %i\n"
		"Редакция процессора, зависящая от архитектуры: %i\n",
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
		"Размер структуры MEMORYSTATUS: %I64d байт\n"
		"Использование памяти: %I64d%%\n\n"
		"Всего физической памяти:          %16I64d байт\n"
		"Свободно физической памяти:       %16I64d байт\n\n"
		"Всего памяти в файле подкачки:    %16I64d байт\n"
		"Свободно памяти в файле подкачки: %16I64d байт\n\n"
		"Всего виртуальной памяти:         %16I64d байт\n"
		"Свободно виртуальной памяти:      %16I64d байт\n",
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
	printf("Введите адрес участка памяти: 0х");
	cin >> hex >> address;
	res = VirtualQuery(address, &lpBuffer, sizeof(lpBuffer));
	// open MEMORY_BASIC_INFORMATION using microsoft documentation
	if (res) {
		printf("Проверен участок памяти с началом 0x%p\n", (void*)address);
		printf(
			"Указатель на базовый адрес области страниц: %p\n"
			"Указатель на базовый адрес диапазона страниц, выделенных функцией VirtualAlloc: %p\n",
			(void*)lpBuffer.BaseAddress,
			(void*)lpBuffer.AllocationBase);
		printf("Параметр защиты памяти при первоначальном выделении региона: \n");
		switch (lpBuffer.Protect)
		{
		case 0:
			printf("	Вызывающий объект не имеет доступа\n");
			break;
		case PAGE_EXECUTE:
			printf("	Позволяет выполнять доступ к зафиксированной области страниц\n");
			break;
		case PAGE_EXECUTE_READ:
			printf("	Позволяет выполнять или только для чтения доступ к зафиксированной области страниц\n");
			break;
		case PAGE_EXECUTE_READWRITE:
			printf("	Включает выполнение, доступ только для чтения или чтения и записи к зафиксированной области страниц\n");
			break;
		case PAGE_EXECUTE_WRITECOPY:
			printf("	Включает доступ на выполнение, только для чтения или копирование при записи в сопоставленное представление объекта сопоставления файлов\n");
			break;
		case PAGE_NOACCESS:
			printf("	Отключает весь доступ к зафиксированной области страниц\n");
			break;
		case PAGE_READONLY:
			printf("	Разрешает доступ только для чтения к зафиксированной области страниц\n");
			break;
		case PAGE_READWRITE:
			printf("	Включает доступ только для чтения или чтения и записи к зафиксированной области страниц\n");
			break;
		case PAGE_WRITECOPY:
			printf("	Включает доступ только для чтения или копирования при записи к сопоставленным представлениям объекта сопоставления файлов\n");
			break;
		case PAGE_TARGETS_INVALID:
			printf("	Задает все расположения на страницах в качестве недопустимых целевых объектов для CFG\n");
			printf("	Страницы в регионе не будут обновлять сведения CFG при изменении защиты для VirtualProtect\n");
			break;
		default:
			break;
		}
		if (lpBuffer.AllocationProtect & PAGE_GUARD)
			printf("	Страницы в регионе становятся страницами защиты\n");
		if (lpBuffer.AllocationProtect & PAGE_NOCACHE)
			printf("	Задает для всех страниц запрет на кэширование\n");
		if (lpBuffer.AllocationProtect & PAGE_WRITECOMBINE)
			printf("	Задает всем страницам режим объединения записей\n");
		printf(
			"Размер региона, начинающегося с базового адреса, в котором все страницы имеют одинаковые атрибуты в байтах: %u\n"
			"Состояние страниц в регионе: \n", lpBuffer.RegionSize);
		switch (lpBuffer.State)
		{
		case MEM_COMMIT:
			printf("	Зафиксированная страница, для нее было выделено физическое хранилище либо в памяти, либо в файле подкачки на диске\n");
			break;
		case MEM_FREE:
			printf("	Свободные страницы недоступны для вызывающего процесса и доступны для выделения\n");
			break;
		case MEM_RESERVE:
			printf("	Зарезервированная страница, где диапазон виртуального адресного пространства процесса зарезервирован без выделения физического хранилища\n");
			break;
		default:
			break;
		}
		printf("Тип страниц в регионе: \n");
		switch (lpBuffer.Type)
		{
		case MEM_IMAGE:
			printf("	Страницы памяти в регионе сопоставляются с представлением раздела изображения\n");
			break;
		case MEM_MAPPED:
			printf("	Страницы памяти в регионе сопоставляются с представлением раздела\n");
			break;
		case MEM_PRIVATE:
			printf("	Страницы памяти в регионе являются частными\n");
			break;
		default:
			break;
		}
	}
	else printf("Ошибка чтения участка памяти\n");
}

void MemReserve() {
	int val;
	void* addr;
	LPVOID result;
	SYSTEM_INFO lpSystemInfo;
	GetSystemInfo(&lpSystemInfo);
	printf(
		"Каким образом выделить память?\n"
		"1 - автоматически\n"
		"2 - с вводом начального региона\n");
	cin >> val;
	switch (val)
	{
	case 1:
		result = VirtualAlloc(NULL, lpSystemInfo.dwPageSize, MEM_RESERVE, PAGE_READWRITE);
		if (result) printf("Память успешно выделена, адрес начала региона: 0x%p\n"
			"Размер региона: %i байт\n", result, lpSystemInfo.dwPageSize);
		else printf("Ошибка выделения памяти: %i", GetLastError());
		break;
	case 2:
		printf("Введите адрес начального региона: 0x");
		cin >> hex >> addr;
		cin >> dec;
		result = VirtualAlloc(addr, lpSystemInfo.dwPageSize, MEM_RESERVE, PAGE_READWRITE);
		if (result) printf("Память успешно выделена, адрес начала региона: 0x%p\n"
			"Размер региона: %i байт\n", result, lpSystemInfo.dwPageSize);
		else printf("Ошибка выделения памяти: %i", GetLastError());
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
		"Каким образом выделить память?\n"
		"1 - автоматически\n"
		"2 - с вводом начального региона\n");
	cin >> val;
	switch (val)
	{
	case 1:
		result = VirtualAlloc(NULL, lpSystemInfo.dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (result) printf("Память успешно выделена и передана, адрес начала региона: 0x%p\n"
			"Размер региона: %i байт\n", (void*)result, lpSystemInfo.dwPageSize);
		else printf("Ошибка выделения памяти: %i\n", GetLastError());
		break;
	case 2:
		printf("Введите адрес начального региона: 0x");
		cin >> hex >> addr;
		cin >> dec;
		result = VirtualAlloc(addr, lpSystemInfo.dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (result) printf("Память успешно выделена и передана, адрес начала региона: 0x%p\n"
			"Размер региона: %i байт\n", result, lpSystemInfo.dwPageSize);
		else printf("Ошибка выделения памяти: %i\n", GetLastError());
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
	printf("Введите адрес участка памяти: 0х");
	cin >> hex >> addr;
	cin >> dec;
	printf("Введите строку данных: ");
	cin >> str;
	res = VirtualQuery(addr, &lpBuffer, sizeof(lpBuffer));
	if (res) {
		if (!((lpBuffer.Protect & (PAGE_EXECUTE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_READWRITE | PAGE_WRITECOPY)) && (lpBuffer.State == MEM_COMMIT))) {
			printf("Ошибка: память не выделена\n");
			return;
		}
		if (!(lpBuffer.RegionSize >= str.length() * sizeof(char))) {
			printf("Слишком длинная строка\n");
			return;
		}
		strAddr = (char*)addr;
		CopyMemory(strAddr, str.c_str(), str.length() * sizeof(char));
		printf("Начальный адрес участка памяти: 0x%p\n", (void*)strAddr);
		printf("Записанная информация: ");
		for (int i = 0; i < str.length(); ++i)
			printf("%c", strAddr[i]);
		printf("\n");
	}
	else printf("Ошибка проверки памяти: %i\n", GetLastError());
}

void MemProtect() {
	void* addr;
	DWORD input, oldProtect;
	SIZE_T res;
	MEMORY_BASIC_INFORMATION lpBuffer;
	printf("Введите адрес участка памяти: 0х");
	cin >> hex >> addr;
	cin >> dec;
	printf("Выберите уровень защиты:\n"
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
		printf("Предыдущий уровень защиты: %i\n"
			   "Новый уровень защиты:      %i\n", oldProtect, input);
	}
	else printf("Ошибка установления уровня защиты: %i\n", GetLastError());
	res = VirtualQuery(addr, &lpBuffer, sizeof(lpBuffer));
	printf("Проверка: Установленный уровень защиты: %i\n", lpBuffer.Protect);
}

void MemFree() {
	void* addr;
	DWORD input, oldProtect;
	MEMORY_BASIC_INFORMATION lpBuffer;
	printf("Введите адрес участка памяти: 0х");
	cin >> hex >> addr;
	cin >> dec;
	if (VirtualFree(addr, 0, MEM_RELEASE)) {
		printf("Регион памяти свободен\n");
	}
	else printf("Ошибка освобождения региона памяти: %i", GetLastError());
}

void Task2() {
	int input;
	printf("Какое приложение запустить:\n"
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
		printf("Неверный ввод\n");
		break;
	}
}

void Writer() {
	void* mapAddr;
	string fileName, viewName, input;
	wstring wfileName, wviewName;
	LPCWSTR lpFileName, lpViewName;
	system("cls");
	printf("Приложение Writer.\n");
	if (fHandle == NULL) {
		printf("Введите полное имя файла (C:\\folder\\file.ext): ");
		cin >> fileName;
		wfileName = wstring(fileName.begin(), fileName.end());
		lpFileName = wfileName.c_str();
		fHandle = CreateFile(lpFileName, GENERIC_ALL, NULL, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);
	}
	else printf("Файл для проецирования открыт, добавление нового отображения:\n");

	if (fHandle) {
		printf("Введите имя отображения файла: ");
		cin >> viewName;
		wviewName = wstring(viewName.begin(), viewName.end());
		lpViewName = wviewName.c_str();
		fMapHandle = CreateFileMapping(fHandle, NULL, PAGE_READWRITE, 0, 256, lpViewName);

		if (fMapHandle) {
			mapAddr = MapViewOfFile(fMapHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

			if (mapAddr) {
				printf("Файл спроецирован\n"
					"Введите строку для записи:\n");
				cin >> input;
				CopyMemory(mapAddr, input.c_str(), sizeof(char) * input.length());
				printf("\nСтрока записана, ее можно увидеть в программе Reader.\n"
					"Строка сохранена в отображении с именем: %s\n", viewName.c_str());
				UnmapViewOfFile(mapAddr);
			}
			else printf("Ошибка проецирования файла: %i\n", GetLastError());
		}
		else printf("Ошибка проецирования файла: %i\n", GetLastError());
	}
	else printf("Ошибка создания файла: %i\n", GetLastError());
}

void Reader() {
	void* mapAddr;
	string viewName;
	wstring wviewName;
	LPCWSTR lpViewName;
	HANDLE fOpenHandle;
	system("cls");
	printf("Приложение Reader.\n"
		"Введите имя отображения файла: ");
	cin >> viewName;
	wviewName = wstring(viewName.begin(), viewName.end());
	lpViewName = wviewName.c_str();
	fOpenHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, lpViewName);

	if (fOpenHandle) {
		mapAddr = MapViewOfFile(fOpenHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		if (mapAddr) {
			printf("Записанная строка: %s\n", (char*)mapAddr);
			UnmapViewOfFile(mapAddr);
		}
		else printf("Ошибка открытия проецирования: %i\n", GetLastError());
		CloseHandle(fOpenHandle);
	}
	else printf("Ошибка открытия проецирования: %i\n", GetLastError());
}
