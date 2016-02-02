							
					/************************************************************************/
					/*           数据接收程序文件，主要处理数据缺失和数据重复               */
					/************************************************************************/
#include "MdSpi.h"
#include "Custom_Methods.h"
using namespace std;
#pragma warning(disable : 4996)

/************************************************************************/
/*                             自定义的变量                             */
/************************************************************************/
//extern FILE * file;  
bool time_morning=true,time_afternoon=true,time_evening=true,initflag=true;			 //设置参数的标志	 												 //用来给下午一点时置位
int timenow;                                                                         //程序启动时间如94112
extern CRITICAL_SECTION cs;                                                          //临界区
extern CMdSpi_Custom * cmdSpi_custom;												 //缓冲区对象
extern CThostFtdcDepthMarketDataField_Custom_Clean*cffex_state,*czce_state,*dce_state,*shfe_state;//四个交易所对象
/************************************************************************/
/*                             原有系统所需变量                         */
/************************************************************************/

extern CThostFtdcMdApi* pUserApi;													// USER_API参数
// 配置参数
extern char FRONT_ADDR[];		
extern TThostFtdcBrokerIDType	BROKER_ID;
extern TThostFtdcInvestorIDType INVESTOR_ID;
extern TThostFtdcPasswordType	PASSWORD;
extern char* ppInstrumentID[];	
extern int iInstrumentID;
extern int iRequestID;																// 请求编号

/************************************************************************/
/* 配置信息                                                             */
/************************************************************************/
string  local_host;
string  user_name;
string  user_pwd;
string  data_base;
string  table;
string  data_port;
string  database_driver;
int	  DATA_TIME_INTERVAL;
//------------------------
//解决最后一分钟的数据
void method_ResolveLastMinute(CThostFtdcDepthMarketDataField_Custom &data);
map<string,CThostFtdcDepthMarketDataField_Custom>lastMinuteData;
/***********************************************************************************************************/
/*                         线程函数																		   */
/***********************************************************************************************************/ 	
DWORD WINAPI Function_Receive(LPVOID lpParamter)
{
	timenow = method_TimeToInt_Int();
	//先睡眠一秒,避免刚开始取不到数据。
	Sleep(1000);
	while(1)
	{
		
 		if (method_Rest_Time())
 		{
 			Sleep(1000);
 		}
		
		EnterCriticalSection(&cs);
		//取数据,按交易所分类
		CThostFtdcDepthMarketDataField_Custom data = ((cmdSpi_custom->methodGetDatas_Before()));
		LeaveCriticalSection(&cs);
		//系统时间，以毫秒计算
		unsigned int ttt= GetTickCount();

		method_ResolveLastMinute(data);

		//取不到数据                                             //取250ms，有些是相隔600,700ms的，即超过500ms就没办法了
		if (data.TradingDay==0)
		{
			if (ttt - (dce_state->firsttime) > TIMEOUT && !dce_state->flag_exchange)     
			{
				dce_state->methodIsMising();
		    }
 		   if (ttt - (shfe_state->firsttime) > TIMEOUT && !shfe_state->flag_exchange)
 			{
 				shfe_state->methodIsMising();
 			}
			 if (ttt - (czce_state->firsttime) > TIMEOUT && !czce_state->flag_exchange)
			{
				czce_state->methodIsMising();
			}
			 if (ttt - (cffex_state->firsttime) > TIMEOUT && !cffex_state->flag_exchange)
			{
				cffex_state->methodIsMising();
			}
			 //睡眠1ms,防止cpu过高      20151222 加
			 Sleep(1);
			continue;
		}
		methodWriteFile(data,"original");            //最原始的数据



		//解决联合竞价时会发出一些9点钟之前的数据，如7点的数据等等。
		//过滤不合理数据
		if (filter_Unreasonable_Price(data.UpdateTime) 
			/*&& filter_Unreasonable_Price(data.UpdateTime,data.TradingDay)*/)
		{
			//过滤
			continue;
		}	

		//找到每一种合约相对应的交易所
		string s = cmdSpi_custom->exchange_map[data.InstrumentID];

		//原有数据没有交易所ID，现在加上  20150921  tangb
		sprintf(data.ExchangeID ,"%s" , s.c_str());

		#pragma region 区分交易所

		//取到数据
		if (s=="dce")
		{
		
			dce_state->methodMoreThan200(data);	
		}	
		else if(s=="shfe")
		{

			shfe_state->methodMoreThan200(data);	
		}
		else if(s=="czce")
		{
			
			czce_state->methodMoreThan200(data);	
		}
		else if(s=="cffex")
		{
		
			cffex_state->methodMoreThan200(data);	
		}
		#pragma endregion 区分交易所

	}//end while

}//end FUN

