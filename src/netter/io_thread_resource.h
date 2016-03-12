//
//  io_thread_resource.h
//  EventLoop
//
//  Created by ziteng on 16-3-11.
//  Copyright (c) 2016年 mgj. All rights reserved.
//

#ifndef __EventLoop__io_thread_resource_H__
#define __EventLoop__io_thread_resource_H__

#include <assert.h>

// io线程资源管理类, EventLoop, PendingPktMgr, HttpParserWrapper都会用到
// 分配和io线程数相匹配的资源数
template<typename T>
class IoThreadResource {
public:
    IoThreadResource();
    ~IoThreadResource();
    
    void Init(int io_thread_num);
    int GetThreadNum() { return io_thread_num_; }
    T* GetMainResource() { return main_thread_resource_; }
    T* GetIOResource(int handle);
    bool IsInited() { return inited_; }
private:
    T*      main_thread_resource_;
    T*      io_thread_resources_;
    int     io_thread_num_;
    bool    inited_;
};


template<typename T>
IoThreadResource<T>::IoThreadResource()
{
    inited_ = false;
    io_thread_num_ = 0;
    main_thread_resource_ = NULL;
    io_thread_resources_ = NULL;
}

template<typename T>
IoThreadResource<T>::~IoThreadResource()
{
    if (main_thread_resource_) {
        delete main_thread_resource_;
    }
    
    if (io_thread_resources_) {
        delete [] io_thread_resources_;
    }
}

template<typename T>
void IoThreadResource<T>::Init(int io_thread_num)
{
    io_thread_num_ = io_thread_num;
    main_thread_resource_ = new T;
    assert(main_thread_resource_);
    if (io_thread_num_ > 0) {
        io_thread_resources_ = new T[io_thread_num_];
    }
    
    inited_ = true;
}

template<typename T>
T* IoThreadResource<T>::GetIOResource(int handle)
{
    if (io_thread_num_ > 0) {
        return &io_thread_resources_[handle % io_thread_num_];
    } else {
        return main_thread_resource_;
    }
}

#endif