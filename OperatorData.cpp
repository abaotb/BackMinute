					/************************************************************************/
					/*  数据处理程序文件，主要将MdSpi.cpp文件的数据处理成5秒，1分钟等数据   */
					/************************************************************************/
#include "MdSpi.h"
#include "OperatorData.h"
#include "Custom_Methods.h"
#include "DataBaseDao.h"
using namespace std;


// USER_API参数
extern CThostFtdcMdApi* pUserApi;
//多线程
//extern FILE * file;
bool is_afternoon=true,is_evening=true;																 //用来给下午一点时置位
extern CRITICAL_SECTION cs;
extern All_Contracts_Array all_contracts_array;                                //当天所有合约大数组
extern TSMD_Map* tsmd_map;                                                      //Tick,5Seconds,One_minute,One_Day组合类
CThostFtdcDepthMarketDataField_Custom data_static;                                     //取到的数据,文档全局变量
extern CMdSpi_Custom * cmdSpi_custom;	

#pragma region 初始化和数据库

#pragma region 初始化

//夜盘重置
void methodYepan(int yepan_time) 
{
	if (yepan_time == 0)                        //夜盘0点
	{
		map<string,TSMD_Arrays*>::iterator iter;
		for (iter=tsmd_map->tsmd_a->begin();iter!=tsmd_map->tsmd_a->end();iter++)
		{
			iter->second->time_fivesecond = 0;   //夜盘0点
			iter->second->time_oneminute = 0;
			iter->second->time_oneday = 0;
		}
	}
}
#pragma endregion 初始化



	#pragma region  ADO 连接mysql方式2
//
////连接数据库
//void Ado_Connect_Mysql(CThostFtdcDepthMarketDataField_Custom_Other ctd_data)
//{
//#pragma region 连接数据库
//	//////////////////////////////////////////////////////////////////////////连接数据库
//	CoInitialize(NULL); 
//	_ConnectionPtr m_pConnection=NULL;
//	//创建Connection对象
//	m_pConnection.CreateInstance("ADODB.Connection");//(__uuidof(Connection));   
//	try 
//	{  
//		//设置连接时间  
//		m_pConnection->ConnectionTimeout = 5;
//		_bstr_t connstr = "DSN=MySqlODBC;server=";
//		connstr += local_host ;
//		connstr += ";database=";
//		connstr += data_base;;
//		//打开数据库连接  
//		HRESULT hr = m_pConnection->Open(connstr,user_name,user_pwd,adModeUnknown); //adModeUnknown 缺省。当前的许可权未设置
//		//m_pConnection->Open("Data Source=adotest;UID=sa;PWD=;","","",adModeUnknown);
//	}  
//	catch(_com_error &e)  
//	{  
//		cout<<"连接错误"<<endl;
//		return ;  
//	}   
//	//////////////////////////////////////////////////////////////////////////连接数据库
//
//
//#pragma endregion 连接数据库
//
//#pragma region 执行SQL语句
//	//////////////////////////////////////////////////////////////////////////添加数据
//	//执行SELETE语句
//	_RecordsetPtr m_pRecordset;
//	//m_pRecordset.CreateInstance("ADODB.Recordset");
//	m_pRecordset.CreateInstance(__uuidof( Recordset )); //创建实例
//	//一定要打开
//	_bstr_t openRecordset="SELECT  * FROM ";
//	openRecordset+= table;
//	openRecordset +=" limit 1";
//	m_pRecordset->Open(openRecordset,m_pConnection.GetInterfacePtr
//		(),adOpenStatic,adLockOptimistic,adCmdText);// 执行SQL语句，得到记录集, m_pConnection必须已知
//	try{
//		if (m_pRecordset!=NULL)
//		{
//			if (ctd_data.TradingDay!=0)
//			{
//				m_pRecordset->AddNew();	
//				m_pRecordset->PutCollect("exchange_id",_variant_t(ctd_data.ExchangeID));
//				m_pRecordset->PutCollect("instrument_id",_variant_t(ctd_data.InstrumentID));
//				m_pRecordset->PutCollect("trading_day",_variant_t((long)(ctd_data.TradingDay)));
//				m_pRecordset->PutCollect("update_time",_variant_t((long)(ctd_data.UpdateTime)));			
//				m_pRecordset->PutCollect("open_price",_variant_t(ctd_data.OpenPrice));
//				m_pRecordset->PutCollect("close_price",_variant_t(ctd_data.ClosePrice));
//				m_pRecordset->PutCollect("highest_price",_variant_t(ctd_data.HighestPrice));
//				m_pRecordset->PutCollect("lowest_price",_variant_t(ctd_data.LowestPrice));	
//				m_pRecordset->PutCollect("turnover",_variant_t(ctd_data.Turnover));			
//				m_pRecordset->PutCollect("open_interest",_variant_t((long)(ctd_data.OpenInterest)));
//				m_pRecordset->PutCollect("open_interestrange",_variant_t((long)(ctd_data.OpenInterestRange)));
//				m_pRecordset->PutCollect("bid_price1",_variant_t(ctd_data.BidPrice1));
//				m_pRecordset->PutCollect("ask_price1",_variant_t(ctd_data.AskPrice1));
//				m_pRecordset->PutCollect("volume",_variant_t((long)(ctd_data.Volume)));
//				m_pRecordset->PutCollect("bid_volume1",_variant_t(ctd_data.BidVolume1));
//				m_pRecordset->PutCollect("ask_volume1",_variant_t(ctd_data.AskVolume1));
//				m_pRecordset->Update();     ///一定要保存到库中，不然无效
//
//			}
//
//		}
//
//	}catch(_com_error &e){
//		cout<<e.ErrorMessage()<<endl;
//		//continue;
//	}
//
//	//////////////////////////////////////////////////////////////////////////添加数据
//#pragma endregion  执行SQL语句
//
//#pragma region 关闭数据连接
//	try
//	{
//		//清空数组
//		memset(all_contracts_array.All_Contracts_Data, 0x0, all_contracts_array.index_all * sizeof(CThostFtdcDepthMarketDataField_Custom));
//		m_pRecordset->Close();
//		//m_pRecordset->Release();
//		m_pConnection->Close();
//		m_pConnection.Release();
//		CoUninitialize();
//	}
//	catch(_com_error &e){
//		cout<<e.ErrorMessage()<<endl;
//		//continue;
//	}
//
//#pragma endregion 关闭数据连接
//
//	return ;
//}
#pragma endregion   ADO 连接mysql 单个，1分钟