void method_ResolveLastMinute(CThostFtdcDepthMarketDataField_Custom &data){

	int time =method_TimeToInt_Int();
	if (data.TradingDay!=0)
	{ 
		if ((data.UpdateTime>=145800&&data.UpdateTime<=150000)   //if (data.UpdateTime>=112800&&data.UpdateTime<=113000)
			||(data.UpdateTime>=112800&&data.UpdateTime<=113000))
		{
			//maplive.insert(pair<int,string>(102,"aclive"));   不会改变
			lastMinuteData[(string)(data.InstrumentID)] = data;;
		}
	}
	
	
	if (time>=150100&&time<150200 && lastMinuteData.size()>0)  //if (time>=113100 && lastMinuteData.size()>0)下午补
	{
		map<string,CThostFtdcDepthMarketDataField_Custom>::iterator iter;
		for (iter=lastMinuteData.begin();iter!=lastMinuteData.end();iter++)
		{
			CThostFtdcDepthMarketDataField_Custom  cda = iter->second;
			while(cda.UpdateTime < 150100)     //while(cda.UpdateTime < 113200)
			{
				cda.UpdateTime++;
				cda.UpdateTime = method_Template_TimeInt(cda.UpdateTime);
				//放到数据整理的缓冲区里去
			    cmdSpi_custom->methodAddDatas_After((cda));
				cmdSpi_custom->methodAddDatas_After((cda));
				//写入文件
				CThostFtdcDepthMarketDataField_Custom ttemmmm = (cda);
				char *ss="补3";
				strcat_s(ttemmmm.ExchangeID,sizeof(ttemmmm.ExchangeID),ss);
				methodWriteFile(ttemmmm,"tick");
			}		
		}
	    lastMinuteData.swap(map<string,CThostFtdcDepthMarketDataField_Custom>());//清除容器并最小化它的容量	
	}
	if (time>=113100 &&time<113200&& lastMinuteData.size()>0)  //if (time>=113100 && lastMinuteData.size()>0)中午补
	{
		map<string,CThostFtdcDepthMarketDataField_Custom>::iterator iter;
		for (iter=lastMinuteData.begin();iter!=lastMinuteData.end();iter++)
		{
			CThostFtdcDepthMarketDataField_Custom  cda = iter->second;
			while(cda.UpdateTime < 113100)     //while(cda.UpdateTime < 113200)
			{
				cda.UpdateTime++;
				cda.UpdateTime = method_Template_TimeInt(cda.UpdateTime);
				//放到数据整理的缓冲区里去
				cmdSpi_custom->methodAddDatas_After((cda));
				cmdSpi_custom->methodAddDatas_After((cda));
				//写入文件
				CThostFtdcDepthMarketDataField_Custom ttemmmm = (cda);
				char *ss="补3";
				strcat_s(ttemmmm.ExchangeID,sizeof(ttemmmm.ExchangeID),ss);
				methodWriteFile(ttemmmm,"tick");
			}		
		}
		lastMinuteData.swap(map<string,CThostFtdcDepthMarketDataField_Custom>());//清除容器并最小化它的容量	
	}
}
void method_AddContracts(CThostFtdcDepthMarketDataField_Custom_Clean_Contract * ccc){
	#pragma region 开机时间的fronttime_1和fronttime_2处理

	int time = method_TimeToInt_Int();
	if (time> CLOSE_EVENING + TIME_DIF &&time< OPEN_MORNING)             //24500 ~ 90000
	{	if (((string)ccc->data.ExchangeID)=="cffex")          //((string)ccc->data.InstrumentID).substr(0,2)=="IF"
		{
			ccc->fronttime_1 = OPEN_MORNING+TIME_DIF;         
			ccc->fronttime_2 = OPEN_MORNING+TIME_DIF;      
		}
		else{
			ccc->fronttime_1 = OPEN_MORNING;         
			ccc->fronttime_2 = OPEN_MORNING;
		}
	}else if (time > CLOSE_NOON + TIME_DIF &&time< OPEN_NOON)				//114500 ~ 130000		
	{
		if(((string)ccc->data.ExchangeID)=="cffex") 
		{
			ccc->fronttime_1 = OPEN_NOON;
			ccc->fronttime_2 = OPEN_NOON;
		}else
		{
			ccc->fronttime_1 = OPEN_NOON+2*TIME_DIF;
			ccc->fronttime_2 = OPEN_NOON+2*TIME_DIF;
		}
		
		
	}else if (time>CLOSE_NOON + 2 * TIME_DIF &&time < OPEN_EVENING)		   //153000 ~ 210000	
	{
		ccc->fronttime_1 = OPEN_EVENING;
		ccc->fronttime_2 = OPEN_EVENING;	
		
	}else{                                                                //在开盘后开启程序
		ccc->fronttime_1 = ccc->data.UpdateTime;
		ccc->fronttime_2 = ccc->data.UpdateTime;
	}
	//更新最后一笔数据的时间
	//(this->lasttime) = ccc->data->ReceiveTime;              
	//写入相应的map中去
	//(*(totalTable))[ccc->data->InstrumentID] = ccc;
	#pragma endregion 开机时间的fronttime_1和fronttime_2处理	
	
}
/*************************************************************************************************************/
/*                            交易所类中的函数定义															 */
/*************************************************************************************************************/

