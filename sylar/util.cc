#include "util.h"


namespace sylar{

    //获取线程ID
    pid_t GetThreadId(){
        return syscall(SYS_gettid);
        }


    //获取协程ID
    uint32_t GetFiberId(){
        return 0;
    }
}