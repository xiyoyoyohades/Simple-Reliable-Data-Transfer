#include "GBN.h"
#include "Global.h"
#include "inc.h"
using namespace GBN;


GBN::Receiver::Receiver(int win_size):
    seq_expecting(init_seq),
    win_size(win_size)
    {
        last_ack.acknum=seq_expecting-1;
        last_ack.checksum=pUtils->calculateCheckSum(last_ack);
    }

GBN::Receiver::~Receiver(){

}

void GBN::Receiver::receive(Packet& packet){
    int checksum=pUtils->calculateCheckSum(packet);
    if((checksum==packet.checksum)&&(packet.seqnum==seq_expecting)){
        pUtils->printPacket("GBN Receiver received right packet",packet);
        Message* msg=new Message;
        std::memcpy(msg->data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER, *msg);

        last_ack.acknum=seq_expecting;
        last_ack.checksum=pUtils->calculateCheckSum(last_ack);
        pUtils->printPacket("GBN Receiver sending ACK",last_ack);
        pns->sendToNetworkLayer(SENDER, last_ack);
		std::cout << std::endl;

        seq_expecting=(seq_expecting+1)%seq_max;
        delete msg;

    }
    else{
        if(checksum!=packet.checksum){
            pUtils->printPacket("GBN Receiver received damaged packet",packet);
        }else if(packet.seqnum!=seq_expecting){
            pUtils->printPacket("GBN Receiver received packet of wrong order",packet);
            std::cout<<"GBN Receiver is expecting packet of SeqNum: "<<seq_expecting<<std::endl;
        }else{
            throw std::runtime_error("unknow problem occured");
        }

        pUtils->printPacket("GBN Receiver resending last ACK",last_ack);
        pns->sendToNetworkLayer(SENDER, last_ack);
		std::cout << std::endl;
    }

}