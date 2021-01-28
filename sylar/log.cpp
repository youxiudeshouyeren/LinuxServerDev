
#include<iostream>
#include<sstream>
#include<map>
#include "log.h"
namespace sylar{


/*----------------------------------------------------------------
*
*/

const char* LogLevel::ToString(LogLevel::Level level){
    switch(level){

        #define XX(name)\
        case LogLevel::name:\
             return #name;\
             break;

        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);

        #undef XX

        default:
        return  "UNKNOWN";
    }
}



class MessageFormatter : public LogFormatter::FormatItem{
      public:

        void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event)override{
            os<<event->getContent();
        }



  };

//等级
  class LevelFormatItem: public LogFormatter::FormatItem{

      public:
      
        void format(std::ostream& os,std::shared_ptr<Logger>logger, LogLevel::Level level,LogEvent::ptr event)override{
            os<<LogLevel::ToString(level);
        }
  };

  class ElaspseFormatItem: public LogFormatter::FormatItem{

      public:

         void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
             os<<event->getContent();
         }
  };

  class NameFormatItem: public LogFormatter::FormatItem{

      public:

         void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
             os<<logger->getName();
         }
  };

//线程号
  class ThreadIdFormatItem: public LogFormatter::FormatItem{
       public:

         void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
             os<<event->getThreadId();
         }
  };

//协程号
   class FiberIdFormatItem:public LogFormatter::FormatItem{
       public:

         void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
             os<<event->getFiberId();
         }
  };

//时间
    class DateTimeFormatItem:public LogFormatter::FormatItem{
       public:
         DateTimeFormatItem(const std::string& format="%Y:%m:%d %H:%M:%S"):m_format(format){}

         void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
             os<<event->getTime();
         }

         private:

         std::string m_format;
  };

//文件名
   class FilenameFormatItem:public LogFormatter::FormatItem{
       public:

         void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
             os<<event->getFile();
         }
  };

//行号
   class LineFormatItem:public LogFormatter::FormatItem{
       public:

         void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
             os<<event->getLine();
         }
  };

  //换行
   class NewLineFormatItem:public LogFormatter::FormatItem{
       public:

         void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
             os<<std::endl;
         }
  };


//
   class StringFormatItem:public LogFormatter::FormatItem{
       public:
       StringFormatItem(const std::string& str):FormatItem(str),m_string(str) {}

         void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
             os<<m_string;
         }

         private:
         std::string m_string;
  };
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

  void FileLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) {
      if(level>=m_level){
          m_filestream<<m_formatter->format(logger,level,event);
      }

    }

    bool FileLogAppender::reopen(){
        if(this->m_filestream){
            m_filestream.close();//文件打开则关闭
        }
        m_filestream.open(m_filename);//重新打开

        return !!m_filestream;//返回打开状态

    }

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) {
        if(level >= this->m_level){
                std::cout<<m_formatter->format(logger,level,event);
        }
    }

  
/*----------------------------------------------------------------
*
*/

  LogFormatter::LogFormatter(const std::string& pattern):m_pattern(pattern){

  }


//日志格式的解析
  void LogFormatter::init(){

      //str format type
      std::vector<std::tuple<std::string,std::string,int>> vec;
      std::string nstr;
      for(size_t i=0;i<m_pattern.size();++i){
          if(m_pattern[i]!='%'){
              nstr.append(1,m_pattern[i]);
              continue;
          }
          if((i=1)<m_pattern.size()){
              if(m_pattern[i+1]=='%'){
                  nstr.append(1,'%');  //对%的转义
                  continue;
              }
          }

          size_t n=i+1;
          int fmt_status;
          size_t fmt_begin;
          std::string fmt;
          std::string str;

          while(n<m_pattern.size()){
              if(isspace(m_pattern[n])){
                  break; //
              }

              if(fmt_status==0){

                if(m_pattern[n]=='{'){
                    str=m_pattern.substr(i+1,n-i-1);
                    fmt_status=1; //解析格式
                    fmt_begin=n;
                    ++n;
                    
                    continue;
              }
              }
              if(fmt_status==1){
                  if(m_pattern[n]=='}'){
                      fmt=m_pattern.substr(fmt_begin+1,n-fmt_begin-1);
                      fmt_status=2;
                      break;
                  }

              }

          }
          if(fmt_status==0){
              if(!nstr.empty()){
                  vec.push_back(std::make_tuple(nstr,"",0));
              }
              
              str=m_pattern.substr(i+1,n-i-1);
              vec.push_back(std::make_tuple(str,fmt,i));
              i=n;
          }else if(fmt_status==1){
              std::cout<<"pattern parse error:"<<m_pattern<<" - "<<m_pattern.substr(i)<<std::endl;
              vec.push_back(std::make_tuple("<<pattern_error>>",fmt,0));
          }else if(fmt_status==2){
              vec.push_back(std::make_tuple(str,fmt,1));
              i=n;
          }
         
      }
      if(!nstr.empty()){
          vec.push_back(std::make_tuple(nstr,"",0));
      }

    static std::map<std::string,std::function<FormatItem::ptr(const std::string& str)> >s_format_items={

        #define XX(str,C)\
        {#str,[](const std::string& fmt){return FormatItem::ptr(new C(fmt));}}



        XX(m,MessageFormatItem),
        XX(p,LevelFormatItem),
        XX(r,ElapseFormatItem),
        XX(c,NameFormatItem),
        XX(t,ThreadIdFormatItem),
        XX(d,DateTimeFormatItem),
        XX(f,FilenameFormatItem),
        XX(l,LineFormatItem),
        XX(n,NewLineFormatItem)

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

     for(auto& i:vec){
         if(std::get<2>(i) ==0){
             m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
         }
         else
         {
             auto it=s_format_items.find(std::get<0>(i));
             if(it==s_format_items.end()){
                 m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %"+std::get<0>(i)+">>")));
             }else{
                 m_items.push_back(it->second(std::get<1>(i)));
             }
         }

         std::cout<<std::get<0>(i)<<" - "<<std::get<1>(i)<<" - "<<std::get<2>(i)<<std::endl;
         }
     }
  
  

  std::string LogFormatter::format(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
      std::stringstream ss;
      for(auto&i:m_items){
          i->format(ss,logger,level,event);
      }
      return ss.str();


  }

  


}