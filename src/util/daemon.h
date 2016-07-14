#ifndef MYSIM_UTIL_DAEMON_H_
#define MYSIM_UTIL_DAEMON_H_

int daemonize(const char *dir=NULL){
    switch(fork()){
        case -1:
            return -1;
        case 0:
            break;
        default:
            exit(0);//父进程退出。
    }
    //之前parent和child运行在同一个session里,parent是会话（session）的领头进程,
    //parent进程作为会话的领头进程，如果exit结束执行的话，那么子进程会成为孤儿进程，并被init收养。
    //执行setsid()之后,child将重新获得一个新的会话(session)id。
    //这时parent退出之后,将不会影响到child了。
    if(setsid() == -1){
        exit(0);    
    }

    if(dir != NULL){
        if(chdir(dir) == -1 ){
            exit(0);    
        }    
    }

    if(close(STDIN_FILENO) == -1){
        exit(0);    
    }
    if(close(STDOUT_FILENO) == -1){
        exit(0);    
    }
    if(close(STDERR_FILENO) == -1){
        exit(0);    
    }

    int fd = open("/dev/null", O_RDWR, 0);
    if(fd == -1){
        exit(0);    
    }
    if(dup2(fd, STDIN_FILENO) == -1){
        exit(0);    
    }
    if(dup2(fd, STDOUT_FILENO) == -1){
        exit(0);    
    }
    if(dup2(fd, STDERR_FILENO) == -1){
        exit(0);    
    }
}

#endif
