#ifndef MYSIM_UTIL_THREAD_H_
#define MYSIM_UTIL_THREAD_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <queue>
#include <vector>

class Mutex {

private:
    pthread_mutex_t mutex;
public:
    Mutex(){
        pthread_mutex_init(&mutex, NULL);
    }

    ~Mutex(){
        pthread_mutex_destroy(&mutex);    
    }

    void lock(){
        pthread_mutex_lock(&mutex);    
    }

    void unlock(){
        pthread_mutex_unlock(&mutex);    
    }
        
};

class Locking{

private:
    Mutex *mutex;
    //不让赋值和拷贝
    Locking(const Locking&);
    void operator=(const Locking&);
public:
    Locking(Mutex *mutex){
        this->mutex = mutex;
        this->mutex->lock();
    }

    ~Locking(){
        this->mutex->unlock();    
    }
};

// Selectable queue, mutil writers, single reader
template<class T>
class SelectableQueue{

private:
    int fds[2];
public:
    Mutex mutex;
    std::queue<T> items;
    SelectableQueue();
    ~SelectableQueue();
    int fd(){
        return fds[0];    
    }

    int size();
    //multi writer
    int push(const T item);
    //single reader
    int pop(T *data);
};

template<class T>
SelectableQueue<T>::SelectableQueue(){
//创建一个简单的管道，若成功则为数组fd分配两个文件描述符，其中fd[0] 用于读取管道，fd[1]用于写入管道
    if(pipe(fds) == -1){
        exit(0);
    }        
}

template<class T>
SelectableQueue<T>::~SelectableQueue(){
    close(fds[0]);        
    close(fds[1]);
}


template<class T>
int SelectableQueue<T>::size(){
    Locking l(&mutex);
    return items.size();
}

template<class T>
int SelectableQueue<T>::push(const T item){
    Locking l(&mutex);
    items.push(item);
    if(::write(fds[1], "1", 1) == -1){
        exit(0);    
    }
    return 1;
}

template<class T>
int SelectableQueue<T>::pop(T *data){
    int n, ret = 1;
    char buf[1];
    while(true){
        n = ::read(fds[0], buf, 1);//当管道为空的时候，这里会被阻塞block
        if(n < 0){
            if(errno == EINTR){
                continue;    
            }else{
                return -1;    
            }
        }else if(n == 0){
            ret = -1;
        }else{
            Locking l(&mutex);
            if(items.empty()){
                fprintf(stderr, "%s %d error!\n", __FILE__, __LINE__);
                return -1;
            }
            *data = items.front();
            items.pop();
        }
        break;
    }
    return ret;
}
#endif
