#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "singleton.h"
#include <stdarg.h>
#include "util.h"

#define SYLAR_LOG_LEVEL(logger, level) \
    if (logger->getLevel() <= level)   \
    sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId(), sylar::GetFiberId(), time(0)))).getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if (logger->getLevel() <= level)                 \
    sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId(), sylar::GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()

#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstance()->getLogger(name)

namespace sylar
{

    class Logger;
    class LoggerManager;

    //日志级别
    class LogLevel
    {
    public:
        enum Level
        {
            UNKNOW = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        }; //日志级别

        static const char *ToString(LogLevel::Level level);
        static LogLevel::Level FromString(const std::string &str);
    };

    //日志事件
    class LogEvent
    {

    public:
        typedef std::shared_ptr<LogEvent> ptr;

        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char *file, int32_t m_line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time);

        //  ~LogEvent();

        const char *getFile() const { return m_file; }

        int32_t getLine() const { return m_line; }

        uint32_t getElapse() const { return m_elapse; }

        uint32_t getThreadId() const { return m_threadId; }

        uint32_t getFiberId() const { return m_fiberId; }

        uint64_t getTime() const { return m_time; }

        std::string getContent() const { return m_ss.str(); }

        std::stringstream &getSS() { return m_ss; }

        std::shared_ptr<Logger> getLogger() const { return m_logger; }

        LogLevel::Level getLevel() const { return m_level; }

        void format(const char *fmt, va_list al);

        void format(const char *fmt, ...);

    private:
        const char *m_file = nullptr; //文件名
        int32_t m_line = 0;           //行号
        uint32_t m_elapse = 0;        //程序启动开始到现在的毫秒数
        uint32_t m_threadId = 0;      //线程ID
        uint32_t m_fiberId = 0;       //协程ID
        std::stringstream m_ss;       //
        uint64_t m_time;              //时间戳

        std::shared_ptr<Logger> m_logger;

        LogLevel::Level m_level;
    };

    class LogEventWrap
    {

    public:
        LogEventWrap(LogEvent::ptr e);

        ~LogEventWrap();

        std::stringstream &getSS();

        LogEvent::ptr getEvent() const { return m_event; }

    private:
        LogEvent::ptr m_event;
    };

    //日志格式器
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;

        //%t %thread_id%m%n
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

        LogFormatter(const std::string &pattern);

        void init();

        bool isError()
        {
            return this->m_error;
        }

        const std::string getPattern() const{return this->m_pattern;}

    public:
        class FormatItem
        {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            virtual ~FormatItem() {}
            virtual void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;

        bool m_error = false; //该formatter的格式是否错误
    };

    //----------------------------------------------------------------

    //日志输出落地点
    class LogAppender
    {

    public:
        typedef std::shared_ptr<LogAppender> ptr;

        virtual ~LogAppender() {}

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

        void setFormatter(LogFormatter::ptr val) { this->m_formatter = val; };

        LogFormatter::ptr getFormatter() { return this->m_formatter; }

        void setLevel(LogLevel::Level level) { this->m_level = level; }

        virtual std::string toYamlString() = 0;

    protected:
        LogLevel::Level m_level = LogLevel::DEBUG; //初始化一个等级
        LogFormatter::ptr m_formatter;
    };

    //日志输出器
    class Logger : public std::enable_shared_from_this<Logger>
    {
        friend class LoggerManager;

    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger(const std::string &name = "root");

        void log(LogLevel::Level level, const LogEvent::ptr event);

        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);
        void cleanAppenders();
        LogLevel::Level getLevel() const { return this->m_level; };
        void setLevel(LogLevel::Level val) { this->m_level = val; };

        const std::string &getName() const { return this->m_name; }

        void setFormatter(LogFormatter::ptr formatter);
        void setFormatter(const std::string &value);
        LogFormatter::ptr getFormatter() const { return this->m_formatter; }

        std::string toYamlString();

    private:
        std::string m_name;      //日志名称
        LogLevel::Level m_level; //日志名称

        std::list<LogAppender::ptr> m_appenders; //appenders集合

        LogFormatter::ptr m_formatter;

        Logger::ptr m_root;
    };

    //控制台输出
    class StdoutLogAppender : public LogAppender
    {

    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

        std::string toYamlString() override;

    private:
    };

    //文件输出
    class FileLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string &filename);
        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

        //重新打开文件，文件打开成功返回true
        bool reopen();
        std::string toYamlString() override;

    private:
        std::string m_filename;
        std::ofstream m_filestream;
    };

    //Logger 管理器
    class LoggerManager
    {

    public:
        LoggerManager();

        Logger::ptr getLogger(const std::string &name);

        void init();

        Logger::ptr getRoot() const { return m_root; }

        std::string toYamlString();

        // std::

    private:
        std::map<std::string, Logger::ptr> m_loggers;
        Logger::ptr m_root;
    };

    typedef sylar::Singleton<LoggerManager> LoggerMgr;

} // namespace sylar

#endif