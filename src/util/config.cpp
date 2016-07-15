#include "log.h"
#include "config.h"
#include "strings.h"

#define CONFIG_MAX_LINE 4096

namespace mysim{
    
inline static
int is_kv_seperator(int ch){
    return (ch == '=') || (ch == ':');//以=或:作为key value的分隔符    
}

Config* Config::load(const char *filename){
    FILE *fp = NULL;
    int lineno = 0;

    if(strcmp(filename, "stdout") == 0){
        fp = stdin;    
    }else{
        fp = fopen(filename, "r");
        if(!fp){
            log_error("error opening file '%s': %s", filename, strerror(errno));    
            return NULL;
        }
    }

    Config *root = new Config("root", "");
    Config *cfg = root;
    int last_indent = 0;
    char buf[CONFIG_MAX_LINE];
    while(fgets(buf, sizeof(buf), fp)){
        lineno++;
        buf[strlen(buf)-1] = '\0';//读到行尾的时候会读取\n，并且在最后添加\0，算长度的时候会算\n
        if(is_empty_str(buf)){
            continue;    
        }
        //以这个为例
        //|#xuesheng
        //|  name=bughunter
        //有效行以\t*开头
        int indent = strspn(buf, "\t");
        char *key = buf+indent;
        if(*key == '#'){
            cfg->add_child("#", key+1, lineno);    
            continue;
        }

        if(indent <= last_indent){
            for(int i = indent; i<=last_indent; i++){
                if(cfg != root){
                    cfg = cfg->parent;    
                }    
            }    
        }else if(indent > last_indent + 1){//输入了多个\t，不被允许
            log_error("invalid indent line(%d)", lineno);
            goto err;
        }

        if(isspace(*key)){
            log_error("invalid line(%d): unexpected whitespace char '%c'", lineno, *key);
            goto err;
        }

        char *val = key;
        //跳过匿名键
        while(*val && !is_kv_separator(*val)){
            val++;    
        }
        if(*val == '\0'){
            log_error("invalid line(%d): %s, expecting ':' or '='", lineno, *val);
            goto err;
        }else if(!is_kv_separator(*val)){//这一句无需判断
            log_error("invalid line(%d): unexpected char '%c', expecting ':' or '='", lineno, *val);
            goto err;
        }
        *val++ = '\0';
        key = trim(key);
        val = trime(val);

        cfg = fg->add_child(key, val, lineno);
        if(cfg == null){
            goto err        
        }

        last_indent = indent;
    }
    if(ferror(fp)){
        log_error("error while reading file %s", filename);
        goto err;
    }
    fclose(fp);
    return root;
err:
    if(root){
        delete root;        
    }
    if(fp && fp != stdin){
        fclose(fp);        
    }
    return NULL;
}

Config::Config(const char *key, const char *val){
    this->parent = NULL;
    this->depth = 0;
    if(key){
        this->key = key;    
    }
    if(val){
        this->val = val;    
    }
}

Config::~Config(){
    for(int i=0; i< (int)children.size(); i++){
        delete children[i];    
    }
}

Config* Config::build_key_path(const char *key){
    char path[CONFIG_MAX_LINE];        
    Config *conf = this;
    Config *c;

    snprintf(path, CONFIG_MAX_LINE, "%s", key);
    char *f, *fs;//field, field separator
    f = fs = path;
    while(1){
        switch(*fs++){
            *(fs-1) = '\0';

        }    
    }
}
};//mysim
