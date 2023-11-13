#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
   
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    wstring WritterSName = wstring(argv[0], argv[0] + strlen(argv[0]));
    wstring ReaderEvent = wstring(argv[1], argv[1] + strlen(argv[1]));
    wstring MemW = wstring(argv[2], argv[2] + strlen(argv[2]));
    
    
    
    cout << "Writer start process..." << endl;

    // Відкриваємо семафор для письменників
    HANDLE hWriterSemaphore = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, WritterSName.c_str());
    HANDLE hEventR = OpenEvent(EVENT_ALL_ACCESS, TRUE, ReaderEvent.c_str());

    if (hWriterSemaphore == NULL) {
        std::cerr << "Помилка відкриття семафора для письменників." << std::endl;
        return 1;
    }

    // Очікуємо, поки семафор для письменників буде відкритий
    WaitForSingleObject(hWriterSemaphore, INFINITE);
    SetEvent(hEventR);

    // Записуємо дані в розділену пам'ять
    const wchar_t* dataToWrite = L"Дані від письменника";
    // Отримуємо дескриптор розділеної пам'яті
    HANDLE hMapFile = OpenFileMappingW(FILE_MAP_WRITE, FALSE, MemW.c_str());
    if (hMapFile != NULL) {

        LPVOID pBuffer = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, sizeof(string));
        if (pBuffer != NULL) {
            string* stringPtr = static_cast<string*>(pBuffer);
            *stringPtr = "text";
            UnmapViewOfFile(pBuffer);
        }
        CloseHandle(hMapFile);
    }
    Sleep(1000);
    ResetEvent(hEventR);
    // Звільняємо семафор для інших письменників

    ReleaseSemaphore(hWriterSemaphore, 1, NULL);

    // Закриваємо дескриптор семафора
    CloseHandle(hWriterSemaphore);

    std::cout << "WriterProcess finished" << std::endl;

    getchar();
    return 0;
}
