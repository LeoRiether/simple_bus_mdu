/*
 *  memoria.cpp
 *  MIPSc++
 *
 *  Created by Ricardo Jacobi on 18/4/15.
 *  Copyright 2015 Universidade de Brasilia. All rights reserved.
 *
 */

#include "memdef.h"
#include "memoria.h"
using namespace std;

void check_address_range(uint32_t address) {
	
	if ((address>>2) > MEM_SIZE) {
		cout << "Erro: endereco fora dos limites da memoria - " << address;
		exit(-1);
	}
}

int32_t lb(const uint32_t &address, uint16_t kte) {
    uint32_t tmp = address + kte;
	
	check_address_range(tmp);
	
    int32_t word = mem[tmp>>2];
    
    return ((word >> 8*(tmp%4))&0xFF);
}

int32_t lh(const uint32_t &address, uint16_t kte) {
    uint32_t tmp = address + kte;
	
	check_address_range(tmp);
	
    if ((tmp%2) != 0) {
        printf("Erro: endereco de meia palavra desalinhado!");
        return -1;
    }
	
	int32_t word = mem[tmp>>2];
	word = (word >> 8*(tmp&2));
	
    return (word & 0xFFFF);
}

int32_t lw(const uint32_t &address, uint16_t kte) {
    uint32_t tmp = address + kte;
	
	check_address_range(tmp);
	
    int32_t word = mem[tmp>>2];
    
    return word;
}

void sb(const uint32_t &address, uint16_t kte, int8_t dado) {
    uint32_t tmp  = address + kte;
	
	check_address_range(tmp);
	
    uint32_t bi = (tmp%4);
    
    uint8_t *pb = (uint8_t *)&mem[tmp>>2];
	pb += bi;
    //for (int i=0; i < bi; i++) pb++;
    
    *pb = (uint8_t)dado;
}

void sh(const uint32_t &address, uint16_t kte, int16_t dado) {
    uint32_t tmp  = address + kte;
	
	check_address_range(tmp);
	
    if ((tmp%2) != 0)
        printf("Erro: endereco de meia palavra desalinhado!");
    
    // com ponteiros
    uint16_t *ph = (uint16_t *)&mem[tmp>>2];
    
    if (tmp&2) ph++;
    
    *ph = (uint16_t) dado;
}

void sw(const uint32_t &address, uint16_t kte, int32_t dado) {
    uint32_t tmp  = address + kte;
	
	check_address_range(tmp);
	
	mem[tmp>>2] = dado;
}