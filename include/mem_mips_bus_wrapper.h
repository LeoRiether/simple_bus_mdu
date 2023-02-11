#pragma once
#include <definitions.h>
#include <mem_if.h>
#include <mem_mips.h>
#include <systemc.h>

#include "simple_bus.h"
#include "simple_bus_types.h"

// Facilita a comunicação com a mem_mips por meio do simple_bus
struct mem_mips_bus_wrapper : public sc_module, public mem_if {
    sc_port<simple_bus_blocking_if> bus;

    SC_CTOR(mem_mips_bus_wrapper) : bus("bus") {
    }

    int load(int op_type, int addr) {
        bus->burst_write(mem_mips::Priority, &op_type, mem_mips::OpTypeReg);
        bus->burst_write(mem_mips::Priority, &addr, mem_mips::AddrReg);

        int data;
        bus->burst_read(mem_mips::Priority, &data, mem_mips::MemAddr);
        return data;
    }

    void store(int op_type, int addr, int data) {
        bus->burst_write(mem_mips::Priority, &op_type, mem_mips::OpTypeReg);
        bus->burst_write(mem_mips::Priority, &addr, mem_mips::AddrReg);
        bus->burst_write(mem_mips::Priority, &data, mem_mips::MemAddr);
    }

    int8_t lb(const uint32_t addr, uint16_t kte) {
        return load(mem_mips::Byte, addr + kte);
    }

    int16_t lh(const uint32_t addr, uint16_t kte) {
        return load(mem_mips::Half, addr + kte);
    }

    int32_t lw(const uint32_t addr, uint16_t kte) {
        return load(mem_mips::Word, addr + kte);
    }

    void sb(const uint32_t addr, sc_uint<5> kte, sc_int<8> dado) {
        store(mem_mips::Byte, addr + kte, dado);
    }

    void sh(const uint32_t addr, sc_uint<5> kte, sc_int<16> dado) {
        store(mem_mips::Half, addr + kte, dado);
    }

    void sw(const uint32_t addr, sc_uint<5> kte, sc_int<32> dado) {
        store(mem_mips::Word, addr + kte, dado);
    }
};
