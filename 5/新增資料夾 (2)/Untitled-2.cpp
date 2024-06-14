#include <iostream>
#include <cstdlib> // 包含 strtol
#include <cerrno>  // 包含 errno
#include <climits> // 包含 LONG_MAX 和 LONG_MIN
#include <cstring> // 包含 strerror

int main() {
    const char* str1 = "1234";
    const char* str2 = "12abc34";
    const char* str3 = "abc123";
    const char* str4 = "!@#";

    char* end;
    errno = 0; // 重置错误标志

    long num1 = strtol(str1, &end, 10);
    std::cout << "num1: " << num1 << std::endl; // "num1: 1234
    if (errno != 0 || *end != '\0') {
        std::cout << "Error converting str1: " << str1 << std::endl;
        std::cout << "errno: " << strerror(errno) << std::endl;

    } else {
        std::cout << "num1: " << num1 << std::endl;
        std::cout << "errno: " << strerror(errno) << std::endl;

    }

    long num2 = strtol(str2, &end, 10);
    std::cout << "num2: " << num2 << std::endl; // "num2: 12
    if (errno != 0 || *end != '\0') {
        std::cout << "Error converting str2: " << str2 << std::endl;
        std::cout << "errno: " << strerror(errno) << std::endl;

    } else {
        std::cout << "num2: " << num2 << std::endl;
    }

    long num3 = strtol(str3, &end, 10);
    std::cout << "num3: " << num3 << std::endl; // "num3: 0
    if (errno != 0 || *end != '\0') {
        std::cout << "Error converting str3: " << str3 << std::endl;
        std::cout << "errno: " << strerror(errno) << std::endl;
    } else {
        std::cout << "num3: " << num3 << std::endl;
    }

    long num4 = strtol(str4, &end, 10);
    std::cout << "num4: " << num4 << std::endl; // "num4: 0
    if (errno != 0 || *end != '\0') {
        std::cout << "Error converting str4: " << str4 << std::endl;
        std::cout << "errno: " << strerror(errno) << std::endl;

    } else {
        std::cout << "num4: " << num4 << std::endl;
    }

    return 0;
}