#pragma endregion 初始化和数据库



/************************************************************************/
/*							数据分类线程                                */
/************************************************************************/
DWORD WINAPI Classify_Data(LPVOID lpParamter)
{

	Sleep(5000);

	bool flag = false;        //每次只处理一个数据

	while(1)
	{
		if(method_Rest_Time())
		{
			Sleep(1000);
			//在休盘时间重置
			if (tsmd_map->tsmd_a->size()!=0)
			{
				tsmd_map= NULL;
				tsmd_map = new TSMD_Map();
			}
		}
		//进入临界区
		EnterCriticalSection(&cs);
		CThostFtdcDepthMarketDataField_Custom* dd =&(cmdSpi_custom->methodGetDatas_After());
		//释放临界区
		LeaveCriticalSection(&cs);
 		if (dd!=NULL)
 		{
			data_static =*dd;		
			flag = true;   				
		}
		if (flag)
		{
			//初始化数据
			methodYepan(data_static.UpdateTime);
			flag = false;  

			//转换数据
			//CThostFtdcDepthMarketDataField_Custom  ->  CThostFtdcDepthMarketDataField_Custom_Tick
			CThostFtdcDepthMarketDataField_Custom_Tick ct = methodChangeData(data_static);
			
#pragma region //如果在map找不到,初始化

					//如果在map找不到
				if (tsmd_map->tsmd_a->find((string)(data_static.InstrumentID))==tsmd_map->tsmd_a->end())
				{				 
					if (ct.TradingDay==0)
					{
						continue;
					}
					//创建对象
					TSMD_Arrays *tcreate=new TSMD_Arrays();
					tcreate->contract = (string)(data_static.InstrumentID);

					//指针放到Map中
					(*(tsmd_map->tsmd_a))[(string)(data_static.InstrumentID)]=tcreate;					

					#pragma region 处理1分钟

					//处理1分钟
					//////////////////////////////////////////////////////////////////////////
					//1分钟间隔初始化
					tcreate->time_oneminute = method_TimeToInt_60(ct.UpdateTime-ct.UpdateTime%100);//60
					//更新1分钟标准
					tcreate->data_oneminute[OM_TICKCOUNT-1].Volume = 0;
					tcreate->data_oneminute[OM_TICKCOUNT-1].Turnover = 0;
					tcreate->data_oneminute[OM_TICKCOUNT-1].OpenInterest = data_static.PreOpenInterest;
					//处理1分钟写到data_tickOneminute[7300]去
					tcreate->methodUpdate_Oneminute(tcreate,ct);
					//////////////////////////////////////////////////////////////////////////
					#pragma endregion 处理1分钟

				}
#pragma endregion //如果在map找不到，初始化
				else{
					//提取出对象,按品种已经区分开来
					TSMD_Arrays * torigin = (*(tsmd_map->tsmd_a))[(string)(data_static.InstrumentID)];
					torigin->contract = (string)(data_static.InstrumentID);

					//处理1分钟的函数
					torigin->method_Oneminute(torigin,ct);
				}

			
		}//end flag				

	}//end while

}//end FUN


