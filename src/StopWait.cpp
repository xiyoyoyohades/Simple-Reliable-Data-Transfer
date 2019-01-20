// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
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
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
	
	return 0;
}

