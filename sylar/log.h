#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include "singleton.h"
#include "util.h"
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <stdarg.h>
#include <stdint.h>
#include <string>
#include <vector>

/**
 * @brief 输入Logger和Level，返回输出流： 控制台输出流、文件输出流
 *
 * 作为下面几种宏的base
 */
#define SYLAR_LOG_LEVEL(logger, level)                                         \
    if (logger->getLevel() <= level)                                           \
    sylar::LogEventWrap(                                                       \
        sylar::LogEvent::ptr(new sylar::LogEvent(                              \
            logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId(),        \
            sylar::GetFiberId(), time(0))))                                    \
        .getSS()

/**
 * @brief 使用logger写入日志界别为debug的日志（流式日志）
 */
#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)

/**
 * @brief 使用logger写入日志级别为INFO的日志（流式日志）
 */
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)

/**
 * @brief 使用logger写入日志级别为WARN的日志
 */
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)

/**
 * @brief 使用logger写入日志级别为ERROR的日志
 */
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)

/**
 * @brief 使用logger写入日志级别为FATAL的日志
 */
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

/**
 * @brief
 */
#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...)                           \
    if (logger->getLevel() <= level)                                           \
    sylar::LogEventWrap(                                                       \
        sylar::LogEvent::ptr(new sylar::LogEvent(                              \
            logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId(),        \
            sylar::GetFiberId(), time(0))))                                    \
        .getEvent()                                                            \
        ->format(fmt, __VA_ARGS__)

#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...)                                  \
    SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...)                                   \
    SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...)                                   \
    SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...)                                  \
    SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...)                                  \
    SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()

#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstance()->getLogger(name)

namespace sylar {

/**
 * @brief Logger类的声明
 */
class Logger;

/**
 * @brief LoggerManager类的声明
 */
class LoggerManager;

/**
 * @brief 日志级别类 定义了多种日志级别
 * 包括：
 * 0：UNKNOW
 * 1：DEBUG
 * 2:INFO
 * 3:WARN
 * 4:ERROR
 * 5:FATAL
 */
class LogLevel {
  public:
    /**
     * @brief 日志级别的枚举
     */
    enum Level {
        UNKNOW = 0,
        DEBUG  = 1,
        INFO   = 2,
        WARN   = 3,
        ERROR  = 4,
        FATAL  = 5
    };

    /**
     * @brief 将枚举转换为std::string
     * @param  level            日志级别
     * @return const char*
     */
    static const char* ToString(LogLevel::Level level);

    /**
     * @brief 从std::string转换为枚举
     * @param  str              待转换字符串 ，包括大小写，如 DEBUG，error
     * @return LogLevel::Level
     */
    static LogLevel::Level FromString(const std::string& str);
};

/**
 * @brief 日志事件类
 */
class LogEvent {
  public:
    typedef std::shared_ptr<LogEvent> ptr;
    /**
     * @brief Construct a new Log Event object
     * @param  logger           日志器
     * @param  level            日志时间等级
     * @param  file             文件名
     * @param  m_line           行号
     * @param  elapse           程序启动到现在的时间（ms)
     * @param  thread_id        线程号
     * @param  fiber_id         协程号
     * @param  time             日志事件（s)
     */
    LogEvent(std::shared_ptr<Logger> logger,
             LogLevel::Level         level,
             const char*             file,
             int32_t                 m_line,
             uint32_t                elapse,
             uint32_t                thread_id,
             uint32_t                fiber_id,
             uint64_t                time);

    /**
     * @brief 返回文件
     * @return const char*
     */
    const char* getFile() const
    {
        return m_file;
    }

    /**
     * @brief 返回行号
     * @return int32_t
     */
    int32_t getLine() const
    {
        return m_line;
    }

    /**
     * @brief 返回程序启动到现在的时间
     * @return uint32_t
     */
    uint32_t getElapse() const
    {
        return m_elapse;
    }

    /**
     * @brief 返回线程号
     * @return uint32_t
     */
    uint32_t getThreadId() const
    {
        return m_threadId;
    }

    /**
     * @brief 返回协程号
     * @return uint32_t
     */
    uint32_t getFiberId() const
    {
        return m_fiberId;
    }

    /**
     * @brief 返回时间
     * @return uint64_t
     */
    uint64_t getTime() const
    {
        return m_time;
    }

    /**
     * @brief 返回日志消息内容
     * @return std::string
     */
    std::string getContent() const
    {
        return m_ss.str();
    }

    /**
     * @brief 返回字符串流
     * @return std::stringstream&
     */
    std::stringstream& getSS()
    {
        return m_ss;
    }

    /**
     * @brief 返回日志器
     * @return std::shared_ptr<Logger>
     */
    std::shared_ptr<Logger> getLogger() const
    {
        return m_logger;
    }

    /**
     * @brief 返回日志级别
     * @return LogLevel::Level
     */
    LogLevel::Level getLevel() const
    {
        return m_level;
    }

