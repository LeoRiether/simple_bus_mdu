#pragma once
#include <systemc.h>

#include "mdu.h"
#include "simple_bus.h"
#include "simple_bus_direct_if.h"

SC_MODULE(mdu_direct_master) {
    sc_port<simple_bus_direct_if> bus;

    SC_CTOR(mdu_direct_master) : bus("bus") {
        SC_THREAD(main_action);
    }

    int read(MDU::Const addr) {
        int data;
        bus->direct_read(&data, addr);
        return data;
    }

    void do_testcase(int opA, int opB) {
        bus->direct_write(&opA, MDU::OpA);
        bus->direct_write(&opB, MDU::OpB);

        cout << "opA = " << opA << ", opB = " << opB << '\n';
        cout << "mHigh = " << read(MDU::MHigh) << ", mLow = " << read(MDU::MLow)
             << '\n';
        cout << "m[high:low] = "
             << ((long long)read(MDU::MHigh) << 32ll) +
                    ((unsigned int)read(MDU::MLow))
             << '\n';
        cout << "div = " << read(MDU::Div) << ", mod = " << read(MDU::Mod)
             << '\n';
        cout << "----------------------------------------" << endl;
    }

    void main_action() {
        do_testcase(2, 3);
        do_testcase(10, 100);
        do_testcase(-1, 123);
        // do_testcase(3, 0);
        do_testcase(90, 7);
        do_testcase(1'000'000'000, 1'000'000'000);
        do_testcase(500'000'000, -400'000'000);

        sc_stop();
    }
};
