#pragma once
#include <systemc.h>

struct mem_if : public sc_interface {
    virtual int8_t lb(const uint32_t addr, uint16_t kte) = 0;
    virtual int16_t lh(const uint32_t addr, uint16_t kte) = 0;
    virtual int32_t lw(const uint32_t addr, uint16_t kte) = 0;
    virtual uint8_t lbu(const uint32_t addr, uint16_t kte) = 0;
    virtual uint16_t lhu(const uint32_t addr, uint16_t kte) = 0;
    virtual void sb(const uint32_t addr, sc_uint<5> kte, sc_int<8> dado) = 0;
    virtual void sh(const uint32_t addr, sc_uint<5> kte, sc_int<16> dado) = 0;
    virtual void sw(const uint32_t addr, sc_uint<5> kte, sc_int<32> dado) = 0;
};