#pragma region 增减数据 
/************************************************************************/
/*                             当取到数据且大于200ms时                  */
/************************************************************************/
//当取到数据且大于200ms时，将合约的标志置位false；
void CThostFtdcDepthMarketDataField_Custom_Clean::methodMoreThan200(CThostFtdcDepthMarketDataField_Custom &datatemp){

	

	//合约是第一次，即是map中还没有该合约
	if (this->totalTable.find(datatemp.InstrumentID) == this->totalTable.end())
	{
		CThostFtdcDepthMarketDataField_Custom_Clean_Contract * ccc =
			new CThostFtdcDepthMarketDataField_Custom_Clean_Contract();		
		//数据状态
		ccc->data = datatemp;
		//修改最后更新时间，即毫秒数
		ccc->end_writetime_Millisec = datatemp.UpdateMillisec;
		ccc->flag_isfirst = true;
		method_AddContracts(ccc);
		//写入相应的map中去
		((totalTable))[ccc->data.InstrumentID] = *ccc;
	}


	
	    #pragma region 这一跳的第一笔数据
	
	//如果map中已经有了合约，则判断是不是这一跳的第一笔数据
	//说明是该交易所在这一跳的第一笔数据
		if ((datatemp.ReceiveTime) - (this->lasttime)> TIMEOUT){
		//this->lasttime为某一个交易所这一批数据的最后修改时间，在下面处理数据methodIsRepetition方法后修改

		//是毫秒级GetTickCount()
		(this->firsttime) = datatemp.ReceiveTime;
		//改变更新的状态
		map<string,CThostFtdcDepthMarketDataField_Custom_Clean_Contract >::iterator iter;
		for (iter = this->totalTable.begin();iter != this->totalTable.end();iter++)
		{
			//合约标志
			(iter->second.flag_contract) = false;
		}
		//交易所标志
	
		(this->flag_exchange) = false;
	}
	#pragma endregion 这一跳的第一笔数据
	

	//处理数据
	this->methodIsRepetition(datatemp);
	
};
/************************************************************************/
/*                            处理数据重复                              */
/************************************************************************/
void CThostFtdcDepthMarketDataField_Custom_Clean::methodIsRepetition(CThostFtdcDepthMarketDataField_Custom &datatemp){
	//合约不是第一次，就从相应的map中取出
	CThostFtdcDepthMarketDataField_Custom_Clean_Contract ccc = ((this->totalTable))[datatemp.InstrumentID];
	//如果合约没被更新过
		if (!((ccc.flag_contract)))
		{

			(ccc.data) = datatemp;
			//处理在交易时间开程序时没赋初值的情况,比如93024开程序                   //20151009    tangbao
			//要区别在235959，所以不用(ccc->fronttime_2==0||ccc->fronttime_2==0)&&datatemp->UpdateTime!=0
			//必须为||，因为在methodIsMising首次被修改了，这里在235959时有问题
			//if ((ccc->fronttime_2==0||ccc->fronttime_2==0)&&datatemp->UpdateTime!=0)
			if (ccc.is_initflag)
			{
				ccc.fronttime_1= datatemp.UpdateTime;
				ccc.fronttime_2= datatemp.UpdateTime;
				ccc.is_initflag= false;
			}


			if (ccc.fronttime_1 == ccc.fronttime_2)
			{
			  ccc.fronttime_1++;
			  ccc.data.UpdateTime = ccc.fronttime_1;
			}else
			{
				ccc.data.UpdateTime = ccc.fronttime_1;
				ccc.fronttime_2 =  ccc.fronttime_1;
			}
			//修改为标准时间  比如102460 . 102500       235959+1 . 0
			ccc.data.UpdateTime = method_Template_TimeInt(ccc.data.UpdateTime);
			ccc.fronttime_2 =   method_Template_TimeInt(ccc.fronttime_2);
			ccc.fronttime_1 =   method_Template_TimeInt(ccc.fronttime_1);
	
			//修改0或者500ms,100ms或者400ms
			ccc.end_writetime_Millisec = (ccc.end_writetime_Millisec+500)%1000;
			ccc.data.UpdateMillisec = ccc.end_writetime_Millisec;

			//两跳数据之间的时间差
			unsigned int diftime=method_TimeToInt_60(datatemp.UpdateTime) - method_TimeToInt_60(ccc.data.UpdateTime);
			//如101500 ~ 103000不写，下面再写
			if (diftime<500)
			{	
				//放到数据整理的缓冲区里去
				cmdSpi_custom->methodAddDatas_After((ccc.data));
				//写入文件
				methodWriteFile((ccc.data),"tick");
			}
		

			#pragma region  交易所的延迟时间,品种两笔时间之间没数据

			/************************************************************************/
			/* 这个循环主要处理不活跃合约的不连续问题，主要是交易所的延迟时间造成的,*/
			/*如132717为上一笔，132719为下一笔，中间没有132718，就得补全了,股指也会
			/*存在这种情况*/
			/************************************************************************/
			//商品期货白天有15分钟和30分钟休息的特例，这段时间不补数据   
			//如101500 ~ 103000		130000 ~ 133000 				  20151012  tangb
			if (
				//135959  -> 140000 有问题,转换成总秒数
				//datatemp->UpdateTime - ccc->data->UpdateTime >1000
				//method_TimeToInt_60(datatemp->UpdateTime) - method_TimeToInt_60(ccc->data->UpdateTime);
				diftime >500
				)  
			{
				ccc.fronttime_1=datatemp.UpdateTime;
				ccc.fronttime_2=datatemp.UpdateTime;
				ccc.data.UpdateTime = datatemp.UpdateTime;		 //重置最后修改时间
				//放到数据整理的缓冲区里去
				cmdSpi_custom->methodAddDatas_After(datatemp);
				//写入文件
				methodWriteFile(datatemp,"tick");
				
			}

			//现在数据最后修改时间大于先前数据最后修改时间,补到相等
			while(datatemp.UpdateTime > ccc.data.UpdateTime)
			{
				if (ccc.fronttime_1 == ccc.fronttime_2)
				{ 
					ccc.fronttime_1++;
				    ccc.data.UpdateTime = ccc.fronttime_1;
				}else
				{
					ccc.data.UpdateTime = ccc.fronttime_1;
					ccc.fronttime_2 =  ccc.fronttime_1;
				}
				ccc.data.UpdateTime = method_Template_TimeInt(ccc.data.UpdateTime);
				ccc.fronttime_2 =   method_Template_TimeInt(ccc.fronttime_2);
				ccc.fronttime_1 =   method_Template_TimeInt(ccc.fronttime_1);

				//修改0或者500ms,100ms或者400ms
				ccc.end_writetime_Millisec = (ccc.end_writetime_Millisec+500)%1000;
				ccc.data.UpdateMillisec = ccc.end_writetime_Millisec;

				//放到数据整理的缓冲区里去
				cmdSpi_custom->methodAddDatas_After((ccc.data));
				//写入文件
				CThostFtdcDepthMarketDataField_Custom ttemmmm = (ccc.data);
				//cerr << ttemmmm.InstrumentID<<"   "<<ttemmmm.UpdateTime<<" 开盘价："<<ttemmmm.LastPrice<<endl;
				char *ss="补2";
				strcat_s(ttemmmm.ExchangeID,sizeof(ttemmmm.ExchangeID),ss);
				methodWriteFile(ttemmmm,"tick");
			}
			#pragma endregion 交易所的延迟时间

			//已更新
			(ccc.flag_contract) = true;
			//更新最后一笔数据的时间，是毫秒级GetTickCount()
			(this->lasttime) = datatemp.ReceiveTime;
			//更新map
			((this->totalTable))[datatemp.InstrumentID] = ccc;
			
		}
		else
		{
			//如果合约已经被更新过

		}
};
/************************************************************************/
/*                            弥补数据								    */
/************************************************************************/
void CThostFtdcDepthMarketDataField_Custom_Clean::methodIsMising(/*int timenow*/)
{

#pragma region 循环补数据

	//如果交易所没被更新过
	if (!((this->flag_exchange)))
	{
    	map<string,CThostFtdcDepthMarketDataField_Custom_Clean_Contract >::iterator iter;
		for (iter = this->totalTable.begin();iter!=this->totalTable.end();iter++)
		{
			string contract = iter->first;
			CThostFtdcDepthMarketDataField_Custom_Clean_Contract cData = iter->second;

			//如果合约没修改过
			if (!cData.flag_contract)                             //iter->second->flag_contract)
			{
				//更新最后更新时间和毫秒数
				if (cData.fronttime_1 == cData.fronttime_2)
				{
					cData.data.UpdateTime = cData.fronttime_1 + 1;
					cData.fronttime_2 = cData.fronttime_1;
					cData.fronttime_1 = cData.fronttime_1 + 1;
				}else
				{
					cData.data.UpdateTime = cData.fronttime_1;//timenow;
					cData.fronttime_2 =  cData.fronttime_1;
				}
				cData.data.UpdateTime = method_Template_TimeInt(cData.data.UpdateTime);
				cData.fronttime_2 = method_Template_TimeInt(cData.fronttime_2);
				cData.fronttime_1 = method_Template_TimeInt(cData.fronttime_1);

				//修改0或者500ms,100ms或者400ms
			    cData.end_writetime_Millisec= (cData.end_writetime_Millisec+500)%1000;
				cData.data.UpdateMillisec =cData.end_writetime_Millisec;

				(cData.flag_contract) = true;    
		
				//处理一天只有几个数据的品种，它修改的时间可能还是上一天   205160112加
				if (filter_Unreasonable_Price(cData.data.UpdateTime) 
					/*&& filter_Unreasonable_Price(cData.data.UpdateTime,cData.data.TradingDay)*/){
					//过滤
				}else
				{
					//放到数据整理后的缓冲区里去
					cmdSpi_custom->methodAddDatas_After((cData.data));
					//更新map
					((this->totalTable))[contract] = cData;
					//写入文件
					CThostFtdcDepthMarketDataField_Custom ttemmmm = (cData.data);
					char *ss="补1";
					strcat_s(ttemmmm.ExchangeID,sizeof(ttemmmm.ExchangeID),ss);
					methodWriteFile(ttemmmm,"tick");//*(iter->second->data)
				}
				
			}

		}
		//交易所已经被更新过
		(this->flag_exchange) = true;	
	}
	#pragma endregion 循环补数据

};

