#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    wstring ReaderSName = wstring(argv[0], argv[0] + strlen(argv[0]));
    wstring ReaderEvent = wstring(argv[1], argv[1] + strlen(argv[1]));
    wstring Mem = wstring(argv[2], argv[2] + strlen(argv[2]));
    

   
 

    // Відкриваємо семафор для читачів
    HANDLE hReaderSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, ReaderSName.c_str());
    HANDLE hEventR = OpenEvent(EVENT_ALL_ACCESS, TRUE, ReaderEvent.c_str());

    if (hReaderSemaphore == NULL) {
        std::cerr << "Помилка відкриття семафора для читачів." << std::endl;
        return 1;
    }

    // Отримуємо доступ до розділеної пам'яті
    HANDLE hMapFile = OpenFileMappingW(FILE_MAP_READ, TRUE,Mem.c_str());

    if (hMapFile == NULL) {
        std::cerr << "Помилка відкриття розділеної пам'яті." << std::endl;
        CloseHandle(hReaderSemaphore);
        return 1;
    }

    // Отримуємо доступ до розділеної пам'яті
    LPVOID pBuffer = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, sizeof(string));

    if (pBuffer == NULL) {
        std::cerr << "Помилка отримання доступу до розділеної пам'яті." << std::endl;
        CloseHandle(hMapFile);
        CloseHandle(hReaderSemaphore);
        return 1;
    }



    string* stringPtr = static_cast<string*>(pBuffer);
    HANDLE arrH[]{ hReaderSemaphore, hEventR };

    WaitForMultipleObjects(2, arrH, TRUE, INFINITE);

    // Читаємо дані з розділеної пам'яті
    cout << *stringPtr << endl;

    Sleep(1000);
    // Звільняємо семафор для інших читачів
    ReleaseSemaphore(hReaderSemaphore, 1, NULL);
 

    // Закриваємо ресурси
    UnmapViewOfFile(pBuffer);
    CloseHandle(hMapFile);
    CloseHandle(hReaderSemaphore);

    std::cout << "ReaderProcess finished" << std::endl;

    getchar();
    return 0;
}
