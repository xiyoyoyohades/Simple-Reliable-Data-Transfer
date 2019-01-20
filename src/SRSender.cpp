#include "SR.h"
#include "Global.h"
#include "inc.h"
using namespace SR;


SR::Sender::Sender(int size){
    win_size=size;
    win_base=SR::init_seq;
    last_sent_seq=-1;
    sender_window=std::deque<Packet*>(win_size,0);
}

SR::Sender::~Sender(){
    int count=sender_window.size();
    for(int i=0;i<count;i++){
        if(sender_window[i]!=0) delete sender_window[i];
    }
}

bool SR::Sender::send(Message &message){
    if((last_sent_seq-win_base+1+SR::seq_max)%SR::seq_max==win_size) return false;
    last_sent_seq=(last_sent_seq+1)%SR::seq_max;
    Packet* tosend=new Packet;
    tosend->seqnum=last_sent_seq;
    std::memcpy(tosend->payload, message.data, sizeof(message.data));
    tosend->checksum=pUtils->calculateCheckSum(*tosend);
    sender_window[(last_sent_seq-win_base+SR::seq_max)%SR::seq_max]=tosend;

    pUtils->printPacket("SR Sender sending new packet",*tosend);
    pns->sendToNetworkLayer(RECEIVER, *tosend);
    pns->startTimer(SENDER, Configuration::TIME_OUT, tosend->seqnum);
	return true;

}

void SR::Sender::receive(Packet &ackPkt){
    if(pUtils->calculateCheckSum(ackPkt)!=ackPkt.checksum){
        pUtils->printPacket("SR Sender received damaged ACK",ackPkt);
		std::cout << std::endl;
        return;
    }
    int index=(ackPkt.acknum-win_base+SR::seq_max)%SR::seq_max;
    if(index>=win_size||((ackPkt.acknum-win_base+SR::seq_max)%SR::seq_max>(last_sent_seq-win_base+SR::seq_max)%SR::seq_max)){
        pUtils->printPacket("SR Sender received ACK out of range",ackPkt);
		std::cout << std::endl;
        return;
    } 

	pUtils->printPacket("SR Sender received right ACK", ackPkt);
    pns->stopTimer(SENDER, ackPkt.acknum);
    Packet* to_ack=sender_window[index];
    sender_window[index]=0;
    delete to_ack;

	if (index == 0) {
		std::cout << "SR Sender window move" << std::endl;
		std::cout << "Before moving" << std::endl;
		print_win();
		int max_num=(last_sent_seq-win_base+1+SR::seq_max)%SR::seq_max;
		while(max_num && sender_window.front()==0){
			sender_window.push_back(0);
			sender_window.pop_front();
			win_base=(win_base+1)%SR::seq_max;
			max_num--;
		}
		std::cout << "After moving" << std::endl;
		print_win();
	}
	else {
		std::cout << "ACK number is not window base, so sender window won't move" << std::endl<<std::endl;
	}
	

}

void SR::Sender::timeoutHandler(int seqNum){
    Packet* to_resend=sender_window[(seqNum-win_base+SR::seq_max)%SR::seq_max];
    std::cout<<"SR Sender resending packet "<<seqNum<<" due to time-out"<<std::endl;
    pns->sendToNetworkLayer(RECEIVER, *to_resend);
    pns->stopTimer(SENDER, seqNum);
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	std::cout << std::endl;

}

bool SR::Sender::getWaitingState(){
    return (last_sent_seq-win_base+1+SR::seq_max)%SR::seq_max==win_size;
}

void SR::Sender::print_win(){
    std::cout<<"Print SR sender window: "<<std::endl;
    if((last_sent_seq - win_base + 1+ SR::seq_max) % SR::seq_max <=0) std::cout<<"SR sender window is empty"<<std::endl;
    else{
		int count = 0;
        for(int i=0;i<(last_sent_seq-win_base+1+ SR::seq_max)%SR::seq_max;i++){
            if(sender_window[i]!=0){
                std::cout<< (win_base + i) % SR::seq_max <<". "<<"Packet unchecked ";
                pUtils->printPacket("",*sender_window[i]);
            }else{
                std::cout<< (win_base + i) % SR::seq_max <<". "<<"Packet checked"<<std::endl;
            }
			count++;
        }
        
        
    }
	std::cout << std::endl;
}