#pragma region 函数定义
//函数定义

//将自定义的tick数据转换成5秒所需的tick数据或者1分钟的数据，即开收高低价为同一价格
CThostFtdcDepthMarketDataField_Custom_Other TSMD_Arrays::methodTickToOther(
	CThostFtdcDepthMarketDataField_Custom_Tick original_data_tick)
{

	CThostFtdcDepthMarketDataField_Custom_Other custom_tick_other;

	strncpy_s(custom_tick_other.ExchangeID,_countof(custom_tick_other.ExchangeID),
		original_data_tick.ExchangeID,strlen(original_data_tick.ExchangeID));

	strncpy_s(custom_tick_other.InstrumentID,_countof(custom_tick_other.InstrumentID),
		original_data_tick.InstrumentID,strlen(original_data_tick.InstrumentID));

	custom_tick_other.TradingDay=original_data_tick.TradingDay;
	//开盘价，收盘价，最高价，最低价
	custom_tick_other.OpenPrice=original_data_tick.LastPrice;
	custom_tick_other.ClosePrice=original_data_tick.LastPrice;
	custom_tick_other.HighestPrice=original_data_tick.LastPrice;
	custom_tick_other.LowestPrice=original_data_tick.LastPrice;

	custom_tick_other.Volume=original_data_tick.Volume;
	custom_tick_other.Turnover=original_data_tick.Turnover;
	custom_tick_other.OpenInterest=original_data_tick.OpenInterest;
	custom_tick_other.UpdateTime=original_data_tick.UpdateTime;
	custom_tick_other.BidPrice1=original_data_tick.BidPrice1;
	custom_tick_other.BidVolume1=original_data_tick.BidVolume1;
	custom_tick_other.AskPrice1=original_data_tick.AskPrice1;
	custom_tick_other.AskVolume1=original_data_tick.AskVolume1;
	custom_tick_other.OpenInterestRange = original_data_tick.OpenInterestRange;

	return custom_tick_other;
};

//写入data_oneminute[650]数组中去
void TSMD_Arrays::methodUpdate_Oneminute(TSMD_Arrays *tsmdarrays,
	CThostFtdcDepthMarketDataField_Custom_Tick custom_tick){
	//将开收高低赋于同一价格
	CThostFtdcDepthMarketDataField_Custom_Other co = tsmdarrays->methodTickToOther(custom_tick);

	tsmdarrays->data_oneminute[(tsmdarrays->index_oneminute)] = co;      //放入data_oneminute[650]数组
	//个数加一
	tsmdarrays->index_oneminute++;
	//更新标准
	tsmdarrays->data_oneminute[OM_TICKCOUNT-1] = co;
};

