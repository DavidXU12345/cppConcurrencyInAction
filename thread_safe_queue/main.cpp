#include "thread_safe_queue.hpp"
#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

// Global queue for communication between threads
threadsafe_queue<int> task_queue;
threadsafe_queue<std::string> result_queue;

// Producer function - generates tasks
void producer(int producer_id, int num_tasks) {
    for(int i = 0; i < num_tasks; ++i) {
        int task = producer_id * 100 + i;
        task_queue.push(task);
        std::cout << "Producer " << producer_id << " pushed task: " << task << std::endl;
        
        // Simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Consumer function - processes tasks
void consumer(int consumer_id) {
    while(true) {
        int task;
        // Use blocking wait - this will wait until a task is available
        task_queue.wait_and_pop(task);
        
        // Check for termination signal (use negative numbers)
        if(task < 0) {
            std::cout << "Consumer " << consumer_id << " received termination signal" << std::endl;
            break;
        }
        
        // Process the task
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        std::string result = "Consumer " + std::to_string(consumer_id) + 
                           " processed task " + std::to_string(task);
        result_queue.push(result);
        
        std::cout << result << std::endl;
    }
}

// Result collector - demonstrates shared_ptr version
void result_collector(int expected_results) {
    int collected = 0;
    std::cout << "\n=== RESULTS COLLECTED ===\n";
    
    while(collected < expected_results) {
        // Use shared_ptr version
        auto result = result_queue.wait_and_pop();
        std::cout << "RESULT: " << *result << std::endl;
        collected++;
    }
    std::cout << "=== ALL RESULTS COLLECTED ===\n";
}

int main() {
    std::cout << "Starting Producer-Consumer Example with Thread-Safe Queue\n\n";
    
    const int num_producers = 2;
    const int num_consumers = 3;
    const int tasks_per_producer = 5;
    const int total_tasks = num_producers * tasks_per_producer;
    
    std::vector<std::thread> threads;
    
    // Start result collector thread
    threads.emplace_back(result_collector, total_tasks);
    
    // Start consumer threads
    for(int i = 0; i < num_consumers; ++i) {
        threads.emplace_back(consumer, i + 1);
    }
    
    // Start producer threads
    for(int i = 0; i < num_producers; ++i) {
        threads.emplace_back(producer, i + 1, tasks_per_producer);
    }
    
    // Wait for producers to finish
    for(int i = num_consumers + 1; i < threads.size(); ++i) {
        threads[i].join();
    }
    std::cout << "All producers finished\n";
    
    // Wait for result collector to finish
    threads[0].join();
    std::cout << "Result collector finished\n";
    
    // Send termination signals to consumers (negative numbers)
    std::cout << "Sending termination signals to consumers...\n";
    for(int i = 0; i < num_consumers; ++i) {
        task_queue.push(-1);
    }
    
    // Wait for consumers to terminate
    for(int i = 1; i <= num_consumers; ++i) {
        threads[i].join();
    }
    std::cout << "All consumers finished\n";
    
    std::cout << "\nAll threads completed successfully!\n";
    
    // Demonstrate copy constructor
    std::cout << "\nTesting copy constructor...\n";
    threadsafe_queue<int> original_queue;
    original_queue.push(100);
    original_queue.push(200);
    original_queue.push(300);
    
    // Copy the queue
    threadsafe_queue<int> copied_queue = original_queue;
    
    std::cout << "Original queue empty: " << (original_queue.empty() ? "true" : "false") << std::endl;
    std::cout << "Copied queue empty: " << (copied_queue.empty() ? "true" : "false") << std::endl;
    
    // Pop from copied queue
    int value;
    if(copied_queue.try_pop(value)) {
        std::cout << "Popped from copied queue: " << value << std::endl;
    }
    
    return 0;
}