// #include <windows.h>
// #include <iostream>
// #include <vector>

// void print_cpu_logical_vs_physical() {
//     DWORD bufferSize = 0;
//     GetLogicalProcessorInformation(nullptr, &bufferSize); // get buffer size

//     std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
//     if (!GetLogicalProcessorInformation(buffer.data(), &bufferSize)) {
//         std::cerr << "Failed to get processor information." << std::endl;
//         return;
//     }

//     int logicalCores = 0;
//     int physicalCores = 0;

//     for (auto &info : buffer) {
//         if (info.Relationship == RelationProcessorCore) {
//             physicalCores++;
//             // Count logical processors in this core
//             DWORD_PTR mask = info.ProcessorMask;
//             int count = 0;
//             for (int i = 0; i < sizeof(DWORD_PTR) * 8; ++i) {
//                 if (mask & (1ULL << i)) count++;
//             }
//             logicalCores += count;
//         }
//     }

//     std::cout << "Physical cores: " << physicalCores << std::endl;
//     std::cout << "Logical cores (with hyperthreading): " << logicalCores << std::endl;
// }

// #include <iostream>
// #include <intrin.h> // for __rdtsc

// int main() {
//     SYSTEM_INFO si;
//     GetSystemInfo(&si);

//     std::cout << "Number of logical processors: " << si.dwNumberOfProcessors << std::endl;
//     std::cout << "Page size: " << si.dwPageSize << " bytes" << std::endl;

//     DWORD_PTR mask = si.dwActiveProcessorMask;
//     std::cout << "Active CPU cores: ";
//     for (int i = 0; i < sizeof(DWORD_PTR) * 8; ++i) {
//         if (mask & (1ULL << i))
//             std::cout << i << " ";
//     }
//     std::cout << std::endl;

//     print_cpu_logical_vs_physical();

//     unsigned long long tsc = __rdtsc();
//     std::cout << "CPU timestamp counter: " << tsc << std::endl;

//     return 0;
// }

// #include <windows.h>
// #include <tlhelp32.h>
// #include <iostream>

// int main() {
//     HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//     if (hSnapshot == INVALID_HANDLE_VALUE) {
//         std::cerr << "Failed to take snapshot of processes." << std::endl;
//         return 1;
//     }
//     PROCESSENTRY32 pe;
//     pe.dwSize = sizeof(PROCESSENTRY32);
//     if (Process32First(hSnapshot, &pe)) {
//         do {
//             std::wcout << L"Process: " << pe.szExeFile << L" (PID: " << pe.th32ProcessID << L")" << std::endl;
//         } while (Process32Next(hSnapshot, &pe));
//     }
//     CloseHandle(hSnapshot);
//     return 0;
// }

// #include <windows.h>
// #include <iostream>

// int main() {
// 	unsigned char code[] = {
// 		0xB8, 0x07, 0x00, 0x00, 0x00,
// 		0x83, 0xC0, 0x23,
// 		0xC3
// 	};
// 	void* mem = VirtualAlloc(
// 		nullptr,
// 		sizeof(code),
// 		MEM_COMMIT | MEM_RESERVE,
// 		PAGE_EXECUTE_READWRITE
// 	);
// 	memcpy(mem, code, sizeof(code));
// 	int (*func)() = (int(*)())mem;
// 	int result = func();
// 	std::cout << "Result from machine code: " << result << std::endl;
// 	VirtualFree(mem, 0, MEM_RELEASE);
// 	return 0;
// }

// #include <fstream>

// int main() {
// 	unsigned char code[] = {
// 		0xB8, 0x7B, 0x00, 0x00, 0x00,
// 		0xC3
// 	};

// 	std::ofstream file("machine.bin", std::ios::binary);
// 	file.write((char*)code, sizeof(code));
// 	file.close();

// 	return 0;
// }

// #include <windows.h>
// #include <iostream>

// int main() {
// 	unsigned char code[] = {
// 		0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00, // mov rax,1        ; syscall number for write
// 		0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00, // mov rdi,1        ; file descriptor = stdout
// 		0x48, 0x8D, 0x35, 0x0A, 0x00, 0x00, 0x00, // lea rsi,[rip+10] ; address of string
// 		0x48, 0xC7, 0xC2, 0x03, 0x00, 0x00, 0x00, // mov rdx,3        ; length
// 		0x0F, 0x05,                               // syscall          ; call kernel
// 		0xC3,                                     // ret              ; return to C++
// 		'H', 'i', '\n'                             // string data
// 	};
// 	void* mem = VirtualAlloc(
// 		nullptr,
// 		sizeof(code),
// 		MEM_COMMIT | MEM_RESERVE,
// 		PAGE_EXECUTE_READWRITE
// 	);
// 	memcpy(mem, code, sizeof(code));

// 	int (*func)() = (int(*)())mem;
// 	int result = func();

// 	std::cout << "Result: " << result << std::endl;

// 	VirtualFree(mem, 0, MEM_RELEASE);
// }

#include <iostream>

int main() {
    unsigned char code[] = {
        0xB8, 0x2A, 0x00, 0x00, 0x00,
        0xC3
    };

    int (*func)() = (int(*)())code;
    int result = func();
    std::cout << "result: " << result << std::endl;
}

