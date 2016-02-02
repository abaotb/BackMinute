#ifndef BASE_H
#define BASE
#include "MdSpi.h"
#include "Custom_Methods.h"
using namespace std;

extern int	  DATA_TIME_INTERVAL;

#pragma once
//����
class BaseData{
public:
	//���캯��
	BaseData(string contract,
		int compare_time,
		CThostFtdcDepthMarketDataField_Custom_Other current_data,
		CThostFtdcDepthMarketDataField_Custom_Other compare_data)
	{

		this->contract = contract;					//��Լ���� 
		this->compare_time = compare_time;			//��¼Tick���ݵ�ǰʱ�� 
		this->current_data = current_data;			//��ǰ����
		this->compare_data = compare_data;			//���Ƚ�����	
	};
	BaseData()
	{
		CThostFtdcDepthMarketDataField_Custom_Other current_data;
		CThostFtdcDepthMarketDataField_Custom_Other compare_data;
		current_data.TradingDay = 0;
		compare_data.TradingDay = 0;
		this->contract = "";						//��Լ���� 
		this->compare_time = 0;						//��¼Tick���ݵ�ǰʱ�� 
		this->current_data = current_data;			//��ǰ����
		this->compare_data = compare_data;			//���Ƚ�����
	};
	~BaseData(){};
public:
	//��Լ����
	string contract;
	//��ǰ���ݣ����Ƚ�����	              
	CThostFtdcDepthMarketDataField_Custom_Other current_data; 
	CThostFtdcDepthMarketDataField_Custom_Other compare_data; 

	//��¼Tick���ݵ�ǰʱ�� 
	int compare_time;
	//�жϺͺ�Լ�Ƿ����map
	virtual bool isExist(string str)const =0;
	//���Ӻ�Լ��map
	virtual void addContractToMap(CThostFtdcDepthMarketDataField_Custom_Tick*ct) const =0;
	//��������
	virtual void updateData(CThostFtdcDepthMarketDataField_Custom_Tick*ct)const =0;
	//���ݵ��߼�����
	virtual void methodLogicalOperation(BaseData *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick &custom_tick)const =0;
	//��ʼ����һ���׶�����
	virtual void methodInitNext(BaseData *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick &custom_tick)const = 0;
	//��һ��tick���ݴ������ҳ�1�����ڵĿ��ոߵͼ۵ȵ�
	virtual CThostFtdcDepthMarketDataField_Custom_Other methodTickToOther(CThostFtdcDepthMarketDataField_Custom_Tick &original_data_tick)const =0;

};

//һ���Ӽ̳���
class  BaseDataInherit:public BaseData{
public:
	//���캯��
	//Triangle(double d,double h):Graph(d,h){};
	BaseDataInherit(string contract,
		int compare_time,
		CThostFtdcDepthMarketDataField_Custom_Other current_data,
		CThostFtdcDepthMarketDataField_Custom_Other compare_data
		):BaseData(contract,compare_time,current_data,compare_data)
	{

		map<string, BaseDataInherit*> *tsmd_a= new map<string, BaseDataInherit*>(); 
		this->tsmd_a = tsmd_a;					    //map
		this->TIME_INTERVAL	= DATA_TIME_INTERVAL;	//ʱ����				/**********Ҫ�ĵĵط�**********/

		this->contract = "";						//��Լ���� 
		this->compare_time = 0;						//��¼Tick���ݵ�ǰʱ�� 
		this->current_data = current_data;			//��ǰ����
		this->compare_data = compare_data;			//���Ƚ�����


	};

