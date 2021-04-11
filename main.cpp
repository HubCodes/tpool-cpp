#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <queue>
#include <utility>
#include <vector>

using std::vector;
using std::shared_ptr;
using std::unique_ptr;

class work_queue {
public:
    explicit work_queue();
    void enqueue(const std::function<void()>& task);
    std::function<void()> dequeue();
    bool has_item();
private:
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_lock;
};

work_queue::work_queue(): tasks(), tasks_lock() {}

void work_queue::enqueue(const std::function<void()>& task) {
    std::lock_guard<std::mutex> lock_guard(tasks_lock);

    tasks.push(task);
}

std::function<void()> work_queue::dequeue() {
    decltype(dequeue()) task;
    {
        std::lock_guard<std::mutex> lock_guard(tasks_lock);

        task = tasks.front();
        tasks.pop();
    }

    return task;
}

bool work_queue::has_item() {
    std::lock_guard<std::mutex> lock_guard(tasks_lock);

    return !tasks.empty();
}

class worker {
public:
    explicit worker(shared_ptr<work_queue> queue, int id);

    [[noreturn]] void execute();
    void join();
private:
    int id;
    std::thread thread;
    shared_ptr<work_queue> queue;
};

worker::worker(shared_ptr<work_queue> queue, int id):
    id(id),
    thread(),
    queue(std::move(queue))
{
    thread = std::thread([this] { execute(); });
}

[[noreturn]] void worker::execute() {
    for (;;) {
        if (queue->has_item()) {
            auto task = queue->dequeue();
            task();
        }
    }
}

void worker::join() {
    thread.join();
}

class thread_pool {
public:
    static unique_ptr<thread_pool> make_thread_pool(uint32_t size);
    void execute(const std::function<void()>& task);
    void join();
private:
    explicit thread_pool(uint32_t size);
    vector<unique_ptr<worker>> workers;
    shared_ptr<work_queue> queue;
};

unique_ptr<thread_pool> thread_pool::make_thread_pool(uint32_t size) {
    if (size == 0) {
        return nullptr;
    }

    return unique_ptr<thread_pool>(new thread_pool(size));
}

thread_pool::thread_pool(uint32_t size):
    workers(),
    queue(std::make_shared<work_queue>())
{
    for (uint32_t i = 0; i < size; i++) {
        workers.push_back(std::make_unique<worker>(queue, i));
    }
}

void thread_pool::execute(const std::function<void()>& task) {
    queue->enqueue(task);
}

void thread_pool::join() {
    std::for_each(
        workers.begin(),
        workers.end(),
        [](const unique_ptr<worker>& worker) {
            worker->join();
        }
    );
}

int main() {
    auto pool = thread_pool::make_thread_pool(4);

    for (int i = 0; i < 100; i++) {
        pool->execute([i]() {
            std::string number = std::to_string(i) + '\n';
            std::cout << number;
        });
    }
    pool->join();

    return 0;
}
