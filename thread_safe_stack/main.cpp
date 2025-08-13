#include "thread_safe_stack.hpp"

int main()
{
    // create multiple threads and push / pop values from the stack
    threadsafe_stack<int> stack;
    std::vector<std::thread> threads;
    for(int i=0;i<10;++i)
    {
        threads.push_back(std::thread([&stack, i](){
            stack.push(i);
            std::cout << "Pushed value: " << i << std::endl;
            auto value=stack.pop();
            std::cout << "Popped value: " << *value << std::endl;
        }));
    }
    for(auto& thread: threads)
    {
        thread.join();
    }
    
    return 0;
}