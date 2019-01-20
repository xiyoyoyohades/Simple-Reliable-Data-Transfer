// StopWait.cpp : 定义控制台应用程序的入口点。
//

#include "inc.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"

//#define GBN_RDT
//#define SR_RDT
#define TCP

#if defined(GBN_RDT)|| defined(TCP)
#include "GBN.h"
using Sender=GBN::Sender;
using Receiver=GBN::Receiver;

#elif defined(SR_RDT)
#include "SR.h"
using Sender=SR::Sender;
using Receiver=SR::Receiver;

#elif defined(STOPWAIT)
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
using Sender=StopWaitRdtSender;
using Receiver=StopWaitRdtReceiver;

#endif

int main(int argc, char* argv[])
{
	#if defined(TCP)
	RdtSender *ps = new GBN::Sender(true);
	RdtReceiver * pr = new GBN::Receiver();
	#elif defined(GBN_RDT)
	RdtSender *ps = new GBN::Sender();
	RdtReceiver * pr = new GBN::Receiver();
	#elif defined(SR_RDT)
	RdtSender *ps = new SR::Sender();
	RdtReceiver * pr = new SR::Receiver();
	#endif

	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("input.txt");
	pns->setOutputFile("output.txt");
	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}

