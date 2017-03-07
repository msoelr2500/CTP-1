// testTraderApi.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include ".\ThostTraderApi\ThostFtdcTraderApi.h"
#include "TraderSpi.h"

// UserApi����
CThostFtdcTraderApi* pUserApi;

// ���ò���
char  FRONT_ADDR[] = "tcp://180.168.146.187:10000";		// ǰ�õ�ַ
TThostFtdcBrokerIDType	BROKER_ID = "9999";				// ���͹�˾����
TThostFtdcInvestorIDType INVESTOR_ID = "035286";		// Ͷ���ߴ���
TThostFtdcPasswordType  PASSWORD = "111111";			// �û�����
TThostFtdcInstrumentIDType INSTRUMENT_ID = "TF1709111";	// ��Լ����
TThostFtdcDirectionType	DIRECTION = THOST_FTDC_D_Buy;	// ��������
TThostFtdcPriceType	LIMIT_PRICE = 98.9; // 38850;				// �۸�

// ������
int iRequestID = 0;

void main(void)
{
	// ��ʼ��UserApi
	pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// ����UserApi
	CTraderSpi* pUserSpi = new CTraderSpi();
	pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);			// ע���¼���
	pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				// ע�ṫ����
	pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);				// ע��˽����
	pUserApi->RegisterFront(FRONT_ADDR);							// connect
	pUserApi->Init();

	//CTraderSpi *ts = new CTraderSpi();
	//ts->OnFrontConnected();

	pUserApi->Join();

//	pUserApi->Release();
}