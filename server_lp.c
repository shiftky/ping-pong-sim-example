#include <string.h>
#include <assert.h>
#include <ross.h>

#include "codes/codes.h"
#include "codes/codes_mapping.h"
#include "codes/configuration.h"
#include "codes/model-net.h"

#include "example.h"
#include "server_lp.h"
#include "client_lp.h"

static void srv_init(srv_state * ns, tw_lp * lp);
static void srv_event(srv_state * ns, tw_bf * b, srv_msg * m, tw_lp * lp);
static void srv_rev_event(srv_state * ns, tw_bf * b, srv_msg * m, tw_lp * lp);
static void srv_finalize(srv_state * ns, tw_lp * lp);

static void handle_srv_ack_event(srv_state * ns, tw_bf * b, srv_msg * m, tw_lp * lp);
static void handle_srv_ack_rev_event(srv_state * ns, tw_bf * b, srv_msg * m, tw_lp * lp);

tw_lptype srv_lp = {
    (init_f) srv_init,
    (pre_run_f) NULL,
    (event_f) srv_event,
    (revent_f) srv_rev_event,
    (final_f)  srv_finalize,
    (map_f) codes_mapping,
    sizeof(srv_state),
};

static void srv_init(srv_state * ns, tw_lp * lp)
{
    ns->ack_cnt = 0;
}

static void srv_event(srv_state * ns, tw_bf * b, srv_msg * m, tw_lp * lp)
{
   switch (m->srv_event_type)
   {
        case SRV_ACK:
            handle_srv_ack_event(ns, b, m, lp);
            break;
        default:
            printf("\n Invalid message type %d ", m->srv_event_type);
            assert(0);
        break;
    }
}

static void srv_rev_event(srv_state * ns, tw_bf * b, srv_msg * m, tw_lp * lp)
{
    switch (m->srv_event_type)
    {
        case SRV_ACK:
            handle_srv_ack_rev_event(ns, b, m, lp);
            break;
        default:
            assert(0);
            break;
    }
}

static void srv_finalize(srv_state * ns, tw_lp * lp)
{
    printf("[server %llu] ack_cnt = %d\n", lp->gid, ns->ack_cnt);
}

static void handle_srv_ack_event(srv_state * ns, tw_bf * b, srv_msg * m, tw_lp * lp)
{
    cli_msg cli_m;

    cli_m.cli_event_type = CLI_RECV;
    cli_m.src = lp->gid;

    model_net_event(net_id, "test", m->src, payload_sz, 0.0,
            sizeof(cli_msg), (const void *)&cli_m,
            0, NULL, lp);

    ns->ack_cnt++;
}

static void handle_srv_ack_rev_event(srv_state * ns, tw_bf * b, srv_msg * m, tw_lp * lp)
{
    ns->ack_cnt--;
    model_net_event_rc(net_id, lp, payload_sz);
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
*/
