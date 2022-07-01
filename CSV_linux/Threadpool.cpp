#include "Threadpool.h"
#include<string.h>
#include<string>
#include<unistd.h>

template<class T>
Threadpool<T>::Threadpool(int min,int max):m_min(min),m_max(max)
{
	do
	{
		task = new Taskqueue<T>;
		if (task == NULL) break;
		workerIDs = new pthread_t[m_max];
		if (workerIDs == nullptr)
		{
			cout << "work queue create fail." << endl;
			break;
		}
		memset(workerIDs, 0, sizeof(pthread_t)*max);
		m_min = min;
		m_max = max;
		m_busy = 0;
		m_kill = 0;
		m_alive = min;
		is_exit = true; //�̳߳ش��� 
		 //�̳߳���,�������� ��ʼ��
		if (pthread_mutex_init(&mtx, NULL)!=0 || pthread_cond_init(&con, NULL) != 0)
		{
			cout << "mutex or condition creates fail." << endl;
			break;
		}
		//���������̺߳͹����߳�
		pthread_create(&managerID,NULL,manager,this);
		cout<<"manageId = "<<managerID <<endl;
		pthread_mutex_lock(&mtx);
		for (int i=0;i<m_min;++i)
		{
			pthread_create(&workerIDs[i],NULL,worker,this);
		}
		for (int i = 0; i < m_min; ++i)
		{
			cout << workerIDs[i] << endl;
		}
		pthread_mutex_unlock(&mtx);
		return;

	} while (0);
	//û����ȫ��ʼ���ɹ��Ļ����ͷ��ڴ�
	if(task)
		delete task;
	if(workerIDs)
		delete workerIDs;
}

template<class T>
Threadpool<T>::~Threadpool()
{
	while (this->m_busy != 0) //�ȴ����й����̹߳������
	{
	}
	while (m_alive != m_min) //�����ó�Ա�����ȰѶ����߳�ɱ��
	{
	}
	is_exit = 0;
	cout << "xigou" << endl;
	
	//���ѹ��������������������̣߳���Ϊ�̳߳��ѹرգ����������Զ��������ٺ���
	for (int i=0;i<m_min;i++)
	{
		pthread_cond_signal(&con);
	}
	//sleep(5);
	//���̵߳ȴ� �������߳̽���
	pthread_join(managerID, NULL);

	if (task) delete task;
	if (workerIDs) delete[] workerIDs;

	pthread_mutex_destroy(&mtx);
	pthread_cond_destroy(&con);

}

template<class T>
void Threadpool<T>::AddTask(Task<T> task)
{
	if (is_exit == 0)
	{
		return;
	}

	//�������
	this->task->add(task);
	pthread_cond_signal(&con);
}

template<class T>
int Threadpool<T>::getbusy()
{
	pthread_mutex_lock(&mtx);
	int num = m_busy;
	pthread_mutex_unlock(&mtx);
	return num;
}

template<class T>
int Threadpool<T>::getalive()
{
	pthread_mutex_lock(&mtx);
	int num = m_alive;
	pthread_mutex_unlock(&mtx);
	return num;
}