//对一个tick数据处理，找出1分钟内的开收高低价等等
void TSMD_Arrays::method_Oneminute(TSMD_Arrays *tsmdarrays,
	CThostFtdcDepthMarketDataField_Custom_Tick custom_tick)
{
	int time_60=method_TimeToInt_60(custom_tick.UpdateTime);
	if(time_60 - tsmdarrays->time_oneminute<60&&time_60 - tsmdarrays->time_oneminute>=0)
	{
		//防止越界
		if (tsmdarrays->index_oneminute>OM_TICKCOUNT-5)
		{
			tsmdarrays->data_oneminute[0] = tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1];
			tsmdarrays->index_oneminute=1;
		}
		//如果高于最高价
		if (custom_tick.LastPrice > tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].HighestPrice)
		{
			//更新最高价
			tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].HighestPrice = custom_tick.LastPrice;

		}else if(custom_tick.LastPrice < tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].LowestPrice)
		{
			//更新最低价
			tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].LowestPrice = custom_tick.LastPrice;		

		}
		//更新收盘价
		tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].ClosePrice = custom_tick.LastPrice;

		//委买委卖
		tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].BidPrice1=custom_tick.BidPrice1;
		tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].BidVolume1=custom_tick.BidVolume1;
		tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].AskPrice1=custom_tick.AskPrice1;
		tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].AskVolume1=custom_tick.AskVolume1;
		//更新持仓量
		tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].OpenInterest=custom_tick.OpenInterest;
		//更新成交量，成交额，持仓变化
		tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].Volume = 
			custom_tick.Volume - tsmdarrays->data_oneminute[OM_TICKCOUNT-1].Volume;

		tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].Turnover =
			custom_tick.Turnover - tsmdarrays->data_oneminute[OM_TICKCOUNT-1].Turnover;

		tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1].OpenInterestRange = 
			custom_tick.OpenInterest- tsmdarrays->data_oneminute[OM_TICKCOUNT-1].OpenInterest;

	}
	else if (time_60 - tsmdarrays->time_oneminute>=60)
	{
		//控制台输出
		CThostFtdcDepthMarketDataField_Custom_Other fff = tsmdarrays->data_oneminute[tsmdarrays->index_oneminute-1];
		//cerr << fff.InstrumentID<<" "<<fff.UpdateTime<<" 最高价："<<fff.HighestPrice<<"最低价： "<<fff.LowestPrice<<" "<<endl;
		//cerr <</* fff.InstrumentID<<*/"        "<<fff.UpdateTime<<" 成交量："<<fff.Volume<<" 成交额："<<fff.Turnover<<" 持仓量："<<fff.OpenInterest<<" 仓差："<<fff.OpenInterestRange<<endl;
	
		//写Mysql数据库
		methodWriteFile2(fff,"today-1min");
	    Ado_Connect_Mysql(fff);
		//更新时间
		tsmdarrays->time_oneminute = time_60;//tsmdarrays->time_oneminute + 60;
		//写入数组
		tsmdarrays->methodUpdate_Oneminute(tsmdarrays,custom_tick);

	}
	
};


 
#pragma endregion 函数定义


// 	CThostFtdcDepthMarketDataField_Custom_Other fff;
// 	fff.TradingDay=10;
// 	char *a="cffex";
// 	char *b="IF1510";
// 	//string a="cffex";
// 	//string b="IF1510";
// 	strcpy_s(fff.ExchangeID,sizeof(fff.ExchangeID),a);
// 	//strcat(fff.ExchangeID,"\0");
// 	strcpy_s(fff.InstrumentID,sizeof(fff.InstrumentID),b);
// 	
// 
// 	Ado_Connect_Mysql(fff);


