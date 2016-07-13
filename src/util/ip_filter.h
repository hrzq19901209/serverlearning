#ifndef MYSIM_UTIL_IPFILTER_H_
#define MYSIM_UTIL_IPFILTER_H_

#include <string>
#include <set>

class IpFilter{
private:
    bool deny_all;
    bool allow_all;
    bool empty_;
    std::set<std::string> deny;
    std::set<std::string> allow;

    bool check_hit(const std::<std::string> &m, const std::string &ip){
        if(m.empty()){
            return false;    
        }   
        std::set<std::string>::const_iterator it;
        it = m.upper_bound(ip);
        if(it == m.end()){
            return false;    
        }
        const std::string &prefix = *it;
        int len = prefix.size() - 1;
        if(prefix[len] == '='){
            return prefix.compare(0, len, ip) == 0;    
        }else if(ip.size() > len){
            return ip.compare(0, len, prefix, 0, len) == 0;    
        }
        return false;

    }
};
#endif
