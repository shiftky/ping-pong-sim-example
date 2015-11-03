#include <stdio.h>
#include <ross.h>

#include "codes/codes.h"
#include "codes/codes_mapping.h"
#include "codes/configuration.h"
#include "codes/model-net.h"

#include "example.h"
#include "server_lp.h"
#include "client_lp.h"

static void cli_init(cli_state *ns, tw_lp *lp);
static void cli_event(cli_state *ns, tw_bf *b, cli_msg *m, tw_lp *lp);
static void cli_rev_event(cli_state *ns, tw_bf *b, cli_msg *m, tw_lp *lp);
static void cli_finalize(cli_state *ns, tw_lp *lp);

static void handle_cli_kickoff_event(cli_state *ns, tw_bf *b, cli_msg *m, tw_lp *lp);
static void handle_cli_kickoff_rev_event(cli_state *ns, tw_bf *b, cli_msg *m, tw_lp *lp);
static void handle_cli_recv_event(cli_state *ns, tw_bf *b, cli_msg *m, tw_lp *lp);
static void handle_cli_recv_rev_event(cli_state *ns, tw_bf *b, cli_msg *m, tw_lp *lp);

tw_lptype cli_lp = {
    (init_f) cli_init,
    (pre_run_f) NULL,
    (event_f) cli_event,
    (revent_f) cli_rev_event,
    (final_f)  cli_finalize,
    (map_f) codes_mapping,
    sizeof(cli_state),
};

static void cli_init(cli_state * ns, tw_lp * lp)
{
    tw_event *e;
    cli_msg *m;
    tw_stime kickoff_time;

    memset(ns, 0, sizeof(*ns));
    ns->recv_cnt = 0;

    kickoff_time = g_tw_lookahead + tw_rand_unif(lp->rng);

    e = codes_event_new(lp->gid, kickoff_time, lp);
    m = tw_event_data(e);
    m->cli_event_type = CLI_KICKOFF;
    tw_event_send(e);
}

static void cli_finalize(cli_state * ns, tw_lp * lp)
{
    printf("[client %llu] cnt = %d\n", lp->gid, ns->recv_cnt);
}

static void cli_event(cli_state * ns, tw_bf * b, cli_msg * m, tw_lp * lp)
{
   switch (m->cli_event_type)
   {
        case CLI_KICKOFF:
            handle_cli_kickoff_event(ns, b, m, lp);
            break;
        case CLI_RECV:
            handle_cli_recv_event(ns, b, m, lp);
            break;
        default:
            printf("\n Invalid message type %d ", m->cli_event_type);
            assert(0);
            break;
    }
}

static void cli_rev_event(cli_state * ns, tw_bf * b, cli_msg * m, tw_lp * lp)
{
    switch (m->cli_event_type)
    {
        case CLI_KICKOFF:
            handle_cli_kickoff_rev_event(ns, b, m, lp);
            break;
        case CLI_RECV:
            handle_cli_recv_rev_event(ns, b, m, lp);
            break;
        default:
            assert(0);
            break;
    }
}

static void handle_cli_kickoff_event(cli_state * ns, tw_bf * b, cli_msg * m, tw_lp * lp)
{
    int index;
    tw_lpid dst_id;
    srv_msg srv_m;

    index = (lp->gid - cli_head_lpid) / offset;
    dst_id = codes_mapping_get_lpid_from_relative(index, "SERVERS", "server", NULL, 0);

    srv_m.srv_event_type = SRV_ACK;
    srv_m.src = lp->gid;

    model_net_event(net_id, "test", dst_id, payload_sz, 0.0,
            sizeof(srv_msg), (const void *)&srv_m,
            0, NULL, lp);
}

static void handle_cli_kickoff_rev_event(cli_state * ns, tw_bf * b, cli_msg * m, tw_lp * lp)
{
    model_net_event_rc(net_id, lp, payload_sz);
}

static void handle_cli_recv_event(cli_state * ns, tw_bf * b, cli_msg * m, tw_lp * lp)
{
    int index;
    tw_lpid dst_id;

    ns->recv_cnt++;

    if ( ns->recv_cnt < num_recvs ) {
        b->c0 = 1;

        tw_event *e = codes_event_new(lp->gid, codes_local_latency(lp), lp);
        cli_msg *cli_m = tw_event_data(e);
        cli_m->cli_event_type = CLI_KICKOFF;
        tw_event_send(e);
    }
}

static void handle_cli_recv_rev_event(cli_state * ns, tw_bf * b, cli_msg * m, tw_lp * lp)
{
    if ( b->c0 ) {
        codes_local_latency_reverse(lp);
    }

    ns->recv_cnt--;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
*/