//写入data_tick[73000]数组中去
/*void TSMD_Arrays::methodUpdate_tick(TSMD_Arrays *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick custom_tick){

	tsmdarrays->data_tick[tsmdarrays->index_tick]=custom_tick;    //放入data_tick[73000]数组
	//更新成交量，成交额，仓差
	tsmdarrays->data_tick[tsmdarrays->index_tick].Volume = custom_tick.Volume -tsmdarrays->data_tick[72999].Volume;
	tsmdarrays->data_tick[tsmdarrays->index_tick].Turnover = custom_tick.Turnover - tsmdarrays->data_tick[72999].Turnover;
	tsmdarrays->data_tick[tsmdarrays->index_tick].OpenInterestRange = custom_tick.OpenInterest - tsmdarrays->data_tick[72999].OpenInterest;

	// 	CThostFtdcDepthMarketDataField_Custom_Tick fff = tsmdarrays->data_tick[tsmdarrays->index_tick];
	// 	cerr << fff.InstrumentID<<" "<<fff.UpdateTime<<" "<<fff.Volume<<" "<<fff.OpenInterest<<" "<<fff.OpenInterestRange<<endl;
	//个数加一
	tsmdarrays->index_tick++;   
	//更新标准
	tsmdarrays->data_tick[TICKCOUNT-1]=custom_tick;
	//控制台输出
	//  	 	CThostFtdcDepthMarketDataField_Custom_Tick fff = tsmdarrays->data_tick[tsmdarrays->index_tick-1];
	//  	  	cerr << fff.InstrumentID<<" "<<fff.UpdateTime<<" "<<fff.LastPrice<<"成交量： "<<fff.Volume<<"持仓量： "<<fff.OpenInterest<<" 仓差："<<fff.OpenInterestRange<<endl;

}; */ 
//写入data_Fiveseconds[7300]数组中去
/*void TSMD_Arrays::methodUpdate_Fiveseconds(TSMD_Arrays *tsmdarrays,
	CThostFtdcDepthMarketDataField_Custom_Tick custom_tick)
{
	//将开收高低赋于同一价格
	CThostFtdcDepthMarketDataField_Custom_Other co = tsmdarrays->methodTickToOther(custom_tick);

	tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds] = co;      //放入data_Fiveseconds[7300]数组
	//个数加一
	tsmdarrays->index_fiveseconds++;
	//更新标准
	tsmdarrays->data_fiveseconds[FS_TICKCOUNT-1] = co;					 //成交量，成交额，持仓变化用到
};*/
//对一个tick数据处理，找出5秒内的开收高低价等等
//void TSMD_Arrays::method_Fiveseconds(TSMD_Arrays *tsmdarrays,
//	CThostFtdcDepthMarketDataField_Custom_Tick custom_tick)
//{
//
//	int time_60=method_TimeToInt_60(custom_tick.UpdateTime);
//	if(time_60 - tsmdarrays->time_fivesecond < 5/*&&time_60 - tsmdarrays->time_fivesecond>0*/)
//	{
//		//如果高于最高价
//		if (custom_tick.LastPrice > tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].HighestPrice)
//		{
//			//更新最高价
//			tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].HighestPrice = custom_tick.LastPrice;
//
//		}else if(custom_tick.LastPrice < tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].LowestPrice)
//		{
//			//更新最低价
//			tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].LowestPrice = custom_tick.LastPrice;		
//
//		}
//		//更新收盘价
//		tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].ClosePrice = custom_tick.LastPrice;
//
//		//委买委卖
//		tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].BidPrice1=custom_tick.BidPrice1;
//		tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].BidVolume1=custom_tick.BidVolume1;
//		tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].AskPrice1=custom_tick.AskPrice1;
//		tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].AskVolume1=custom_tick.AskVolume1;
//		//更新持仓量
//		tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].OpenInterest=custom_tick.OpenInterest;
//		//更新成交量，成交额，持仓变化
//		tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].Volume =
//			custom_tick.Volume - tsmdarrays->data_fiveseconds[FS_TICKCOUNT-1].Volume; 
//
//		tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].Turnover =
//			custom_tick.Turnover - tsmdarrays->data_fiveseconds[FS_TICKCOUNT-1].Turnover;	
//
//		tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].OpenInterestRange = 
//			custom_tick.OpenInterest- tsmdarrays->data_fiveseconds[FS_TICKCOUNT-1].OpenInterest;
//
//
//
//	}else{
//		// 				//更新成交量，成交额，持仓变化
//		// 				tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].Volume = custom_tick.Volume - tsmdarrays->data_fiveseconds[7299].Volume;
//		// 				tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].Turnover=custom_tick.Turnover - tsmdarrays->data_fiveseconds[7299].Turnover;		
//		// 				tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].OpenInterestRange= custom_tick.OpenInterest- tsmdarrays->data_fiveseconds[7299].OpenInterest;
//		// 				//更新持仓量
//		// 				tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1].OpenInterest=custom_tick.OpenInterest;
//		//控制台输出
// 		CThostFtdcDepthMarketDataField_Custom_Other fff = tsmdarrays->data_fiveseconds[tsmdarrays->index_fiveseconds-1];
////		cerr << fff.InstrumentID<<" "<<fff.UpdateTime<<" 最高价："<<fff.HighestPrice<<"最低价： "<<fff.LowestPrice<<" "<<endl;
////		cerr <</* fff.InstrumentID<<*/"        "<<fff.UpdateTime<<" 成交量："<<fff.Volume<<" 成交额："<<fff.Turnover<<" 持仓量："<<fff.OpenInterest<<" 仓差："<<fff.OpenInterestRange<<endl;
//// 		string path=".\\datas2\\today-5sec";
//// 		FILE* file = _fsopen(path.c_str(),"ab+",_SH_DENYWR);
//// 		fwrite(&fff,sizeof(fff),1,file);
//// 		fclose(file);
//		//写Mysql数据库
//		methodWriteFile2(fff,"today-5sec");
//		Ado_Connect_Mysql(fff);
//		//更新时间
//		tsmdarrays->time_fivesecond = tsmdarrays->time_fivesecond + 5;
//		//写入数组
//		tsmdarrays->methodUpdate_Fiveseconds(tsmdarrays,custom_tick);
//
//	}
//};
//void TSMD_Arrays::method_Oneday(TSMD_Arrays *tsmdarrays,
//	CThostFtdcDepthMarketDataField_Custom_Tick custom_tick)
//{
//
//	strncpy_s(tsmdarrays->data_oneday.ExchangeID,_countof(tsmdarrays->data_oneday.ExchangeID),
//		custom_tick.ExchangeID,strlen(custom_tick.ExchangeID));
//
//	strncpy_s(tsmdarrays->data_oneday.InstrumentID,_countof(tsmdarrays->data_oneday.InstrumentID),
//		custom_tick.InstrumentID,strlen(custom_tick.InstrumentID));
//
//	tsmdarrays->data_oneday.TradingDay=custom_tick.TradingDay;
//
//	//开盘价，收盘价，最高价，最低价
//	/*tsmdarrays->data_oneday.OpenPrice=custom_tick.LastPrice;*/
//	tsmdarrays->data_oneday.ClosePrice=custom_tick.LastPrice;
//	if (tsmdarrays->data_oneday.HighestPrice < custom_tick.LastPrice)
//	{
//		tsmdarrays->data_oneday.HighestPrice = custom_tick.LastPrice;
//	}else if (tsmdarrays->data_oneday.LowestPrice > custom_tick.LastPrice)
//	{
//		tsmdarrays->data_oneday.LowestPrice = custom_tick.LastPrice;
//	}
//	tsmdarrays->data_oneday.Volume=custom_tick.Volume;
//	tsmdarrays->data_oneday.Turnover=custom_tick.Turnover;
//	tsmdarrays->data_oneday.OpenInterest=custom_tick.OpenInterest;
//	tsmdarrays->data_oneday.UpdateTime=custom_tick.UpdateTime;
//	tsmdarrays->data_oneday.BidPrice1=custom_tick.BidPrice1;
//	tsmdarrays->data_oneday.BidVolume1=custom_tick.BidVolume1;
//	tsmdarrays->data_oneday.AskPrice1=custom_tick.AskPrice1;
//	tsmdarrays->data_oneday.AskVolume1=custom_tick.AskVolume1;
//	tsmdarrays->data_oneday.OpenInterestRange = custom_tick.OpenInterestRange;
//};


