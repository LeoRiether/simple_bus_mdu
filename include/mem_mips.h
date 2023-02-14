#pragma once
#include <definitions.h>
#include <mem_if.h>
#include <simple_bus.h>
#include <simple_bus_slave_if.h>
#include <simple_bus_types.h>
#include <systemc.h>

// Memória lenta, comunicação feita através do simple_bus.
// Para ler e escrever corretamente na memória, são necessarias 3 operações:
//
// - write(Byte ou Half ou Word, OpTypeReg)
// Define o modo de leitura/escrita na memória
//
// - write(endereço da leitura/escrita, AddrReg)
// Define o endereço em `mem` que será utilizado
//
// - read/write(&data, MemAddr)
// Executa a leitura/escrita com o endereço e modo definidos anteriormente
//
// Essas 3 operações são feitas no módulo mem_mips_bus_wrapper
struct mem_mips : public sc_module, public simple_bus_slave_if, public mem_if {
   public:
    enum OpType { Byte, Half, Word };
    enum Const {
        Priority = 2,
        StartAddr = 1024,
        OpTypeReg = 1024,  // lê/escreve no op_type_reg
        AddrReg = 1028,    // lê/escreve no addr_reg
        MemAddr = 1032,    // lê/escreve na memória
        EndAddr = 1035,
    };

   private:
    int32_t *mem;
    size_t size;

    int addr_reg;        // Define o endereço da leitura/escrita
    OpType op_type_reg;  // Define se eu vou ler/escrever um byte, ou halfword,
                         // ou word

    int wait_states;  // número de ciclos que uma operação demora para terminar
    int wait_left;  // número de ciclos restantes até a operação atual terminar

   public:
    sc_in<bool> clk;

    SC_HAS_PROCESS(mem_mips);
    mem_mips(sc_module_name name, uint32_t _size, int _wait_states,
             bool _verbose = false)
        : sc_module(name),
          size(_size),
          addr_reg(0),
          op_type_reg(Word),
          wait_states(_wait_states),
          wait_left(-1),
          clk("clk") {
        mem = new int32_t[size];

        SC_METHOD(update_wait);
        sensitive << clk.pos();
    }

    ~mem_mips() {
        delete[] mem;
    }

    void update_wait() {
        if (wait_left >= 0)
            wait_left--;
    }

    ///////////////////////////////////////
    //        Load/Store Handlers        //
    ///////////////////////////////////////
    void check_address_range(uint32_t addr) {
        if ((addr >> 2) > size) {
            cerr << "Erro: endereco fora dos limites da memoria - " << addr;
            exit(-1);
        }
    }

    int8_t lb(const uint32_t addr, uint16_t kte) {
        uint32_t tmp = addr + kte;

        check_address_range(tmp);

        int32_t word = mem[tmp >> 2];

        return ((word >> 8 * (tmp % 4)) & 0xFF);
    }

    int16_t lh(const uint32_t addr, uint16_t kte) {
        uint32_t tmp = addr + kte;

        check_address_range(tmp);

        if ((tmp % 2) != 0) {
            printf("Erro: endereco de meia palavra desalinhado!");
            return -1;
        }

        int32_t word = mem[tmp >> 2];
        word = (word >> 8 * (tmp & 2));

        return (word & 0xFFFF);
    }

    int32_t lw(const uint32_t addr, uint16_t kte) {
        uint32_t tmp = addr + kte;

        check_address_range(tmp);

        int32_t word = mem[tmp >> 2];

        return word;
    }

    void sb(const uint32_t addr, sc_uint<5> kte, sc_int<8> dado) {
        uint32_t tmp = addr + kte;

        check_address_range(tmp);

        uint32_t bi = (tmp % 4);

        uint8_t *pb = (uint8_t *)&mem[tmp >> 2];
        pb += bi;
        // for (int i=0; i < bi; i++) pb++;

        *pb = (uint8_t)dado;
    }

