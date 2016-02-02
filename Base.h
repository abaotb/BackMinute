#ifndef BASE_H
#define BASE
#include "MdSpi.h"
#include "Custom_Methods.h"
using namespace std;

extern int	  DATA_TIME_INTERVAL;

#pragma once
//基类
class BaseData{
public:
	//构造函数
	BaseData(string contract,
		int compare_time,
		CThostFtdcDepthMarketDataField_Custom_Other current_data,
		CThostFtdcDepthMarketDataField_Custom_Other compare_data)
	{

		this->contract = contract;					//合约代码 
		this->compare_time = compare_time;			//记录Tick数据当前时间 
		this->current_data = current_data;			//当前数据
		this->compare_data = compare_data;			//被比较数据	
	};
	BaseData()
	{
		CThostFtdcDepthMarketDataField_Custom_Other current_data;
		CThostFtdcDepthMarketDataField_Custom_Other compare_data;
		current_data.TradingDay = 0;
		compare_data.TradingDay = 0;
		this->contract = "";						//合约代码 
		this->compare_time = 0;						//记录Tick数据当前时间 
		this->current_data = current_data;			//当前数据
		this->compare_data = compare_data;			//被比较数据
	};
	~BaseData(){};
public:
	//合约代码
	string contract;
	//当前数据，被比较数据	              
	CThostFtdcDepthMarketDataField_Custom_Other current_data; 
	CThostFtdcDepthMarketDataField_Custom_Other compare_data; 

	//记录Tick数据当前时间 
	int compare_time;
	//判断和合约是否存在map
	virtual bool isExist(string str)const =0;
	//添加合约到map
	virtual void addContractToMap(CThostFtdcDepthMarketDataField_Custom_Tick*ct) const =0;
	//更新数据
	virtual void updateData(CThostFtdcDepthMarketDataField_Custom_Tick*ct)const =0;
	//数据的逻辑运算
	virtual void methodLogicalOperation(BaseData *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick &custom_tick)const =0;
	//初始化下一个阶段数据
	virtual void methodInitNext(BaseData *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick &custom_tick)const = 0;
	//对一个tick数据处理，找出1分钟内的开收高低价等等
	virtual CThostFtdcDepthMarketDataField_Custom_Other methodTickToOther(CThostFtdcDepthMarketDataField_Custom_Tick &original_data_tick)const =0;

};

//一分钟继承类
class  BaseDataInherit:public BaseData{
public:
	//构造函数
	//Triangle(double d,double h):Graph(d,h){};
	BaseDataInherit(string contract,
		int compare_time,
		CThostFtdcDepthMarketDataField_Custom_Other current_data,
		CThostFtdcDepthMarketDataField_Custom_Other compare_data
		):BaseData(contract,compare_time,current_data,compare_data)
	{

		map<string, BaseDataInherit*> *tsmd_a= new map<string, BaseDataInherit*>(); 
		this->tsmd_a = tsmd_a;					    //map
		this->TIME_INTERVAL	= DATA_TIME_INTERVAL;	//时间间隔				/**********要改的地方**********/

		this->contract = "";						//合约代码 
		this->compare_time = 0;						//记录Tick数据当前时间 
		this->current_data = current_data;			//当前数据
		this->compare_data = compare_data;			//被比较数据


	};

	BaseDataInherit()
	{

		map<string, BaseDataInherit*> *tsmd_a= new map<string, BaseDataInherit*>(); 
		CThostFtdcDepthMarketDataField_Custom_Other current_data;
		CThostFtdcDepthMarketDataField_Custom_Other compare_data;
		current_data.TradingDay = 0;
		compare_data.TradingDay = 0;
		this->tsmd_a = tsmd_a;						//map
		this->TIME_INTERVAL = DATA_TIME_INTERVAL;	//时间间隔			/**********要改的地方**********/

		this->contract = "";						//合约代码 
		this->compare_time = 0;						//记录Tick数据当前时间 
		this->current_data = current_data;
		this->compare_data = compare_data;

	};
	~ BaseDataInherit(){};	

public:
	int TIME_INTERVAL;								//时间间隔
	map<string, BaseDataInherit*> *tsmd_a;          //合约，map
	//判断和合约是否存在map
	bool isExist(string str) const;
	//添加合约到map
	void addContractToMap(CThostFtdcDepthMarketDataField_Custom_Tick*ct) const;
	//更新数据
	void updateData(CThostFtdcDepthMarketDataField_Custom_Tick*ct) const;
	//数据的逻辑运算
	void methodLogicalOperation(BaseData *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick &custom_tick)const;

	//初始化下一个阶段数据
	void methodInitNext(BaseData *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick &custom_tick) const;
	//将自定义的tick数据转换成5秒所需的tick数据或者1分钟的数据，即开收高低价为同一价格
	CThostFtdcDepthMarketDataField_Custom_Other  methodTickToOther(CThostFtdcDepthMarketDataField_Custom_Tick &original_data_tick) const;
};
	

#endif BASE_H