#ifndef _SERVER_LP_H
#define _SERVER_LP_H

extern tw_lptype srv_lp;

enum srv_event
{
    SRV_ACK
};

typedef struct srv_state srv_state;
struct srv_state
{
    int ack_cnt;
};

typedef struct srv_msg srv_msg;
struct srv_msg
{
    enum srv_event srv_event_type;
    tw_lpid src;
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
*/

