#pragma once
#include<queue>
#include<pthread.h>
using namespace std;
using callback = void(*)(void*);
class Task
{
public:
	Task()
	{
		function = nullptr;
		arg = nullptr;
	}
	Task(callback f,void*arg)
	{
		function = f;
		this->arg = arg;
	}
private:
	callback function;
	void*arg;
};

class Taskqueue
{
	Taskqueue();
	~Taskqueue();
public:
	void add(Task task); //添加任务
	void add(callback f,void*arg); //重载
	Task get(); //取出任务
	inline int num() //返回任务队列中任务个数
	{
		return Task_queue.size();
	}
private:
	queue<Task>Task_queue;
	pthread_mutex_t mutex; //任务队列锁
};

