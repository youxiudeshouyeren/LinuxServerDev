
#include <iostream>
#include <sstream>
#include <map>
#include "log.h"
#include <functional>
#include <time.h>
#include <string.h>
#include "config.h"

namespace sylar
{

    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char *file, int32_t m_line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time)
        : m_file(file),
          m_line(m_line),
          m_elapse(elapse),
          m_threadId(thread_id),
          m_fiberId(fiber_id),
          m_time(time),
          m_logger(logger),
          m_level(level)
    {
    }

    void LogEvent::format(const char *fmt, va_list al)
    {
        char *buf = nullptr;
        int len = vasprintf(&buf, fmt, al);
        if (len != -1)
        {
            m_ss << std::string(buf, len);
            free(buf);
        }
    }

    void LogEvent::format(const char *fmt, ...)
    {
        va_list al;
        va_start(al, fmt);
        format(fmt, al);
        va_end(al);
    }

    LogEventWrap::LogEventWrap(LogEvent::ptr e) : m_event(e)
    {
    }

    LogEventWrap::~LogEventWrap()
    {

        m_event->getLogger()->log(m_event->getLevel(), m_event);
    }

    std::stringstream &LogEventWrap::getSS()
    {
        return m_event->getSS();
    }

    /*----------------------------------------------------------------
*
*/

    const char *LogLevel::ToString(LogLevel::Level level)
    {
        switch (level)
        {

#define XX(name)         \
    case LogLevel::name: \
        return #name;    \
        break;

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);

#undef XX

        default:
            return "UNKNOWN";
        }
    }

    LogLevel::Level LogLevel::FromString(const std::string &str)
    {

#define XX(name)               \
    if (str == #name)          \
    {                          \
        return LogLevel::name; \
    }

        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
        return LogLevel::UNKNOWN;

#undef XX
    }
    class MessageFormatItem : public LogFormatter::FormatItem
    {
    public:
        MessageFormatItem(const std::string &str = "")
        {
            //std::cout <<"构造MessageFormatItem"<<std::endl;
        }
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {

            //   std::cout<<"调用messageForma1"<<std::endl;
            os << event->getContent();
        }
    };

    //等级
    class LevelFormatItem : public LogFormatter::FormatItem
    {

    public:
        LevelFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << LogLevel::ToString(level);
        }
    };

    //消息主体
    class ElaspseFormatItem : public LogFormatter::FormatItem
    {

    public:
        ElaspseFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getContent();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem
    {

    public:
        NameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << logger->getName();
        }
    };

    //线程号
    class ThreadIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getThreadId();
        }
    };

    //协程号
    class FiberIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        FiberIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFiberId();
        }
    };

    //时间
    class DateTimeFormatItem : public LogFormatter::FormatItem
    {
    public:
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S") : m_format(format)
        {
            if (m_format.empty())
            {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }

        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            struct tm tm;
            time_t time = event->getTime();
            localtime_r(&time, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), m_format.c_str(), &tm);
            os << buf;
        }

    private:
        std::string m_format;
    };

    //文件名
    class FilenameFormatItem : public LogFormatter::FormatItem
    {
    public:
        FilenameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFile();
        }
    };

    //行号
    class LineFormatItem : public LogFormatter::FormatItem
    {
    public:
        LineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLine();
        }
    };

    //换行
    class NewLineFormatItem : public LogFormatter::FormatItem
    {
    public:
        NewLineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << std::endl;
        }
    };

    //
    class StringFormatItem : public LogFormatter::FormatItem
    {
    public:
        StringFormatItem(const std::string &str) : m_string(str) {}

        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << m_string;
        }

    private:
        std::string m_string;
    };

    //新增Tab类 v2.0版本

    class TabFormatItem : public LogFormatter::FormatItem
    {
    public:
        TabFormatItem(const std::string &str) {}

        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << "\t";
        }
    };
    /*----------------------------------------------------------------
*
*/
    Logger::Logger(const std::string &name) : m_name(name), m_level(LogLevel::Level::DEBUG)
    {
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T [%p]%T[%t]%T[%c]%T %f:%l%T %m  %n"));
        if (name == "root")
        {
            m_appenders.push_back(StdoutLogAppender::ptr(new StdoutLogAppender)); //默认配置
        }
    }

    void Logger::addAppender(LogAppender::ptr appender)
    {
        if (!appender->getFormatter())
        {
            appender->setFormatter(m_formatter); //如果appender没有formatter 就把自己的给他
        }
        m_appenders.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender)
    {
        for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it)
        {
            if (*it == appender)
            {
                m_appenders.erase(it);
                break;
            }
        }
    }

    void Logger::cleanAppenders()
    {
        m_appenders.clear();
    }

    void Logger::setFormatter(LogFormatter::ptr formatter)
    {

        this->m_formatter = formatter;
    }

    void Logger::setFormatter(const std::string &value)
    {
        sylar::LogFormatter::ptr new_val(new sylar::LogFormatter(value));
        if (new_val->isError())
        {
            std::cout << "Logger  set  formatter name=" << m_name << "  value= " << value << " invalid formatter" << std::endl; //不能在日志里使用日志 防止死循环
            return;                                                                                                             // 格式错误则不更改默认值
        }
        this->m_formatter = new_val;
    }

    void Logger::log(LogLevel::Level level, const LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            auto self = shared_from_this();
            if (!m_appenders.empty())
            { //如果appender为空 调用root Logger的log
                for (auto &i : m_appenders)
                {
                    //  std::cout <<"Jinruappenders"<<m_appenders.size()<< std::endl;

                    i->log(self, level, event);
                }
            }
            else
            {
                m_root->log(level, event);
            }
        }
    }

    void Logger::debug(LogEvent::ptr event)
    {
        log(LogLevel::DEBUG, event);
    }

    void Logger::info(LogEvent::ptr event)
    {
        log(LogLevel::INFO, event);
    }

    void Logger::warn(LogEvent::ptr event)
    {
        log(LogLevel::WARN, event);
    }

    void Logger::error(LogEvent::ptr event)
    {
        log(LogLevel::ERROR, event);
    }

    void Logger::fatal(LogEvent::ptr event)
    {
        log(LogLevel::FATAL, event);
    }

    /*----------------------------------------------------------------
*
*/
    FileLogAppender::FileLogAppender(const std::string &filename) : m_filename(filename)
    {
        reopen();
    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            m_filestream << m_formatter->format(logger, level, event);
        }
    }

    bool FileLogAppender::reopen()
    {
        if (this->m_filestream)
        {
            m_filestream.close(); //文件打开则关闭
        }
        m_filestream.open(m_filename); //重新打开

        return !!m_filestream; //返回打开状态
    }

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= this->m_level)

        {
            // std::cout<<"jinru stdapemasdasf"<<std::endl;
            std::cout << m_formatter->format(logger, level, event) << std::endl;
        }
    }

    /*----------------------------------------------------------------
*
*/

    LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern)
    {
        init();
    }

    //日志格式的解析
    void LogFormatter::init()
    {

        //str format type
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string nstr;
        std::cout << m_pattern << std::endl;
        for (size_t i = 0; i < m_pattern.size(); ++i)
        {
            if (m_pattern[i] != '%')
            {
                nstr.append(1, m_pattern[i]);
                continue;
            }
            if ((i + 1) < m_pattern.size())
            {
                if (m_pattern[i + 1] == '%')
                {
                    nstr.append(1, '%'); //对%的转义
                    continue;
                }
            }

            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;
            std::string fmt;
            std::string str;

            while (n < m_pattern.size())
            {
                if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}'))
                {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break; //
                }

                if (fmt_status == 0)
                {

                    if (m_pattern[n] == '{')
                    {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        fmt_status = 1; //解析格式
                        fmt_begin = n;
                        ++n;

                        continue;
                    }
                }
                else if (fmt_status == 1)
                {
                    if (m_pattern[n] == '}')
                    {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if (n == m_pattern.size())
                {
                    if (str.empty())
                    {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }
            if (fmt_status == 0)
            {
                if (!nstr.empty())
                {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }

                //str = m_pattern.substr(i + 1, n - i - 1);
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            }
            else if (fmt_status == 1)
            {
                std::cout << "pattern parse error:" << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
            else if (fmt_status == 2)
            {
                if (!nstr.empty())
                {
                    vec.push_back(std::make_tuple(nstr, "", 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
        }
        if (!nstr.empty())
        {
            vec.push_back(std::make_tuple(nstr, std::string(), 0));
        }

        static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)>> s_format_items = {

#define XX(str, C)                                                               \
    {                                                                            \
#str, [](const std::string &fmt) { return FormatItem::ptr(new C(fmt)); } \
    }

            XX(m, MessageFormatItem),
            XX(p, LevelFormatItem),
            XX(r, ElaspseFormatItem),
            XX(c, NameFormatItem),
            XX(t, ThreadIdFormatItem),
            XX(d, DateTimeFormatItem),
            XX(f, FilenameFormatItem),
            XX(l, LineFormatItem),
            XX(n, NewLineFormatItem),

            //v2.0新增
            XX(T, TabFormatItem),
            XX(F, FiberIdFormatItem),

#undef XX
        };
        /*
      * %m --消息体
      * %p -- Level
      * %r --启动后的时间
      * %c --日志名称
      * %t --线程id
      * %n --回车换行
      * %d -- 时间
      * %f -- 文件名
      * %l -- 行号
      * %T -- \t
      * %F --协程号
      * %
      */

        for (auto &i : vec)
        {
            if (std::get<2>(i) == 0)
            {
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            }
            else
            {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end())
                {
                    m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_error = true; //格式错误
                }
                else
                {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }

            //   std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
        // std::cout << m_items.size() << std::endl;
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        std::stringstream ss;
        // std::cout << "m_itrems" << m_items.size()<<std::endl;
        for (auto &i : m_items)

        {
            //std::cout<<"jinru items"<<std::endl;
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }

    LoggerManager::LoggerManager()
    {
        m_root.reset(new Logger); //TODO:指针reset含义

        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender()));

        init();
    }

    Logger::ptr LoggerManager::getLogger(const std::string &name)
    {
        auto it = m_loggers.find(name);
        if (it != m_loggers.end())
        {
            return it->second;
        }

        Logger::ptr logger(new Logger(name));
        logger->m_root = m_root;
        m_loggers[name] = logger;
        return logger; //有则返回 无则创建
    }

    //配置模块与日志模块整合
    struct LogAppenderDefine
    {
        int type = 0; ///appender的类型  1:FileAppender 2:Stdout
        LogLevel::Level level = LogLevel::UNKNOWN;
        std::string formatter;
        std::string file;

        bool operator==(const LogAppenderDefine &oth) const
        {
            return type == oth.type && level == oth.level && formatter == oth.formatter && file == oth.file;
        }
    };

    struct LogDefine
    {
        std::string name;
        LogLevel::Level level = LogLevel::UNKNOWN;
        std::string formatter;

        std::vector<LogAppenderDefine> appenders;

        bool operator==(const LogDefine &oth) const
        {
            return name == oth.name && level == oth.level && formatter == oth.formatter && appenders == oth.appenders;
        }

        bool operator<(const LogDefine &oth) const
        {
            return name < oth.name; // TODO: 为什么set要重载小于号
        }
    };

    template <> //TODO: 还有这写法？
    class LexicalCast<std::string, std::set<LogDefine>>
    {

    public:
        std::set<LogDefine> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            std::set<LogDefine> vec; //模板在实例化之前并不知道std::vector<T>是什么，使用typename可以让定义确认下来
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {

                auto n = node[i];
                if (!n["name"].IsDefined())
                {
                    std::cout << "log config error： name is null," << n << std::endl;
                    continue;
                }

                LogDefine ld;
                ld.name = n["name"].as<std::string>();
                ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
                if (n["formatter"].IsDefined())
                {
                    ld.formatter = n["formatter"].as<std::string>();
                }
                if (n["appenders"].IsDefined())
                {
                    for (size_t x = 0; x < n["formatter"].size(); ++x)
                    {
                        auto a = n["formatter"][x];
                        if (!a["type"].IsDefined())
                        {

                            std::cout << "log config error： name is null," << n << std::endl;
                            continue;
                        }
                        std::string type = a["type"].as<std::string>();
                        LogAppenderDefine lad;
                        if (type == "FileLogAppender")
                        {
                            lad.type = 1;
                            if (!n["file"].IsDefined())
                            {
                                std::cout << "log config erro :  fileappenders file is null" << std::endl;
                                continue;
                            }

                            lad.file = n["file"].as<std::string>();
                            if (n["formatter"].IsDefined())
                            {
                                lad.formatter = n["formatter"].as<std::string>();
                            }
                        }
                        else if (type == "StdoutLogAppender")
                        {
                            lad.type = 2;
                        }
                        else
                        {
                            std::cout << "log config error: appenders type is Invalid" << std::endl;
                            continue;
                        }
                        ld.appenders.push_back(lad);
                    }
                }

                vec.insert(ld);
            }
            return vec;
        }
    };

    template <class T> //TODO: 还有这写法？
    class LexicalCast<std::set<LogDefine>, std::string>
    {

    public:
        std::string operator()(const std::set<LogDefine> &v)
        {
            YAML::Node node;
            for (auto &i : v)
            {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }

            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    //配置系统中读取的
    sylar::ConfigVar<std::set<LogDefine>>::ptr g_log_defines = sylar::Config::Lookup("logs", std::set<LogDefine>(), "logs config");

    struct LogIniter
    {
        LogIniter()
        {
            g_log_defines->addListener(0xF1E231, [](const std::set<LogDefine> &old_values, const std::set<LogDefine> &new_values) {
            
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<" on_logger_conf_changed";
            
            //新增
            for (auto &i : new_values)
            {
                auto it = old_values.find(i);
                sylar::Logger::ptr logger;
                if (it == old_values.end())
                {
                    //没找到 说明新增Logger
                    logger.reset(new sylar::Logger(i.name));
                }

                else
                {
                    if (!(i == *it))
                    {
                        //修改的Logger

                        logger=SYLAR_LOG_NAME(i.name);
                    }
                    logger->setLevel(i.level);
                    if (!i.formatter.empty())
                    {
                        logger->setFormatter(i.formatter);
                    }

                    logger->cleanAppenders();

                    for (auto &a : i.appenders)
                    {
                        sylar::LogAppender::ptr ap;
                        if (a.type == 1)
                        {
                            ap.reset(new FileLogAppender(a.file));
                        }
                        else if (a.type == 2)
                        {
                            ap.reset(new StdoutLogAppender);
                        }
                        ap->setLevel(a.level);
                        logger->addAppender(ap);
                    }
                }

                //删除
                for (auto &i : old_values)
                {
                    auto it = new_values.find(i);
                    if (it == new_values.end())
                    {
                        //删除logger

                        auto logger = SYLAR_LOG_NAME(i.name);
                        logger->setLevel((LogLevel::Level)100); //日志级别调高 隐藏
                        logger->cleanAppenders();
                    }
                }
            } });
        }
    };

    static LogIniter __log_initer; //
    //TODO:在main函数执行之前执行事件

} // namespace sylar
