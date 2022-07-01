#include "Taskqueue.h"
#include<string>
#include<iostream>

template<class T>
Taskqueue<T>::Taskqueue()
{
	//初始化锁
	pthread_mutex_init(&mutex,NULL);
}

template<class T>
Taskqueue<T>::~Taskqueue()
{
	//销毁锁
	pthread_mutex_destroy(&mutex);
}

template<class T>
void Taskqueue<T>::add(Task<T> task) //添加任务
{
	pthread_mutex_lock(&mutex);
	Task_queue.push(task);
	pthread_mutex_unlock(&mutex);
}

template<class T>
void Taskqueue<T>::add(callback f, void * arg) // 重载
{
	pthread_mutex_lock(&mutex);
	Task_queue.push(Task<T>(f,arg));
	pthread_mutex_unlock(&mutex);
}

template<class T>
Task<T> Taskqueue<T>::get() //取任务
{
	Task<T> task;
	pthread_mutex_lock(&mutex);
	if (!Task_queue.empty()) //不为空就取任务
	{
		task = Task_queue.front();
		Task_queue.pop();
	}
	pthread_mutex_unlock(&mutex);
	return task;
}

template<class T>
size_t Taskqueue<T>:: num() //返回任务队列中任务个数
{
	return Task_queue.size();
}