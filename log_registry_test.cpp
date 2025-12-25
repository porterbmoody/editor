#include <iostream>

int main() {
    int length = 5;
    int width = 5;

    int area = length * width;
    int perimeter = 2 * (length + width);

    std::cout << "Rectangle Properties:" << std::endl;
    std::cout << "Length: " << length << std::endl;
    std::cout << "Width: " << width << std::endl;
    std::cout << "Area: " << area << std::endl;
    std::cout << "Perimeter: " << perimeter << std::endl;

    return 0;
}