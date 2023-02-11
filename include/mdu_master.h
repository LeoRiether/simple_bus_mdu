#pragma once
#include <systemc.h>

#include "mdu.h"
#include "simple_bus.h"
#include "simple_bus_direct_if.h"

SC_MODULE(mdu_master) {
    sc_port<simple_bus_blocking_if> bus;

    SC_CTOR(mdu_master) : bus("bus") {
        SC_THREAD(main_action);
    }

    void write(int* data, unsigned int addr) {
        bus->burst_write(MDU::Priority, data, addr);
    }

    int read(unsigned int addr) {
        int data;
        bus->burst_read(MDU::Priority, &data, addr);
        return data;
    }

    void do_testcase(int opA, int opB, MDU::Const opcode) {
        cout << "@" << sc_time_stamp() << ": starting testcase " << opA << " "
             << opB << endl;

        // Escrevemos os operandos nos registradores opA e opB
        write(&opA, MDU::OpA);
        write(&opB, MDU::OpB);

        const char* opcode_string =   opcode == MDU::MHigh ? "MHigh"
                                    : opcode == MDU::MLow  ? "MLow"
                                    : opcode == MDU::MHigh ? "MHigh"
                                    : opcode == MDU::Div   ? "Div"
                                    : opcode == MDU::Mod   ? "Mod"
                                                           : "<UnknownMDUAddr>";
        cout << "@" << sc_time_stamp() << ": opA = " << opA << ", opB = " << opB
             << ", addr = " << opcode_string << '\n';

        // Executamos a operação e esperamos o resultado (de forma blocante) 
        int result = read(opcode);
        cout << "@" << sc_time_stamp() << ": result = " << result << endl;
    }

    void main_action() {
        do_testcase(2, 3, MDU::MLow);
        do_testcase(10, 100, MDU::MLow);
        do_testcase(-1, 123, MDU::MLow);
        // do_testcase(3, 0);
        do_testcase(90, 7, MDU::Div);
        do_testcase(90, 7, MDU::Mod);
        do_testcase(1'000'000'000, 1'000'000'000, MDU::MHigh);
        do_testcase(1'000'000'000, 1'000'000'000, MDU::MLow);
        do_testcase(500'000'000, -400'000'000, MDU::MHigh);
        do_testcase(500'000'000, -400'000'000, MDU::MLow);

        sc_stop();
    }
};
