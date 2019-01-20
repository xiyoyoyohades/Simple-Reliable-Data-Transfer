
#include <queue>
#include "RdtReceiver.h"
#include "RdtSender.h"


namespace GBN{
    const int init_seq=0;
    const int seq_max=8;
    const int init_win_size=GBN::seq_max>>1;

    class Sender: public RdtSender{

        public:

        bool fast_resend_en=false;
        int win_size=GBN::init_win_size;
        int win_base;
        int missed_ack_num=0;
		int dup_ack = -1;

        std::queue<Packet> sender_window;

        Sender(bool fr_en=false, int win_size=GBN::init_win_size);
        virtual ~Sender();

        virtual bool send(Message &message);
        virtual void receive(Packet &ackPkt);
        virtual void timeoutHandler(int seqNum);
        virtual bool getWaitingState();
        virtual void print_win();

    };

    class Receiver: public RdtReceiver{
        public:

        int win_size=GBN::init_win_size;
        int seq_expecting=0;
        Packet last_ack;

        Receiver(int win_size=GBN::init_win_size);
        virtual ~Receiver();
        virtual void receive(Packet& packet);

    };
};