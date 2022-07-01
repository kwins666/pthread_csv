#include "Task.h"

Taskqueue::Taskqueue()
{
	//��ʼ����
	pthread_mutex_init(&mutex,NULL);
}

Taskqueue::~Taskqueue()
{
	//������
	pthread_mutex_destroy(&mutex);
}

void Taskqueue::add(Task task) //�������
{
	pthread_mutex_lock(&mutex);
	Task_queue.push(task);
	pthread_mutex_unlock(&mutex);
}

void Taskqueue::add(callback f, void * arg) //ȡ���� ����
{
	pthread_mutex_lock(&mutex);
	Task_queue.push(Task(f,arg));
	pthread_mutex_unlock(&mutex);
}

Task Taskqueue::get() //ȡ����
{
	Task task;
	pthread_mutex_lock(&mutex);
	if (!Task_queue.empty()) //��Ϊ�վ�ȡ����
	{
		task = Task_queue.front();
		Task_queue.pop();
	}
	pthread_mutex_unlock(&mutex);
	return task;
}
