
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

    /**
 * @brief Construct a new Log Event:: Log Event object
 * 
 * @param  logger           My Param doc
 * @param  level            My Param doc
 * @param  file             My Param doc
 * @param  m_line           My Param doc
 * @param  elapse           My Param doc
 * @param  thread_id        My Param doc
 * @param  fiber_id         My Param doc
 * @param  time             My Param doc
 */
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

    /**
 * @brief 
 * @param  fmt              My Param doc
 * @param  al               My Param doc
 */
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
            return "UNKNOW";
        }
    }

    LogLevel::Level LogLevel::FromString(const std::string &str)
    {

#define XX(level,v)               \
    if (str == #v)         \
    {                          \
        return LogLevel::level; \
    }

        XX(DEBUG,debug);
        XX(INFO,info);
        XX(WARN,warn);
        XX(ERROR,error);
        XX(FATAL,fatal);

        XX(DEBUG,DEBUG);
        XX(INFO,INFO);
        XX(WARN,WARN);
        XX(ERROR,ERROR);
        XX(FATAL,FATAL);
        return LogLevel::UNKNOW;

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
            else if (m_root) //TODO:bug?
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
            std::cout << (m_formatter == nullptr) << std::endl; //TODO:找到2 formatter是空的！！！
            if (m_formatter == nullptr)
            {
                m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T [%p]%T[%t]%T[%c]%T %f:%l%T %m  %n"));
            }
            std::cout << m_formatter->format(logger, level, event) << std::endl;
        }
    }

    std::string StdoutLogAppender::toYamlString()
    {
        YAML::Node node;
        node["type"] = "StdoutLogAppender";
        node["level"] = LogLevel::ToString(m_level);
        if (m_formatter)
        {
            node["formatter"] = m_formatter->getPattern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    std::string FileLogAppender::toYamlString()
    {
        YAML::Node node;
        node["type"] = "FileLogAppender";
        node["file"] = m_filename;
        node["level"] = LogLevel::ToString(m_level);
        if (m_formatter)
        {
            node["formatter"] = m_formatter->getPattern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    std::string Logger::toYamlString()
    {
        YAML::Node node;
        node["name"] = this->m_name;
        node["level"] = LogLevel::ToString(m_level);
        if (m_formatter)
        {
            node["formatter"] = m_formatter->getPattern();
        }

        for (auto &i : m_appenders)
        {
            node["appenders"].push_back(YAML::Load(i->toYamlString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
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
                // std::cout <<__FILE__<<__LINE__ << m_items.size() << std::endl;
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            }
            else
            {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end())
                {
                    //   std::cout <<__FILE__<<__LINE__ << m_items.size() << std::endl;
                    m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_error = true; //格式错误
                }
                else
                {
                    //   std::cout <<__FILE__<<__LINE__ << m_items.size() << std::endl;
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }

            //   std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
        // std::cout << __FILE__ << __LINE__ << m_items.size() << std::endl;
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        std::stringstream ss;

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

        m_loggers[m_root->m_name] = m_root;

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
        int type = 0; //1 File, 2 Stdout
        LogLevel::Level level = LogLevel::Level::UNKNOW;
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
        LogLevel::Level level = LogLevel::Level::UNKNOW;
        std::string formatter;
        std::vector<LogAppenderDefine> appenders;

        bool operator==(const LogDefine &oth) const
        {
            return name == oth.name && level == oth.level && formatter == oth.formatter && appenders == appenders;
        }

        bool operator<(const LogDefine &oth) const
        {
            return name < oth.name;
        }

        bool isValid() const
        {
            return !name.empty();
        }
    };

    template <>
    class LexicalCast<std::string, LogDefine>
    {
    public:
        LogDefine operator()(const std::string &v)
        {
            YAML::Node n = YAML::Load(v);
            LogDefine ld;
            if (!n["name"].IsDefined())
            {
                std::cout << "log config error: name is null, " << n
                          << std::endl;
                throw std::logic_error("log config name is null");
            }
            ld.name = n["name"].as<std::string>();
            ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
            if (n["formatter"].IsDefined())
            {
                ld.formatter = n["formatter"].as<std::string>();
            }

            if (n["appenders"].IsDefined())
            {
                //std::cout << "==" << ld.name << " = " << n["appenders"].size() << std::endl;
                for (size_t x = 0; x < n["appenders"].size(); ++x)
                {
                    auto a = n["appenders"][x];
                    if (!a["type"].IsDefined())
                    {
                        std::cout << "log config error: appender type is null, " << a
                                  << std::endl;
                        continue;
                    }
                    std::string type = a["type"].as<std::string>();
                    LogAppenderDefine lad;
                    if (type == "FileLogAppender")
                    {
                        lad.type = 1;
                        if (!a["file"].IsDefined())
                        {
                            std::cout << "log config error: fileappender file is null, " << a
                                      << std::endl;
                            continue;
                        }
                        lad.file = a["file"].as<std::string>();
                        if (a["formatter"].IsDefined())
                        {
                            lad.formatter = a["formatter"].as<std::string>();
                        }
                    }
                    else if (type == "StdoutLogAppender")
                    {
                        lad.type = 2;
                        if (a["formatter"].IsDefined())
                        {
                            lad.formatter = a["formatter"].as<std::string>();
                        }
                    }
                    else
                    {
                        std::cout << "log config error: appender type is invalid, " << a
                                  << std::endl;
                        continue;
                    }

                    ld.appenders.push_back(lad);
                }
            }
            return ld;
        }
    };

    template <>
    class LexicalCast<LogDefine, std::string>
    {
    public:
        std::string operator()(const LogDefine &i)
        {
            YAML::Node n;
            n["name"] = i.name;
            if (i.level != LogLevel::Level::UNKNOW)
            {
                n["level"] = LogLevel::ToString(i.level);
            }
            if (!i.formatter.empty())
            {
                n["formatter"] = i.formatter;
            }

            for (auto &a : i.appenders)
            {
                YAML::Node na;
                if (a.type == 1)
                {
                    na["type"] = "FileLogAppender";
                    na["file"] = a.file;
                }
                else if (a.type == 2)
                {
                    na["type"] = "StdoutLogAppender";
                }
                if (a.level != LogLevel::Level::UNKNOW)
                {
                    na["level"] = LogLevel::ToString(a.level);
                }

                if (!a.formatter.empty())
                {
                    na["formatter"] = a.formatter;
                }

                n["appenders"].push_back(na);
            }
            std::stringstream ss;
            ss << n;
            return ss.str();
        }
    };

    sylar::ConfigVar<std::set<sylar::LogDefine>>::ptr g_log_defines =
        sylar::Config::Lookup("logs", std::set<sylar::LogDefine>(), "logs config");

    struct LogIniter
    {
        LogIniter()
        {
            g_log_defines->addListener((uint64_t)0xF1E231, [](const std::set<LogDefine> &old_value,
                                                              const std::set<LogDefine> &new_value) {
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "on_logger_conf_changed";
                for (auto &i : new_value)
                {
                    auto it = old_value.find(i);
                    sylar::Logger::ptr logger;
                    if (it == old_value.end())
                    {
                        //新增logger
                        logger = SYLAR_LOG_NAME(i.name);
                    }
                    else
                    {
                        if (!(i == *it))
                        {
                            //修改的logger
                            logger = SYLAR_LOG_NAME(i.name);
                        }
                        else
                        {
                            continue;
                        }
                    }
                    logger->setLevel(i.level);
                    //std::cout << "** " << i.name << " level=" << i.level
                    //<< "  " << logger << std::endl;
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
                            // if(!sylar::EnvMgr::GetInstance()->has("d")) {
                            ap.reset(new StdoutLogAppender);
                            //  } else {
                            //     continue;
                            // }
                        }
                        ap->setLevel(a.level);
                        if (!a.formatter.empty())
                        {
                            LogFormatter::ptr fmt(new LogFormatter(a.formatter));
                            if (!fmt->isError())
                            {
                                ap->setFormatter(fmt);
                            }
                            else
                            {
                                std::cout << "log.name=" << i.name << " appender type=" << a.type
                                          << " formatter=" << a.formatter << " is invalid" << std::endl;
                            }
                        }
                        logger->addAppender(ap);
                    }
                }

                for (auto &i : old_value)
                {
                    auto it = new_value.find(i);
                    if (it == new_value.end())
                    {
                        //删除logger
                        auto logger = SYLAR_LOG_NAME(i.name);
                        logger->setLevel((LogLevel::Level)0);
                        logger->cleanAppenders();
                    }
                }
            });
        }
    };

    static LogIniter __log_init;

    std::string LoggerManager::toYamlString()
    {
        //  MutexType::Lock lock(m_mutex);
        YAML::Node node;
        for (auto &i : m_loggers)
        {
            node.push_back(YAML::Load(i.second->toYamlString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    void LoggerManager::init()
    {
    }

} // namespace sylar
