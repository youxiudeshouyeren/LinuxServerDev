#ifndef __SYLAR_CONFIG_H__
#define __SYLAR_CONFIG_H__


#include <memory>
#include<sstream>
#include<boost/lexical_cast.hpp> //TODO:待查lexical_cast
#include<map>
#include"log.h"

namespace sylar{


    class ConfigVarBase{
        public:
        typedef std::shared_ptr<ConfigVarBase> ptr;
        ConfigVarBase(const std::string& name,const std::string&description=""):
        m_name(name),m_description(description){}

        virtual ~ConfigVarBase(){}

        const std::string&getName() const{return m_name;}
        const std::string&getDescription() const{return m_description;}

        virtual std::string toString() =0;
        virtual bool fromString(const std::string& val)=0;

        protected:
        std::string m_name;
        std::string m_description;
    };



    template<class T>
    class ConfigVar: public ConfigVarBase{
            public:
                typedef std::shared_ptr<ConfigVar> ptr;
                ConfigVar(const std::string &name,const T& default_value,const std::string&description=""):ConfigVarBase(name,description),m_val(default_value){}

                std::string toString() override{
                    try{
                        return boost::lexical_cast<std::string>(m_val);

                    }catch(std::exception& e){
                        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT())<<"ConfigVar::toString exception"<<e.what()<<" Convert "<<typeid(m_val).name()<<" to String";

                    }
                    return "";
                }

                bool fromString(const std::string& val) override{
                    try{
                        m_val=boost::lexical_cast<T>(val);

                    }catch(std::exception& e ){
                       SYLAR_LOG_ERROR(SYLAR_LOG_ROOT())<<"ConfigVar::fromString exception"<<e.what()<<" Convert string to"<<typeid(m_val).name();
                    }
                    return false;
                }

                const T getValue() const{return m_val;}
                void setValue(const T& v){m_val=v;}

            private:
                T m_val;
    };


    class Config{
        public:
        typedef std::map<std::string,ConfigVarBase::ptr> ConfigVarMap;

      
       
       template<class T>
       static typename ConfigVar<T>::ptr Lookup(const std::string& name){
           auto it=s_datas.find(name);
           if(it==s_datas.end()){
               return nullptr;
           }

           return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);//TODO: dynamic_pointer_cast
       }
      
      template<class T>//TODO：模板子类继承非模板子类   返回值前面加typename
       static typename ConfigVar<T>::ptr Lookup(const std::string &name,const T& default_value,const std::string&description=""){
                 auto temp=Lookup<T>(name); //TODO:不加<T>会报错
                 if(temp){
                     SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<"Lookup name ="<<name<<" exists";
                     return temp;
                 }
                 if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._012345678")!=std::string::npos) {//TODO: 

                    SYLAR_LOG_ERROR(SYLAR_LOG_ROOT())<<"Lookup name invalid = "<<name;
                    throw std::invalid_argument(name);
                 }

                 typename ConfigVar<T>::ptr v(new ConfigVar<T>(name,default_value,description));
                 s_datas[name]=v;

                 return v;
       }
        private:
        static ConfigVarMap s_datas;
    };
}





#endif