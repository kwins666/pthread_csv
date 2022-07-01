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
		is_exit = true; //线程池存在 
		 //线程池锁,条件变量 初始化
		if (pthread_mutex_init(&mtx, NULL)!=0 || pthread_cond_init(&con, NULL) != 0)
		{
			cout << "mutex or condition creates fail." << endl;
			break;
		}
		//创建工作线程和管理线程
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
	//没有完全初始化成功的话，释放内存
	if(task)
		delete task;
	if(workerIDs)
		delete workerIDs;
}

template<class T>
Threadpool<T>::~Threadpool()
{
	while (this->m_busy != 0) //等待所有工作线程工作完毕
	{
	}
	while (m_alive != m_min) //用内置成员方法先把多余线程杀死
	{
	}
	is_exit = 0;
	cout << "xigou" << endl;
	
	//叫醒工作队列中所有阻塞的线程，因为线程池已关闭，所以他们自动调用销毁函数
	for (int i=0;i<m_min;i++)
	{
		pthread_cond_signal(&con);
	}
	//sleep(5);
	//主线程等待 管理者线程结束
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

	//添加任务
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
void * Threadpool<T>::worker(void * arg) //工作者线程任务函数 - 不断尝试接任务并执行
{
	//这个函数是静态的，要访问非静态成员，就需要借助this指针，因此传进来一个this
	Threadpool* pool = static_cast<Threadpool*>(arg);
	while (1)
	{
		pthread_mutex_lock(&pool->mtx);
		//线程池开启，且任务队列为空
		//2种情况 ：
		//1 暂时没往任务队列加任务  
		while (pool->task->num()==0 && pool->is_exit==1)
		{
			//等待任务 阻塞
			pthread_cond_wait(&pool->con,&pool->mtx);
			
			//任务是不是有点少了，需要减少工作线程不？
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
		//2 不再往任务队列加任务（线程池关闭了）
		if (pool->is_exit == 0)
		{
			//pthread_mutex_unlock(&pool->mtx);
			//cout << "2222" << endl;
			pool->quit();
		}

		//取任务
		Task<T> t;
		t = pool->task->get();
		pool->m_busy++;
		pthread_mutex_unlock(&pool->mtx);

		cout << "thread " << pthread_self() << " " << "starts working..." ;

		//执行任务
		t.function(t.arg);
		
		//结束任务
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
		//3s检测一次
		sleep(2);

		//获取任务队列的任务数量 和 工作队列的存活线程数
		pthread_mutex_lock(&pool->mtx);
		int task_num = pool->task->num();
		int alive_num = pool->m_alive;
		int busy_num = pool->m_busy;
		pthread_mutex_unlock(&pool->mtx);

		//获取正在工作的工作队列线程数量
		/*pthread_mutex_lock(&pool->mtx);
		int busy_num = pool->m_busy;
		pthread_mutex_unlock(&pool->mtx);*/

		//是否需要添加或者删除空闲线程
		//任务太多 增加一点线程
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

		//任务太少，减少一些工作线程
		cout <<"working num:"<< busy_num <<" alivenum:"<<alive_num <<endl;
		if ( ((pool->m_busy*2) < pool->m_alive) && (pool->m_alive > pool->m_min)) 
		{
			pthread_mutex_lock(&pool->mtx);
			pool->m_kill++;
			cout << "m_kill:"<<pool->m_kill << endl;
			pthread_mutex_unlock(&pool->mtx);

			//唤醒阻塞的线程，没接到任务就会自己销毁
			pthread_cond_signal(&pool->con);
		}
		
	}
	while (pool->m_alive!=0) //阻塞管理者线程，等待工作线程全部退出完毕
	{

	}
	cout << "manager quit:" << pthread_self();
	pthread_exit(NULL);
	return NULL;
}

template<class T>
void Threadpool<T>::quit()  //退出单个线程
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