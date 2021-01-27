#include "log.h"
#include<iostream>
#include<sstream>

namespace sylar{


/*----------------------------------------------------------------
*
*/
    Logger::Logger(const std::string& name="root"):m_name(name){

    }

    void Logger::addAppender(LogAppender::ptr appender){
        this->m_appenders.push_back(appender);

    }

    void Logger::delAppender(LogAppender::ptr appender){
        for(auto it=m_appenders.begin(); it!=m_appenders.end();++it){
            if(*it==appender){
                m_appenders.erase(it);
                break;
            }
        }
    }

     void Logger::log( LogLevel::Level level,const LogEvent::ptr event){
         if(level>=m_level){
             for(auto& i:m_appenders){

             }
         }
     }

    void Logger::debug( LogEvent::ptr event){
        log(LogLevel::DEBUG,event);
    }

    void Logger::info( LogEvent::ptr event){
         log(LogLevel::INFO,event);
    }

    void Logger::warn( LogEvent::ptr event){
        log(LogLevel::WARN,event);
    }

    void Logger::error( LogEvent::ptr event){
        log(LogLevel::ERROR,event);
    }

    void Logger::fatal( LogEvent::ptr event){
        log(LogLevel::FATAL,event);
    } 


/*----------------------------------------------------------------
*
*/
    FileLogAppender::FileLogAppender(const std::string& filename):m_filename(filename){

    }

  void FileLogAppender::log(LogLevel::Level level,LogEvent::ptr event) {
      if(level>=m_level){
          m_filestream<<m_formatter.format(event);
      }

    }

    bool FileLogAppender::reopen(){
        if(this->m_filestream){
            m_filestream.close();//文件打开则关闭
        }
        m_filestream.open(m_filename);//重新打开

        return !!m_filestream;//返回打开状态

    }

    void StdoutLogAppender::log(LogLevel::Level level,LogEvent::ptr event) {
        if(level >= this->m_level){
                std::cout<<m_formatter.format(event);
        }
    }

  
/*----------------------------------------------------------------
*
*/

  LogFormatter::LogFormatter(const std::string& pattern):m_pattern(pattern){

  }

  void LogFormatter::init(){

  }

  std::string LogFormatter::format(LogEvent::ptr event){
      std::stringstream ss;
      for(auto&i:m_items){
          i->format(ss,event);
      }
      return ss.str();

  }

}