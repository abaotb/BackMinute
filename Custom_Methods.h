#ifndef CUSTOM_METHODS_H
#define CUSTOM_METHODS_H

#include"OperatorData.h"
using namespace std;
extern CMdSpi_Custom * cmdSpi_custom;	
extern char* ppInstrumentID[];
#pragma once
/************************************************************************/
/*							各种时间转换函数					       */
/************************************************************************/
//1）将11:45:32的冒号去掉 11:45:32->114532
int method_TimeToInt(char *tick_UpdateTime);
//2）转成60秒计数，将时分秒换成秒相加，参数为114316,返回总秒数11*3600+43*60+16
extern int method_TimeToInt_60(int itime);
//3）将不规则的时分秒换成秒相加转换成规则的时分秒换成秒相加 102460 -> 102500
int method_Template_TimeInt(int ntime);
//4）将时分秒换成秒相加 重载int method_TimeToInt_60(int itime)
int method_TimeToInt_60();
//5）如104316，将时分秒连接起来  将系统时间 ->104316形式
int method_TimeToInt_Int();
//6）是否停盘休息时间
bool method_Rest_Time();
//7）过滤联合竞价等一些不合理数据
bool filter_Unreasonable_Price(int datetime);
//8）过滤联合竞价等一些不合理数据
bool filter_Unreasonable_Price(int datetime,int day);
/************************************************************************/
/*                          写入文件用来测试                             */
/************************************************************************/
//写入文件
//写收到最原始的tick数据，和处理后的tick数据
void methodWriteFile(CThostFtdcDepthMarketDataField_Custom &data,string filename);
//写处理后的5秒数据，1分数据
void methodWriteFile2(CThostFtdcDepthMarketDataField_Custom_Other &data,string filename);
void methodWriteFile3(vector<CThostFtdcDepthMarketDataField_Custom_Other> &cco_vector,string filename);
//将接收到且第一次修改的标准的Tick数据转换成自定义的Tick数据---OperatorData.cpp用的
CThostFtdcDepthMarketDataField_Custom_Tick methodChangeData(CThostFtdcDepthMarketDataField_Custom &original_data);


/************************************************************************/
/*                          打印数据测试                                */
/************************************************************************/
void readdata1();   //tick quotation TESTtest222  CThostFtdcDepthMarketDataField_Custom
void readdata2();   //original    CThostFtdcDepthMarketDataField_Custom         fprintf(fw,"%s,%s,%d,%d,%.2f,%ld\r\n",tb.ExchangeID,tb.InstrumentID,tb.UpdateTime,tb.UpdateMillisec,tb.LastPrice,tb.ReceiveTime);
void readdata3();   //today-1min  CThostFtdcDepthMarketDataField_Custom_Other   fprintf(fw,"%s,%d,%.2f,%.2f,%.2f,%.2f\r\n",tb.InstrumentID,tb.UpdateTime,tb.OpenPrice,tb.LowestPrice,tb.HighestPrice,tb.ClosePrice);
//自动生成合约代码表声明,用C语言的strtok分割，可自动增加年月
void methodAutoContract();
//自动生成合约代码表声明，用vector<string>split分割，但不可自动增加年月
void methodAutoContract2();
//自动生成合约代码表声明，用vector<string>split分割，但可自动增加年月
void methodAutoContract3();


/************************************************************************/
/*                           连接数据库                                  */
/************************************************************************/
//连接mysql数据库
void Ado_Connect_Mysql(CThostFtdcDepthMarketDataField_Custom_Other &ctd_data);

bool  priceRange(CThostFtdcDepthMarketDataField_Custom_Other &current_data,CThostFtdcDepthMarketDataField_Custom_Tick &custom_tick);
#endif CUSTOM_METHODS_H