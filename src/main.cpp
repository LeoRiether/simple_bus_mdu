#include <systemc.h>
#include "simple_bus.h"
#include "simple_bus_arbiter.h"
#include "mdu.h"
#include "mdu_direct_master.h"
#include "mdu_master.h"

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clock", 1, SC_NS);
    simple_bus bus("bus");
    simple_bus_arbiter arbiter("arbiter");
    MDU mdu("MDU", 3, true);
    mdu_master master("mdu_master"); // pode ser também o mdu_direct_master

    bus.clock(clock);
    mdu.clock(clock);

    master.bus(bus);
    bus.slave_port(mdu);
    bus.arbiter_port(arbiter);

    sc_start();

    return 0;
}
