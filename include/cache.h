#pragma once
#include <mem_if.h>
#include <mem_mips_bus_wrapper.h>
#include <systemc.h>

#include "simple_bus.h"
#include "simple_bus_types.h"

struct AddressParts {
    sc_uint<18> tag;
    sc_uint<8> index;
    sc_uint<4> block_offset;
    sc_uint<2> byte_offset;

    AddressParts(uint32_t addr) {
        byte_offset = addr & 0b11;
        addr >>= 2;
        block_offset = addr & 0b1111;
        addr >>= 4;
        index = addr & 0b11111111;
        addr >>= 8;
        tag = addr;
    }
};

struct CacheEntry {
    sc_uint<1> valid;
    sc_uint<1> dirty;
    sc_uint<18> tag;
    sc_uint<32> data[16];

    CacheEntry() : valid(0), dirty(0) {
    }

    // Limpa uma entrada da cache (colocando o bit "valid" = 0).
    // Se a entrada estava "dirty", salvamos a entrada na memória principal.
    void clear(mem_if& main_mem, sc_uint<8> entry_index) {
        if (!valid)
            return;
        if (dirty) {
            // Push entry to main memory
            for (int block = 0; block < 16; block++) {
                uint32_t block_addr =
                    (tag << 14) | (entry_index << 6) | (block << 2);
                main_mem.sw(block_addr, 0, (sc_int<32>)data[block]);
            }
        }
        valid = false;
    }

    // Garante que a tag da entrada é a mesma que a que desejamos acessar.
    // Se a tag for diferente, há um cache miss, e devemos carregar a linha
    // da cache
    void ensure_correct_tag(mem_if& main_mem, const AddressParts& addr) {
        if (!valid || tag != addr.tag) {
            cerr << "!";
            clear(main_mem, addr.index);
            tag = addr.tag;

            // Load entry from main memory
            for (int block = 0; block < 16; block++) {
                uint32_t block_addr =
                    (tag << 14) | (addr.index << 6) | (block << 2);
                data[block] = main_mem.lw(block_addr, 0);
            }
            valid = true;
        } else {
            cerr << ".";
        }
    }

    int8_t lb(mem_if& main_mem, const AddressParts& a) {
        ensure_correct_tag(main_mem, a);
        int32_t word = data[a.block_offset];
        word = (word >> 8 * a.byte_offset);
        return word & 0xFF;
    }
    int16_t lh(mem_if& main_mem, const AddressParts& a) {
        ensure_correct_tag(main_mem, a);
        int32_t word = data[a.block_offset];
        word = (word >> 8 * (a.byte_offset & 2));
        return word & 0xFFFF;
    }
    int32_t lw(mem_if& main_mem, const AddressParts& a) {
        ensure_correct_tag(main_mem, a);
        return data[a.block_offset];
    }
    void sb(mem_if& main_mem, const AddressParts& a, sc_int<8> dado) {
        ensure_correct_tag(main_mem, a);
        uint8_t* pb = (uint8_t*)&data[a.block_offset];
        pb += a.byte_offset;
        *pb = (uint8_t)dado;
        dirty = true;
    }
    void sh(mem_if& main_mem, const AddressParts& a, sc_int<16> dado) {
        ensure_correct_tag(main_mem, a);
        uint16_t* ph = (uint16_t*)&data[a.block_offset];
        ph += (a.byte_offset & 2) >> 1;
        *ph = (uint16_t)dado;
        dirty = true;
    }
    void sw(mem_if& main_mem, const AddressParts& a, sc_int<32> dado) {
        ensure_correct_tag(main_mem, a);
        data[a.block_offset] = dado;
        dirty = true;
    }
};

struct Cache : sc_module, mem_if {
    sc_port<simple_bus_blocking_if> bus;
    mem_mips_bus_wrapper main_mem;

    // index = addr[13:6]
    enum { Entries = 256 };
    CacheEntry* entries;

    SC_HAS_PROCESS(Cache);
    Cache(sc_module_name name_)
        : sc_module(name_), bus("bus"), main_mem("main_mem") {
        main_mem.bus(bus);
        entries = new CacheEntry[Entries];
    }

    ~Cache() {
        delete[] entries;
    }

    int8_t lb(const uint32_t addr, uint16_t kte) {
        AddressParts a(addr + kte);
        return entries[a.index].lb(main_mem, a);
    }
    int16_t lh(const uint32_t addr, uint16_t kte) {
        AddressParts a(addr + kte);
        return entries[a.index].lh(main_mem, a);
    }
    int32_t lw(const uint32_t addr, uint16_t kte) {
        AddressParts a(addr + kte);
        return entries[a.index].lw(main_mem, a);
    }
    void sb(const uint32_t addr, sc_uint<5> kte, sc_int<8> dado) {
        AddressParts a(addr + kte);
        return entries[a.index].sb(main_mem, a, dado);
    }
    void sh(const uint32_t addr, sc_uint<5> kte, sc_int<16> dado) {
        AddressParts a(addr + kte);
        return entries[a.index].sh(main_mem, a, dado);
    }
    void sw(const uint32_t addr, sc_uint<5> kte, sc_int<32> dado) {
        AddressParts a(addr + kte);
        return entries[a.index].sw(main_mem, a, dado);
    }
};
