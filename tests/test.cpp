#include<iostream>
#include "../sylar/log.h"

int main(int argc, char **argv){
    sylar::Logger::ptr logger(new sylar::Logger());

    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender()));

    sylar::LogEvent::ptr event(new sylar::LogEvent(__FILE__, __LINE__,0,1,2,time(0)));

    event->getSS()<<"v1.0 日志模块. May the force be with you!"<<std::endl;

    logger->log(sylar::LogLevel::DEBUG, event);

    

    return 0;
}