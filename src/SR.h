

#include <deque>
#include "RdtReceiver.h"
#include "RdtSender.h"

namespace SR{
    const int init_seq=0;
    const int seq_max=8;
    const int init_win_size=SR::seq_max>>1;

    class Sender: public RdtSender{
        public:

        int win_size=SR::init_win_size;
        int win_base;
        int last_sent_seq;
        std::deque<Packet*> sender_window;

        Sender(int size=SR::init_win_size);
        virtual ~Sender();

        virtual bool send(Message &message);
	    virtual void receive(Packet &ackPkt);
	    virtual void timeoutHandler(int seqNum);
	    virtual bool getWaitingState();
        virtual void print_win();

    };

    class Receiver: public RdtReceiver{
        public:
        int win_size=SR::init_win_size;
        int win_base;
        Packet last_ack;
        std::deque<Packet*> receiver_window;

        Receiver(int size=SR::init_win_size);
        virtual ~Receiver();

        virtual void receive(Packet &packet);
        virtual void print_win();

    };
}