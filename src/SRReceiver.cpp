#include "SR.h"
#include "Global.h"
#include "inc.h"
using namespace SR;

SR::Receiver::Receiver(int size){
    win_size=size;
    win_base=SR::init_seq;
    receiver_window=std::deque<Packet*>(win_size,0);
    last_ack.acknum=-1;
    last_ack.checksum=pUtils->calculateCheckSum(last_ack);
}

SR::Receiver::~Receiver(){
    int count=receiver_window.size();
    for(int i=0;i<count;i++){
        if(receiver_window[i]!=0) delete receiver_window[i];
    }
}

void SR::Receiver::receive(Packet &packet){
    if(pUtils->calculateCheckSum(packet)!=packet.checksum){
        pUtils->printPacket("SR Receivcer received damaged packet", packet);
        pUtils->printPacket("SR Receiver resending ACK packet", last_ack);

        pns->sendToNetworkLayer(SENDER, last_ack);
		std::cout << std::endl;
        return;
    }
    int index=(packet.seqnum-win_base+SR::seq_max)%SR::seq_max;
    if(index==0){
		pUtils->printPacket("SR Receivcer received packet, make window move", packet);
        receiver_window[0]=new Packet;
        *receiver_window[0]=packet;
        Message to_deliver;
        std::cout<<"SR Receiver window move"<<std::endl;
        std::cout<<"Before moving"<<std::endl;
        print_win();
        while(receiver_window.front()!=0){
            std::memcpy(to_deliver.data, receiver_window.front()->payload, sizeof(packet.payload));
            pns->delivertoAppLayer(SENDER, to_deliver);
            delete receiver_window.front();
            win_base=(win_base+1+SR::seq_max)%SR::seq_max;
            receiver_window.push_back(0);
            receiver_window.pop_front();
        }
        std::cout<<"After moving"<<std::endl;
        print_win();
    }else if(index<win_size){
        pUtils->printPacket("SR Receivcer received packet", packet);
        receiver_window[index]=new Packet;
        *receiver_window[index]=packet;
    }else {
        pUtils->printPacket("SR Receivcer received packet of last window", packet);
    }

    last_ack.acknum=packet.seqnum;
    last_ack.checksum=pUtils->calculateCheckSum(last_ack);
    //std::cout<<"SR Receiver sending ACK"<<std::endl;
	pUtils->printPacket("SR Receiver sending ACK", last_ack);
    pns->sendToNetworkLayer(SENDER, last_ack);
	std::cout << std::endl;

}

void SR::Receiver::print_win(){
    std::cout<<"Print SR receiver window: "<<std::endl;
    if(receiver_window.empty()) std::cout<<"SR receiver window is empty"<<std::endl;
    else{
		int count = 0;
        for(int i=0;i<win_size;i++){
            if(receiver_window[i]!=0){
                std::cout<<(win_base+i)%SR::seq_max<<". "<<"Packet received ";
                pUtils->printPacket("",*receiver_window[i]);
            }else{
                std::cout<< (win_base + i) % SR::seq_max <<". "<<"Packet not received"<<std::endl;
            }
			count++;
        }
        
        
    }
	std::cout << std::endl;
}