	BaseDataInherit()
	{

		map<string, BaseDataInherit*> *tsmd_a= new map<string, BaseDataInherit*>(); 
		CThostFtdcDepthMarketDataField_Custom_Other current_data;
		CThostFtdcDepthMarketDataField_Custom_Other compare_data;
		current_data.TradingDay = 0;
		compare_data.TradingDay = 0;
		this->tsmd_a = tsmd_a;						//map
		this->TIME_INTERVAL = DATA_TIME_INTERVAL;	//ʱ����			/**********Ҫ�ĵĵط�**********/

		this->contract = "";						//��Լ���� 
		this->compare_time = 0;						//��¼Tick���ݵ�ǰʱ�� 
		this->current_data = current_data;
		this->compare_data = compare_data;

	};
	~ BaseDataInherit(){};	

public:
	int TIME_INTERVAL;								//ʱ����
	map<string, BaseDataInherit*> *tsmd_a;          //��Լ��map
	//�жϺͺ�Լ�Ƿ����map
	bool isExist(string str) const
	{
		if (this->tsmd_a != NULL)                   //ע����this
		{
			//������
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
	//���Ӻ�Լ��map
	void addContractToMap(CThostFtdcDepthMarketDataField_Custom_Tick*ct) const 
	{
		//��������
		BaseDataInherit* torigin= new  BaseDataInherit();
		torigin->contract=(string)(ct->InstrumentID);
		(*(this->tsmd_a))[(string)(ct->InstrumentID)] = torigin;							//ע����this,����torgin	
		torigin->TIME_INTERVAL = this->TIME_INTERVAL;										//�޸�ʱ����
		int remainder = TIME_INTERVAL >=60 ? 100 : TIME_INTERVAL;							//100//60	/**********Ҫ�ĵĵط�**********/
		torigin->compare_time = method_TimeToInt_60(ct->UpdateTime-ct->UpdateTime%remainder);
		//����1���ӱ�׼
		torigin->compare_data.Volume = 0;
		torigin->compare_data.Turnover = 0;
		torigin->compare_data.OpenInterest =0;//ct->OpenInterestRange;
		//��ʼ����һ���׶�
		torigin->methodInitNext(torigin,*ct);


	}
	//��������
	void updateData(CThostFtdcDepthMarketDataField_Custom_Tick*ct) const
	{
		BaseDataInherit * torigin= (*(this->tsmd_a))[(string)(ct->InstrumentID)];            //ע����this
		torigin->methodLogicalOperation(torigin,*ct);
	}
	//���ݵ��߼�����
	void methodLogicalOperation(BaseData *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick &custom_tick)const
	{	
		//����ҹ��0��ʱʱ��ıȽ�
		if (custom_tick.UpdateTime == 0)                        
		{	
			tsmdarrays->compare_time = 0;
			
// 			 //20151106
// 			CThostFtdcDepthMarketDataField_Custom_Other fff = tsmdarrays->current_data;
// 			char  filename[10];
// 			itoa(TIME_INTERVAL, filename,10);
// 			methodWriteFile2(fff,(string)filename);//"today-1min"
// 			Ado_Connect_Mysql(fff);
// 			//д������         //20151113
// 			tsmdarrays->methodInitNext(tsmdarrays,custom_tick);

			//20151119
			map<string, BaseDataInherit*>::iterator iter;
			for (iter=this->tsmd_a->begin();iter!=this->tsmd_a->end();iter++)
			{
				if (iter->second->compare_time!=0)//ҹ��0��
				{
					iter->second->compare_time = 0;
				}
			
			}
		}
		//cerr << custom_tick.InstrumentID<<"     method_Base"<<endl;

		int time_60=method_TimeToInt_60(custom_tick.UpdateTime);
		if(time_60 - tsmdarrays->compare_time < TIME_INTERVAL && time_60 - tsmdarrays->compare_time >=0)
		{
			if (strcmp(custom_tick.InstrumentID,tsmdarrays->current_data.InstrumentID)!=0)
			{
				return;
			}	
			

			//���������߼�
			if (custom_tick.LastPrice >tsmdarrays->current_data.HighestPrice && priceRange(custom_tick.LastPrice,tsmdarrays->current_data.HighestPrice))
			{
				//������߼�
				tsmdarrays->current_data.HighestPrice = custom_tick.LastPrice;

			}
			if(custom_tick.LastPrice <  tsmdarrays->current_data.LowestPrice && priceRange(custom_tick.LastPrice,tsmdarrays->current_data.LowestPrice))
			{
				//������ͼ�
				tsmdarrays->current_data.LowestPrice = custom_tick.LastPrice;		

			}
			//�������̼�
			tsmdarrays->current_data.ClosePrice = custom_tick.LastPrice;

			//ί��ί��
			tsmdarrays->current_data.BidPrice1=custom_tick.BidPrice1;
			tsmdarrays->current_data.BidVolume1=custom_tick.BidVolume1;
			tsmdarrays->current_data.AskPrice1=custom_tick.AskPrice1;
			tsmdarrays->current_data.AskVolume1=custom_tick.AskVolume1;
			//���³ֲ���
			tsmdarrays->current_data.OpenInterest=custom_tick.OpenInterest;
			//���³ɽ������ɽ���ֱֲ仯
			tsmdarrays->current_data.Volume = 
				custom_tick.Volume -  tsmdarrays->compare_data.Volume;

			tsmdarrays->current_data.Turnover =
				custom_tick.Turnover -  tsmdarrays->compare_data.Turnover;

			tsmdarrays->current_data.OpenInterestRange = 
				custom_tick.OpenInterest-  tsmdarrays->compare_data.OpenInterest;

		}
		else if (time_60 - tsmdarrays->compare_time>=TIME_INTERVAL)
		{
			//����̨���
			CThostFtdcDepthMarketDataField_Custom_Other fff = tsmdarrays->current_data;
			//cerr << fff.InstrumentID<<" "<<fff.UpdateTime<<" ��߼ۣ�"<<fff.HighestPrice<<"��ͼۣ� "<<fff.LowestPrice<<" "<<endl;
			//cerr <</* fff.InstrumentID<<*/"        "<<fff.UpdateTime<<" �ɽ�����"<<fff.Volume<<" �ɽ��"<<fff.Turnover<<" �ֲ�����"<<fff.OpenInterest<<" �ֲ"<<fff.OpenInterestRange<<endl;

			//дMysql���ݿ�
			//itoa(number, string,10);
			char  filename[10];
		    itoa(TIME_INTERVAL, filename,10);
			methodWriteFile2(fff,(string)filename);//"today-1min"

			////////////////////////////////���˲������۸�
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

			//����ʱ��
			tsmdarrays->compare_time = time_60;//tsmdarrays->time_oneminute + 60;
			//д������
			tsmdarrays->methodInitNext(tsmdarrays,custom_tick);

		}

	};

	//��ʼ����һ���׶�����
	void methodInitNext(BaseData *tsmdarrays,CThostFtdcDepthMarketDataField_Custom_Tick &custom_tick) const
	{
		//cerr << custom_tick.InstrumentID<<"       methodUpdate_Base "<<endl;
		//�����ոߵ͸���ͬһ�۸�	
		CThostFtdcDepthMarketDataField_Custom_Other co = tsmdarrays->methodTickToOther(custom_tick);
		tsmdarrays->current_data = co;      //����data_oneminute[650]����
		tsmdarrays->compare_data= co;       //���±�׼

	};
	//���Զ����tick����ת����5�������tick���ݻ���1���ӵ����ݣ������ոߵͼ�Ϊͬһ�۸�
	CThostFtdcDepthMarketDataField_Custom_Other methodTickToOther
		(CThostFtdcDepthMarketDataField_Custom_Tick &original_data_tick) const
	{

		CThostFtdcDepthMarketDataField_Custom_Other custom_tick_other;

		strncpy_s(custom_tick_other.ExchangeID,_countof(custom_tick_other.ExchangeID),
			original_data_tick.ExchangeID,strlen(original_data_tick.ExchangeID));

		strncpy_s(custom_tick_other.InstrumentID,_countof(custom_tick_other.InstrumentID),
			original_data_tick.InstrumentID,strlen(original_data_tick.InstrumentID));

		custom_tick_other.TradingDay=original_data_tick.TradingDay;
		//���̼ۣ����̼ۣ���߼ۣ���ͼ�
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


};
	

#endif BASE_H