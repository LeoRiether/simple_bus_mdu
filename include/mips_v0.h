#pragma once
#include <cache.h>
#include <definitions.h>
#include <mdu.h>
#include <mem_if.h>
#include <systemc.h>

#include <unordered_map>

#include "mem_mips_bus_wrapper.h"
#include "simple_bus.h"
#include "simple_bus_types.h"

std::unordered_map<uint8_t, const char*> FUNCT_STR = {
    {0x20, "ADD"}, {0x21, "ADDU"}, {0x22, "SUB"},     {0x18, "MUL"},
    {0x1A, "DIV"}, {0x24, "AND"},  {0x25, "OR"},      {0x26, "XOR"},
    {0x27, "NOR"}, {0x2A, "SLT"},  {0x08, "JR"},      {0x00, "SLL"},
    {0x02, "SRL"}, {0x03, "SRA"},  {0x0C, "SYSCALL"}, {0x10, "MFHI"},
    {0x12, "MFHI"},
};

std::unordered_map<uint8_t, const char*> OPCODE_STR = {
    {0x00, "FUN"},   {0x23, "LW"},   {0x20, "LB"},    {0x21, "LH"},
    {0x25, "LHU"},   {0x0F, "LUI"},  {0x2B, "SW"},    {0x28, "SB"},
    {0x29, "SH"},    {0x04, "BEQ"},  {0x05, "BNE"},   {0x06, "BLEZ"},
    {0x07, "BGTZ"},  {0x08, "ADDI"}, {0x09, "ADDIU"}, {0x0A, "SLTI"},
    {0x0B, "SLTIU"}, {0x0C, "ANDI"}, {0x0D, "ORI"},   {0x0E, "XORI"},
    {0x02, "J"},     {0x03, "JAL"},
};