    /**
     * @brief 设定格式
     * @param  fmt              My Param doc
     * @param  al               My Param doc
     */
    void format(const char* fmt, va_list al);

    /**
     * @brief 设定格式
     * @param  fmt              My Param doc
     * @param  ...              My Param doc
     */
    void format(const char* fmt, ...);

  private:
    const char*       m_file     = nullptr;  //文件名
    int32_t           m_line     = 0;        //行号
    uint32_t          m_elapse   = 0;  //程序启动开始到现在的毫秒数
    uint32_t          m_threadId = 0;  //线程ID
    uint32_t          m_fiberId  = 0;  //协程ID
    std::stringstream m_ss;            //字符串流  vbnmn
    uint64_t          m_time;          //时间戳

    std::shared_ptr<Logger> m_logger;

    LogLevel::Level m_level;
};

class LogEventWrap {
  public:
    LogEventWrap(LogEvent::ptr e);

    ~LogEventWrap();

    std::stringstream& getSS();

    LogEvent::ptr getEvent() const
    {
        return m_event;
    }

  private:
    LogEvent::ptr m_event;
};

//日志格式器
class LogFormatter {
  public:
    typedef std::shared_ptr<LogFormatter> ptr;

    //%t %thread_id%m%n
    std::string format(std::shared_ptr<Logger> logger,
                       LogLevel::Level         level,
                       LogEvent::ptr           event);

    LogFormatter(const std::string& pattern);

    void setFormatter(LogFormatter::ptr formatter);

    void init();

    bool isError()
    {
        return this->m_error;
    }

    const std::string getPattern() const
    {
        return this->m_pattern;
    }

  public:
    class FormatItem {
      public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual void format(std::ostream&           os,
                            std::shared_ptr<Logger> logger,
                            LogLevel::Level         level,
                            LogEvent::ptr           event) = 0;
    };

  private:
    std::string m_pattern;

    LogFormatter::ptr m_formatter;

    std::vector<FormatItem::ptr> m_items;

    bool m_error = false;  //该formatter的格式是否错误

    bool m_hasFormatter = false;
};

//----------------------------------------------------------------

//日志输出落地点
class LogAppender {
    friend class Logger;

  public:
    typedef std::shared_ptr<LogAppender> ptr;

    virtual ~LogAppender() {}

    virtual void log(std::shared_ptr<Logger> logger,
                     LogLevel::Level         level,
                     LogEvent::ptr           event) = 0;

    void setFormatter(LogFormatter::ptr val)
    {
        this->m_formatter = val;
    };

    LogFormatter::ptr getFormatter()
    {
        return this->m_formatter;
    }

    void setLevel(LogLevel::Level level)
    {
        this->m_level = level;
    }

    virtual std::string toYamlString() = 0;

  protected:
    LogLevel::Level   m_level = LogLevel::DEBUG;  //初始化一个等级
    LogFormatter::ptr m_formatter;
    bool m_hasFormatter = false;  //标识该Appender有无formatter
};

//日志输出器
class Logger : public std::enable_shared_from_this<Logger> {
    friend class LoggerManager;

  public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name = "root");

    void log(LogLevel::Level level, const LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void            addAppender(LogAppender::ptr appender);
    void            delAppender(LogAppender::ptr appender);
    void            cleanAppenders();
    LogLevel::Level getLevel() const
    {
        return this->m_level;
    };
    void setLevel(LogLevel::Level val)
    {
        this->m_level = val;
    };

    const std::string& getName() const
    {
        return this->m_name;
    }

    void              setFormatter(LogFormatter::ptr formatter);
    void              setFormatter(const std::string& value);
    LogFormatter::ptr getFormatter() const
    {
        return this->m_formatter;
    }

    std::string toYamlString();

  private:
    std::string     m_name;   //日志名称
    LogLevel::Level m_level;  //日志名称

    std::list<LogAppender::ptr> m_appenders;  // appenders集合

    LogFormatter::ptr m_formatter;

    Logger::ptr m_root;
};

//控制台输出
class StdoutLogAppender : public LogAppender {
  public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void                                       log(Logger::ptr     logger,
                                                   LogLevel::Level level,
                                                   LogEvent::ptr   event) override;

    std::string toYamlString() override;

  private:
};

//文件输出
class FileLogAppender : public LogAppender {
  public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(Logger::ptr     logger,
             LogLevel::Level level,
             LogEvent::ptr   event) override;

    //重新打开文件，文件打开成功返回true
    bool        reopen();
    std::string toYamlString() override;

  private:
    std::string   m_filename;
    std::ofstream m_filestream;
};

// Logger 管理器
class LoggerManager {
  public:
    LoggerManager();

    Logger::ptr getLogger(const std::string& name);

    void init();

    Logger::ptr getRoot() const
    {
        return m_root;
    }

    std::string toYamlString();

    // std::

  private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr                        m_root;
};

typedef sylar::Singleton<LoggerManager> LoggerMgr;

}  // namespace sylar

#endif