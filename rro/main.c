#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct node_mem_fis {
    char total_page[256];
};

struct node_page_table {
    int index_mem_fis;
    int bit_sinal;
    int time;
};

struct node_tlb {
    int num_pag;
    int index_mem_fis;
    int bit_sinal;
    int time;
};

//pra todos que tiverem o bit de sinal 1 ==> time ++


int bin_to_dec (int bin) {
    int total = 0;
    int pot = 1;

    while (bin > 0) {
        total += bin % 10 * pot;
        bin /= 10;
        pot = pot * 2;
    }  
    return total;
}

// 0./vm  1 argumento adresses 2 algorithm_page_table 3 algorithm_tlb
int main(int agrc, char *argv[]) {
    char input_addresses_name[20];
    char algorithm_page_table[5];
    char algorithm_tlb[5];
    strcpy(input_addresses_name,argv[1]);
    strcpy(algorithm_page_table,argv[2]);
    strcpy(algorithm_tlb,argv[3]);

    FILE *fp, *fp_backstore;
    int PAGE_SIZE = 256, PAGE_TABLE_SIZE = 256, FIS_MEM_SIZE = 128, TLB_SIZE = 16;
    char str_adress[7];
    int dec_adress;
    int int_page_offset[16];
    char str_page_offset[17];
    char str_offset[9], str_num_page[9];
    int bin_int_num_page, bin_int_offset;
    int desloc_on_BS;
    struct node_page_table page_table[PAGE_TABLE_SIZE];
    struct node_mem_fis mem_fis[FIS_MEM_SIZE];
    struct node_tlb tlb[TLB_SIZE];
    int cont_mem_fis = 0, cont_page_fault = 0, cont_non_page_fault = 0, cont_tlb = 0, tlb_hit = 0;;
    
    // inicializar os atributos da struct da page table;
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        page_table[i].bit_sinal = 0;
        page_table[i].time = 0;
    }
    
    // inicializar os atributos da struct da tlb;
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].bit_sinal = 0;
        tlb[i].time = 0;
    }
    

    fp = fopen ("addresses.txt", "r");
    while (fgets(str_adress, 7, fp) != NULL) {

        
        int int_num_page = 0, int_offset = 0;

        dec_adress = atoi(str_adress);
        int virtual_adress = dec_adress;
        // Convertendo o inteiro para binario
        for (int cont = 15; cont >= 0; cont--) {
            if (dec_adress % 2 == 0)
                int_page_offset[cont] = 0;
            else
                int_page_offset[cont] = 1;
            dec_adress = dec_adress / 2;
        }

        // []
        // porem, dessa vez, estará pronto para ser dividido em page e offset
        for (int cont = 0; cont <= 15; cont++) {
            str_page_offset[cont] = int_page_offset[cont] + '0';
        }

        
        for (int i = 0; i < 8; i++) {
            str_num_page[i] = str_page_offset[i]; 
            str_offset[i] = str_page_offset[8+i];
        }
        //// printf("%s\t%s\n", num_page, offset);
        // str_offset e str_num_page sao strings em binario 
        //transformando eles em inteiros, porem ainda estarao em 1 e 0
        bin_int_num_page = atoi(str_num_page);
        bin_int_offset = atoi(str_offset);

        //colocando os inteiros na base decimal
        int_num_page = bin_to_dec(bin_int_num_page);
        int_offset = bin_to_dec(bin_int_offset);

        //se o num_page ja estiver na tabela de paginas, procurar na page table e acessar a memoria fisica
        // se nao, adicionar do backing store na memoria fisica e atualizar a page table e depois fazer o de cima
        int end_value;
        int physical_adress;
        
        
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
        //Algortimo de FIFO page_table:
        if (strcmp(algorithm_page_table, "fifo") == 0) {

            // casos
            // 1-tlb n_achou --> procurar na page_table  ---(esta)---> pegar na memoria ---> atualizar page table e tlb
            //                                           --(n_esta)--> pegar no bin -------> botar na memoria ----> atualizar page table e tlb.
            // 
            // 2-tlb achou ----> pegar da memoria ----> atualizar a page table e tlb.
        
            if (strcmp(algorithm_tlb, "fifo") == 0) {
                int found = 0;
                int index_tlb;
                for (int i = 0; i < TLB_SIZE; i++) {
                    if (tlb[i].bit_sinal == 1) {
                        if(tlb[i].num_pag == int_num_page) {
                            found = 1;
                            index_tlb = i;
                        }
                    } else break;
                }
                
                //if achou tlb
                if (found == 1) {
                    //procurar na memoria
                    end_value = mem_fis[tlb[index_tlb].index_mem_fis].total_page[int_offset];
                    physical_adress = tlb[index_tlb].index_mem_fis * PAGE_SIZE + int_offset;
                    printf("Virtual address: %d Physical address: %d Value: %d\n", virtual_adress, physical_adress, end_value);
                    tlb_hit++;
                    cont_non_page_fault++;
                    
                    // atualizar a tlb
                    // se ta na tlb a atualização dela é nao mudar nada;
                    
                } else {  // nao achou tlb ---> pocurar na page_table

                    if (page_table[int_num_page].bit_sinal == 1) {
                        //procurar página na memoria fisica 
                        end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                        physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                        printf("Virtual address: %d Physical address: %d Value: %d\n", virtual_adress, physical_adress, end_value);

                        // atualizar a tlb 
                        tlb[cont_tlb % TLB_SIZE].bit_sinal = 1;
                        tlb[cont_tlb % TLB_SIZE].index_mem_fis = page_table[int_num_page].index_mem_fis;
                        tlb[cont_tlb % TLB_SIZE].num_pag = int_num_page;
                        cont_non_page_fault++;

                    } else {
                        if (cont_mem_fis < FIS_MEM_SIZE) {
                            if (cont_tlb <= TLB_SIZE) {
                                tlb_hit++;
                            }
                            //caminhos 
                            // 1- nao ta na page table e quer add(normal) 
                            //(abrir o BS e add a pagina na memoria fisica (que pode ser acrescentando ou no lugar de outra) e atualizar a page table)
                            fp_backstore = fopen ("BACKING_STORE.bin", "rb");
                            desloc_on_BS = int_num_page * PAGE_SIZE;
                            fseek(fp_backstore, desloc_on_BS, SEEK_SET);
                            fread(mem_fis[cont_mem_fis].total_page, PAGE_SIZE, 1, fp_backstore);
                            
                            // reload the page table
                            page_table[int_num_page].bit_sinal = 1;
                            page_table[int_num_page].index_mem_fis = cont_mem_fis;
                            end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                            physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                            printf("Virtual address: %d Physical address: %d Value: %d\n", virtual_adress, physical_adress, end_value);
                            tlb[cont_tlb % TLB_SIZE].bit_sinal = 1;
                            tlb[cont_tlb % TLB_SIZE].index_mem_fis = page_table[int_num_page].index_mem_fis;
                            tlb[cont_tlb % TLB_SIZE].num_pag = int_num_page;

                            
                            cont_page_fault++;
                            cont_mem_fis++;
                            fclose(fp_backstore);
                        } else {
                            // 2- ta na page table, mas nao esta na memoria (atualizar a page_table pros 2(tanto pra ele quanto pra quem sair))
                            // ex cont_mem_fis = 128
                            // nao precisa atualizar pra tlb, como sao sempre os 16 mais recentes, ele nunca vai pegar algum que sera excluido da memoria pelo FIFO
                            int get_page_index;
                            fp_backstore = fopen ("BACKING_STORE.bin", "rb");
                            desloc_on_BS = int_num_page * PAGE_SIZE;
                            fseek(fp_backstore, desloc_on_BS, SEEK_SET);
                            fread(mem_fis[cont_mem_fis % FIS_MEM_SIZE].total_page, PAGE_SIZE, 1, fp_backstore);
                            
                            // reload the page table for the 2 values
                            // for the 1rst(the one that's gonna get out of the physical memory)
                            for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                                if (page_table[i].index_mem_fis == cont_mem_fis % FIS_MEM_SIZE)
                                page_table[i].bit_sinal = 0;
                            }

                            for (int i = 0; i < TLB_SIZE; i++) {
                                if (tlb[i].index_mem_fis == cont_mem_fis % FIS_MEM_SIZE)
                                tlb[i].bit_sinal = 0;
                            }
                            tlb[int_num_page].bit_sinal = 1;
                            page_table[int_num_page].index_mem_fis = cont_mem_fis % FIS_MEM_SIZE;

                            // for the 2nd(the substitute in the physical memory)
                            page_table[int_num_page].bit_sinal = 1;
                            page_table[int_num_page].index_mem_fis = cont_mem_fis % FIS_MEM_SIZE;
                            end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                            physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                            printf("Virtual address: %d Physical address: %d Value: %d\n", virtual_adress, physical_adress, end_value);
                            
                            
                            cont_page_fault++;
                            cont_mem_fis++;
                            fclose(fp_backstore);
                        }
                    }
                }
                cont_tlb++;
            } else if (strcmp(algorithm_tlb, "lru") == 0) {
                //implemtentação sem tlb 


                printf("fifo lru implementação sem tlb\n");



                if (page_table[int_num_page].bit_sinal == 1) {
                //procurar página na memoria fisica 
                end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                printf("Virtual address: %d Physical address: %d Value: %d\n", virtual_adress, physical_adress, end_value);
                cont_non_page_fault++;

            } else {
                if (cont_mem_fis < FIS_MEM_SIZE) {
                    //caminhos 
                    // 1- nao ta na page table e quer add(normal)
                    //(abrir o BS e add a pagina na memoria fisica (que pode ser acrescentando ou no lugar de outra) e atualizar a page table)
                    fp_backstore = fopen ("BACKING_STORE.bin", "rb");
                    desloc_on_BS = int_num_page * PAGE_SIZE;
                    fseek(fp_backstore, desloc_on_BS, SEEK_SET);
                    fread(mem_fis[cont_mem_fis].total_page, PAGE_SIZE, 1, fp_backstore);
                    
                    // reload the page table
                    page_table[int_num_page].bit_sinal = 1;
                    page_table[int_num_page].index_mem_fis = cont_mem_fis;
                    end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                    physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                    printf("Virtual address: %d Physical address: %d Value: %d\n", virtual_adress, physical_adress, end_value);
                    cont_page_fault++;
                    cont_mem_fis++;
                    fclose(fp_backstore);
                } else {
                    // 2- ta na page table, mas nao esta na memoria (atualizar a page_table pros 2(tanto pra ele quanto pra quem sair))
                    // ex cont_mem_fis = 128
                    int get_page_index;
                    fp_backstore = fopen ("BACKING_STORE.bin", "rb");
                    desloc_on_BS = int_num_page * PAGE_SIZE;
                    fseek(fp_backstore, desloc_on_BS, SEEK_SET);
                    fread(mem_fis[cont_mem_fis % FIS_MEM_SIZE].total_page, PAGE_SIZE, 1, fp_backstore);
                    
                    // reload the page table for the 2 values
                    // for the 1rst(the one that's gonna get out of the physical memory)
                    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                        if (page_table[i].index_mem_fis == cont_mem_fis % FIS_MEM_SIZE)
                        page_table[i].bit_sinal = 0;
                    }

                    // for the 2nd(the substitute in the physical memory)
                    page_table[int_num_page].bit_sinal = 1;
                    page_table[int_num_page].index_mem_fis = cont_mem_fis % FIS_MEM_SIZE;
                    end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                    physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                    printf("Virtual address: %d Physical address: %d Value: %d\n", virtual_adress, physical_adress, end_value);
                    cont_page_fault++;
                    cont_mem_fis++;
                    fclose(fp_backstore);
                }
            }
            }
             
            ////faltando as outras opçoes com tlb. o else if ta errado;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        } else if (strcmp(algorithm_page_table, "lru") == 0) {//ALGORTIMO LRU page_table
            
            if (strcmp(algorithm_tlb, "fifo") == 0) {
                int found = 0;
                int index_tlb;
                for (int i = 0; i < TLB_SIZE; i++) {
                    if (tlb[i].bit_sinal == 1) {
                        if(tlb[i].num_pag == int_num_page) {
                            found = 1;
                            index_tlb = i;
                        }
                    } else break;
                }
                //if achou tlb
                if (found == 1) {
                    //procurar na memoria
                    end_value = mem_fis[tlb[index_tlb].index_mem_fis].total_page[int_offset];
                    physical_adress = tlb[index_tlb].index_mem_fis * PAGE_SIZE + int_offset;
                    printf("0 - Virtual address: %d Physical address: %d Value: %d\n", virtual_adress, physical_adress, end_value);
                    // tem que zerar o time na page_table e reload dos 2
                    tlb[index_tlb].time = 0;
                    page_table[tlb[index_tlb].num_pag].time = 0;
                    tlb_hit++;
                    cont_non_page_fault++;
                } else { 
                    //opcoes:

                    if (page_table[int_num_page].bit_sinal == 1) {
                        //correto
                        //pagina ja está na page_table, acessa-la
                        end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                        physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                        page_table[int_num_page].time = 0;

                        printf("1 -\tVirtual address: %d Physical address: %d Value: %d\n", virtual_adress, physical_adress, end_value);
                        //increment time
                        for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                            if (page_table[i].bit_sinal == 1)
                                page_table[i].time++; //so incrementa os que estao na mem fisica
                        }
                        //reload tlb
                        tlb[cont_tlb % TLB_SIZE].bit_sinal = 1;
                        tlb[cont_tlb % TLB_SIZE].index_mem_fis = page_table[int_num_page].index_mem_fis;
                        tlb[cont_tlb % TLB_SIZE].num_pag = int_num_page;
                        tlb[cont_tlb % TLB_SIZE].time = page_table[int_num_page].time;
            
                        cont_non_page_fault++;
                    } else {
                        //correto
                        //pagina ainda nao está na memoria e precisa paginar com LRU (mais velho vaza)
                        //casos:
                        if (cont_mem_fis < FIS_MEM_SIZE) {
                            // SE OS TLBHITS DEREM ERRADO O PROBLEMA TA AQUI
                            // quando ele entra aqui nessa condição de baixo, tem que dar tlb hit pois na teoria ele vai reler o addresses.txt e
                            // assim, vai estar na tlb.
                            //casos em que nao estao na memoria e a memoria física ainda tem espaços livres(so add normal la na memoria)
                            fp_backstore = fopen ("BACKING_STORE.bin", "rb");
                            desloc_on_BS = int_num_page * PAGE_SIZE;
                            fseek(fp_backstore, desloc_on_BS, SEEK_SET);
                            fread(mem_fis[cont_mem_fis].total_page, PAGE_SIZE, 1, fp_backstore);

                            // reload the page table
                            page_table[int_num_page].bit_sinal = 1;
                            page_table[int_num_page].index_mem_fis = cont_mem_fis;
                            page_table[int_num_page].time = 0;

                            end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                            physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                            printf("2 -\tVirtual address: %d Physical address: %d Value: %d\n", virtual_adress, physical_adress, end_value);
                            //incrementando o tempo de todos os que estao na memoria fisica
                            for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                                if (page_table[i].bit_sinal == 1)
                                    page_table[i].time++;
                            }
                            tlb[cont_tlb % TLB_SIZE].bit_sinal = 1;
                            tlb[cont_tlb % TLB_SIZE].index_mem_fis = page_table[int_num_page].index_mem_fis;
                            tlb[cont_tlb % TLB_SIZE].num_pag = int_num_page;
                            tlb[cont_tlb % TLB_SIZE].time = page_table[int_num_page].time;
                            
                            cont_page_fault++;
                            cont_mem_fis++;
                            fclose(fp_backstore);
                        } else {
                            //casos em que esta na page_table mas a memoria física não tem mais espaços livres
                            //(cont_mem_fis >= FIS_MEM_SIZE)
            
                            //achar o que vai ser trocado;
                            int change = 0;
                            int index_page_table_highiest_time;
                            for (int i = 0; i < PAGE_SIZE; i++) {
                                if (page_table[i].time > change) {
                                    change = page_table[i].time;
                                    index_page_table_highiest_time = i;
                                }
                            }
                            //tendo o indice do que vai ser trocado, pegamos do BS.bin a pagina
                            fp_backstore = fopen ("BACKING_STORE.bin", "rb");
                            desloc_on_BS = int_num_page * PAGE_SIZE;
                            fseek(fp_backstore, desloc_on_BS, SEEK_SET);
                            //e assim trocamos na memoria fisica
                            fread(mem_fis[page_table[index_page_table_highiest_time].index_mem_fis].total_page, PAGE_SIZE, 1, fp_backstore);
                            //e atualizamos na page_table para os 2 valores
                            //(o cont_mem_fis indexava o antigo)
                            int mem_fis_add;
                            //atualizando o antigo(primeiro coloca tanto o antigo quanto o novo para zero dps atualiza o novo):
                            for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                                if (i == index_page_table_highiest_time) { 
                                    page_table[i].bit_sinal = 0;
                                    mem_fis_add = page_table[i].index_mem_fis;
                                    page_table[i].index_mem_fis = -1;
                                    page_table[i].time = 0;
                                }
                            }




                            //atualizando o novo:
                            page_table[int_num_page].bit_sinal = 1;
                            page_table[int_num_page].index_mem_fis = mem_fis_add;//using index_page_table_highiest_time take the fis_mem index;
                            page_table[int_num_page].time = 0;


                            end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                            physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                            //printing results:
                            printf("3 -\tVirtual address: %d Physical address: %d Value: %d\n", virtual_adress, physical_adress, end_value);

                            //reloading counters and time
                            for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                                if (page_table[i].bit_sinal == 1)
                                    page_table[i].time++;
                            }
                            //reload tlb
                            tlb[cont_tlb % TLB_SIZE].bit_sinal = 1;
                            tlb[cont_tlb % TLB_SIZE].index_mem_fis = page_table[int_num_page].index_mem_fis;
                            tlb[cont_tlb % TLB_SIZE].num_pag = int_num_page;
                            tlb[cont_tlb % TLB_SIZE].time = page_table[int_num_page].time;
                            cont_page_fault++;
                            cont_mem_fis++;
                            fclose(fp_backstore);
                        }
                    }
                }
            cont_tlb++;
            } else if (strcmp(algorithm_tlb, "lru") == 0) { // lru implementado sem a tlb 
                

                printf("lru lru implementação sem tlb\n");
                
                
                if (page_table[int_num_page].bit_sinal == 1) {
                    //correto
                    //pagina ja está na page_table, acessa-la
                    end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                    physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                    page_table[int_num_page].time = 0;

                    printf("1 - OFFSET %d\tVirtual address: %d Physical address: %d Value: %d\n",int_offset, virtual_adress, physical_adress, end_value);
                    //increment time
                    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                        if (page_table[i].bit_sinal == 1)
                            page_table[i].time++; //so incrementa os que estao na mem fisica
                    }
        
                    cont_non_page_fault++;
                } else {
                    //correto
                    //pagina ainda nao está na memoria e precisa paginar com LRU (mais velho vaza)
                    //casos:
                    if (cont_mem_fis < FIS_MEM_SIZE) {
                        //casos em que nao estao na memoria e a memoria física ainda tem espaços livres(so add normal la na memoria)
                        fp_backstore = fopen ("BACKING_STORE.bin", "rb");
                        desloc_on_BS = int_num_page * PAGE_SIZE;
                        fseek(fp_backstore, desloc_on_BS, SEEK_SET);
                        fread(mem_fis[cont_mem_fis].total_page, PAGE_SIZE, 1, fp_backstore);

                        // reload the page table
                        page_table[int_num_page].bit_sinal = 1;
                        page_table[int_num_page].index_mem_fis = cont_mem_fis;
                        page_table[int_num_page].time = 0;

                        end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                        physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                        printf("2 - OFFSET %d\tVirtual address: %d Physical address: %d Value: %d\n",int_offset, virtual_adress, physical_adress, end_value);
                        //incrementando o tempo de todos os que estao na memoria fisica
                        for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                            if (page_table[i].bit_sinal == 1)
                                page_table[i].time++;
                        }
                        cont_page_fault++;
                        cont_mem_fis++;
                        fclose(fp_backstore);
                    } else {
                        //casos em que esta na page_table mas a memoria física não tem mais espaços livres
                        //(cont_mem_fis >= FIS_MEM_SIZE)
        
                        //achar o que vai ser trocado;
                        int change = 0;
                        int index_page_table_highiest_time;
                        for (int i = 0; i < PAGE_SIZE; i++) {
                            if (page_table[i].time > change) {
                                change = page_table[i].time;
                                index_page_table_highiest_time = i;
                            }
                        }
                        //tendo o indice do que vai ser trocado, pegamos do BS.bin a pagina
                        fp_backstore = fopen ("BACKING_STORE.bin", "rb");
                        desloc_on_BS = int_num_page * PAGE_SIZE;
                        fseek(fp_backstore, desloc_on_BS, SEEK_SET);
                        //e assim trocamos na memoria fisica
                        fread(mem_fis[page_table[index_page_table_highiest_time].index_mem_fis].total_page, PAGE_SIZE, 1, fp_backstore);
                        //e atualizamos na page_table para os 2 valores
                        //(o cont_mem_fis indexava o antigo)
                        int mem_fis_add;
                        //atualizando o antigo(primeiro coloca tanto o antigo quanto o novo para zero dps atualiza o novo):
                        for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                            if (i == index_page_table_highiest_time) { 
                                page_table[i].bit_sinal = 0;
                                mem_fis_add = page_table[i].index_mem_fis;
                                page_table[i].index_mem_fis = -1;
                                page_table[i].time = 0;
                            }
                        }




                        //atualizando o novo:
                        page_table[int_num_page].bit_sinal = 1;
                        page_table[int_num_page].index_mem_fis = mem_fis_add;//using index_page_table_highiest_time take the fis_mem index;
                        page_table[int_num_page].time = 0;

                        end_value = mem_fis[page_table[int_num_page].index_mem_fis].total_page[int_offset];
                        physical_adress = page_table[int_num_page].index_mem_fis * PAGE_SIZE + int_offset;
                        //printing results:
                        printf("3 - OFFSET %d\tVirtual address: %d Physical address: %d Value: %d\n",int_offset, virtual_adress, physical_adress, end_value);

                        //reloading counters and time
                        for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                            if (page_table[i].bit_sinal == 1)
                                page_table[i].time++;
                        }
                        cont_page_fault++;
                        cont_mem_fis++;
                        fclose(fp_backstore);
                    }
                }
            }
        }
    }


    fclose(fp);
    printf("Number of Translated Addresses = %d\n", cont_non_page_fault + cont_page_fault);
    printf("Page Faults = %d\n", cont_page_fault);
    printf("Page Fault Rate = %.3f\n", (float)cont_page_fault/(float)(cont_non_page_fault + cont_page_fault));
    if (strcmp(algorithm_tlb, "fifo") == 0) {
        printf("TLB Hits = %d\n", tlb_hit);
        printf("TLB Hits Rate = %.3f\n", tlb_hit/(float)(cont_non_page_fault + cont_page_fault));
    }
    return 0;
}