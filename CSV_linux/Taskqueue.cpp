#include "Taskqueue.h"
#include<string>
#include<iostream>

template<class T>
Taskqueue<T>::Taskqueue()
{
	//��ʼ����
	pthread_mutex_init(&mutex,NULL);
}

template<class T>
Taskqueue<T>::~Taskqueue()
{
	//������
	pthread_mutex_destroy(&mutex);
}

template<class T>
void Taskqueue<T>::add(Task<T> task) //�������
{
	pthread_mutex_lock(&mutex);
	Task_queue.push(task);
	pthread_mutex_unlock(&mutex);
}

template<class T>
void Taskqueue<T>::add(callback f, void * arg) // ����
{
	pthread_mutex_lock(&mutex);
	Task_queue.push(Task<T>(f,arg));
	pthread_mutex_unlock(&mutex);
}

template<class T>
Task<T> Taskqueue<T>::get() //ȡ����
{
	Task<T> task;
	pthread_mutex_lock(&mutex);
	if (!Task_queue.empty()) //��Ϊ�վ�ȡ����
	{
		task = Task_queue.front();
		Task_queue.pop();
	}
	pthread_mutex_unlock(&mutex);
	return task;
}

template<class T>
size_t Taskqueue<T>:: num() //��������������������
{
	return Task_queue.size();
}