# Projeto Memória Virtual
Atividade da cadeira de Infraestrutura de Software com o objetivo de simular uma memória virtual.<br />
Foi implemtentado no sistema operacional Linux. Com apenas 1 arquivo de codificaç, o main.c<br />
Nele ainda existem mais 4 arquivos: o addresses.txt o BACKING_STORE.bin e o README.md e o Makefile<br />
O programa vai ler de um arquivo.txt números e transformá-los em binário para que possam ser acessados <br />
os endereços de localização na memoria secundária(nesse caso, tambem um arquivo, o "BACKING_STORE.bin").<br />
Assim, o programa tem o número da página que deve ser acessada e o offset, para encontrar o valor correspondente<br />
na memoria secundária e salvá-lo na memória principal(memória física). São implementados algoritmos, como FIFO e
o LRU,<br /> para o processo de carregamento da página na memoria física como tambem na tabela de páginas. Tabela de<br /> 
páginas, que serve para verificicar se a página ja foi carregada na memória física ou não.<br />

## Como clonar o repositório:
$ git clone https://github.com/RenzoOliveira1784/virtual_memory_simulation.git <br />
$ cd virtual_memory_simulation<br />

## Como utilizar o programa:
$ make<br />
Apenas compila o arquivo main.c<br />
$ ./vm addresses.txt algorithm1, algorithm2<br />
Nesse segundo comando, executa-se o arquivo compilado e 3 argumentos são passados:<br />
O addresses.txt - que é o arquivo texto que contem os endereços de cada instrução da memoria secundária<br />
O algorithm1 - que é o tipo de algoritmo usado para atualizar a memória física(fifo ou lru)<br />
O algorithm2 - que é o tipo de algoritmo usado para atualizar a tlb(fifo ou lru)<br />
$ make clean<br />
Comando para limpar da memória o arquivo objeto<br />

### Como saber se está funcionando:
$ make test_fifofifo
O resultado das últimas linhas do make test_fifofifo no arquivo correct.txt é<br />
Virtual address: 45563 Physical address: 6395 Value: 126<br />
Virtual address: 12107 Physical address: 6475 Value: -46<br />
Number of Translated Addresses = 1000<br />
Page Faults = 538<br />
Page Fault Rate = 0.538<br />
TLB Hits = 54<br />
TLB Hits Rate = 0.054<br />

