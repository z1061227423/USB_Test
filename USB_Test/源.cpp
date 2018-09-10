#pragma once
#include <stdio.h>
#include <windows.h>
#include <dbt.h>
#include <math.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <io.h>
//#include "json/json.h"


using namespace std;

/*
函数名称：ReadJsonFromFile
函数功能：从Json文件中读取数据
函数入参：文件名 
返回值：0
*/

string ReadJsonFromFile(const char *filename)
{
	string MasterKey = "";
	string KCV = "";
	Json::Reader reader;//解析json用Json::Reader
	Json::Value root;//Json::Value是一个很重要的类型，可以代表任意类型，如int,string,object,array.
	ifstream is;
	is.open(filename, ios::binary);
	if (reader.parse(is, root, FALSE))
	{
		MasterKey = root["MasterKey"].asString();
		//cout << "MasterKey:" << MasterKey << endl;

		KCV = root["KCV"].asString();
		//cout << "KCV:" << KCV << endl;
	}
	string MainKey = MasterKey +"@"+ KCV;
	is.close();
	if (MasterKey == "" || KCV == "")
		return "FALSE";
	return MainKey;
}

/*
函数名称：GetUDiskRoot
函数功能：判断U盘是否插入并获得U盘盘符
函数入参：无
返回值：若有U盘插入，返回其盘符；无U盘插入，返回空字符
*/

string GetUDiskRoot()
{
	string UDiskRoot = "";
	UINT DiskType;
	size_t szAllDriveStr = GetLogicalDriveStrings(0, NULL);
	char *pDriveStr = new char[szAllDriveStr];
	char *pForDelete = pDriveStr;
	GetLogicalDriveStrings(szAllDriveStr, pDriveStr);
	size_t szDriveStr = strlen(pDriveStr);
	while (szDriveStr > 0)
	{
		DiskType = GetDriveType(pDriveStr);
		switch (DiskType)
		{
		case DRIVE_NO_ROOT_DIR:
			break;
		case DRIVE_REMOVABLE:
			// 移动存储设备
			UDiskRoot = pDriveStr;
			break;
		case DRIVE_FIXED:
			// 固定硬盘驱动器
			break;
		case DRIVE_REMOTE:
			// 网络驱动器
			break;
		case DRIVE_CDROM:
			// 光盘驱动器
			break;
		}
		pDriveStr += szDriveStr + 1;
		szDriveStr = strlen(pDriveStr);
	}
	delete pForDelete;
	return UDiskRoot;
}

