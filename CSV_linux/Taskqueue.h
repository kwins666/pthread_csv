#pragma once
#include<queue>
#include<pthread.h>
#include<string>
using namespace std;
using callback = void(*)(void*);
extern vector<vector<string>>steam;


template<class T>
class Task
{
//friend class Threadpool;
public:
	Task()
	{
		function = nullptr;
		arg = nullptr;
	}
	Task(callback f,void*arg)
	{
		function = f;
		this->arg = (T*)arg;
	}
public:
	callback function;
	T*arg;
};

template<class T>
class Taskqueue
{
public:
	Taskqueue();
	~Taskqueue();
public:
	void add(Task<T> task); //添加任务
	void add(callback f,void*arg); //重载
	Task<T> get(); //取出任务
	size_t num(); //返回任务队列中任务个数

private:
	queue<Task<T>> Task_queue;
	pthread_mutex_t mutex; //任务队列锁
};