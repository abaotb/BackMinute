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


/************************************************************************/
/*							数据分类线程                                */
/************************************************************************/
DWORD WINAPI Classify_Data2(LPVOID lpParamter)
{

	OneMinute oneninute;
	BaseData *base;
	base= &oneninute;


	bool flag = false;        //每次只处理一个数据

 	while(1)
 	{

		if(method_Rest_Time())
		{
			Sleep(1000);
		    //在休盘时间重置    清除内存中数据							/**********要改的地方**********/
			if (oneninute.tsmd_a->size()!=0)
			{
				oneninute.TIME_INTERVAL=0;
				oneninute.contract="";
				oneninute.compare_time=0;
				memset(&(oneninute.current_data),0,sizeof(oneninute.current_data));
				memset(&(oneninute.compare_data),0,sizeof(oneninute.compare_data));
				oneninute.tsmd_a = NULL;
				oneninute.tsmd_a= new map<string,OneMinute*>();
				base = NULL;
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
    		flag = true;   				
    	}
    	if (flag)
 		{
			
 			flag = false;  
 
 			//转换数据
			CThostFtdcDepthMarketDataField_Custom_Tick ct = methodChangeData(*dd);
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
