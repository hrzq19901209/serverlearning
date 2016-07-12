#include <iostream>
#include <errno.h>

#include "log.h"

using namespace std;

int main(int argc, char **argv){
        log_open("mylog.txt", Logger::LEVEL_DEBUG, true, 1024);
        for(int i=0; i<1000; i++){
            log_error("error opening file '%s'", "hello.txt");
        }
}