    void sh(const uint32_t addr, sc_uint<5> kte, sc_int<16> dado) {
        uint32_t tmp = addr + kte;

        check_address_range(tmp);

        if ((tmp % 2) != 0)
            printf("Erro: endereco de meia palavra desalinhado!");

        // com ponteiros
        uint16_t *ph = (uint16_t *)&mem[tmp >> 2];

        if (tmp & 2)
            ph++;

        *ph = (uint16_t)dado;
    }

    uint8_t lbu(const uint32_t addr, uint16_t kte) {
        uint32_t tmp = addr + kte;

        check_address_range(tmp);

        uint32_t word = mem[tmp >> 2];

        return ((word >> 8 * (tmp % 4)) & 0xFF);
    }

    uint16_t lhu(const uint32_t addr, uint16_t kte) {
        uint32_t tmp = addr + kte;

        check_address_range(tmp);

        if ((tmp % 2) != 0) {
            printf("Erro: endereco de meia palavra desalinhado!");
            return -1;
        }

        uint32_t word = mem[tmp >> 2];
        word = (word >> 8 * (tmp & 2));

        return (word & 0xFFFF);
    }

    void sw(const uint32_t addr, sc_uint<5> kte, sc_int<32> dado) {
        uint32_t tmp = addr + kte;

        check_address_range(tmp);

        mem[tmp >> 2] = dado;
    }

    void dump_mem(int start, int end, char format) {
        switch (format) {
            case 'h':
            case 'H':
                for (int i = start; i <= end; i += 4)
                    printf("%x \t%x\n", i, lw(i, 0));
                break;
            case 'd':
            case 'D':
                for (int i = start; i <= end; i += 4)
                    printf("%x \t%d\n", i, lw(i, 0));
                break;
            default:
                break;
        }
    }

    int load_mem(const char *fn, int start) {
        FILE *fptr;
        int *m_ptr = mem + (start >> 2);
        int size = 0;

        fptr = fopen(fn, "rb");
        if (!fptr) {
            printf("Arquivo nao encontrado!");
            return -1;
        } else {
            while (!feof(fptr)) {
                fread(m_ptr, 4, 1, fptr);
                m_ptr++;
                size++;
            }
            fclose(fptr);
        }
        return size;
    }

    ///////////////////////////////////////
    //        simple_bus_slave_if        //
    ///////////////////////////////////////
    simple_bus_status read(int *data, unsigned int address) {
        // A memória ainda está lendo os dados
        if (wait_left > 0)
            return SIMPLE_BUS_WAIT;

        // A memória acabou de ler os dados
        if (wait_left == 0) {
            direct_read(data, address);
            return SIMPLE_BUS_OK;
        }

        // A memória começa a ler os dados agora
        // Apenas ler da memória principal é demorado
        if (address == MemAddr)
            wait_left = wait_states;
        else
            wait_left = 0; 
        return SIMPLE_BUS_WAIT;
    }

    simple_bus_status write(int *data, unsigned int addr) {
        // Há um cálculo em andamento
        if (wait_left >= 0)
            return SIMPLE_BUS_WAIT;

        // Não há cálculo em andamento e podemos escrever
        direct_write(data, addr);
        return SIMPLE_BUS_OK;
    }

    unsigned int start_address() const {
        return StartAddr;
    }
    unsigned int end_address() const {
        return EndAddr;
    }

    ////////////////////////////////////////
    //        simple_bus_direct_if        //
    ////////////////////////////////////////
    bool direct_read(int *data, unsigned int address) {
        if (op_type_reg == Byte)
            *data = lb(addr_reg, 0);
        else if (op_type_reg == Half)
            *data = lh(addr_reg, 0);
        else
            *data = lw(addr_reg, 0);
        return true;
    }
    bool direct_write(int *data, unsigned int address) {
        if (address == OpTypeReg)
            op_type_reg = (OpType)*data;
        else if (address == AddrReg)
            addr_reg = *data;
        else if (address == MemAddr) {
            if (op_type_reg == Byte)
                sb(addr_reg, 0, *data);
            else if (op_type_reg == Half)
                sh(addr_reg, 0, *data);
            else
                sw(addr_reg, 0, *data);
        }
        return true;
    }
};
