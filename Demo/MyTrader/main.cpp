#include <iostream>
#include <string>
using namespace std;

// 模拟 OgsApiDataType.h 文件
typedef string CODE;
typedef double PRICE;
typedef int VOLUME;

// 模拟 OgsApiStruct.h
typedef struct SendOrder {
	CODE code;
	PRICE price;
	VOLUME volume;
} *PSendOrder;

// 模拟 OgsApiTrader.h
// 供用户继承回调的接口
class OgsApiTraderRsp
{
public:
	virtual int RspSendOrder(int requestId, bool isLast, int sysOrderId) = 0;
	virtual int RspQueryOrder(int requestId, bool isLast, int dealVolume) = 0;
private:

};

// 给用户定义的供交易调用的接口
class OgsApiTrader
{
public:
	int ReqSendOrder(int requestId, SendOrder sendOrder)
	{
		cout << "ReqSendOrder, requestId = " << requestId << ", sendOrderVolume = " << sendOrder.volume << endl;
		m_rsp->RspSendOrder(requestId, true, 123456);
		m_rsp->RspQueryOrder(requestId, false, sendOrder.volume / 3);
		m_rsp->RspQueryOrder(requestId, false, sendOrder.volume / 3 * 2);
		m_rsp->RspQueryOrder(requestId, true, sendOrder.volume);
		return 0;
	}

	int RegisterRsp(OgsApiTraderRsp *pRsp)
	{
		m_rsp = pRsp;
		return 0;
	}

private:
	OgsApiTraderRsp *m_rsp;
};

// 模拟用户客户端如何继承我们的OgsApiTraderRsp类，处理OGS推送回来的消息
class MyTraderRsp : public OgsApiTraderRsp
{
public:
	int RspSendOrder(int requestId, bool isLast, int sysOrderId)
	{
		cout << "OGS RspSendOrder, requestId = " << requestId << ", isLast = " << isLast << ", sysOrderId = " << sysOrderId << endl;
		return 0;
	}

	int RspQueryOrder(int requestId, bool isLast, int dealVolume)
	{
		cout << "OGS RspQueryOrder, requestId = " << requestId << ", isLast = " << isLast << ", dealVolume = " << dealVolume << endl;
		return 0;
	}
};

int g_requestId = 1;
int main()
{
	OgsApiTrader *pApi = new OgsApiTrader();
	MyTraderRsp *pMyRsp = new MyTraderRsp();
	pApi->RegisterRsp(pMyRsp);

	SendOrder sendOrder;
	sendOrder.code = "600446";
	sendOrder.price = 88.88;
	sendOrder.volume = 1200;
	pApi->ReqSendOrder(g_requestId++, sendOrder);

	delete pApi;
	delete pMyRsp;

	return 0;
}