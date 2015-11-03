#ifndef _CLIENT_LP_H
#define _CLIENT_LP_H

extern tw_lptype cli_lp;

enum cli_event
{
    CLI_KICKOFF,
    CLI_ACK,
    CLI_RECV
};

typedef struct cli_state cli_state;
struct cli_state
{
    int recv_cnt;
};

typedef struct cli_msg cli_msg;
struct cli_msg
{
    enum cli_event cli_event_type;
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

