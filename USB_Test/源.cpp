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
�������ƣ�ReadJsonFromFile
�������ܣ���Json�ļ��ж�ȡ����
������Σ��ļ��� 
����ֵ��0
*/

string ReadJsonFromFile(const char *filename)
{
	string MasterKey = "";
	string KCV = "";
	Json::Reader reader;//����json��Json::Reader
	Json::Value root;//Json::Value��һ������Ҫ�����ͣ����Դ����������ͣ���int,string,object,array.
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
�������ƣ�GetUDiskRoot
�������ܣ��ж�U���Ƿ���벢���U���̷�
������Σ���
����ֵ������U�̲��룬�������̷�����U�̲��룬���ؿ��ַ�
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
			// �ƶ��洢�豸
			UDiskRoot = pDriveStr;
			break;
		case DRIVE_FIXED:
			// �̶�Ӳ��������
			break;
		case DRIVE_REMOTE:
			// ����������
			break;
		case DRIVE_CDROM:
			// ����������
			break;
		}
		pDriveStr += szDriveStr + 1;
		szDriveStr = strlen(pDriveStr);
	}
	delete pForDelete;
	return UDiskRoot;
}

/*
�������ƣ�getFiles
�������ܣ����ض�Ŀ¼�²����ض���׺�����ļ�
������Σ�path��exd��
*/
void getFiles(string path, string exd, vector<string>& files)
{
	//�ļ����
	long   hFile = 0;
	//�ļ���Ϣ
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
			//������ļ����������ļ���,����֮
			//�������,�����б�
			//���Ƽ�ʹ�ã�ӲҪʹ�õĻ�����Ҫ�޸�else ��������
			/*if((fileinfo.attrib &  _A_SUBDIR))
			{
			if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
			getFiles( pathName.assign(path).append("\\").append(fileinfo.name), exd, files );
			}
			else */
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					//files.push_back(pathName.assign(path).append("\\").append(fileinfo.name)); // Ҫ�õ�����Ŀ¼ʹ�ø����
					//���ʹ��
					files.push_back(fileinfo.name); // ֻҪ�õ��ļ�����ʹ�ø����
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

/*
�������ƣ�GetMainKey
�������ܣ���ȡ����Կ
������Σ���
����ֵ������Կ
*/
string GetMainKey()
{
	string strTemp = GetUDiskRoot();//�ļ���Ŀ¼
	//cout << "strTemp:" << strTemp << endl;
	if (strTemp == "")
	{
		cout << "please insert USB!!" << endl;
		system("pause");
		return "false";
	}
	string FileCatalog = strTemp + "key";
	string FileExtension = "json";//�ļ���׺��
	vector<string> files;

	//��ȡ��·���µ�����json�ļ�
	getFiles(FileCatalog, FileExtension, files);

	int size = files.size();
	for (int i = 0; i < size; i++)
	{
		cout<<files[i].c_str()<<endl;
	}

	string FILE = FileCatalog + "\\" + files[0].c_str();
	string MasterKey = "";//������Կ��MasterKey�ֶε�ֵ
	string KCV = "";//������Կ��KCV�ֶ��е�ֵ
	string MainKey = "";//����Master+KCV��ֵ���м���@����
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
��LRESULT--ָ���ǴӴ��ڳ�����߻ص��������ص�32λֵ
 CALLBACK--�ص�����
 ���豸������/�γ���ʱ��WINDOWS����ÿ�����巢��WM_DEVICECHANGE ��Ϣ��
 ����Ϣ��wParam ֵ���� DBT_DEVICEARRIVAL ʱ����ʾMedia�豸�����벢���Ѿ����ã�
 ���wParamֵ����DBT_DEVICEREMOVECOMPLETE����ʾMedia�豸�Ѿ����Ƴ���
 HWND--��������������ʾ���(handle)��Wnd �Ǳ���������������ʾ���ڣ�����hWnd ��ʾ���ھ��
 UINT msg--windows��������Ϣ����Ϣ������ʶ����
 WPARAM--32λ���ͱ������޷���������unsigned int���������ʾʲô������message
 LPARM--32λ���ͱ����������ͣ�long���������ʾʲô������message
 DWORD--ȫ��Double Word����ָע���ļ�ֵ��ÿ��wordΪ2���ֽڵĳ��ȣ�DWORD ˫�ּ�Ϊ4���ֽڣ�ÿ���ֽ���8λ����32λ��

 typedef struct _DEV_BROADCAST_VOLUME {
 DWORD dbcv_size;
 DWORD dbcv_devicetype;
 DWORD dbcv_reserved;
 DWORD dbcv_unitmask;
 WORD dbcv_flags;
 } DEV_BROADCAST_VOLUME, *PDEV_BROADCAST_VOLUME;
 ����dbcv_unitmask �ֶα�ʾ��ǰ�ı�����������룬��һλ��ʾ��������A���ڶ�λ��ʾ��������B������λ��ʾ��������C���Դ����ơ���
 dbcv_flags ��ʾ������������������1�����ǹ����������������2�����������������������Ӳ�̡�U���򶼵���0
 */
LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == WM_DEVICECHANGE)//�ж��Ƿ���Media�豸����
	{
		if ((DWORD)wp == DBT_DEVICEARRIVAL)//�ж�Media�豸�Ƿ��ѱ����벢�ҿ���ʹ��
		{
			DEV_BROADCAST_VOLUME* p = (DEV_BROADCAST_VOLUME*)lp;
			if (p->dbcv_devicetype == DBT_DEVTYP_VOLUME)
			{
				int l = (int)(log(double(p->dbcv_unitmask)) / log(double(2)));
				printf("%c�̲������\n", 'A' + l);
			}
		}
		else if ((DWORD)wp == DBT_DEVICEREMOVECOMPLETE)//�ж�Media�豸�Ƿ��Ѿ����Ƴ�
		{
			DEV_BROADCAST_VOLUME* p = (DEV_BROADCAST_VOLUME*)lp;
			if (p->dbcv_devicetype == DBT_DEVTYP_VOLUME)
			{
				int l = (int)(log(double(p->dbcv_unitmask)) / log(double(2)));
				printf("%c�̱��ε���\n", 'A' + l);
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