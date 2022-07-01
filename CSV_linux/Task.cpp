#include "Task.h"

Taskqueue::Taskqueue()
{
	//初始化锁
	pthread_mutex_init(&mutex,NULL);
}

Taskqueue::~Taskqueue()
{
	//销毁锁
	pthread_mutex_destroy(&mutex);
}

void Taskqueue::add(Task task) //添加任务
{
	pthread_mutex_lock(&mutex);
	Task_queue.push(task);
	pthread_mutex_unlock(&mutex);
}

void Taskqueue::add(callback f, void * arg) //取任务 重载
{
	pthread_mutex_lock(&mutex);
	Task_queue.push(Task(f,arg));
	pthread_mutex_unlock(&mutex);
}

Task Taskqueue::get() //取任务
{
	Task task;
	pthread_mutex_lock(&mutex);
	if (!Task_queue.empty()) //不为空就取任务
	{
		task = Task_queue.front();
		Task_queue.pop();
	}
	pthread_mutex_unlock(&mutex);
	return task;
}