#pragma endregion 增减数据 

#pragma region 入队出队
/************************************************************************/                       							 
/*							1)添加整理前数据							*/
/************************************************************************/
void CMdSpi_Custom::methodAddDatas_Before(CThostFtdcDepthMarketDataField* data,unsigned int receivetime){
	//CThostFtdcDepthMarketDataField  ->  CThostFtdcDepthMarketDataField_Custom	
	CThostFtdcDepthMarketDataField_Custom cdata = cmdSpi_custom->methodChangeToCustom(data,receivetime);
	cmdSpi_custom->before_data_buffer.push(cdata);
};
/************************************************************************/                       							 
/*							2)取整理前数据							    */
/************************************************************************/
CThostFtdcDepthMarketDataField_Custom CMdSpi_Custom::methodGetDatas_Before(){
	try
	{
		CThostFtdcDepthMarketDataField_Custom da;
		da.TradingDay =0;
		if (cmdSpi_custom->before_data_buffer.size())
		{
			da= cmdSpi_custom->before_data_buffer.front();
			cmdSpi_custom->before_data_buffer.pop();
		}

		return da;
	}
	catch (std::exception& e)
	{
		std::cerr <<"exception caught:methodGetDatas_Before "<< e.what() <<'\n';
	}	

};
/************************************************************************/                       							 
/*							3)添加整理后数据							 */
/************************************************************************/
void CMdSpi_Custom::methodAddDatas_After(CThostFtdcDepthMarketDataField_Custom data){

	EnterCriticalSection(&cs);
	cmdSpi_custom->after_data_buffer.push(data);
	//20150824 tangb  用于写数据库的队列
	//cmdSpi_custom->data_to_database.push(data);
	LeaveCriticalSection(&cs);

};
/************************************************************************/                       							 
/*							4)取整理后数据								*/
/************************************************************************/
CThostFtdcDepthMarketDataField_Custom CMdSpi_Custom::methodGetDatas_After(){
	try
	{
		CThostFtdcDepthMarketDataField_Custom da;
		da.TradingDay =0;
		if (!(cmdSpi_custom->after_data_buffer.empty()))
		{
			da= (cmdSpi_custom->after_data_buffer.front());
			cmdSpi_custom->after_data_buffer.pop();
		}

		return da;
	}
	catch (std::exception& e)
	{
		std::cerr <<"exception caught:methodGetDatas_After "<< e.what() <<'\n';
	}	

};

