#include "GBN.h"
#include "Global.h"
#include "inc.h"
using namespace GBN;


GBN::Sender::Sender(bool fr_en, int win_size):
    fast_resend_en(fr_en),
    win_size(win_size),
    win_base(init_seq),
	dup_ack(-1)
{
    if(fast_resend_en) std::cout<<"GBN Sender using fast resend"<<std::endl<<std::endl;
}

GBN::Sender::~Sender() {

}

bool GBN::Sender::send(Message &message){
    if(sender_window.size()==win_size)  return false;
    Packet* tosend=new Packet;
    tosend->seqnum=(win_base+sender_window.size())%seq_max;
    std::memcpy(tosend->payload, message.data, sizeof(message.data));
    tosend->checksum=pUtils->calculateCheckSum(*tosend);
    sender_window.push(*tosend);

	pUtils->printPacket("GBN Sender sending packet", *tosend);

    pns->sendToNetworkLayer(RECEIVER, *tosend);

    if(tosend->seqnum==win_base){
        pns->stopTimer(SENDER, 0);
        pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		std::cout << std::endl;
    }
    delete tosend;
    
    return true;

}

void GBN::Sender::receive(Packet &ackPkt){
	int dup = 0;
    int checksum=pUtils->calculateCheckSum(ackPkt);
    if(checksum==ackPkt.checksum){
        //fast resend
        if(ackPkt.acknum!=win_base%seq_max){
			if (dup_ack != ackPkt.acknum) {
				pUtils->printPacket("GBN Sender received ACK of wrong order but will work", ackPkt);
				missed_ack_num = 0;
				dup_ack = ackPkt.acknum;
				dup = 0;
			}
			else {
				dup = 1;
				missed_ack_num++;
				if (fast_resend_en) {
					std::cout << "GBN Sender received " << missed_ack_num << " duplicate ACK" <<" ,acknum="<<ackPkt.acknum <<std::endl<<std::endl;
				}
				
			}
			
            
            if(missed_ack_num==3){
                if(fast_resend_en && !sender_window.empty()){
                    pns->stopTimer(SENDER, 0);
					pUtils->printPacket("GBN Sender processing fast resend", sender_window.front());
                    pns->sendToNetworkLayer(RECEIVER, sender_window.front());
                    pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
                }
                missed_ack_num=0;
            }
			
        }
        else{
			dup_ack = ackPkt.acknum;
			pUtils->printPacket("GBN Sender received right ACK", ackPkt);
            missed_ack_num=0;
        }

		if (!dup) {
		win_base=(ackPkt.acknum+1)%GBN::seq_max;
		std::cout << "GBN Sender window before moving:" << std::endl;
        print_win();
		//std::cout << sender_window.front().seqnum << std::endl;
        while(!sender_window.empty()){
			if (sender_window.front().seqnum != win_base) {
				sender_window.pop();
			}
			else {
				break;
			}
            
        }
		std::cout << "GBN Sender window after moving:" << std::endl;
        print_win();
        pns->stopTimer(SENDER, 0);
        if(!sender_window.empty()) pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		std::cout << std::endl;
		}
		else {
			std::cout << "GBN Sender received duplicate ACK, so window won't move" << std::endl;
			pUtils->printPacket("", ackPkt);
			std::cout << std::endl;
		}

        
    }
    return;
}

void GBN::Sender::timeoutHandler(int seqNum){

    int count=sender_window.size();
    while(count){
		pUtils->printPacket("GBN Sender resend non-ACKed packets due to timeout",sender_window.front());
        pns->sendToNetworkLayer(RECEIVER, sender_window.front());
        
        sender_window.push(sender_window.front());
        sender_window.pop();
        count--;
    }

    pns->stopTimer(SENDER, 0);
    pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	std::cout << std::endl;

}

bool GBN::Sender::getWaitingState(){
    return sender_window.size()==win_size;
}

void GBN::Sender::print_win(){
    std::cout<<"Print GBN sender window: "<<std::endl;
    if(sender_window.empty()) std::cout<<"GBN sender window is empty"<<std::endl;
    else{
        int count=sender_window.size();
		//std::cout << count << std::endl;
        while(count){
        pUtils->printPacket("",sender_window.front());
        sender_window.push(sender_window.front());
        sender_window.pop();
        count--;
    }
		//std::cout << "fuck" << std::endl;
    }
}