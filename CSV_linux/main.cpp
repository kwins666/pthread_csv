#include<cstdio>
#include<unistd.h>
#include"Threadpool.h"
#include"Threadpool.cpp"
#include<fstream>
#include<sstream>
#include<thread>
using namespace std;
vector<vector<string>>steam; //steam.csv 下的道具名称以及价格


//线程池的测试函数
void f1(void*arg)
{
	int num = *static_cast<int*>(arg);
	cout << "task " << num << endl;
	sleep(2);
}


//比较价格的算法
float compare(float &a,float &b)
{
	//b扣除5%手续费卖出后还比买入a的价格高，那就返回赚了多少
	if (b*0.95 > a) {
		return (b * 0.95-a);
	}
	else return -1.0;
}


//处理csv文件
void f2(void*arg)
{
	string s = *static_cast<string*>(arg);
	stringstream ss(s);
	string temp = "";
	vector<string>v;
	while (getline(ss, temp, ','))
	{
		v.push_back(temp);
	}

	//在steam容器中匹配道具名称
	for (int i=0;i<steam.size();++i)
	{
		//道具名称是否相同
		if (v[0] == steam[i][0])
		{
			//比较价格，决定是否买入
			float price1 = stof(v[1]);
			float price2 = stof(steam[i][1]);
			float res = compare(price1, price2);
			if (res > 0.0) {
				cout << "Buy " << v[0] << " and earn $" << res << "." << endl;

				//道具相关数据写入res中
				ofstream ofs;
				ofs.open("res.csv", ios::app);
				if (!ofs.is_open()) {
					cout << "failed to open res.csv!" << endl;
					//ofs.close();
				}
				else {
					ofs << v[0] << "," << to_string(res) << endl;
					ofs.close();
				}
			}
			break;
		}
	}
	sleep(3);
}


//给线程池发任务
void f3(Threadpool<string>&tp)
{
	//读取网易buff道具名称及价格，并按行扔入任务队列
	ifstream ifs;
	ifs.open("wangyi.csv", ios::in);
	if (!ifs.is_open()) {
		cout << "failed to open wangyi.csv!" << endl;
		ifs.close();
		return ;
	}

	//线程池初始化
	//Threadpool<string>*tp=new Threadpool<string>(3, 10);
	//读取 wangyi.csv 数据
	string s = "";
	while (getline(ifs, s))
	{
		//每次读取一行数据，并扔给任务队列
		string* new_s = new string(s);
		tp.AddTask(Task<string>(f2, new_s));
	}
	ifs.close();
}


//把steam道具名称价格全部读取并存入全局变量 vector<vector<string>>steam 中
void unit_steam()
{
	ifstream ifs;
	ifs.open("steam.csv", ios::in);
	if (!ifs.is_open()) {
		cout << "failed to open steam.csv!" << endl;
		ifs.close();
		return;
	}
	//临时存储名称以及价格
	string s = "";
	vector<string>v;
	while (getline(ifs, s))
	{
		stringstream ss(s);
		string temp = "";
		while (getline(ss, temp, ','))
		{
			v.push_back(temp);
		}
		//处理好的数据压入steam容器中
		steam.push_back(v);
		v.clear();
	}
	for (auto it=steam.begin();it<steam.end();it++)
	{
		cout << "name:" << (*it)[0] << " price:" << (*it)[1] << endl;
	}
	cout << "steam unit finished." << endl;
	ifs.close();

	//初始化res
	ofstream ofs;
	ofs.open("res.csv", ios::app);
	if (!ofs.is_open()) {
		cout << "failed to open res.csv!" << endl;
		//ofs.close();
	}
	else {
		ofs << "name" << "," << "revenue" << endl;
		ofs.close();
	}
}

int main()
{
	//初始化steam
	unit_steam();

	Threadpool<string> tp(3, 10);
	f3(tp);
	//延时1s，让工作线程工作起来使得m_busy！=0，在~Threadpool() 内可等待所有线程完成工作
	sleep(1);
	return 0;
}