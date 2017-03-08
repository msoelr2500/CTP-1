// testTraderApi.cpp : 定义控制台应用程序的入口点。
//
#include "../api/ThostFtdcTraderApi.h"
#include "TraderSpi.h"

// UserApi对象
CThostFtdcTraderApi* pUserApi;

// 配置参数
char  FRONT_ADDR[] = "tcp://180.168.146.187:10000";		// 前置地址
TThostFtdcBrokerIDType	BROKER_ID = "9999";				// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "035286";		// 投资者代码
TThostFtdcPasswordType  PASSWORD = "111111";			// 用户密码
TThostFtdcInstrumentIDType INSTRUMENT_ID = "TF1709";	// 合约代码
TThostFtdcDirectionType	DIRECTION = THOST_FTDC_D_Buy;	// 买卖方向
TThostFtdcPriceType	LIMIT_PRICE = 98.9; // 38850;				// 价格

// 请求编号
int iRequestID = 0;

void main(void)
{
	// 初始化UserApi
	pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// 创建UserApi
	CTraderSpi* pUserSpi = new CTraderSpi();
	pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);			// 注册事件类
	pUserApi->RegisterFront(FRONT_ADDR);							// connect
	pUserApi->Init();

	////CTraderSpi *ts = new CTraderSpi();
	////ts->OnFrontConnected();
	//pUserApi->ReqUserLogin();
	pUserApi->Join();

//	pUserApi->Release();
}