#include <iostream>

int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(5, 7);
    std::cout << "The result is: " << result << std::endl;
    return 0;
}