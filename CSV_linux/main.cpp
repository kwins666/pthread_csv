#include<cstdio>
#include<unistd.h>
#include"Threadpool.h"
#include"Threadpool.cpp"
#include<fstream>
#include<sstream>
#include<thread>
using namespace std;
vector<vector<string>>steam; //steam.csv �µĵ��������Լ��۸�


//�̳߳صĲ��Ժ���
void f1(void*arg)
{
	int num = *static_cast<int*>(arg);
	cout << "task " << num << endl;
	sleep(2);
}


//�Ƚϼ۸���㷨
float compare(float &a,float &b)
{
	//b�۳�5%�����������󻹱�����a�ļ۸�ߣ��Ǿͷ���׬�˶���
	if (b*0.95 > a) {
		return (b * 0.95-a);
	}
	else return -1.0;
}


//����csv�ļ�
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

	//��steam������ƥ���������
	for (int i=0;i<steam.size();++i)
	{
		//���������Ƿ���ͬ
		if (v[0] == steam[i][0])
		{
			//�Ƚϼ۸񣬾����Ƿ�����
			float price1 = stof(v[1]);
			float price2 = stof(steam[i][1]);
			float res = compare(price1, price2);
			if (res > 0.0) {
				cout << "Buy " << v[0] << " and earn $" << res << "." << endl;

				//�����������д��res��
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


//���̳߳ط�����
void f3(Threadpool<string>&tp)
{
	//��ȡ����buff�������Ƽ��۸񣬲����������������
	ifstream ifs;
	ifs.open("wangyi.csv", ios::in);
	if (!ifs.is_open()) {
		cout << "failed to open wangyi.csv!" << endl;
		ifs.close();
		return ;
	}

	//�̳߳س�ʼ��
	//Threadpool<string>*tp=new Threadpool<string>(3, 10);
	//��ȡ wangyi.csv ����
	string s = "";
	while (getline(ifs, s))
	{
		//ÿ�ζ�ȡһ�����ݣ����Ӹ��������
		string* new_s = new string(s);
		tp.AddTask(Task<string>(f2, new_s));
	}
	ifs.close();
}


//��steam�������Ƽ۸�ȫ����ȡ������ȫ�ֱ��� vector<vector<string>>steam ��
void unit_steam()
{
	ifstream ifs;
	ifs.open("steam.csv", ios::in);
	if (!ifs.is_open()) {
		cout << "failed to open steam.csv!" << endl;
		ifs.close();
		return;
	}
	//��ʱ�洢�����Լ��۸�
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
		//����õ�����ѹ��steam������
		steam.push_back(v);
		v.clear();
	}
	for (auto it=steam.begin();it<steam.end();it++)
	{
		cout << "name:" << (*it)[0] << " price:" << (*it)[1] << endl;
	}
	cout << "steam unit finished." << endl;
	ifs.close();

	//��ʼ��res
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
	//��ʼ��steam
	unit_steam();

	Threadpool<string> tp(3, 10);
	f3(tp);
	//��ʱ1s���ù����̹߳�������ʹ��m_busy��=0����~Threadpool() �ڿɵȴ������߳���ɹ���
	sleep(1);
	return 0;
}