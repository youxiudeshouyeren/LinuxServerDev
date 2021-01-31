#ifndef __SYLAR_CONFIG_H__
#define __SYLAR_CONFIG_H__

#include <memory>
#include<string>
#include <sstream>
#include <boost/lexical_cast.hpp> //TODO:待查lexical_cast
#include <map>
#include "log.h"
#include <yaml-cpp/yaml.h>
#include <vector>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set> //TODO:区别

namespace sylar
{

    //针对基础类型的转换 从F 类型 转换为 T类型
    template <class F, class T>
    class LexicalCast
    {
    public:
        T operator()(const F &v)
        {
            return boost::lexical_cast<T>(v);
        }
    };




// vector与string
//----------------------------------------------------------------
    template <class T> //TODO: 还有这写法？
    class LexicalCast<std::string, std::vector<T>>
    {

    public:
        std::vector<T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::vector<T> vec; //模板在实例化之前并不知道std::vector<T>是什么，使用typename可以让定义确认下来
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<std::string, T>()(ss.str()));
            }

            return vec;
        }
    };

    template <class T> //TODO: 还有这写法？
    class LexicalCast<std::vector<T>, std::string>
    {

    public:
        std::string operator()(const std::vector<T> &v)
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


//list与string
//----------------------------------------------------------------

   template <class T> //TODO: 还有这写法？
    class LexicalCast<std::string, std::list<T>>
    {

    public:
        std::list<T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::list<T> vec; //模板在实例化之前并不知道std::vector<T>是什么，使用typename可以让定义确认下来
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<std::string, T>()(ss.str()));
            }

            return vec;
        }
    };

    template <class T> //TODO: 还有这写法？
    class LexicalCast<std::list<T>, std::string>
    {

    public:
        std::string operator()(const std::list<T> &v)
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


//set与string
//----------------------------------------------------------------

   template <class T> //TODO: 还有这写法？
    class LexicalCast<std::string, std::set<T>>
    {

    public:
        std::set<T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::set<T> vec; //模板在实例化之前并不知道std::vector<T>是什么，使用typename可以让定义确认下来
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                
                vec.insert(LexicalCast<std::string, T>()(ss.str()));// 相比list的改动
            }

            return vec;
        }
    };

    template <class T> //TODO: 还有这写法？
    class LexicalCast<std::set<T>, std::string>
    {

    public:
        std::string operator()(const std::set<T> &v)
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



//unordered_set与string
//----------------------------------------------------------------

   template <class T> //TODO: 还有这写法？
    class LexicalCast<std::string, std::unordered_set<T>>
    {

    public:
        std::unordered_set<T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::unordered_set<T> vec; //模板在实例化之前并不知道std::vector<T>是什么，使用typename可以让定义确认下来
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                
                vec.insert(LexicalCast<std::string, T>()(ss.str()));// 相比list的改动
            }

            return vec;
        }
    };

    template <class T> //TODO: 还有这写法？
    class LexicalCast<std::unordered_set<T>, std::string>
    {

    public:
        std::string operator()(const std::unordered_set<T> &v)
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


    //map与string
//----------------------------------------------------------------

   template <class T> //TODO: 还有这写法？
    class LexicalCast<std::string, std::map<std::string,T>>
    {

    public:
        std::map<std::string,T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::map<std::string,T> vec; //模板在实例化之前并不知道std::vector<T>是什么，使用typename可以让定义确认下来
            std::stringstream ss;
            for (auto it=node.begin();it!=node.end();++it)
            {
                ss.str("");
                ss << it->second;
                
                vec.insert(std::make_pair(it->first.Scalar(),LexicalCast<std::string, T>()(ss.str())));//
            }

            return vec;
        }
    };

    template <class T> //TODO: 还有这写法？
    class LexicalCast<std::map<std::string,T>, std::string>
    {

    public:
        std::string operator()(const std::map<std::string,T> &v)
        {
            YAML::Node node;
            for (auto &i : v)
            {
                node[i.first] = YAML::Load(LexicalCast<T,std::string>()(i.second));
            }

            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };


    class ConfigVarBase
    {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;

        ConfigVarBase(const std::string &name, const std::string &description = "") : m_name(name), m_description(description)
        {
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower); //名称转换为小写
        }

        virtual ~ConfigVarBase() {}

        const std::string &getName() const { return m_name; }

        const std::string &getDescription() const { return m_description; }

        virtual std::string toString() = 0;

        virtual bool fromString(const std::string &val) = 0;

    protected:
        std::string m_name;
        std::string m_description;
    };

    //FromStr T operator()(const std::string&)
    //TOStr std::string operator()(const T&)
    //类型的仿函数转换具有默认参数
    template <class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
    class ConfigVar : public ConfigVarBase
    {
    public:
        typedef std::shared_ptr<ConfigVar> ptr;

        ConfigVar(const std::string &name, const T &default_value, const std::string &description = "") : ConfigVarBase(name, description), m_val(default_value) {}

        std::string toString() override
        {
            try
            {
                // return boost::lexical_cast<std::string>(m_val);
                return ToStr()(m_val);
            }
            catch (std::exception &e)
            {
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception" << e.what() << " Convert " << typeid(m_val).name() << " to String";
            }
            return "";
        }

        bool fromString(const std::string &val) override
        {
            try
            {
                // m_val = boost::lexical_cast<T>(val);
                setValue(FromStr()(val));
                return true;
            }
            catch (std::exception &e)
            {
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::fromString exception" << e.what() << " Convert string to" << typeid(m_val).name();
            }
            return false;
        }

        const T getValue() const { return m_val; }
        void setValue(const T &v) { m_val = v; }

    private:
        T m_val;
    };

    class Config
    {
    public:
        typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

        template <class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string &name)
        {
            auto it = s_datas.find(name);
            if (it == s_datas.end())
            {
                return nullptr;
            }

            return std::dynamic_pointer_cast<ConfigVar<T>>(it->second); //TODO: dynamic_pointer_cast 当指针是智能指针时候，向下转换，父类指针向子类指针转换
        }

        template <class T> //TODO：模板子类继承非模板子类   返回值前面加typename
        static typename ConfigVar<T>::ptr Lookup(const std::string &name, const T &default_value, const std::string &description = "")
        {
            auto temp = Lookup<T>(name); //TODO:不加<T>会报错
            if (temp)
            {
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name =" << name << " exists";
                return temp;
            }
            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos)
            { //TODO:

                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name invalid = " << name;
                throw std::invalid_argument(name);
            }

            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
            s_datas[name] = v;

            return v;
        }

        static ConfigVarBase::ptr LookupBase(const std::string &name);

        static void LoadFromYaml(const YAML::Node root);

    private:
        static ConfigVarMap s_datas;
    };
} // namespace sylar

#endif