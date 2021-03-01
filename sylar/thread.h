#ifndef __SYLAR_THREAD_H__
#define __SYLAR_THREAD_H__

#include <functional>
#include <memory>
#include <thread>

namespace sylar {

class Thread {
  public:
    typedef std::shared_ptr<Thread> ptr;

    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    const std::string& getName() const
    {
        return m_name;
    }

    pid_t getId() const
    {
        return m_id;
    }


    void join();

    static Thread* GetThis();

    
    static const std::string& GetName();

    static void SetName(const std::string& name);

  private:
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;
    Thread(const Thread&&)           = delete;  //禁止拷贝

    static void* run(void* arg);

  private:
    pid_t                 m_id=-1;  //线程ID
    pthread_t             m_thread=0;
    std::function<void()> m_cb;
    std::string           m_name;
};

}  // namespace sylar

#endif