#pragma endregion 入队出队

/************************************************************************/                       							 
/*							转换数据							     	 */
/************************************************************************/
CThostFtdcDepthMarketDataField_Custom CMdSpi_Custom::methodChangeToCustom(
	CThostFtdcDepthMarketDataField* data,unsigned int receivetime)
{

	//CThostFtdcDepthMarketDataField  ->  CThostFtdcDepthMarketDataField_Custom	
	try
	{
		CThostFtdcDepthMarketDataField_Custom custom;//=new CThostFtdcDepthMarketDataField_Custom();
		custom.TradingDay= method_TimeToInt(data->TradingDay);
		// 	strcpy_s(custom.InstrumentID,sizeof(custom.InstrumentID),data->InstrumentID);
		// 	strcpy_s(custom.ExchangeID,sizeof(custom.ExchangeID),data->ExchangeID);
		//strncpy_s(b,_countof(b),a,strlen(a));

		strncpy_s(custom.InstrumentID,_countof(custom.InstrumentID),data->InstrumentID,strlen(data->InstrumentID));
		strncpy_s(custom.ExchangeID,_countof(custom.ExchangeID),data->ExchangeID,strlen(data->ExchangeID));
		custom.LastPrice=data->LastPrice;
		custom.PreClosePrice=data->PreClosePrice;
		custom.PreOpenInterest=data->PreOpenInterest;
		custom.Volume=data->Volume;
		custom.Turnover=data->Turnover;
		custom.OpenPrice=data->OpenPrice;
		custom.ClosePrice=data->ClosePrice;
		custom.OpenInterest=data->OpenInterest;
		custom.UpperLimitPrice=data->UpperLimitPrice;
		custom.LowerLimitPrice=data->LowerLimitPrice;
		custom.PreDelta=data->PreDelta;
		custom.CurrDelta=data->CurrDelta;
		custom.UpdateTime=method_TimeToInt(data->UpdateTime);
		custom.UpdateMillisec=data->UpdateMillisec;
		custom.BidPrice1=data->BidPrice1;
		custom.BidVolume1=data->BidVolume1;
		custom.AskPrice1=data->AskPrice1;
		custom.AskVolume1=data->AskVolume1;
		custom.AveragePrice=data->AveragePrice;
		custom.OpenInterestRange = 0;
		custom.ReceiveTime = receivetime;
		return custom; 
	}
	catch (std::exception& e)
	{
		std::cerr <<"exception caught:methodChangeToCustom "<< e.what() <<'\n';
	}	
};




