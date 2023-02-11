# MDU -- Unidade de Multiplicação e Divisão
## Utilizando o simple_bus do SystemC

## O Projeto
A MDU criada se comunica com outros módulos por meio do
[simple_bus](https://github.com/systemc/systemc-2.2.0/tree/master/examples/sysc/simple_bus)

- estrutura do projeto com src/, include/ e simple_bus_headers/

## Executando o Código
A compilação do projeto requer o `cmake` e pode ser feita através do comando
`make` (o makefile chama o `cmake`). Para compilar e executar o binário, pode-se
usar o comando `make run`.

Eu tentei não usar features mais recentes do C++, então é possível que o projeto
rode em versões mais antigas, mas não foi possível testar essa afirmação.

## handle_request()
<!-- TODO: -->
