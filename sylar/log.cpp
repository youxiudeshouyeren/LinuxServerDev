
#include <iostream>
#include <sstream>
#include <map>
#include "log.h"
#include <functional>
#include <time.h>
#include <string.h>
namespace sylar
{

    LogEvent::LogEvent(const char *file, int32_t m_line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time)
        : m_file(file),
          m_line(m_line),
          m_elapse(elapse),
          m_threadId(thread_id),
          m_fiberId(fiber_id),
          m_time(time)
    {
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

    class MessageFormatItem : public LogFormatter::FormatItem
    {
    public:
        MessageFormatItem(const std::string &str = "") {
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
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S") : m_format(format) {
            if(m_format.empty()){
                m_format="%Y-%m-%d %H:%M:%S";
            }

        }

        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            struct tm tm;
            time_t time=event->getTime();
            localtime_r (&time, &tm);
            char buf[64];
            strftime(buf,sizeof(buf), m_format.c_str(),&tm);
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
    /*----------------------------------------------------------------
*
*/
    Logger::Logger(const std::string &name) : m_name(name), m_level(LogLevel::Level::DEBUG)
    {
        m_formatter.reset(new LogFormatter("%d  [%p] <%f:%l>  %m  %n"));
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

    void Logger::log(LogLevel::Level level, const LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            auto self = shared_from_this();
            for (auto &i : m_appenders)
            {
              //  std::cout <<"Jinruappenders"<<m_appenders.size()<< std::endl;

                i->log(self, level, event);
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
            std::cout << m_formatter->format(logger, level, event)<< std::endl;
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
        std::cout<<m_pattern<<std::endl;
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
                if(n == m_pattern.size()) {
                if(str.empty()) {
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
                i = n-1;
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
            XX(n, NewLineFormatItem)

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
      * 
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

} // namespace sylar