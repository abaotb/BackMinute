#ifndef MDSPI_H
#define MDSPI_H
#include <map>
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <ctime>
#include<cmath>
#include<share.h>
#include <windows.h>
#include <cstring>
#include <string.h>
#include ".\ThostTraderApi\ThostFtdcMdApi.h"

#pragma region 用于Mysql
//#include <WinSock2.h>  
#include "winsock.h" 
#include "mysql.h"

#pragma comment(lib,"libmysql.lib")  
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF") rename("BOF","adoBOF")
#pragma endregion  用于Mysql


using namespace std;


#pragma once
//#define local_host "192.168.211.126"  
//#define user_name "root"  
//#define user_pwd "root"  
//#define data_base "exchangedata"  
//#define table "oneminute_data"
//#define data_port 3306 
//#define database_driver "DSN=mysql"

//------------------------
//数据库配置信息  
//#define local_host "192.168.200.207"  
//#define user_name "root"  
//#define user_pwd "mysql"  
//#define data_base "exchangedata"  
//#define table "oneminute_data"
//#define data_port 3306 
//#define database_driver "DSN=mysql"
//------------------------

//------------------------
//缓冲区大小
#define MAXNUMBER 300000
//合约的个数
#define CONTRACTCOUNT 500
//存满多少个就写入文件401
#define SUM 500
#define TICKCOUNT 73000
#define FS_TICKCOUNT 7300
#define OM_TICKCOUNT 650
//#define DATA_TIME_INTERVAL 60          //默认一分钟
//------------------------


//------------------------
//2tick时间差
#define TIMEOUT 300

#pragma region 早中晚开程序时间设置 
//早中晚开程序时间设置 

#define OPEN_MORNING 90000           //早盘开盘时间
#define OPEN_NOON    130000			 //午盘开盘时间
#define OPEN_EVENING 210000			 //夜盘开盘时间
#pragma endregion 早中晚开程序时间设置

#pragma region 停盘时间
//停盘时间
#define CLOSE_MORNING 113000		//中午停盘时间
#define CLOSE_NOON	  150000		//下午停盘时间
#define CLOSE_EVENING 23000			//晚上停盘时间

#define TIME_DIF      1500           //偏差，用于90000+1500 150000+1500
#pragma endregion 停盘时间 

//------------------------

//////////////////////////////////////////////////////////////////////////
///自定义深度行情-Tick------------在OperatorData.h中用到  
///主要将开高低收价赋于统一价格
typedef struct 
{
	///交易所代码
	char	ExchangeID[15];
	///合约代码
	char InstrumentID[10];
	///交易日
	int TradingDay;
	///最后修改时间
	int	UpdateTime;
	///最新价
	TThostFtdcPriceType	LastPrice;
	///成交金额
	TThostFtdcMoneyType	Turnover;
	///持仓量
	TThostFtdcLargeVolumeType	OpenInterest;
	//仓差
	TThostFtdcLargeVolumeType OpenInterestRange;
	///申买价一
	TThostFtdcPriceType	BidPrice1;
	///申卖价一
	TThostFtdcPriceType	AskPrice1;
	///数量
	TThostFtdcVolumeType	Volume;
	///申买量一
	TThostFtdcVolumeType	BidVolume1;
	///申卖量一
	TThostFtdcVolumeType	AskVolume1;

}CThostFtdcDepthMarketDataField_Custom_Tick;
///自定义深度行情-Other  today-1min
///K线图的开高低收价
typedef struct{
	///交易所代码
	char	ExchangeID[15];
	///合约代码
	char InstrumentID[10];
	///交易日
	int TradingDay;
	///最后修改时间
	int	UpdateTime;
	///今开盘
	TThostFtdcPriceType	OpenPrice;
	///今收盘
	TThostFtdcPriceType	ClosePrice;
	///最高价
	TThostFtdcPriceType	HighestPrice;
	///最低价
	TThostFtdcPriceType	LowestPrice;
	///成交金额
	TThostFtdcMoneyType	Turnover;
	///持仓量
	TThostFtdcLargeVolumeType	OpenInterest;
	//仓差
	TThostFtdcLargeVolumeType OpenInterestRange;
	///申买价一
	TThostFtdcPriceType	BidPrice1;
	///申卖价一
	TThostFtdcPriceType	AskPrice1;
	///数量
	TThostFtdcVolumeType	Volume;
	///申买量一
	TThostFtdcVolumeType	BidVolume1;
	///申卖量一
	TThostFtdcVolumeType	AskVolume1;

}CThostFtdcDepthMarketDataField_Custom_Other;
//////////////////////////////////////////////////////////////////////////