template<class T>
void * Threadpool<T>::worker(void * arg) //�������߳������� - ���ϳ��Խ�����ִ��
{
	//��������Ǿ�̬�ģ�Ҫ���ʷǾ�̬��Ա������Ҫ����thisָ�룬��˴�����һ��this
	Threadpool* pool = static_cast<Threadpool*>(arg);
	while (1)
	{
		pthread_mutex_lock(&pool->mtx);
		//�̳߳ؿ��������������Ϊ��
		//2����� ��
		//1 ��ʱû��������м�����  
		while (pool->task->num()==0 && pool->is_exit==1)
		{
			//�ȴ����� ����
			pthread_cond_wait(&pool->con,&pool->mtx);
			
			//�����ǲ����е����ˣ���Ҫ���ٹ����̲߳���
			if (pool->m_kill > 0)
			{
				if (pool->m_alive > pool->m_min)
				{
					pool->m_kill--;
					//pool->m_alive--;
					//pthread_mutex_unlock(&pool->mtx);
					cout << "kill a thread." << endl;
					pool->quit();
				}
			}
		}
		//2 ������������м������̳߳عر��ˣ�
		if (pool->is_exit == 0)
		{
			//pthread_mutex_unlock(&pool->mtx);
			//cout << "2222" << endl;
			pool->quit();
		}

		//ȡ����
		Task<T> t;
		t = pool->task->get();
		pool->m_busy++;
		pthread_mutex_unlock(&pool->mtx);

		cout << "thread " << pthread_self() << " " << "starts working..." ;

		//ִ������
		t.function(t.arg);
		
		//��������
		delete t.arg;
		t.arg = nullptr;
		cout << "thread " << pthread_self() << " " << "ends working..." << endl<<endl;
		pthread_mutex_lock(&pool->mtx);
		pool->m_busy--;
		pthread_mutex_unlock(&pool->mtx);
	
	}
	return nullptr;
}

template<class T>
void * Threadpool<T>::manager(void * arg)
{
	Threadpool* pool = static_cast<Threadpool*>(arg);
	while (pool->is_exit==1)
	{
		//3s���һ��
		sleep(2);

		//��ȡ������е��������� �� �������еĴ���߳���
		pthread_mutex_lock(&pool->mtx);
		int task_num = pool->task->num();
		int alive_num = pool->m_alive;
		int busy_num = pool->m_busy;
		pthread_mutex_unlock(&pool->mtx);

		//��ȡ���ڹ����Ĺ��������߳�����
		/*pthread_mutex_lock(&pool->mtx);
		int busy_num = pool->m_busy;
		pthread_mutex_unlock(&pool->mtx);*/

		//�Ƿ���Ҫ��ӻ���ɾ�������߳�
		//����̫�� ����һ���߳�
		if (task_num>pool->m_alive && pool->m_alive < pool->m_max)
		{
			pthread_mutex_lock(&pool->mtx);
			for (int i=0;i<pool->m_max;++i)
			{
				if (pool->workerIDs[i] == 0)
				{
					pthread_create(&pool->workerIDs[i],NULL,&pool->worker,pool);
					pool->m_alive++;
					cout << "create a new thread" << endl;
					break;
				}
			}
			pthread_mutex_unlock(&pool->mtx);
			//cout << "tiaochu xunhuan." << endl;
		}

		//����̫�٣�����һЩ�����߳�
		cout <<"working num:"<< busy_num <<" alivenum:"<<alive_num <<endl;
		if ( ((pool->m_busy*2) < pool->m_alive) && (pool->m_alive > pool->m_min)) 
		{
			pthread_mutex_lock(&pool->mtx);
			pool->m_kill++;
			cout << "m_kill:"<<pool->m_kill << endl;
			pthread_mutex_unlock(&pool->mtx);

			//�����������̣߳�û�ӵ�����ͻ��Լ�����
			pthread_cond_signal(&pool->con);
		}
		
	}
	while (pool->m_alive!=0) //�����������̣߳��ȴ������߳�ȫ���˳����
	{

	}
	cout << "manager quit:" << pthread_self();
	pthread_exit(NULL);
	return NULL;
}

template<class T>
void Threadpool<T>::quit()  //�˳������߳�
{
	pthread_t id = pthread_self();
	//cout << id << endl;
	//pthread_mutex_lock(&mtx);
	for (int i=0;i<m_max;i++)
	{
		//cout << workerIDs[i] << endl;
		if (id == workerIDs[i])
		{
			workerIDs[i] = 0;
			cout << "thread " << id << " was quited" << endl;
			this->m_alive--;
			//cout<<"this in the quit() :" << this->m_alive<<endl;
			break;
		}
	}
	pthread_mutex_unlock(&mtx);
	pthread_exit(NULL);
}