
#include "DataBaseDao.h"
using namespace std;

//连接数据库
void Ado_Connect_Mysql(CThostFtdcDepthMarketDataField_Custom_Other ctd_data)
{
#pragma region 连接数据库
	//////////////////////////////////////////////////////////////////////////连接数据库
	CoInitialize(NULL); 
	_ConnectionPtr m_pConnection=NULL;
	//创建Connection对象
	m_pConnection.CreateInstance("ADODB.Connection");//(__uuidof(Connection));   
	try 
	{  
		//设置连接时间  
		m_pConnection->ConnectionTimeout = 5;
		_bstr_t connstr = "DSN=MySqlODBC;server=";
		connstr += local_host ;
		connstr += ";database=";
		connstr += data_base;;
		//打开数据库连接  
		HRESULT hr = m_pConnection->Open(connstr,user_name,user_pwd,adModeUnknown); //adModeUnknown 缺省。当前的许可权未设置
		//m_pConnection->Open("Data Source=adotest;UID=sa;PWD=;","","",adModeUnknown);
	}  
	catch(_com_error &e)  
	{  
		cout<<"连接错误"<<endl;
		return ;  
	}   
	//////////////////////////////////////////////////////////////////////////连接数据库


#pragma endregion 连接数据库

#pragma region 执行SQL语句
	//////////////////////////////////////////////////////////////////////////添加数据
	//执行SELETE语句
	_RecordsetPtr m_pRecordset;
	//m_pRecordset.CreateInstance("ADODB.Recordset");
	m_pRecordset.CreateInstance(__uuidof( Recordset )); //创建实例
	//一定要打开
	_bstr_t openRecordset="SELECT  * FROM ";
	openRecordset+= table;
	openRecordset +=" limit 1";
	m_pRecordset->Open(openRecordset,m_pConnection.GetInterfacePtr
		(),adOpenStatic,adLockOptimistic,adCmdText);// 执行SQL语句，得到记录集, m_pConnection必须已知
	try{
		if (m_pRecordset!=NULL)
		{
			if (ctd_data.TradingDay!=0)
			{
				m_pRecordset->AddNew();	
				m_pRecordset->PutCollect("exchange_id",_variant_t(ctd_data.ExchangeID));
				m_pRecordset->PutCollect("instrument_id",_variant_t(ctd_data.InstrumentID));
				m_pRecordset->PutCollect("trading_day",_variant_t((long)(ctd_data.TradingDay)));
				m_pRecordset->PutCollect("update_time",_variant_t((long)(ctd_data.UpdateTime)));			
				m_pRecordset->PutCollect("open_price",_variant_t(ctd_data.OpenPrice));
				m_pRecordset->PutCollect("close_price",_variant_t(ctd_data.ClosePrice));
				m_pRecordset->PutCollect("highest_price",_variant_t(ctd_data.HighestPrice));
				m_pRecordset->PutCollect("lowest_price",_variant_t(ctd_data.LowestPrice));	
				m_pRecordset->PutCollect("turnover",_variant_t(ctd_data.Turnover));			
				m_pRecordset->PutCollect("open_interest",_variant_t((long)(ctd_data.OpenInterest)));
				m_pRecordset->PutCollect("open_interestrange",_variant_t((long)(ctd_data.OpenInterestRange)));
				m_pRecordset->PutCollect("bid_price1",_variant_t(ctd_data.BidPrice1));
				m_pRecordset->PutCollect("ask_price1",_variant_t(ctd_data.AskPrice1));
				m_pRecordset->PutCollect("volume",_variant_t((long)(ctd_data.Volume)));
				m_pRecordset->PutCollect("bid_volume1",_variant_t(ctd_data.BidVolume1));
				m_pRecordset->PutCollect("ask_volume1",_variant_t(ctd_data.AskVolume1));
				m_pRecordset->Update();     ///一定要保存到库中，不然无效

			}

		}

	}catch(_com_error &e){
		cout<<e.ErrorMessage()<<endl;
		//continue;
	}

	//////////////////////////////////////////////////////////////////////////添加数据
#pragma endregion  执行SQL语句

#pragma region 关闭数据连接
	try
	{
		//清空数组
		//memset(all_contracts_array.All_Contracts_Data, 0x0, all_contracts_array.index_all * sizeof(CThostFtdcDepthMarketDataField_Custom));
		m_pRecordset->Close();
		//m_pRecordset->Release();
		m_pConnection->Close();
		m_pConnection.Release();
		CoUninitialize();
	}
	catch(_com_error &e){
		cout<<e.ErrorMessage()<<endl;
		//continue;
	}

#pragma endregion 关闭数据连接

	return ;
}