/*
函数名称：getFiles
函数功能：在特定目录下查找特定后缀名的文件
函数入参：path，exd，
*/
void getFiles(string path, string exd, vector<string>& files)
{
	//文件句柄
	long   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string pathName, exdName;

	if (0 != strcmp(exd.c_str(), ""))
	{
		exdName = "\\*." + exd;
	}
	else
	{
		exdName = "\\*";
	}

	if ((hFile = _findfirst(pathName.assign(path).append(exdName).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是文件夹中仍有文件夹,迭代之
			//如果不是,加入列表
			//不推荐使用，硬要使用的话，需要修改else 里面的语句
			/*if((fileinfo.attrib &  _A_SUBDIR))
			{
			if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
			getFiles( pathName.assign(path).append("\\").append(fileinfo.name), exd, files );
			}
			else */
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					//files.push_back(pathName.assign(path).append("\\").append(fileinfo.name)); // 要得到绝对目录使用该语句
					//如果使用
					files.push_back(fileinfo.name); // 只要得到文件名字使用该语句
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

/*
函数名称：GetMainKey
函数功能：获取主密钥
函数入参：无
返回值：主密钥
*/
string GetMainKey()
{
	string strTemp = GetUDiskRoot();//文件夹目录
	//cout << "strTemp:" << strTemp << endl;
	if (strTemp == "")
	{
		cout << "please insert USB!!" << endl;
		system("pause");
		return "false";
	}
	string FileCatalog = strTemp + "key";
	string FileExtension = "json";//文件后缀名
	vector<string> files;

	//获取该路径下的所有json文件
	getFiles(FileCatalog, FileExtension, files);

	int size = files.size();
	for (int i = 0; i < size; i++)
	{
		cout<<files[i].c_str()<<endl;
	}

	string FILE = FileCatalog + "\\" + files[0].c_str();
	string MasterKey = "";//保存密钥中MasterKey字段的值
	string KCV = "";//保存密钥中KCV字段中的值
	string MainKey = "";//保存Master+KCV的值，中间以@隔开
	MainKey = ReadJsonFromFile(FILE.c_str());
	if (MainKey == "")
	{
		cout << "read MainKey error!" << endl;
		return "false";
	}
	//cout << "MainKey:" << MainKey << endl;

	return MainKey;
}

int main()
{
	string MainKey = GetMainKey();
	cout << "MainKey:" << MainKey << endl;
	system("pause");
	return 0;
}

/*
　LRESULT--指的是从窗口程序或者回调函数返回的32位值
 CALLBACK--回调函数
 当设备被插入/拔出的时候，WINDOWS会向每个窗体发送WM_DEVICECHANGE 消息，
 当消息的wParam 值等于 DBT_DEVICEARRIVAL 时，表示Media设备被插入并且已经可用；
 如果wParam值等于DBT_DEVICEREMOVECOMPLETE，表示Media设备已经被移出。
 HWND--是类型描述，表示句柄(handle)，Wnd 是变量对象描述，表示窗口，所以hWnd 表示窗口句柄
 UINT msg--windows上来的消息（消息常量标识符）
 WPARAM--32位整型变量，无符号整数（unsigned int），具体表示什么处决于message
 LPARM--32位整型变量，长整型（long），具体表示什么处决于message
 DWORD--全称Double Word，是指注册表的键值，每个word为2个字节的长度，DWORD 双字即为4个字节，每个字节是8位，共32位。

 typedef struct _DEV_BROADCAST_VOLUME {
 DWORD dbcv_size;
 DWORD dbcv_devicetype;
 DWORD dbcv_reserved;
 DWORD dbcv_unitmask;
 WORD dbcv_flags;
 } DEV_BROADCAST_VOLUME, *PDEV_BROADCAST_VOLUME;
 其中dbcv_unitmask 字段表示当前改变的驱动器掩码，第一位表示驱动器号A，第二位表示驱动器号B，第三位表示驱动器号C，以此类推……
 dbcv_flags 表示驱动器的类别，如果等于1，则是光盘驱动器；如果是2，则是网络驱动器；如果是硬盘、U盘则都等于0
 */
LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == WM_DEVICECHANGE)//判断是否有Media设备插入
	{
		if ((DWORD)wp == DBT_DEVICEARRIVAL)//判断Media设备是否已被插入并且可以使用
		{
			DEV_BROADCAST_VOLUME* p = (DEV_BROADCAST_VOLUME*)lp;
			if (p->dbcv_devicetype == DBT_DEVTYP_VOLUME)
			{
				int l = (int)(log(double(p->dbcv_unitmask)) / log(double(2)));
				printf("%c盘插进来了\n", 'A' + l);
			}
		}
		else if ((DWORD)wp == DBT_DEVICEREMOVECOMPLETE)//判断Media设备是否已经被移出
		{
			DEV_BROADCAST_VOLUME* p = (DEV_BROADCAST_VOLUME*)lp;
			if (p->dbcv_devicetype == DBT_DEVTYP_VOLUME)
			{
				int l = (int)(log(double(p->dbcv_unitmask)) / log(double(2)));
				printf("%c盘被拔掉了\n", 'A' + l);
			}
		}
		//cout << TRUE << endl;
		return 0;
	}
	else return DefWindowProc(h, msg, wp, lp);
}

void main01()
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.lpszClassName = TEXT("myusbmsg");
	wc.lpfnWndProc = WndProc;

	RegisterClass(&wc);
	HWND h = CreateWindow(TEXT("myusbmsg"), TEXT(""), 0, 0, 0, 0, 0, 0, 0, GetModuleHandle(0), 0);
	MSG msg;
	cout << wc.lpfnWndProc << endl;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}