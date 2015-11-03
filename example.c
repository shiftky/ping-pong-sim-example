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

int net_id = 0;
int payload_sz = 0;
int offset = 2;
int num_recvs = 0;
int num_servers = 0;
tw_lpid cli_head_lpid;

static char *group_name = "SERVERS";
static char *param_group_nm = "server_pings";
static char *num_recvs_key = "num_reqs";
static char *payload_sz_key = "payload_sz";

extern const tw_lptype* srv_get_lp_type();
extern const tw_lptype* cli_get_lp_type();
static void add_lp_type();

static tw_stime ns_to_s(tw_stime ns);
static tw_stime s_to_ns(tw_stime ns);

static char conf_file_name[256] = {0};
const tw_optdef app_opt [] =
{
    TWOPT_GROUP("Model net test case" ),
    TWOPT_CHAR("codes-config", conf_file_name, "name of codes configuration file"),
    TWOPT_END()
};

int main(int argc, char **argv)
{
    int nprocs, rank;
    int num_nets, *net_ids;

    g_tw_ts_end = s_to_ns(60*60*24*365); 

    tw_opt_add(app_opt);
    tw_init(&argc, &argv);

    if (!conf_file_name[0]) {
        fprintf(stderr, "Expected \"codes-config\" option, please see --help.\n");
        MPI_Finalize();
        return 1;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (configuration_load(conf_file_name, MPI_COMM_WORLD, &config)) {
        fprintf(stderr, "Error loading config file %s.\n", conf_file_name);
        MPI_Finalize();
        return 1;
    }

    model_net_register();
    add_lp_type();
    codes_mapping_setup();

    net_ids = model_net_configure(&num_nets);
    assert(num_nets==1);
    net_id = *net_ids;
    free(net_ids);

    if ( net_id != SIMPLENET ) {
        printf("\n The test works with simple-net configuration only! ");
        MPI_Finalize();
        return 0;
    }

    num_servers = codes_mapping_get_lp_count(group_name, 0, "server", NULL, 1);
    codes_mapping_get_lp_id("CLIENTS", "client", NULL, 0, 0, 0, &cli_head_lpid);

    configuration_get_value_int(&config, param_group_nm, num_recvs_key, NULL, &num_recvs);
    configuration_get_value_int(&config, param_group_nm, payload_sz_key, NULL, &payload_sz);

    tw_run();

    model_net_report_stats(net_id);

    tw_end();

    return 0;
}

const tw_lptype* srv_get_lp_type()
{
    return &srv_lp;
}

const tw_lptype* cli_get_lp_type()
{
    return &cli_lp;
}

static void add_lp_type()
{
    lp_type_register("server", srv_get_lp_type());
    lp_type_register("client", cli_get_lp_type());
}

static tw_stime ns_to_s(tw_stime ns)
{
    return (ns / (1000.0 * 1000.0 * 1000.0));
}

static tw_stime s_to_ns(tw_stime ns)
{
    return (ns * (1000.0 * 1000.0 * 1000.0));
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
*/
