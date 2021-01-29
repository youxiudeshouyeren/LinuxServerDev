#include<iostream>
#include "../sylar/log.h"
#include "../sylar/util.h"

int main(int argc, char **argv){
    sylar::Logger::ptr logger(new sylar::Logger());

    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender()));

  sylar::FileLogAppender::ptr file_appender(new sylar::FileLogAppender("./test_log.txt"));
  logger->addAppender(file_appender);



    SYLAR_LOG_INFO(logger)<<"ttest Iinfo";

    SYLAR_LOG_FMT_ERROR(logger,"test fmt logger error %s","你好啊");

    auto l=sylar::LoggerMgr::GetInstance()->getLogger("xx");
    SYLAR_LOG_INFO(l)<<"单例模式支持";

    

    return 0;
}