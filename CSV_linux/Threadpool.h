#pragma once
#include<iostream>
#include<pthread.h>
#include"Taskqueue.h"
#include"Taskqueue.cpp"
using namespace std;
extern vector<vector<string>>steam;


template<class T>
class Threadpool
{
public:
	//创建线程池并初始化
	Threadpool(int min,int max);

	//销毁线程池
	~Threadpool();

	//给线程池添加任务
	void AddTask(Task<T> task);

	//获取正忙线程个数
	int getbusy();

	//获取存活线程个数
	int getalive();

private:
	//工作者线程任务函数
	static void* worker(void*arg);

	//管理者线程任务函数
	static void* manager(void*arg);

	//退出线程
	void quit();

private:
	Taskqueue<T>* task; //管理任务队列
	pthread_t managerID; //管理者线程ID
	pthread_t* workerIDs; //工作者线程数组 指针
	int m_min; //最少工作线程数
	int m_max;//最大工作线程数
	int m_busy;//正忙工作线程数
	int m_alive;//存活工作线程数
	int m_kill; //需要杀死的空闲线程数
	pthread_mutex_t mtx; //线程池的锁
	pthread_cond_t con; //任务队列是否为空 用于阻塞工作线程
	bool is_exit = 0;  //线程池未启动
};