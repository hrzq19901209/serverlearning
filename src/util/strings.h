#ifndef MYSIM_UTIL_STRING_H
#define MYSIM_UTIL_STRING_H

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <inttypes.h>
#include <string>
#include <math.h>

inline static 
int is_empty_str(const char *str){
    const char *p = str;
    while(*p && isspace(*p)){
        p++;
    }
    return *p == '\0';
}

inline static 
char *ltrim(const char *str){
    const char *p = str;
    while(*p && isspace(*p)){
        p++;
    }
    return (char *)p;
}

inline static
char *rtrim(char *str){
    char *p;
    p = str+strlen(str)-1;
    while(p >= str && isspace(*p)){
        p--;
    }
    *(++p) = '\0';
    return str;
}

inline static
char *lrtrim(char *str){
    char *p;
    p = ltrim(str);
    rtrim(p);
    return p;
}

inline static 
void strtolower(std::string *str){
    std::transform(str->begin(), str->end(), str->begin(), ::tolower);        
}

inline static 
void strtoupper(std::string *str){
    std::transform(str->begin(), str->end(), str->begin(), ::toupper);        
}

inline static
std::string real_dirname(const char *filepath){
    std::string dir;
    if(filepath[0] != '/'){
        char buff[2014];
        char *p = getcwd(buff,sizeof(buff));
        if(p != NULL){
            dir.append(p);
        }
        dir.append("/");
    }
    const char *p = strrchr(filepath, '/');
    if(p != NULL){
        dir.append(filepath, p-filepath);    
    }
    return dir;

}

inline static 
std::string str_escape(const char *s, int size){
    static const char *hex = "0123456789abcdef";
    std::string ret;
    for(int i=0; i<size; i++){
        char c=s[i];
        switch(c){
            case '\r':
                ret.append("\\r");
                break;
            case '\n':
                ret.append("\\n");
                break;
            case '\t':
                ret.append("\\t");
                break;
            case '\\':
                ret.append("\\\\");
                break;
            case ' ':
                ret.push_back(c);
                break;
            default:
                if(c >= '!' && c <= '~'){
                    ret.push_back(c);
                }else{//转换为16进制
                    ret.append("\\x");
                    unsigned char d = c;
                    ret.push_back(hex[d >> 4]);
                    ret.push_back(hex[d & 0x0f]);
                }
                break;
        }

    }
    return ret;
}

inline static
std::string str_escape(const std::string &s){
        return str_escape(s.data(), (int)s.size());
}

inline static
int hex_int(char c){
        if(c >= '0' && c <= '9'){
            return c-'0';
        }else{
            return c-'a'+10;
        }
}

inline static
std::string un_escape(const char *s, int size){
    std::string ret;
    for(int i=0; i<size; i++){
        char c = s[i];
        if(c != '\\'){
            ret.push_back(c);
        }else{
            if(i >= size-1){
                continue;
            }else{
                char cc = s[++i];
                switch(cc){
                    case 'a':
                        ret.push_back('\a');
                        break;
                    case 'b':
                        ret.push_back('\b');
                        break;
                    case 't':
                        ret.push_back('\t');
                        break;
                    case 'n':
                        ret.push_back('\n');
                        break;
                    case 'r':
                        ret.push_back('\r');
                        break;
                    case 'f':
                        ret.push_back('\f');
                        break;
                    case 'v':
                        ret.push_back('\v');
                        break;
                    case '\\':
                        ret.push_back('\\');
                        break;
                    case 'x':{
                        if(i < size-2){
                            char pre = s[++i];
                            char suf = s[++i];
                            ret.push_back((char)((hex_int(pre) << 4)+hex_int(suf)));
                        }
                        break;
                    default:
                        ret.push_back(cc);
                        break;
                }
            }
        }
    }
    }
    return ret;
}


inline static
std::string un_escape(const std::string &s){
    return un_escape(s.data(), (int)s.size());        
}

inline static
std::string hexmem(const void *p, int size){
    return str_escape((char *)p, size);        
}

inline static
void dump(const void *p, int size, const char *msg = NULL){
    if(msg == NULL){
        printf("dump <");
    }else{
        printf("%s <", msg);
    }
    std::string s = hexmem(p, size);
    printf("%s>\n", s.c_str());
}

inline static
std::string str(const char *s){
    return std::string(s);        
}

