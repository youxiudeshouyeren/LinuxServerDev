#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__


#include<string>
#include<stdint.h>
#include<memory>
#include<list>
#include<fstream>
#include<vector>

namespace sylar{

//日志时间
    class LogEvent{

        public:

        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent();

        private:
        const char* m_file=nullptr; //文件名
        uint32_t m_line=0;  //行号
        uint32_t m_elapse=0; //程序启动开始到现在的毫秒数
        uint32_t m_threadId=0; //线程ID
        uint32_t m_fiberId=0;//协程ID
        std::string m_content; //
        uint64_t m_time;//时间戳


    };


//日志级别
class LogLevel{
    public:
        enum Level{
                DEBUG=1,
                INFO=2,
                WARN=3,
                ERROR=4,
                FATAL=5
            };//日志级别
};


//日志格式器
    class LogFormatter{
        public:

        typedef std::shared_ptr<LogFormatter> ptr;

        //%t %thread_id%m%n
        std::string format(LogEvent::ptr event);

        LogFormatter(const std::string& pattern);

        void init();

        private:
            class FormatterItem{
                public:
                typedef std::shared_ptr<FormatterItem> ptr;
                virtual ~FormatterItem(){}
                virtual std::string format(std::ostream&os,LogEvent::ptr event)=0;
            };
        private:
        std::string m_pattern;
        std::vector<FormatterItem::ptr> m_items;

    };

//----------------------------------------------------------------

    //日志输出落地点
    class LogAppender{

        public:

        typedef std::shared_ptr<LogAppender> ptr;

        virtual ~LogAppender(){}

        virtual void log(LogLevel::Level level,LogEvent::ptr event)=0;

        void setFormatter(LogFormatter::ptr val){this->m_formatter=val;};

        LogFormatter::ptr getFormatter(){return this->m_formatter;}

        protected:

            LogLevel::Level m_level;
            LogFormatter::ptr m_formatter;

    };

    //日志输出器
    class Logger{

        public:

            typedef std::shared_ptr<Logger> ptr;

        
            Logger(const std::string& name="root");

            void log( LogLevel::Level level,const LogEvent::ptr event);

            void debug( LogEvent::ptr event);
            void info( LogEvent::ptr event);
            void warn( LogEvent::ptr event);
            void error( LogEvent::ptr event);
            void fatal( LogEvent::ptr event);

            void addAppender(LogAppender::ptr appender);
            void delAppender(LogAppender::ptr appender);
            LogLevel::Level getLevel() const{return this->m_level;};
            void setLevel( LogLevel::Level val){this->m_level = val;};

        private:

        std::string m_name; //日志名称
        LogLevel::Level m_level; //日志名称

        std::list<LogAppender::ptr> m_appenders; //appenders集合

    };

    
//控制台输出
class StdoutLogAppender : public LogAppender{

    public:

        typedef std::shared_ptr<StdoutLogAppender> ptr;
         void log(LogLevel::Level level,LogEvent::ptr event) override;

    private:


};

//文件输出
class FileLogAppender : public LogAppender{
    public:

    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
     void log(LogLevel::Level level,LogEvent::ptr event) override;


    //重新打开文件，文件打开成功返回true
     bool reopen();

    private:

    std::string m_filename;
    std::ofstream m_filestream;
};
    
}














































#endif