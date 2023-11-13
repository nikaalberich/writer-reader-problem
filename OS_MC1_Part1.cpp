#include <iostream>
#include <Windows.h>
#include <string>
#include <tchar.h>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    const int ReaderConst = 8;
    const int WritersConst = 5;


    const wstring reader_path = L"C:\\Users\\Ника\\source\\repos\\OS_MC1_Part1\\x64\\Debug\\Reader.exe";
    const wstring writter_path = L"C:\\Users\\Ника\\source\\repos\\OS_MC1_Part1\\x64\\Debug\\Writter.exe";

    // Створюємо імена семафорів для синхронізації
    const wstring readerSemaphoreName = L"ReaderSemaphore";
    const wstring writerSemaphoreName = L"WriterSemaphore";

    // Створення розділеної пам'яті
    const wstring sharedMemoryName = L"MySharedMemory";
    wstring ReaderEvent = L"OnReaderEvent";

    int memory_size = sizeof(string);


    // ініціалізація
    wstring readerCmd = readerSemaphoreName + L" " + ReaderEvent + L" " + sharedMemoryName;
    wstring writerCmd = writerSemaphoreName + L" " + ReaderEvent + L" " + sharedMemoryName;

    // Створюємо семафори для читачів і письменників
    HANDLE hReaderSemaphore = CreateSemaphoreW(NULL, ReaderConst, ReaderConst, &readerSemaphoreName[0]);
    HANDLE hWriterSemaphore = CreateSemaphoreW(NULL, 1, 1, &writerSemaphoreName[0]);

    if (hReaderSemaphore == NULL || hWriterSemaphore == NULL) {
        std::cerr << "Помилка створення семафорів." << std::endl;
        return 1;
    }

   
    HANDLE hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, memory_size, &sharedMemoryName[0]);

    if (hMapFile == NULL) {
        std::cerr << "Помилка створення розділеної пам'яті." << std::endl;
        CloseHandle(hReaderSemaphore);
        CloseHandle(hWriterSemaphore);
        return 1;
    }

    // Отримання доступу до розділеної пам'яті
    LPVOID pBuffer = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, memory_size);

    if (pBuffer == NULL) {
        std::cerr << "Помилка отримання доступу до розділеної пам'яті." << std::endl;
        CloseHandle(hMapFile);
        CloseHandle(hReaderSemaphore);
        CloseHandle(hWriterSemaphore);
        return 1;
    }
   
    HANDLE readers[ReaderConst], writers[WritersConst];


    // Запуск процесів C (письменників)
    for (int i = 0; i < WritersConst; i++) {

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        CreateProcess(&writter_path[0], &writerCmd[0], NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
        writers[i] = pi.hProcess;


    }

    // Запуск процесів B (читачів)
    for (int i = 0; i < ReaderConst; i++) {
       
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));


        CreateProcess(&reader_path[0], &readerCmd[0], NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
            readers[i] = pi.hProcess;
              
    }

    

    WaitForMultipleObjects(ReaderConst,readers, TRUE, INFINITE);  // Очікуємо завершення процесу
    WaitForMultipleObjects(5, writers, TRUE, INFINITE);

    // завершення роботи
    for (int i = 0; i < ReaderConst; i++)
        CloseHandle(readers[i]);
    for (int i = 0; i < WritersConst; i++)
        CloseHandle(writers[i]);


    // Закриваємо семафори
    // Закриваємо ресурси
    UnmapViewOfFile(pBuffer);
    CloseHandle(hMapFile);
    CloseHandle(hReaderSemaphore);
    CloseHandle(hWriterSemaphore);

    

    return 0;
}
