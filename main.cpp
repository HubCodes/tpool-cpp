#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

using std::vector;
using std::unique_ptr;

class worker;

class thread_pool {
public:
    explicit thread_pool(uint32_t size);
private:
    uint32_t size;
    vector<unique_ptr<worker>> workers;
};

thread_pool::thread_pool(uint32_t size): size(size), workers() {}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
