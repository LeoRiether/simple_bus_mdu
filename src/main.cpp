///////////////////////////////////////////////////////////////////////////
//                                                                       //
//                    Mais informações no README.md                      //
//             https://github.com/LeoRiether/simple_bus_mdu/             //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <mdu.h>
#include <definitions.h>
#include <mdu_direct_master.h>
#include <mdu_master.h>
#include <mips_v0.h>
#include <systemc.h>
#include <mem_mips.h>

#include "simple_bus.h"
#include "simple_bus_arbiter.h"

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clock", 1, SC_NS);
    simple_bus bus("bus");
    simple_bus_arbiter arbiter("arbiter");
    MDU mdu("MDU", 4, false);

    bus.clock(clock);
    mdu.clock(clock);

    // master.bus(bus);
    bus.slave_port(mdu);
    bus.arbiter_port(arbiter);

    mdu_master master("mdu_master");  // pode ser também o mdu_direct_master
    master.bus(bus);

    // Pode descomentar essa parte, já está funcionando!
    // Mas lembre-se de comentar o mdu_master
    //
    // sc_signal<bool> reset("reset");
    // mips_v0 mips("mips", false);
    // mips.clk(clock);
    // mips.bus(bus);
    // mips.reset(reset);
    //
    // mem_mips mem("mem", MEM_SIZE, 2);
    // mem.clk(clock);
    // bus.slave_port(mem);
    //
    // // Settings > Memory Configuration > Compact, Text at Address 0
    // // decbin.asm
    // mem.load_mem("MipsCpp/Testes/code.bin", 0);
    // mem.load_mem("MipsCpp/Testes/data.bin", DATA_SEGMENT_START);

    sc_start();

    // mips.dump_breg();

    return 0;
}
