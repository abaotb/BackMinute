#include "Base.h"

//判断和合约是否存在map
bool BaseDataInherit::isExist(string str) const
{
	if (this->tsmd_a != NULL)                   //注意是this
	{
		//不存在
		if (this->tsmd_a->find(str)==this->tsmd_a->end())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
};
//添加合约到map
void  BaseDataInherit::addContractToMap(CThostFtdcDepthMarketDataField_Custom_Tick*ct) const 
{
	//创建对象
	BaseDataInherit* torigin= new  BaseDataInherit();
	torigin->contract=(string)(ct->InstrumentID);
	(*(this->tsmd_a))[(string)(ct->InstrumentID)] = torigin;							//注意是this,不是torgin	
	torigin->TIME_INTERVAL = this->TIME_INTERVAL;										//修改时间间隔
	int remainder = TIME_INTERVAL >=60 ? 100 : TIME_INTERVAL;							//100//60	/**********要改的地方**********/
	torigin->compare_time = method_TimeToInt_60(ct->UpdateTime-ct->UpdateTime%remainder);
	//更新1分钟标准
	torigin->compare_data.Volume = 0;
	torigin->compare_data.Turnover = 0;
	torigin->compare_data.OpenInterest =0;//ct->OpenInterestRange;
	//初始化下一个阶段
	torigin->methodInitNext(torigin,*ct);


}
//更新数据
void  BaseDataInherit::updateData(CThostFtdcDepthMarketDataField_Custom_Tick*ct) const
{
	BaseDataInherit * torigin= (*(this->tsmd_a))[(string)(ct->InstrumentID)];            //注意是this
	//处理夜盘0点时时间的比较
// 	if (ct->UpdateTime == 0)                        
// 	{	
// 		torigin->compare_time = 0;
// 
// 	}
	torigin->methodLogicalOperation(torigin,*ct);
}
//数据的逻辑运算
void  BaseDataInherit::methodLogicalOperation(BaseData *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick &custom_tick)const
{	
	//处理夜盘0点时时间的比较
// 	if (custom_tick.UpdateTime == 101400)                        
// 	{	
// 		tsmdarrays->compare_time = 1;
// 
// 		// 			 //20151106
// 		// 			CThostFtdcDepthMarketDataField_Custom_Other fff = tsmdarrays->current_data;
// 		// 			char  filename[10];
// 		// 			itoa(TIME_INTERVAL, filename,10);
// 		// 			methodWriteFile2(fff,(string)filename);//"today-1min"
// 		// 			Ado_Connect_Mysql(fff);
// 		// 			//写入数组         //20151113
// 		// 			tsmdarrays->methodInitNext(tsmdarrays,custom_tick);
// 
// 		//20151119
//  		//map<string, BaseDataInherit*>::iterator iter;
//  		//for (iter=this->tsmd_a->begin();iter!=this->tsmd_a->end();iter++)
//  		//{
//  		//	if (iter->second->compare_time!=0)//夜盘0点
//  		//	{
//  		//		iter->second->compare_time = 0;
//  		//	}
//  
//  		//}
// 	}
	//cerr << custom_tick.InstrumentID<<"     method_Base"<<endl;
	int time_60=method_TimeToInt_60(custom_tick.UpdateTime);

	//过滤不合理价格      20151125   
	if (priceRange(tsmdarrays->current_data,custom_tick))
	{
		return;
	}
	if(time_60 - tsmdarrays->compare_time < TIME_INTERVAL && time_60 - tsmdarrays->compare_time >=0)
	{
		
		//如果高于最高价
		if (custom_tick.LastPrice >tsmdarrays->current_data.HighestPrice )
		{
			//更新最高价
			tsmdarrays->current_data.HighestPrice = custom_tick.LastPrice;

		}
		if(custom_tick.LastPrice <  tsmdarrays->current_data.LowestPrice)
		{
			//更新最低价
			tsmdarrays->current_data.LowestPrice = custom_tick.LastPrice;		

		}
		//更新收盘价
		tsmdarrays->current_data.ClosePrice = custom_tick.LastPrice;

		//委买委卖
		tsmdarrays->current_data.BidPrice1=custom_tick.BidPrice1;
		tsmdarrays->current_data.BidVolume1=custom_tick.BidVolume1;
		tsmdarrays->current_data.AskPrice1=custom_tick.AskPrice1;
		tsmdarrays->current_data.AskVolume1=custom_tick.AskVolume1;
		//更新持仓量
		tsmdarrays->current_data.OpenInterest=custom_tick.OpenInterest;
		//更新成交量，成交额，持仓变化
		tsmdarrays->current_data.Volume = 
			custom_tick.Volume -  tsmdarrays->compare_data.Volume;

		tsmdarrays->current_data.Turnover =
			custom_tick.Turnover -  tsmdarrays->compare_data.Turnover;

		tsmdarrays->current_data.OpenInterestRange = 
			custom_tick.OpenInterest-  tsmdarrays->compare_data.OpenInterest;

	}
	else if (time_60 - tsmdarrays->compare_time>=TIME_INTERVAL )		//20151217加 && custom_tick.UpdateTime%100 == 0
	{
		//控制台输出
		CThostFtdcDepthMarketDataField_Custom_Other fff = tsmdarrays->current_data;
		//cerr << fff.InstrumentID<<" "<<fff.UpdateTime<<" 最高价："<<fff.HighestPrice<<"最低价： "<<fff.LowestPrice<<" "<<endl;
		//cerr <</* fff.InstrumentID<<*/"        "<<fff.UpdateTime<<" 成交量："<<fff.Volume<<" 成交额："<<fff.Turnover<<" 持仓量："<<fff.OpenInterest<<" 仓差："<<fff.OpenInterestRange<<endl;
		//cerr << fff.InstrumentID<<"   "<<fff.UpdateTime<<" 开盘价："<<fff.OpenPrice<<" 收盘价："<<fff.ClosePrice<<endl;
		//写Mysql数据库
		//itoa(number, string,10);
		char  filename[10];
		itoa(TIME_INTERVAL, filename,10);
		methodWriteFile2(fff,(string)filename);//"today-1min"

		////////////////////////////////过滤不合理价格
		// 			if (fff.HighestPrice >  fff.OpenPrice * 1.2)
		// 			{
		// 				fff.HighestPrice = fff.OpenPrice;
		// 			}
		// 			if (fff.LowestPrice >  fff.OpenPrice * 0.8 )
		// 			{
		// 				fff.LowestPrice = fff.OpenPrice;
		// 			}
		/////////////////////////////////////////////20151112 tangb
		Ado_Connect_Mysql(fff);

		//更新时间
		tsmdarrays->compare_time = time_60;//tsmdarrays->time_oneminute + 60;
		//写入数组
		tsmdarrays->methodInitNext(tsmdarrays,custom_tick);

	}else if (time_60 - tsmdarrays->compare_time < 0)                       //20151217   加
	{
		tsmdarrays->compare_time =0;
	}

};

//初始化下一个阶段数据
void  BaseDataInherit::methodInitNext(BaseData *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick &custom_tick) const
{
	//cerr << custom_tick.InstrumentID<<"       methodUpdate_Base "<<endl;
	//将开收高低赋于同一价格	
	CThostFtdcDepthMarketDataField_Custom_Other co = tsmdarrays->methodTickToOther(custom_tick);
	tsmdarrays->current_data = co;      //放入data_oneminute[650]数组
	tsmdarrays->compare_data= co;       //更新标准

};
//将自定义的tick数据转换成5秒所需的tick数据或者1分钟的数据，即开收高低价为同一价格
CThostFtdcDepthMarketDataField_Custom_Other  BaseDataInherit::methodTickToOther
	(CThostFtdcDepthMarketDataField_Custom_Tick &original_data_tick) const
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