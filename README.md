# Projeto Memória Virtual
Atividade da cadeira de Infraestrutura de Software com o objetivo de simular uma memória virtual.<br />
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
$ ./vm addresses.txt algorithm1, algorithm2<br />
nesse segundo comando, executa-se o arquivo compilado e 3 argumentos são passados:<br />
O addresses.txt - que é o arquivo texto que contem os endereços de cada instrução da memoria secundária<br />
O algorithm1 - que é o tipo de algoritmo usado para atualizar a memória física(fifo ou lru)<br />
O algorithm2 - que é o tipo de algoritmo usado para atualizar a tlb(fifo ou lru)<br />
