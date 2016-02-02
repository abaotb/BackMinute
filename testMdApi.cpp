// testTraderApi.cpp : 定义控制台应用程序的入口点。
//

#include "MdSpi.h"
#include "OperatorData.h"
#include "Custom_Methods.h"
using namespace std;


/************************************************************************/
/*                             自定义的变量                             */
/************************************************************************/

//FILE * file;
HANDLE hMutex_Receive,hMutex_Classify;															 //多线程
CRITICAL_SECTION cs;															 //临界区
CMdSpi_Custom * cmdSpi_custom;													 //缓冲区对象
CThostFtdcDepthMarketDataField_Custom_Clean*cffex_state,*czce_state,*dce_state,*shfe_state;//四个交易所对象
DWORD WINAPI Function_Receive(LPVOID lpParamter);                                             //多线程函数
DWORD WINAPI Classify_Data(LPVOID lpParamter);
DWORD WINAPI Classify_Data2(LPVOID lpParamter);
All_Contracts_Array all_contracts_array;									     //当天所有合约大数组
vector<CThostFtdcDepthMarketDataField_Custom_Other> cco_vector;								 //一分钟数据所有合约大数组
TSMD_Map* tsmd_map;																 //Tick,5Seconds,One_minute,One_Day组合类
/************************************************************************/
/*                             原有系统所需变量                         */
/************************************************************************/

CThostFtdcMdApi* pUserApi;														// UserApi对象							 
// 配置参数
char FRONT_ADDR[] = "tcp://180.169.124.125:41213";								// 前置地址
TThostFtdcBrokerIDType	BROKER_ID = "2030";										// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "00092";									// 投资者代码
TThostFtdcPasswordType  PASSWORD = "888888";								    // 用户密码
char *ppInstrumentID[CONTRACTCOUNT];//={"IF1407","SR407","SR409","TA409"};	// 行情订阅列表
int iInstrumentID = CONTRACTCOUNT;												// 行情订阅数量
int SUM_CONTRACTS;
int iRequestID = 0;																// 请求编号

/************************************************************************/
/*                                主程序入口                            */
/************************************************************************/
void main(void)
{

	//自动生成合约代码表声明
	//void methodAutoContract();
	//测试收集到的数据   
 	//readdata1();
 	//readdata2(); 
	//readdata3();
	
	tsmd_map = new TSMD_Map();
  	cmdSpi_custom = new CMdSpi_Custom();										//缓冲区类对象
	cffex_state=new CThostFtdcDepthMarketDataField_Custom_Clean();			    //四个交易所类对象
	dce_state = new CThostFtdcDepthMarketDataField_Custom_Clean();
	czce_state= new CThostFtdcDepthMarketDataField_Custom_Clean();
	shfe_state= new CThostFtdcDepthMarketDataField_Custom_Clean();

	//自动生成合约代码表
	//methodAutoContract();
  	//methodAutoContract2();
	methodAutoContract3();

	//	cout<<sizeof(CThostFtdcDepthMarketDataField_Custom_Other)<<" "<<sizeof(TSMD_Arrays);
	if (cmdSpi_custom->exchange_map.size()==0)
	{
		cout<<"nULL"<<endl;
	}
    for (int i=0;i<cmdSpi_custom->exchange_map.size()/*SUM_CONTRACTS*/;i++)
    {
   		
      	cout<<i<<"----------"<<ppInstrumentID[i]<<endl;
  	}
	cout<<"----------------------"<<endl;
	//初始化临界区
	InitializeCriticalSection(&cs);
	// 初始化UserApi
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();								// 创建UserApi
	CThostFtdcMdSpi* pUserSpi = new CMdSpi();
	pUserApi->RegisterSpi(pUserSpi);											// 注册事件类
	pUserApi->RegisterFront(FRONT_ADDR);										// connect
	pUserApi->Init();   
	//启动自定义线程
	hMutex_Receive = CreateThread(NULL, 0, Function_Receive, NULL, 0, NULL);
	//hMutex_Classify = CreateThread(NULL,0,Classify_Data,NULL,0,NULL);
	hMutex_Classify = CreateThread(NULL,0,Classify_Data2,NULL,0,NULL);   //基类方式调用
	
	
	pUserApi->Join();
	
	//pUserApi->Release();
}
