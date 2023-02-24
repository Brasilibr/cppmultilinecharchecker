
#ifndef fconcurrentqueue_h
#define fconcurrentqueue_h
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

template<typename Data>
class ConcurrentQueue
{
private:
    std::queue<Data> the_queue;
    std::mutex the_mutex;
    std::condition_variable the_condition_variable;
public:
    void push(Data data)
    {
        std::unique_lock<std::mutex> ulk(the_mutex);
        the_queue.push(data);
        the_condition_variable.notify_one();
    }
    bool empty()
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        return the_queue.empty();
    }
    Data try_pop()
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        if(the_queue.empty())
        {
            return NULL;
        }
        
        Data ret = the_queue.front();
        the_queue.pop();
        return ret;
    }

    Data wait_and_pop()
    {
        std::unique_lock<std::mutex> ulk(the_mutex);
        while(the_queue.empty())
        {
            the_condition_variable.wait(ulk);
        }
        Data ret =the_queue.front();
        the_queue.pop();
        return ret;
    }
};

#endif