/************************************************************************************************************/
/*                 原有CTP系统接收数据函数定义																*/
/************************************************************************************************************/
#pragma region 原有CTP系统接收数据函数定义

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo); 
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
	cerr << "--->>> " << "OnFrontConnected" << endl;
	///用户登录请求
	ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
		// 请求订阅行情
		SubscribeMarketData();	
	}
}

void CMdSpi::SubscribeMarketData()
{
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cerr << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cerr <<pSpecificInstrument->InstrumentID<< endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspUnSubMarketData" << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{	

	EnterCriticalSection(&cs);
	//记录接收数据的时间
	unsigned int receivetime = GetTickCount();
	//CThostFtdcDepthMarketDataField  ->  CThostFtdcDepthMarketDataField_Custom	
	cmdSpi_custom->methodAddDatas_Before(pDepthMarketData,receivetime);

	LeaveCriticalSection(&cs);
	//	cerr <<"品种："<<pDepthMarketData->InstrumentID<<"时间： "<<pDepthMarketData->UpdateTime<<"价格："<<pDepthMarketData->LastPrice<<"前结算："<<pDepthMarketData->PreSettlementPrice<<"今结算："<<pDepthMarketData->SettlementPrice<<endl;
	//	cerr <<pDepthMarketData->InstrumentID<<"-- "<<pDepthMarketData->UpdateTime<<" =="<<pDepthMarketData->LastPrice<<endl;
	// 	if (((string)pDepthMarketData->InstrumentID).substr(0,2)=="IF")
	// 	{
	// 	
	// 	cerr << pDepthMarketData->InstrumentID<<" "<<pDepthMarketData->UpdateTime<<" "<<pDepthMarketData->LastPrice<<" "<</*pDepthMarketData->ReceiveTime<<" "<<*/endl;
	// 	}
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

#pragma endregion 原有CTP系统接收数据函数定义

