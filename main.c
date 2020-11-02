#include <stdio.h>
#include <stdlib.h>

/*
* Limite máximo de processo criados
* Quantum
* Tempos de serviço de I/O para cada processo
* Tempo de duração de cada tipo de I/O
* Gerencia de processo (pid, prioridade, ppid, status)
* 3 filas (1 de alta prioridade e 1 de baixa prioridade pra CPU | 1 fila de I/O)
* 3 tipos de I/O (Disco - Baixa Prioridade | Fita Magnetica - Alta Prioridade | Impressora - Alta Prioridade)
* Ordem de entrada na fila de prontos: 
* - Processos novos: fila de alta prioridade
* - Processos que retornam de I/O: depende do tipo de I/O
* - Processos que sofrem preempção: fila de baixa prioridade
*/

typedef struct Process{
    int pid;
    int prioridade;
    // int execucao;
    // int executado;
    // int chegada;
    // int espera;
    // int deadline;
    // int turnaround;
} Process;


typedef struct Queue {
    int capacity;
    Process* processList;
    int first;
    int last;
    int numberOfItems;
} Queue;

void createQueue(Queue* Q, int capacity);
void insertProcess(Queue* Q, Process p);
Process removeProcess(Queue* Q);
int isEmptyQueue(Queue* Q);
int isFullQueue(Queue* Q);


int main (int argc, char **argv) {

    Queue AltaPrioridadeCPU;
    Queue BaixaPrioridadeCPU;
    Queue IO;

    createQueue(&AltaPrioridadeCPU, 10);
    createQueue(&BaixaPrioridadeCPU, 10);
    createQueue(&IO, 10);

    return 0;
}

void createQueue(Queue* Q, int capacity) {
    Q->capacity = capacity;
    Q->processList = (Process*) malloc(Q->capacity * sizeof(Process));  
    Q->first = 0;
    Q->last = -1;
    Q->numberOfItems = 0;
}

void insertProcess(Queue* Q, Process p) {
    if(Q->last == Q->capacity - 1) {
        Q->last = -1;
    }
    Q->last++;
    Q->processList[Q->last] = p;
    Q->numberOfItems++;
}

Process removeProcess(Queue* Q) {
    Process temp = Q->processList[Q->capacity];
    if(Q->first == Q->capacity) {
        Q->first = 0;
    }
    Q->numberOfItems--;
    return temp;
}

int isEmptyQueue(Queue* Q) {
    return (Q->numberOfItems == 0);
}

int isFullQueue(Queue* Q) {
    return (Q->numberOfItems == Q->capacity);
}

void RR()