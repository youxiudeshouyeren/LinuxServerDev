#include "../sylar/config.h"
#include "../sylar/log.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

sylar::ConfigVar<int>::ptr g_int_value_config = sylar::Config::Lookup("system.port", (int)8080, "system port");
sylar::ConfigVar<float>::ptr g_float_value_config = sylar::Config::Lookup("system.value", (float)10.2f, "system value");

sylar::ConfigVar<float>::ptr g_float_error_value_config = sylar::Config::Lookup("system.port", (float)1000, "system port");

sylar::ConfigVar<std::vector<int>>::ptr g_int_vect_init_value_config = sylar::Config::Lookup("system.int_vec", std::vector<int>{1, 2, 3}, "system int vec");

sylar::ConfigVar<std::list<int>>::ptr g_int_list_init_value_config = sylar::Config::Lookup("system.int_list", std::list<int>{1, 2, 3}, "system int list");

sylar::ConfigVar<std::set<int>>::ptr g_int_set_init_value_config = sylar::Config::Lookup("system.int_set", std::set<int>{1, 2, 3}, "system int set");

sylar::ConfigVar<std::unordered_set<int>>::ptr g_int_unorderedset_init_value_config = sylar::Config::Lookup("system.int_unorderedset", std::unordered_set<int>{11, 22, 33}, "system int unordered_set");

sylar::ConfigVar<std::map<std::string, int>>::ptr g_int_map_init_value_config = sylar::Config::Lookup("system.int_map", std::map<std::string, int>{{"k", 2}}, "system int unordered_set");

sylar::ConfigVar<std::unordered_map<std::string, int>>::ptr g_int_unorderedmap_init_value_config = sylar::Config::Lookup("system.int_unorderedmap", std::unordered_map<std::string, int>{{"k", 2}}, "system int unordered_set");

//打印YAML
void print_yaml(const YAML::Node &node, int level)
{
    if (node.IsScalar())
    {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                                         << node.Scalar() << " - " << node.Type() << " - " << level;
    }
    else if (node.IsNull())
    {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                                         << "NULL - " << node.Type() << " - " << level;
    }
    else if (node.IsMap())
    {
        for (auto it = node.begin();
             it != node.end(); ++it)
        {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                                             << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    }
    else if (node.IsSequence())
    {
        for (size_t i = 0; i < node.size(); ++i)
        {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                                             << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml()
{
    YAML::Node root = YAML::LoadFile("/home/syr/桌面/serverDev/bin/conf/log.yml");
    print_yaml(root, 0);
    //SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<root;
}

void test_config()
{

//一般STL
#define XX(g_var, name, prefix)                                                               \
    {                                                                                         \
        auto &v = g_var->getValue();                                                          \
        for (auto &i : v)                                                                     \
        {                                                                                     \
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name ": " << i;                  \
        }                                                                                     \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
    }

//针对map的测试
#define XX_M(g_var, name, prefix)                                                                                 \
    {                                                                                                             \
        auto &v = g_var->getValue();                                                                              \
        for (auto &i : v)                                                                                         \
        {                                                                                                         \
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name ": {" << i.first << " - " << i.second << " } "; \
        }                                                                                                         \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString();                     \
    }

    //SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<"before"<<g_float_error_value_config->getValue();
    XX(g_int_vect_init_value_config, int_vec, before);
    XX(g_int_list_init_value_config, int_list, before);
    XX(g_int_set_init_value_config, int_set, before);
    XX(g_int_unorderedset_init_value_config, int_unordered_set, before);
    XX_M(g_int_map_init_value_config, int_map, before);
    XX_M(g_int_unorderedmap_init_value_config, int_unorderedmap, before);

    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<"before"<<g_float_value_config->toString();

    // auto v=g_int_vect_init_value_config->getValue();
    // for(auto& i:v){
    //     SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<"before int_vec "<<i;
    // }

    YAML::Node root = YAML::LoadFile("/home/syr/桌面/serverDev/bin/conf/test.yml");
    sylar::Config::LoadFromYaml(root);

    XX(g_int_vect_init_value_config, int_vec, after);
    XX(g_int_list_init_value_config, int_list, after);
    XX(g_int_set_init_value_config, int_set, after);
    XX(g_int_unorderedset_init_value_config, int_unordered_set, after);
    XX_M(g_int_map_init_value_config, int_map, after);
    XX_M(g_int_unorderedmap_init_value_config, int_unorderedmap, after);

    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<"after"<<g_int_value_config->getValue();
    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<"after"<<g_float_value_config->toString();
    // v=g_int_vect_init_value_config->getValue();
    // for(auto& i:v){
    //     SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<" after int_vec "<<i;
    // }
}

class Person
{
public:
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const
    {
        std::stringstream ss;
        ss << "(Person name=" << m_name << " age=" << m_age << " sex=" << m_sex << " )";
        return ss.str();
    }

    bool operator==(const Person &other) const
    {
        return this->m_name == other.m_name && this->m_age == other.m_age && this->m_sex == other.m_sex;
    }
};

namespace sylar
{

    //person与string
    //----------------------------------------------------------------

    template <>
    class LexicalCast<std::string, Person>
    {

    public:
        Person operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            Person p;
            p.m_name = node["name"].as<std::string>();
            p.m_age = node["age"].as<int>();
            p.m_sex = node["sex"].as<bool>();
            std::cout << "调用string转person" << std::endl;
            return p;
        }
    };

    template <>
    class LexicalCast<Person, std::string>
    {

    public:
        std::string operator()(const Person &v)
        {
            YAML::Node node;
            node["name"] = v.m_name;
            node["sex"] = v.m_sex;
            node["age"] = v.m_age;
            std::stringstream ss;
            ss << node;
            std::cout << "调用person转string" << std::endl;
            return ss.str();
        }
    };
} // namespace sylar

sylar::ConfigVar<Person>::ptr g_init_person = sylar::Config::Lookup("class.person", Person(), "system person");

sylar::ConfigVar<std::map<std::string, Person>>::ptr g_init_map_person = sylar::Config::Lookup("class.map", std::map<std::string, Person>{}, "system person");

void test_class()
{

    //  SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "  before" << g_init_person->getValue().toString() << " -  " << g_init_person->toString();

#define XX_PM(g_var, name, prefix)                                                                          \
    {                                                                                                       \
        auto m = g_var->getValue();                                                                         \
        for (auto &i : m)                                                                                   \
        {                                                                                                   \
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << prefix << " : " << i.first << " - " << i.second.toString(); \
        }                                                                                                   \
    }
 g_init_person->addListener(10, [](const Person &old_value, const Person &new_value) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << " old value: " << old_value.toString() << " new value: " << new_value.toString();
    });
    XX_PM(g_init_map_person, "", "before person map:");
    YAML::Node root = YAML::LoadFile("/home/syr/桌面/serverDev/bin/conf/log.yml");
    sylar::Config::LoadFromYaml(root);

    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "  after" << g_init_person->getValue().toString() << " -  " << g_init_person->toString();

    XX_PM(g_init_map_person, "", "after person map:");

   
}

int main(int argc, char **argv)
{
    //  test_yaml();
    //test_config();

    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<g_init_value_config->getValue();

    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<g_init_value_config->toString();
    test_class();

    return 0;
}