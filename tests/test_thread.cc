#include "../sylar/sylar.h"

#include<iostream>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void fun1()
{
       SYLAR_LOG_INFO(g_logger)
        << "name: " << sylar::Thread::GetName()
        << " this.name: " << sylar::Thread::GetThis()->getName()
        << " id: " << sylar::GetThreadId()
        << " this.id: " << sylar::Thread::GetThis()->getId() << std::endl;
}

void fun2() {}

int main(int argc, char** argv)
{
    std::vector<sylar::Thread::ptr> thrs;
    for (int i = 0; i < 5; ++i) {
        sylar::Thread::ptr thr(
            new sylar::Thread(&fun1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
        std::cout<<"加入yige"<<std::endl;
    }

    for (size_t i = 0; i <thrs.size(); ++i) {
        thrs[i]->join();
        std::cout<<"size个数"<<thrs.size()<<std::endl;
    }

    return 0;
}