inline static
std::string str(int v){
    char buf[21] = {0};        
    snprintf(buf,sizeof(buf), "%d", v);
    return std::string(buf);
}

inline static
std::string str(uint32_t v){
    char buf[21] = {0};
    snprintf(buf, sizeof(buf), "%d", v);
    return std::string(buf);
}

static inline
std::string str(int64_t v){
        char buf[21] = {0};
        snprintf(buf, sizeof(buf), "%" PRId64 "", v);
        return std::string(buf);
}

static inline
std::string str(uint64_t v){
        char buf[21] = {0};
        snprintf(buf, sizeof(buf), "%" PRIu64 "", v);
        return std::string(buf);
}

static inline
std::string str(double v){
    char buf[21] = {0};
    if(v - floor(v) == 0){
        snprintf(buf, sizeof(buf), "%.0f", v);
    }else{
        snprintf(buf, sizeof(buf), "%f", v);
    }
    return std::string(buf);
}

static inline
std::string str(float v){
    return str((double)v);
}

// all str_to_xx methods set errno on error

static inline
int str_to_int(const std::string &str){
    const char *start = str.c_str();
    char *end;
    int ret = (int)strtol(start, &end, 10);
    // the WHOLE string must be string represented integer
    if(*end == '\0' && size_t(end - start) == str.size()){
        errno = 0;
    }else{
        // strtoxx do not set errno all the time!
        if(errno == 0){
            errno = EINVAL;
        }
    }
    return ret;
}

static inline
int str_to_int(const char *p, int size){
    return str_to_int(std::string(p, size));
}

static inline
int64_t str_to_int64(const std::string &str){
    const char *start = str.c_str();
    char *end;
    int64_t ret = (int64_t)strtoll(start, &end, 10);
    // the WHOLE string must be string represented integer
    if(*end == '\0' && size_t(end - start) == str.size()){
        errno = 0;
    }else{
        // strtoxx do not set errno all the time!
        if(errno == 0){
            errno = EINVAL;
        }
    }
    return ret;
}

static inline
int64_t str_to_int64(const char *p, int size){
    return str_to_int64(std::string(p, size));
}

static inline
uint64_t str_to_uint64(const std::string &str){
    const char *start = str.c_str();
    char *end;
    uint64_t ret = (uint64_t)strtoull(start, &end, 10);
    // the WHOLE string must be string represented integer
    if(*end == '\0' && size_t(end - start) == str.size()){
        errno = 0;
    }else{
        // strtoxx do not set errno all the time!
        if(errno == 0){
            errno = EINVAL;
        }
    }
    return ret;
}

static inline
uint64_t str_to_uint64(const char *p, int size){
    return str_to_uint64(std::string(p, size));
}

static inline
double str_to_double(const char *p, int size){
    return atof(std::string(p, size).c_str());
}

static inline
std::string substr(const std::string &str, int start, int size){
    if(start < 0){
        start = (int)str.size() + start;
    }
    if(size < 0){
        // 忽略掉 abs(size) 个字节
        size = ((int)str.size() + size) - start;
    }
    if(start < 0 || size_t(start) >= str.size() || size < 0){
        return "";
    }
    return str.substr(start, size);
}

static inline
std::string str_slice(const std::string &str, int start, int end){
    if(start < 0){
        start = (int)str.size() + start;
    }
    int size;
    if(end < 0){
        size = ((int)str.size() + end + 1) - start;
    }else{
        size = end - start + 1;
    }
    if(start < 0 || size_t(start) >= str.size() || size < 0){
        return "";
    }
    return str.substr(start, size);
}

static inline
int bitcount(const char *p, int size){
    int n = 0;
    for(int i=0; i<size; i++){
        unsigned char c = (unsigned char)p[i];
        while(c){
            n += c & 1;
            c = c >> 1;
        }
    }
    return n;
}

// is big endia. TODO: auto detect
#if 0
    #define big_endian(v) (v)
#else
    static inline
    uint16_t big_endian(uint16_t v){
        return (v>>8) | (v<<8);
    }

    static inline
    uint32_t big_endian(uint32_t v){
        return (v >> 24) | ((v >> 8) & 0xff00) | ((v << 8) & 0xff0000) | (v << 24);
    }

    static inline
    uint64_t big_endian(uint64_t v){
        uint32_t h = v >> 32;
        uint32_t l = v & 0xffffffffull;
        return big_endian(h) | ((uint64_t)big_endian(l) << 32);
    }
#endif


#endif
