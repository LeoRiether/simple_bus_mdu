# MDU -- Unidade de Multiplicação e Divisão
## Utilizando o simple_bus do SystemC

Por Leonardo Alves Riether - 190032413

Também disponível em [https://github.com/LeoRiether/simple_bus_mdu/](https://github.com/LeoRiether/simple_bus_mdu/) (com markdown bem formatado :) )

## Módulos
- Os módulos do [simple_bus](https://github.com/systemc/systemc-2.3/tree/master/examples/sysc/simple_bus) são disponibilizados pelo próprio SystemC, e foram copiados para os diretórios src/ e simple_bus_headers/
- **MDU**: definida em include/mdu.h, é um módulo de multiplicação e divisão de inteiros de 32 bits, que conta com uma interface para comunicação através do simple_bus. A MDU possui dois registradores, `opA` e `opB`, que devem ser escritos antes de realizar a operação. Depois, ao realizar um `read` no endereço apropriado (`MLow`, `MHigh`, `Div` ou `Mod`), a MDU coloca no bus o valor do cálculo efetuado, após um certo número de ciclos (configurável por meio da variável `wait_states`).
- O módulo **mdu_direct_master** é definido em include/mdu_direct_master.h, e executa testes da MDU, por meio de sua interface direta (`direct_read` e `direct_writes`).
- Já o **mdu_master**, definido em include/mdu_master.h, executa os testes por meio da interface blocante da MDU, que possui um delay de alguns ciclos para retornar um resultado (`burst_read` e `burst_write`). 
- Também podem ser encontrados módulos do MIPS (include/mem_mips.h, include/mem_mips_bus_wrapper.h e include/mips_v0.h) com sua interface de comunicação pelo simple_bus, mas ainda não foram efetuados testes dessa parte.

## Executando o Código
A compilação do projeto requer o `cmake` e pode ser feita através do comando
`make` (o makefile chama o `cmake`). Para compilar e executar o binário, pode-se
usar o comando `make run`.

Eu tentei não usar features mais recentes do C++, então é possível que o projeto
rode em versões mais antigas, mas não foi possível testar essa afirmação.

## Funcionamento do handle_request()
Na borda de descida do relógio, o simple_bus executa sua *main_action*.
Nela, caso haja uma requisição pendente, é chamado o método `simple_bus::handle_request`, que processa a requisição da seguinte maneira:

Primeiro, o módulo escravo apropriado é encontrado, baseado no endereço da requisição.
Depois, os dados da requisição são passados para o *slave*, que retorna um status, que pode ser `SIMPLE_BUS_OK`, `SIMPLE_BUS_WAIT` ou `SIMPLE_BUS_ERROR`. Caso o status seja `SIMPLE_BUS_WAIT`, a requisição não pode ser processada neste ciclo de clock, e continua pendente, mas caso seja `SIMPLE_BUS_OK` ou `SIMPLE_BUS_ERROR`, o mestre da transação é notificado por meio de um evento.
