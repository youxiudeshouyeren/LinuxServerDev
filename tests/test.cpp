#include<iostream>
#include "sylar/log.h"

int main(int argc, char **argv){
    sylar::Logger::ptr logger(new sylar::Logger());
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

    return 0;
}