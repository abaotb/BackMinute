					/************************************************************************/
					/*  数据处理程序文件，主要将MdSpi.cpp文件的数据处理成5秒，1分钟等数据   */
					/************************************************************************/
#include "Base.h"
using namespace std;


// USER_API参数
extern CThostFtdcMdApi* pUserApi;
//多线程

extern CRITICAL_SECTION cs;

//CThostFtdcDepthMarketDataField_Custom data_static2;                                     //取到的数据,文档全局变量
extern CMdSpi_Custom * cmdSpi_custom;	


/************************************************************************/
/*							数据分类线程                                */
/************************************************************************/
DWORD WINAPI Classify_Data2(LPVOID lpParamter)
{
	map<string,OneMinute*> *tsmd_a= new map<string,OneMinute*>(); 
	CThostFtdcDepthMarketDataField_Custom_Other current_data;
	CThostFtdcDepthMarketDataField_Custom_Other compare_data;
	current_data.TradingDay=0; compare_data.TradingDay=0;
	
	OneMinute oneninute("",0,current_data,compare_data,tsmd_a);
	BaseData *base;
	base= &oneninute;


	bool flag = false;        //每次只处理一个数据

 	while(1)
 	{

		if(method_Rest_Time())
		{
			Sleep(1000);
		    //在休盘时间重置
			if (oneninute.tsmd_a->size()!=0)
			{
				oneninute.contract="";
				oneninute.compare_time=0;
				memset(&(oneninute.current_data),0,sizeof(oneninute.current_data));
				memset(&(oneninute.compare_data),0,sizeof(oneninute.compare_data));
				oneninute.tsmd_a=NULL;
				oneninute.tsmd_a= new map<string,OneMinute*>();
				base= NULL;
				base = &oneninute;
			}
		}

 		//进入临界区
 		EnterCriticalSection(&cs);
 		CThostFtdcDepthMarketDataField_Custom* dd =cmdSpi_custom->methodGetDatas_After();
 		//释放临界区
 		LeaveCriticalSection(&cs);
     	if (dd!=NULL)
     	{
    		//data_static2 =*dd;		
    		flag = true;   				
    	}
    	if (flag)
 		{
			//夜盘置0
			//methodYepan2(data_static2.UpdateTime,&oneninute);

 			flag = false;  
 
 			//转换数据
			CThostFtdcDepthMarketDataField_Custom_Tick ct = methodChangeData(*dd/*data_static2*/);
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
 
}//end FUN











//夜盘重置
//void methodYepan2(int yepan_time,OneMinute*oneninute) 
//{
//	if (yepan_time == 0)                        //夜盘0点
//	{
//		map<string,OneMinute*>::iterator iter;
//		for (iter = oneninute->tsmd_a->begin();iter != oneninute->tsmd_a->end();iter++)
//		{
//			   //夜盘0点
//			iter->second->compare_time = 0;
//		
//		}
//	}
//};


/*dd = new CThostFtdcDepthMarketDataField_Custom();
			dd->TradingDay=11;
			strcpy(dd->InstrumentID,"IF1511");
				data_static2 =*dd;		*/
