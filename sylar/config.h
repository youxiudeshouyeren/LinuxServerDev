#ifndef __SYLAR_CONFIG_H__
#define __SYLAR_CONFIG_H__

#include <memory>
#include <string>
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
#include <functional>
#include <stdint.h>

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

                vec.insert(LexicalCast<std::string, T>()(ss.str())); // 相比list的改动
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

                vec.insert(LexicalCast<std::string, T>()(ss.str())); // 相比list的改动
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
    class LexicalCast<std::string, std::map<std::string, T>>
    {

    public:
        std::map<std::string, T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::map<std::string, T> vec; //模板在实例化之前并不知道std::vector<T>是什么，使用typename可以让定义确认下来
            std::stringstream ss;
            for (auto it = node.begin(); it != node.end(); ++it)
            {
                ss.str("");
                ss << it->second;

                vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str()))); //
            }

            return vec;
        }
    };

    template <class T> //TODO: 还有这写法？
    class LexicalCast<std::map<std::string, T>, std::string>
    {

    public:
        std::string operator()(const std::map<std::string, T> &v)
        {
            YAML::Node node;
            for (auto &i : v)
            {
                node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
            }

            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    //unordered_map与string
    //----------------------------------------------------------------

    template <class T> //TODO: 还有这写法？
    class LexicalCast<std::string, std::unordered_map<std::string, T>>
    {

    public:
        std::unordered_map<std::string, T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::unordered_map<std::string, T> vec; //模板在实例化之前并不知道std::vector<T>是什么，使用typename可以让定义确认下来
            std::stringstream ss;
            for (auto it = node.begin(); it != node.end(); ++it)
            {
                ss.str("");
                ss << it->second;

                vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str()))); //
            }

            return vec;
        }
    };

    template <class T> //TODO: 还有这写法？
    class LexicalCast<std::unordered_map<std::string, T>, std::string>
    {

    public:
        std::string operator()(const std::unordered_map<std::string, T> &v)
        {
            YAML::Node node;
            for (auto &i : v)
            {
                node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
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
        virtual std::string getTypeName() const = 0;

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
        typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb; //配置更改后的回调  //TODO:待查

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

        void setValue(const T &v)
        {
            if (v == m_val)
            {
                return;
            }
            for (auto &i : m_cbs)
            {
                i.second(m_val, v); //修改前执行配置事件的回调
            }
            m_val = v;
        }

        std::string getTypeName() const override { return typeid(T).name(); }

        void addListener(uint64_t key, on_change_cb cb)
        {
            m_cbs[key] = cb;
        }

        void delListener(uint64_t key)
        {

            m_cbs.erase(key);
        }

        on_change_cb getListener(uint64_t key)
        {
            auto it = m_cbs.find(key);
            return it == m_cbs.end() ? nullptr : it->second;
        }

        void cleanListener()
        {
            m_cbs.clear();
        }

    private:
        T m_val;

        //变更回调函数组 ， uint64_t key，要求唯一，一般可以用hash
        std::map<uint64_t, on_change_cb> m_cbs;
    };

    class Config
    {
    public:
        typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

        template <class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string &name)
        {
            auto it = GetDatas().find(name);
            if (it == GetDatas().end())
            {
                return nullptr;
            }

            return std::dynamic_pointer_cast<ConfigVar<T>>(it->second); //TODO: dynamic_pointer_cast 当指针是智能指针时候，向下转换，父类指针向子类指针转换
        }

        template <class T> //TODO：模板子类继承非模板子类   返回值前面加typename
        static typename ConfigVar<T>::ptr Lookup(const std::string &name, const T &default_value, const std::string &description = "")
        {

            auto it = GetDatas().find(name);
            if (it != GetDatas().end())
            {
                auto temp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second); //转换失败返回空指针 则类型不匹配
                if (temp)
                {
                    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name =" << name << " exists";
                    return temp;
                }
                else
                {
                    //类型错误
                    SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name =" << name << " exists but type not" << typeid(T).name() << " real type=" << it->second->getTypeName() << " real value:" << it->second->toString();
                    return nullptr;
                }
            }

            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos)
            { //TODO:

                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name invalid = " << name;
                throw std::invalid_argument(name);
            }

            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
            GetDatas()[name] = v;

            return v;
        }

        static ConfigVarBase::ptr LookupBase(const std::string &name);

        static void LoadFromYaml(const YAML::Node root);

    private:
        // static ConfigVarMap s_datas;
        //TODO: 静态成员的初始化顺序  s_datas必须先初始化
        static ConfigVarMap &GetDatas()
        {
            static ConfigVarMap s_datas;
            return s_datas;
        }
    };
} // namespace sylar

#endif