///自定义深度行情-Tick------------下面用到   original或tick  
///稍微修改了一下CThostFtdcDepthMarketDataField,主要还是修改了一下日期TradingDay类型
///去除一些数据
typedef struct 
{
	///交易所代码
	char	ExchangeID[15];
	///合约代码
	char	InstrumentID[10];
	///交易日
	int	TradingDay;
	///最新价
	TThostFtdcPriceType	LastPrice;
	///昨收盘
	TThostFtdcPriceType	PreClosePrice;
	///昨持仓量
	TThostFtdcLargeVolumeType	PreOpenInterest;
	///数量
	TThostFtdcVolumeType	Volume;
	///成交金额
	TThostFtdcMoneyType	Turnover;
	///今开盘
	TThostFtdcPriceType	OpenPrice;
	///今收盘
	TThostFtdcPriceType	ClosePrice;
	///持仓量
	TThostFtdcLargeVolumeType	OpenInterest;
	///涨停板价
	TThostFtdcPriceType	UpperLimitPrice;
	///跌停板价
	TThostFtdcPriceType	LowerLimitPrice;
	///昨虚实度
	TThostFtdcRatioType	PreDelta;
	///今虚实度
	TThostFtdcRatioType	CurrDelta;
	///最后修改时间
	int	UpdateTime;
	///最后修改毫秒
	TThostFtdcMillisecType	UpdateMillisec;
	///申买量一
	TThostFtdcVolumeType	BidVolume1;
	///申卖量一
	TThostFtdcVolumeType	AskVolume1;
	///申买价一
	TThostFtdcPriceType	BidPrice1;
	///申卖价一
	TThostFtdcPriceType	AskPrice1;	
	///当日均价
	TThostFtdcPriceType	AveragePrice;
	//仓差
	TThostFtdcLargeVolumeType OpenInterestRange;
	///接收数据的系统时间
	unsigned int ReceiveTime;
}CThostFtdcDepthMarketDataField_Custom;

//声明
class CMdSpi : public CThostFtdcMdSpi
{
public:
	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);
		
	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();
	
	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

private:
	void ReqUserLogin();
	void SubscribeMarketData();
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
};
// 合约类
class  CThostFtdcDepthMarketDataField_Custom_Clean_Contract{
	//自定义数据
 public :
	CThostFtdcDepthMarketDataField_Custom data;
	//20151013                 tangb
	//是否是初始化标志  区别第一次是否给fronttime_1和fronttime_2赋值为datatemp->UpdateTime
	bool is_initflag;
	//上一笔数据的时间
	int fronttime_1;
	//上两笔数据的时间
	int fronttime_2;
	///最后修改毫秒，0和500，或者400和900。。。
	int	end_writetime_Millisec;
	//合约是否修改过
	bool flag_contract;
	//第一笔数据是否补
	bool flag_isfirst;
	//构造函数
	CThostFtdcDepthMarketDataField_Custom_Clean_Contract(){
		//data = new CThostFtdcDepthMarketDataField_Custom();	
		is_initflag = false;
		fronttime_1 = 0;
		fronttime_2 = 0;
		end_writetime_Millisec = 0;
		flag_contract= false;
		flag_isfirst=false;
	}
	~CThostFtdcDepthMarketDataField_Custom_Clean_Contract(){};
};
//交易所数据状态类，主要负责查缺补漏数据。
class CThostFtdcDepthMarketDataField_Custom_Clean
{
public:	
	//每个交易所的第一笔时间,以毫秒计算
	unsigned int firsttime;
	//每个交易所的最后一笔时间,以毫秒计算
	unsigned int lasttime;
	//交易所是否修改过
	bool flag_exchange;

	//清洗数据的map，string为合约名
	map<string,CThostFtdcDepthMarketDataField_Custom_Clean_Contract >totalTable;
	//大于200ms时
	void methodMoreThan200(CThostFtdcDepthMarketDataField_Custom &datatemp);
	//判断数据重复
	void methodIsRepetition(CThostFtdcDepthMarketDataField_Custom &datatemp);	

	//弥补数据
	void methodIsMising();
	//构造函数
	CThostFtdcDepthMarketDataField_Custom_Clean(){
		//totalTable = new map<string,CThostFtdcDepthMarketDataField_Custom_Clean_Contract >();
		firsttime = GetTickCount();
		lasttime = GetTickCount();
		flag_exchange = false;	
	}
	~CThostFtdcDepthMarketDataField_Custom_Clean(){};
};
//缓冲区类，主要负责添加数据，取数据，转换数据，还有找出每种合约相对应的交易所
class CMdSpi_Custom:public  CMdSpi
{
public:
	//每一种合约对应的交易所
	map<string,string>exchange_map;                                       //（合约IF1510，交易所cffex）
	//数据整理前的缓冲区
	queue<CThostFtdcDepthMarketDataField_Custom> before_data_buffer;      
	//数据整理后的缓冲区
	queue<CThostFtdcDepthMarketDataField_Custom> after_data_buffer;          
	//数据整理后的缓冲区
	queue<CThostFtdcDepthMarketDataField_Custom> data_to_database;          //20150924 tangb 用于写数据库的队列,和 after_data_buffer一模一样
	//区分第一笔还是第二笔的毫秒数，是0ms还是500ms
	//int MILLISEC;
	//添加整理前数据
	void methodAddDatas_Before(CThostFtdcDepthMarketDataField* data,unsigned int receivetime);
	//取整理前数据
	CThostFtdcDepthMarketDataField_Custom methodGetDatas_Before();
	//添加整理后数据
	void methodAddDatas_After(CThostFtdcDepthMarketDataField_Custom data);
	//取整理后数据
	CThostFtdcDepthMarketDataField_Custom methodGetDatas_After();
	//转换数据
	CThostFtdcDepthMarketDataField_Custom methodChangeToCustom(CThostFtdcDepthMarketDataField *data,unsigned int receivetime);
	//构造函数
	CMdSpi_Custom(){
	}
	~CMdSpi_Custom(){}
};

#endif MDSPI_H


