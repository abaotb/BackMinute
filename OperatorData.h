#ifndef OPERATORDATA_H
#define OPERATORDATA_H
#include "MdSpi.h"
using namespace std;
#pragma once
//四个类组合    //五个部分
class TSMD_Arrays{
public:
	//合约代码
	string contract;
	//1)记录Tick数据,通过5秒\1分钟\1天的Tick数据处理后的数组数据
	//CThostFtdcDepthMarketDataField_Custom_Tick data_tick[TICKCOUNT];												
	CThostFtdcDepthMarketDataField_Custom_Other data_fiveseconds[FS_TICKCOUNT];              
	CThostFtdcDepthMarketDataField_Custom_Other data_oneminute[OM_TICKCOUNT];                
	CThostFtdcDepthMarketDataField_Custom_Other data_oneday;  
	//2)记录Tick数据合约被插入的个数
	int index_tick;															       
	int index_fiveseconds;
	int index_oneminute; 
	int index_oneday; 
	//3)记录Tick数据当前时间 
	int time_tick;                                                                           
	int time_fivesecond;
	int time_oneminute;
	int time_oneday;
	//数据转化成自定义数据
	/*CThostFtdcDepthMarketDataField_Custom_Tick methodChangeToCustom(CThostFtdcDepthMarketDataField original_data);*/
	CThostFtdcDepthMarketDataField_Custom_Other methodTickToOther(CThostFtdcDepthMarketDataField_Custom_Tick original_data_tick);
	
	//4)找出x秒内的开收高低价等等
	//对一个tick数据处理，找出5秒内的开收高低价等等
	//void method_Fiveseconds(TSMD_Arrays *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick custom_tick);
	//对一个tick数据处理，找出1分钟内的开收高低价等等
	void method_Oneminute(TSMD_Arrays *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick custom_tick);
	
	
	//5)记录到数组中去
	//写入data_tick[73000]数组中去
	//void methodUpdate_tick(TSMD_Arrays *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick custom_tick);             
	//写入data_Fiveseconds[7300]数组中去
	//void methodUpdate_Fiveseconds(TSMD_Arrays *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick custom_tick);
	//写入data_Oneminute[650]数组中去
	void methodUpdate_Oneminute(TSMD_Arrays *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick custom_tick);
	//写入data_oneday去
	//void method_Oneday(TSMD_Arrays *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick custom_tick);

	TSMD_Arrays()
	{
		//合约代码
		contract="";
		//记录Tick数据,通过5秒\1分钟\1天的Tick数据处理后的数组数据

		//记录Tick数据合约被插入的个数
		index_tick=0;															       
		index_fiveseconds=0;
		index_oneminute=0; 
		index_oneday=0;		 
		//记录Tick数据当前时间 
		time_tick=0;                                                                     
		time_fivesecond=0;
		time_oneminute=0;
		time_oneday=0;

	};
	~TSMD_Arrays(){};
};

//Tick,5Seconds,One_minute,One_Day
class TSMD_Map{
	//string为合约代码，TSMD_Arrays为4个类的组合
public:map<string,TSMD_Arrays*>*tsmd_a; 
	   TSMD_Map()
	   {
		  tsmd_a = new map<string,TSMD_Arrays*>();
	   };
	   ~TSMD_Map(){};

};

//////////////////////////////////////////////////////////////////////////
//当天所有合约大数组
class All_Contracts_Array{
public:
	//数据总数
	int index_all;		
	//数据缓冲区
	CThostFtdcDepthMarketDataField_Custom All_Contracts_Data[MAXNUMBER];       //用于tickdata
	//CThostFtdcDepthMarketDataField_Custom_Other  All_Contracts_Data[MAXNUMBER];  //用于1分钟，5秒钟，x分钟
	//更新的时间
	int end_write_time;
	//更新的位置
	int end_write_index;
};
//////////////////////////////////////////////////////////////////////////

#endif OPERATORDATA_H