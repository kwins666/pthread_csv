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
	//�����̳߳ز���ʼ��
	Threadpool(int min,int max);

	//�����̳߳�
	~Threadpool();

	//���̳߳��������
	void AddTask(Task<T> task);

	//��ȡ��æ�̸߳���
	int getbusy();

	//��ȡ����̸߳���
	int getalive();

private:
	//�������߳�������
	static void* worker(void*arg);

	//�������߳�������
	static void* manager(void*arg);

	//�˳��߳�
	void quit();

private:
	Taskqueue<T>* task; //�����������
	pthread_t managerID; //�������߳�ID
	pthread_t* workerIDs; //�������߳����� ָ��
	int m_min; //���ٹ����߳���
	int m_max;//������߳���
	int m_busy;//��æ�����߳���
	int m_alive;//�����߳���
	int m_kill; //��Ҫɱ���Ŀ����߳���
	pthread_mutex_t mtx; //�̳߳ص���
	pthread_cond_t con; //��������Ƿ�Ϊ�� �������������߳�
	bool is_exit = 0;  //�̳߳�δ����
};