## OGS平台新方案 ##
Write by：杨翌超 卢郴群。2017/3/8 16:40:41 
## 1. 简介 ##
为了描述方便，将OGS平台新方案，暂时在本文档以OGS5名称替代。  
OGS5基于OGS3进行开发，主要工作是，提供相应的库以及的接口文件，将以前仅供OMS接入的OGS改为可以方便供其它系统如TWD等进行接入。
## 2. OGS3依赖库列表 ##
OGS5采用的库跟OGS3采用的库一致。大概列举如下：  
- QTP: QTP封装的库。  
- Libevent：轻量级的开源高性能网络库  
- Spdlog：日志打印库  
- RocksDB：本地数据库
## 3. 运行环境 ##
Linux X86_X64，如果时间宽裕，打算提供Windows版本。
## 4. OGS 通讯协议 ##
主要用于OGS5系统与其下端客户端之间进行交易所需的数据交换和通讯。主要介绍一下OGS与客户端通讯的两种模式。  
**对话通讯模式**：是有客户端主动发起通讯请求。该请求被OGS接受并处理并调用券商接口，并向客户端返回相应响应。例如送单。这种通讯类似于客户端与服务端进行的一问一答。如下图所示：  
![对话通讯模式](http://i.imgur.com/15QubaI.png)  
**推送模式**：是指OGS主动向客户端推送消息。如订单状态更新推送。如下图所示：  
![推送模式](http://i.imgur.com/gwBhjzT.png)   
## 5.OGS系统结构图 ##
如下图所示：  
![](http://i.imgur.com/E0KlYch.png)  
## 6.API 设计（主要更新内容） ##
由于期货跟现货的接口数据相差比较大，打算提供两种不同的API交易接口。通过我们提供的交易接口，实现客户端与OGS的对接。  
### 6.1接口文件列表 ###
|     文件名      		  |             简要说明                |
| :------------ 		  | :--------------------------------- |
| OgsApiTrader.h   		  | 现货的接口 						   |
| OgsApiDataType.h        | 现货的数据类型定义					   |
| OgsApiStruct.h      	  | 现货的数据接口					   |
| OgsApiFuturesTrader.h   | 期货的接口						   |
| OgsApiFuturesDataType.h | 期货的数据类型定义					   |
| OgsApiFuturesStruct.h   | 期货的数据接口					   |
| OgsApiTrader.a 		  | 现货库文件					       |
| OgsApiTrader.so   	  | 现货动态库文件					   |
| OgsApiFuturesTrader.a   | 期货库文件				   		   |
| OgsApiFuturesTrader.so  | 期货动态库文件					   |
| error.txt  			  | 所有错误返回码参考信息				   |
### 6.2类设计 ###
**Rsp**：如OgsApiTraderRsp。包含所有的响应和回报函数，用于接收OGS或者券商那边发送给OGS转发的消息。开发者需要继承该接口类，并实现其中相应的虚函数。从而完成客户端所需的功能。  
**Api**：如OgsApiTrader。由OGS提供给开发者发起请求的接口函数，开发者直接调用即可。可参考如下图：
![接口类示意图](http://i.imgur.com/qZ95faH.png)
### 6.3函数命名设计 ###
|     功能      		  |     格式		  |             示例	              |
| :------------ 	  | :------------ | :-----------------------------|
| 请求   		      | Req   		  | ReqSendOrder()				  |
| 回报				  | Rsp			  | RspSendOrder()				  |
### 6.4如何将请求数据与回报数据联系起来 ###
对于每个请求函数，都会带一个requestId参数，回报的时候，将返回跟请求相同的编号。因为有的时候，一个请求函数，会触发多个不同的响应函数一次或者多次。如ReqSendOrder调用，可能触发的函数有RspSendOrder() -> RspQueryOrder() -> RspQueryOrder() ...。这时候，能将请求关联起来的就只有请求编号了。
对于每个回报函数，还会带有一个isLast参数。因为有时候当一次查询多条数据的时候，会调用回报函数多次。比如一次性查询当天的所有订单。会多次触发RspQueryOrder()，每次回报一个订单信息，当isLast为true的时候，表明所有的订单已经全部推送给客户端。
### 6.5一个简单的Demo用于展示思路 ###
```C++
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
```
输出显示
> ReqSendOrder, requestId = 1, sendOrderVolume = 1200 // 调用送单接口
> OGS RspSendOrder, requestId = 1, isLast = 1, sysOrderId = 123456 // 送单回报
> OGS RspQueryOrder, requestId = 1, isLast = 0, dealVolume = 400   // 推送第一次订单成交回报
> OGS RspQueryOrder, requestId = 1, isLast = 0, dealVolume = 800   // 推送第二次订单成交回报
> OGS RspQueryOrder, requestId = 1, isLast = 1, dealVolume = 1200  // 推送第三次订单成交回报（全成）