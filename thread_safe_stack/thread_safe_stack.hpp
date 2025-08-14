#include <exception>
#include <memory>
#include <stack>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>

struct empty_stack: std::exception
{
    const char* what() const throw() {
        return "Stack is empty";
    }
};

template<typename T>
class threadsafe_stack
{
public:
    threadsafe_stack(){};
    threadsafe_stack(const threadsafe_stack& other)
    {
        std::lock_guard<std::mutex> lock(other.m);
        data=other.data;
    }
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;
    void push(T value)
    {
        std::lock_guard<std::mutex> lock(m);
        data.push(value);
    }
    std::shared_ptr<T> pop() 
    {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_stack();
        std::shared_ptr<T> const res(std::make_shared<T>(data.top()));  // Copy happens here
        data.pop();  // Only remove after successful copy
        return res;  // shared_ptr copy/move is noexcept
    }
    // Dangerous - what if this throws?
    /*
    T pop() {
        if (empty()) throw empty_stack();
        T result = data.top();  // What if T's copy constructor throws here?
        data.pop();             // The element is already removed!
        return result;          // Exception thrown, element is lost forever!
    }
    */

    void pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_stack();
        value=data.top();
        data.pop();
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
private:
    std::stack<T> data;
    mutable std::mutex m;
};

