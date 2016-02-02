					/************************************************************************/
					/*  数据处理程序文件，主要将MdSpi.cpp文件的数据处理成5秒，1分钟等数据   */
					/************************************************************************/
#include "Base.h"
using namespace std;


// USER_API参数
extern CThostFtdcMdApi* pUserApi;
//多线程
extern CRITICAL_SECTION cs;
extern CMdSpi_Custom * cmdSpi_custom;	
//处理郑商所夜盘日期问题
int TradingDay;

extern vector<CThostFtdcDepthMarketDataField_Custom_Other> cco_vector;	

/************************************************************************/
/*							数据分类线程                                */
/************************************************************************/
DWORD WINAPI Classify_Data2(LPVOID lpParamter)
{

	BaseDataInherit basedatainherit;
	basedatainherit.TIME_INTERVAL =DATA_TIME_INTERVAL;   //默认时间间隔


	BaseData *base;										 //基类指针
	base= &basedatainherit;

	bool flag = false;        //每次只处理一个数据

 	while(1)
 	{
		//休盘时间清空数据
		if(method_Rest_Time())
		{
			Sleep(1000);
			//在休盘时间重置    清除内存中数据							//要改的地方
			if (basedatainherit.tsmd_a->size()!=0)
			{
				basedatainherit.TIME_INTERVAL = DATA_TIME_INTERVAL;
				basedatainherit.contract="";
				basedatainherit.compare_time=0;
				memset(&(basedatainherit.current_data),0,sizeof(basedatainherit.current_data));
				memset(&(basedatainherit.compare_data),0,sizeof(basedatainherit.compare_data));
				basedatainherit.tsmd_a = NULL;
				basedatainherit.tsmd_a= new map<string,BaseDataInherit*>();
				base = NULL;
				base = &basedatainherit;
			}
			if ((method_TimeToInt_Int() > CLOSE_NOON  + 2 * TIME_DIF  && method_TimeToInt_Int()< OPEN_EVENING - TIME_DIF)&&cco_vector.size()!=0)
			{				
				SYSTEMTIME st; 
				GetLocalTime(&st);  
				int y =st.wYear;
				int m =st.wMonth;
				int d =st.wDay;
				int date= (y*10000)+(m*100)+d;
				char  filename[10]={0};
				memset(filename,0,10);
				sprintf(filename,"%d",date);
				//itoa(date, filename,10);
				cout<<"finename:"<<filename<<"     date:"<<date<<endl;
				methodWriteFile3(cco_vector,(string)filename);
				cco_vector.swap(vector<CThostFtdcDepthMarketDataField_Custom_Other>());  //清除容器并最小化它的容量
			}
		}
		
 		//进入临界区
 		EnterCriticalSection(&cs);
 		CThostFtdcDepthMarketDataField_Custom dd =cmdSpi_custom->methodGetDatas_After();
 		//释放临界区
 		LeaveCriticalSection(&cs);	
     	if (dd.TradingDay!=0)
     	{	
    		flag = true;   				
    	}else{
			//睡眠10ms,防止cpu过高      20151222 加
			Sleep(10);
		}
    	if (flag)
 		{
			//////////////////////////////////////////////////////////////////////////
			//处理郑商所夜盘日期问题
			if (dd.UpdateTime>=210000&&dd.UpdateTime<210300)
			{			
				if ((string)(dd.ExchangeID)=="shfe")
				{
					TradingDay = dd.TradingDay;
				}
			}
			if (dd.UpdateTime>=210000||dd.UpdateTime<23000)
			{
				if ((string)(dd.ExchangeID)=="czce")
				{
					dd.TradingDay=TradingDay;
				}
			}	
			//////////////////////////////////////////////////////////////////////////
 			flag = false;  

 			//转换数据
			CThostFtdcDepthMarketDataField_Custom_Tick ct = methodChangeData(dd);
			if (!base->isExist((string)(ct.InstrumentID)))
 			{
				base->addContractToMap(&ct);
 			}
 			else
			{
 				base->updateData(&ct);			
 			}
			
 			
 		}//end flag				
 
 	}//end while
 
}//end Classify_Data2






// 	CThostFtdcDepthMarketDataField_Custom_Other current_data;
// 	current_data.TradingDay =0;
// 	Ado_Connect_Mysql(current_data);
// 	while(1)
// 	{
// 
// 	}




//BaseDataInherit容器
//vector<BaseDataInherit>vBasedata;
//BaseDataInherit basedatainherit2;
//basedatainherit.TIME_INTERVAL =5;   
//
////添加BaseDataInherit对象
//vBasedata.push_back(basedatainherit);
//vBasedata.push_back(basedatainherit2);



// 	BaseDataInherit bdi[2];
// 	bdi[0] = basedatainherit;
// 	bdi[1] = basedatainherit;

//BaseDataInherit basedatainherit2;
//basedatainherit2.TIME_INTERVAL = 5;					 //5秒时间间隔

//methodClearDatas(bdi,2);
/*
void methodClearDatas(BaseDataInherit bdi,int len)
{

	
	if(method_Rest_Time())
	{
		Sleep(1000);
		for (int i=0;i<len;i++)
		{	
			BaseDataInherit	basedatainherit = bdi[i];
			//在休盘时间重置    清除内存中数据							
			if (basedatainherit.tsmd_a->size()!=0)
			{
				basedatainherit.TIME_INTERVAL = DATA_TIME_INTERVAL;
				basedatainherit.contract="";
				basedatainherit.compare_time=0;
				memset(&(basedatainherit.current_data),0,sizeof(basedatainherit.current_data));
				memset(&(basedatainherit.compare_data),0,sizeof(basedatainherit.compare_data));
				basedatainherit.tsmd_a = NULL;
				basedatainherit.tsmd_a= new map<string,BaseDataInherit*>();
				base = NULL;
				base = &basedatainherit;
			}
		}
	  
	}

}*/
/*
//出租车转车
base= &basedatainherit2;
if (!base->isExist((string)(ct.InstrumentID)))
{
	base->addContractToMap(&ct);
}
else
{
	base->updateData(&ct);			
}
*/