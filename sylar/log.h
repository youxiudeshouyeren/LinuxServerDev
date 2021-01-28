#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__


#include<string>
#include<stdint.h>
#include<memory>
#include<list>
#include<fstream>

#include<vector>

namespace sylar{

    class Logger;

//日志事件
    class LogEvent{

        public:

        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent(const char* file,int32_t m_line,uint32_t elapse,uint32_t thread_id,uint32_t fiber_id,uint64_t time);

        const char* getFile()const{return m_file;}

        int32_t getLine() const {return m_line;}

        uint32_t getElapse()const {return m_elapse;}

        uint32_t getThreadId() const{return m_threadId;}

        uint32_t getFiberId() const{return m_fiberId;}

        uint64_t getTime() const {return m_time;}

        const std::string& getContent() const{return m_content;}


        private:
        const char* m_file=nullptr; //文件名
        int32_t m_line=0;  //行号
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
                UNKNOWN=0,
                DEBUG=1,
                INFO=2,
                WARN=3,
                ERROR=4,
                FATAL=5
            };//日志级别
            
            
        static const char* ToString(LogLevel::Level level);
};


//日志格式器
    class LogFormatter{
        public:

        typedef std::shared_ptr<LogFormatter> ptr;

        //%t %thread_id%m%n
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event);

        LogFormatter(const std::string& pattern);

        void init();

        public:
            class FormatItem{
                public:
              
                typedef std::shared_ptr<FormatItem> ptr;
                virtual ~FormatItem(){}
                virtual void format(std::ostream&os,std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)=0;
            };
        private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;

    };

//----------------------------------------------------------------

    //日志输出落地点
    class LogAppender{

        public:

        typedef std::shared_ptr<LogAppender> ptr;

        virtual ~LogAppender(){}

        virtual void log(std::shared_ptr<Logger>logger, LogLevel::Level level,LogEvent::ptr event)=0;

        void setFormatter(LogFormatter::ptr val){this->m_formatter=val;};

        LogFormatter::ptr getFormatter(){return this->m_formatter;}

        protected:

            LogLevel::Level m_level;
            LogFormatter::ptr m_formatter;

    };

    //日志输出器
    class Logger :public std::enable_shared_from_this<Logger>{

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

            const std::string& getName() const{return this->m_name;}

        private:

        std::string m_name; //日志名称
        LogLevel::Level m_level; //日志名称

        std::list<LogAppender::ptr> m_appenders; //appenders集合

        LogFormatter::ptr m_formatter;

    };

    
//控制台输出
class StdoutLogAppender : public LogAppender{

    public:

        typedef std::shared_ptr<StdoutLogAppender> ptr;
         void log(Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override;

    private:


};

//文件输出
class FileLogAppender : public LogAppender{
    public:

    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
     void log(Logger::ptr  logger, LogLevel::Level level,LogEvent::ptr event) override;


    //重新打开文件，文件打开成功返回true
     bool reopen();

    private:

    std::string m_filename;
    std::ofstream m_filestream;
};
    
}














































#endif