#pragma once

#include <systemc.h>

#include "simple_bus_slave_if.h"
#include "simple_bus_types.h"

struct MDU : public simple_bus_slave_if, public sc_module {
    sc_in<bool> clock;
    int wait_count;
    bool verbose;
    int opA, opB, mLow, mHigh, div, mod;

    // Constantes
    enum Const {
        StartAddr = 256,
        OpA = 256,
        OpB = 260,
        MLow = 264,
        MHigh = 268,
        Div = 272,
        Mod = 276,
        EndAddr = 319,
        WaitStates = 5,
    };

    // Retorna uma referência para o inteiro no endereço `addr`
    // Exemplo: `int_at_addr(OpA) = 5;` é o mesmo que `opA = 5;`
    int& int_at_addr(unsigned int addr) {
        if (addr == OpA) return opA;
        if (addr == OpB) return opB;
        if (addr == MLow) return mLow;
        if (addr == MHigh) return mHigh;
        if (addr == Div) return div;
        return mod;
    }

    SC_HAS_PROCESS(MDU);
    MDU(sc_module_name name_, bool verbose_ = false)
        : sc_module(name_), clock("clock"), wait_count(-1), verbose(verbose_) {
        SC_METHOD(update_wait_count);
        dont_initialize();
        sensitive << clock.pos();
    }

    void update_wait_count() {
        if (wait_count >= 0)
            wait_count--;

        if (verbose)
            cout << "@" << sc_time_stamp()
                 << ": MDU_wait_count == " << wait_count << endl;
    }

    // Calcula os resultados da multiplicação e da divisão
    void calculate() {
        mLow = (1ll * opA * opB) & 0xFFFFFFFF;
        mHigh = (1ll * opA * opB) >> 32ll;
        div = opA / opB;
        mod = opA % opB;
    }

    ///////////////////////////////////
    //        Slave Interface        //
    ///////////////////////////////////
    simple_bus_status read(int *data, unsigned int address) {
        if (wait_count < 0) {
            wait_count = WaitStates;
            return SIMPLE_BUS_WAIT;
        }

        if (wait_count == 0) {
            *data = int_at_addr(address);
            return SIMPLE_BUS_OK;
        }

        return SIMPLE_BUS_WAIT;
    }

    simple_bus_status write(int *data, unsigned int address) {
        if (wait_count < 0) {
            wait_count = WaitStates;
            return SIMPLE_BUS_WAIT;
        }

        if (wait_count == 0) {
            int_at_addr(address) = *data;
            calculate();
            wait_count = WaitStates;  // o resultado só fica pronto daqui a
                                      // `WaitStates` estados
            return SIMPLE_BUS_OK;
        }

        return SIMPLE_BUS_WAIT;
    }

    //////////////////////////////////////////////
    //        Direct BUS/Slave Interface        //
    //////////////////////////////////////////////
    bool direct_read(int *data, unsigned int address) {
        *data = int_at_addr(address);
        return true;
    }

    bool direct_write(int *data, unsigned int address) {
        int_at_addr(address) = *data;
        calculate();
        return true;
    }

    unsigned int start_address() const {
        return (unsigned int)StartAddr;
    }

    unsigned int end_address() const {
        return (unsigned int)EndAddr;
    }
};
