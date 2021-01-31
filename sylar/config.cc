#include "config.h"

namespace sylar{

    Config::ConfigVarMap Config::s_datas; //静态类型记得初始化

    ConfigVarBase::ptr Config::LookupBase(const std::string& name){
        auto it=s_datas.find(name);
        return it==s_datas.end() ?nullptr :it->second;

    }

//"A.B", 10
//A:
//  B: 10
//  C: str
    static void ListAllMember(const std::string& prefix,const YAML::Node& node,std::list<std::pair<std::string,const YAML::Node>>& output){
            if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._1234567890")!=std::string::npos){
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Config invalid name: "<<prefix<<" : "<<node;
                return;
            }

            output.push_back(std::make_pair(prefix,node));
            if(node.IsMap()){
                for(auto it=node.begin();it!=node.end(); ++it){
                    ListAllMember((prefix.empty()?it->first.Scalar():(prefix+"." + it->first.Scalar())),it->second,output);

                }
            }

            

    }

    void Config::LoadFromYaml(const YAML::Node root){
        std::list<std::pair<std::string,const YAML::Node>> all_nodes;
        ListAllMember("",root,all_nodes);

        for(auto &it : all_nodes){
            std::string key=it.first;
            if(key.empty()){
                continue;
            }
            std::transform(key.begin(),key.end(),key.begin(),::tolower);
            ConfigVarBase::ptr var=LookupBase(key);
            if(var){
                if(it.second.IsScalar()){
                    var->fromString(it.second.Scalar());
                }else {
                    std::stringstream ss;
                    ss<<it.second;
                    var->fromString(ss.str());
                }
            }
        }
    }

}