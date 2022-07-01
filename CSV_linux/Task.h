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
	void add(Task task); //�������
	void add(callback f,void*arg); //����
	Task get(); //ȡ������
	inline int num() //��������������������
	{
		return Task_queue.size();
	}
private:
	queue<Task>Task_queue;
	pthread_mutex_t mutex; //���������
};