SC_MODULE(mips_v0) {
   private:
    bool verbose;

    int32_t breg[32];
    int32_t pc,  // contador de programa
        ri,      // registrador de intrucao
        hi,      // 32 MSB da multiplicacao
        lo,      // 32 LSB da multiplicacao
        kte26;   // constante instrucao tipo J

    int32_t opcode, rs, rt, rd, shamt, funct, kte16;

    Cache pmem;

   public:
    sc_in<bool> clk;
    sc_port<simple_bus_blocking_if> bus;
    sc_in<bool> reset;

    SC_HAS_PROCESS(mips_v0);
    mips_v0(sc_module_name name, bool _verbose = false)
        : verbose(_verbose),
          pmem("pmem"),
          clk("clk"),
          bus("bus"),
          reset("reset") {
        pmem.bus(bus);

        SC_THREAD(run);
        sensitive << clk.pos();

        SC_METHOD(init);
        sensitive << reset;
    }

    void init() {
        pc = ri = hi = lo = 0;
        memset(breg, 0, sizeof(breg));
        breg[29] = DATA_SEGMENT_START - 4;  // stack pointer
    }

    void fetch() {
        ri = pmem.lw(pc, 0);
        pc += 4;
    }

    void decode() {
        opcode = (ri >> 26) & 0x3F;
        rs = (ri >> 21) & 0x1F;
        rt = (ri >> 16) & 0x1F;
        rd = (ri >> 11) & 0x1F;
        shamt = (ri >> 6) & 0x1F;
        funct = ri & 0x3F;
        kte16 = (int16_t)(ri & 0xFFFF);
        kte26 = ri & 0x03FFFFFF;

        if (verbose) {
            fprintf(stderr,
                    "[decode pc=%x]\n"
                    "ri = %x\n"
                    "opcode = %x (%s)\n"
                    "rs = %d\n"
                    "rt = %d\n"
                    "rd = %d\n"
                    "shamt = %x\n"
                    "funct = %x (%s)\n"
                    "kte16 = %d\n"
                    "kte26 = %d\n\n",
                    pc - 4, ri, opcode, OPCODE_STR[opcode], rs, rt, rd, shamt,
                    funct, FUNCT_STR[funct], kte16, kte26);
        }
    }

    void debug_decode() {
        cout << "PC = " << pc << endl;
        cout << "opcode = " << opcode << " rs = " << rs << " rt = " << rt
             << " rd = " << rd << " shamt = " << shamt << " funct = " << funct
             << endl;
    }

    void dump_breg() {
        for (int i = 0; i < 32; i++) {
            printf("BREG[%2d] = \t%8d \t\t\t%8x\n", i, breg[i], breg[i]);
        }
    }

    void execute() {
        char c;
        size_t pos;

        breg[0] = 0;
        switch (opcode) {
            case FUN:
                switch (funct) {
                    case SYSCALL:            // emula as chamadas do MARS
                        switch (breg[V0]) {  // NOTE: V0 -> breg[V0]
                            case 1:
                                cout << breg[A0];
                                break;
                            case 2:
                                break;
                            case 3:
                                break;
                            case 4:
                                // NOTE: também tive que mudar essa parte do
                                // código, visto que o mips_v0 não tem acesso
                                // direto ao array da memória
                                pos = breg[A0];
                                while ((c = pmem.lb(pos++, 0)) != 0)
                                    cout << c;
                                break;
                            case 5:
                                cin >> breg[V0];
                                break;
                            case 6:
                                break;
                            case 10:
                                // NOTE: aqui era um exit(0); antes
                                sc_stop();
                                return;
                            default:
                                break;
                        }
                        break;
                    case SLL:
                        breg[rd] = breg[rt] << shamt;
                        break;
                    case SRL:
                        breg[rd] = (uint32_t)breg[rt] >> shamt;
                        break;
                    case SRA:
                        breg[rd] = breg[rt] >> shamt;
                        break;
                    case JR:
                        pc = breg[rs];
                        break;
                    case MUL:
                        // NOTE: MUL e DIV utilizam a MDU para realizar os
                        // cálculos. A comunicação é feita através do simple_bus
                        bus->burst_write(MDU::Priority, &breg[rs], MDU::OpA);
                        bus->burst_write(MDU::Priority, &breg[rt], MDU::OpB);
                        bus->burst_read(MDU::Priority, &lo, MDU::MLow);
                        bus->burst_read(MDU::Priority, &hi, MDU::MHigh);
                        break;
                    case DIV:
                        bus->burst_write(MDU::Priority, &breg[rs], MDU::OpA);
                        bus->burst_write(MDU::Priority, &breg[rt], MDU::OpB);
                        bus->burst_read(MDU::Priority, &lo, MDU::Div);
                        bus->burst_read(MDU::Priority, &hi, MDU::Mod);
                        break;
                    case MFLO:
                        breg[rd] = lo;
                        break;
                    case MFHI:
                        breg[rd] = hi;
                        break;
                    case ADD:
                        breg[rd] = breg[rs] + breg[rt];
                        break;
                    case ADDU:
                        breg[rd] = breg[rs] + breg[rt];
                        break;
                    case SUB:
                        breg[rd] = breg[rs] - breg[rt];
                        break;
                    case AND:
                        breg[rd] = breg[rs] & breg[rt];
                        break;
                    case OR:
                        breg[rd] = breg[rs] | breg[rt];
                        break;
                    case XOR:
                        breg[rd] = breg[rs] ^ breg[rt];
                        break;
                    case NOR:
                        breg[rd] = ~(breg[rs] | breg[rt]);
                        break;
                    case SLT:
                        breg[rd] = breg[rs] < breg[rt];
                        break;
                    default:
                        printf("Instrucao Invalida (*0x%x~0x%x)\n", pc, ri);
                }
                break;
            case JAL:
                breg[RA] = pc;
                pc = (pc & 0xf0000000) | (kte26 << 2);
                break;

            case J:
                pc = (pc & 0xf0000000) | (kte26 << 2);
                break;

            case BEQ:
                if (breg[rs] == breg[rt])
                    pc += (kte16 << 2);
                break;
            case BNE:
                if (breg[rs] != breg[rt])
                    pc += (kte16 << 2);
                break;
            case BLEZ:
                if (breg[rs] <= 0)
                    pc += (kte16 << 2);
                break;
            case BGTZ:
                if (breg[rs] > 0)
                    pc += (kte16 << 2);
                break;
            case ADDI:
                breg[rt] = breg[rs] + (short)kte16;
                break;
            case ADDIU:
                breg[rt] = (unsigned)breg[rs] + (short)kte16;
                break;
            case SLTI:
                breg[rt] = breg[rs] < (short)kte16;
                break;
            case SLTIU:
                breg[rt] = (unsigned)breg[rs] < (unsigned)kte16;
                break;
            case ANDI:
                breg[rt] = breg[rs] & kte16;
                break;
            case ORI:
                breg[rt] = breg[rs] | kte16;
                break;
            case XORI:
                breg[rt] = breg[rs] ^ kte16;
                break;
            case LUI:
                breg[rt] = (kte16 << 16);
                break;
            case LB:
                // NOTE: Como o mips_v0 não tem acesso direto à memória, o
                // código comentado abaixo não dá mais certo breg[rt] =
                // (char)(mem[breg[rs] + kte16]);
                breg[rt] = (char)(pmem.lb(breg[rs] + kte16, 0));
                break;
            case LH:
                breg[rt] = pmem.lh(breg[rs], kte16);
                break;
            case LW:
                breg[rt] = pmem.lw(breg[rs], kte16);
                break;
            case LHU:
                // NOTE: Como o mips_v0 não tem acesso direto à memória, o
                // código comentado abaixo não dá mais certo breg[rt] =
                // (uint32_t)mem[breg[rs] + kte16];
                breg[rt] = (uint32_t)pmem.lw(breg[rs] + kte16, 0);
                break;
            case SB:
                pmem.sb(breg[rs], kte16, get_byte_0(breg[rt]));
                break;
            case SH:
                pmem.sh(breg[rs], kte16, (uint16_t)(breg[rt] & 0xFFFF));
                break;
            case SW:
                pmem.sw(breg[rs], kte16, breg[rt]);
                break;
            default:
                break;
        }
    }

    void step() {
        fetch();
        decode();
        execute();
    }

    void run() {
        init();
        while (pc < DATA_SEGMENT_START) {
            wait();
            step();
        }

        if (verbose)
            fprintf(stderr, "run ended! pc = %x\n", pc);
        sc_stop();
    }
};
