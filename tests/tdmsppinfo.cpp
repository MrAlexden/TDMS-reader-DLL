#include <iostream>
#include <vector>

#define NOLOG

#include "../src/tdms.hpp"
#include "../src/log.hpp"

TDMS::file * f;

std::string str_tolower(const std::string & s)
{
    std::string out = s;
    for (int i = 0; i < strlen(out.c_str()); ++i)
        out[i] = tolower(s[i]);

    return out;
}

extern "C" __declspec(dllexport) int __stdcall readTDMS(const char* FILE_PATH)
{
    f = new TDMS::file(FILE_PATH);
    
    return 0;
}

extern "C" __declspec(dllexport) unsigned long long __stdcall get_channel_bytes(const char* CHANNEL_NAME)
{
    if (f->cleared)
        return 0;

    setlocale(LC_ALL, "ru"); // чтобы tolower работал с кирилицей

    for (TDMS::object* o : *f)
        if (o->_has_data)
            if (str_tolower(o->get_path()).find(str_tolower((std::string)CHANNEL_NAME)) != std::string::npos)
                return o->bytes();

    return 0;
}

extern "C" __declspec(dllexport) float* __stdcall get_channel_dataptr(const char* CHANNEL_NAME)
{
    if (f->cleared)
        return nullptr;

    setlocale(LC_ALL, "ru"); // чтобы tolower работал с кирилицей

    for (TDMS::object* o : *f)
        if (o->_has_data)
            if (str_tolower(o->get_path()).find(str_tolower((std::string)CHANNEL_NAME)) != std::string::npos)
                return (float*)o->data();

    return nullptr;
}

extern "C" __declspec(dllexport) int __stdcall clear()
{
    if (f->cleared)
        return -1;

    delete f; 

    return 0;
}

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring & wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(LC_ALL, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(LC_ALL, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string & str)
{
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(LC_ALL, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(LC_ALL, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,  // handle to DLL module
                    DWORD fdwReason,     // reason for calling function
                    LPVOID lpvReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:

        if (lpvReserved != nullptr)
        {
            break; // do not do cleanup if process termination scenario
        }

        if (f->cleared)
            break;

        delete f;

        // Perform any necessary cleanup.
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}