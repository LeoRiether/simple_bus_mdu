#pragma once

#include <systemc.h>

#include "simple_bus_slave_if.h"
#include "simple_bus_types.h"

struct MDU : public simple_bus_slave_if, public sc_module {
    sc_in<bool> clock;

    // Registradores de entrada
    int opA, opB;

    int wait_states; // número de ciclos que uma operação demora para terminar
    int wait_left;   // número de ciclos restantes até a operação atual terminar 

    bool verbose;

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
    };

    SC_HAS_PROCESS(MDU);
    MDU(sc_module_name name_, int wait_states_ = 5, bool verbose_ = false)
        : sc_module(name_),
          clock("clock"),
          wait_states(wait_states_),
          wait_left(-1),
          verbose(verbose_) {
        SC_METHOD(update_wait_left);
        dont_initialize();
        sensitive << clock.pos();
    }

    void update_wait_left() {
        if (wait_left >= 0)
            wait_left--;

        if (verbose)
            cout << "@" << sc_time_stamp()
                 << ": MDU_wait_left == " << wait_left << endl;
    }

    // Calcula os resultados da multiplicação e da divisão
    int calculate(unsigned int address) {
        switch (address) {
            case MLow:  return (1ll * opA * opB) & 0xFFFFFFFF;
            case MHigh: return (1ll * opA * opB) >> 32ll;
            case Div:   return opA / opB;
            case Mod:   return opA % opB;
            default:    return 0;
        }
    }

    ///////////////////////////////////
    //        Slave Interface        //
    ///////////////////////////////////
    simple_bus_status read(int *data, unsigned int address) {
        // O cálculo ainda está sendo feito...
        if (wait_left > 0)
            return SIMPLE_BUS_WAIT;

        // O cálculo acabou de ser feito
        if (wait_left == 0) {
            *data = calculate(address);
            return SIMPLE_BUS_OK;
        }

        // O cálculo deve começar agora
        wait_left = wait_states;
        return SIMPLE_BUS_WAIT;
    }

    simple_bus_status write(int *data, unsigned int address) {
        // Há um cálculo em andamento
        if (wait_left >= 0)
            return SIMPLE_BUS_WAIT;

        // Não há cálculo em andamento e podemos escrever nos registradores
        if (address == OpA)
            opA = *data;
        else if (address == OpB)
            opB = *data;

        return SIMPLE_BUS_OK;
    }

    //////////////////////////////////////////////
    //        Direct BUS/Slave Interface        //
    //////////////////////////////////////////////
    bool direct_read(int *data, unsigned int address) {
        *data = calculate(address);
        return true;
    }

    bool direct_write(int *data, unsigned int address) {
        if (address == OpA)
            opA = *data;
        else if (address == OpB)
            opB = *data;
        return true;
    }

    unsigned int start_address() const {
        return (unsigned int)StartAddr;
    }

    unsigned int end_address() const {
        return (unsigned int)EndAddr;
    }
};
