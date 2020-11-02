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

#define QUANTUM 5
#define MAX_PROCESS_QUANTITY 10
#define DISK_READ 1
#define DISK_WRITE 2
#define MAGNETIC_TAPE_READ 2
#define MAGNETIC_TAPE_WRITE 3
#define PRINTER_READ 3
#define PRINTER_WRITE 4

typedef struct Process{
    int pid;
    int priority;
    int start;
    int duration;
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
void removeProcess(Queue* Q);
int isEmptyQueue(Queue* Q);
int isFullQueue(Queue* Q);
Process createProcess(int pid, int priority, int start, int duration);

int main (int argc, char **argv) {

    int TOTAL_TIME = 0;

    Queue AltaPrioridadeCPU;
    Queue BaixaPrioridadeCPU;
    Queue IO;

    createQueue(&AltaPrioridadeCPU, 10);
    createQueue(&BaixaPrioridadeCPU, 10);
    createQueue(&IO, 10);

    Process p0 = createProcess(1,1,0,10);
    Process p1 = createProcess(1,1,5,20);
    insertProcess(&AltaPrioridadeCPU, p0);
    insertProcess(&AltaPrioridadeCPU, p1);

    while(AltaPrioridadeCPU.numberOfItems != 0) {
        
    }

        

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

void removeProcess(Queue* Q) {    
    if(Q->first == Q->capacity) {
        Q->first = 0;
    }
    Q->numberOfItems--;    
}

int isEmptyQueue(Queue* Q) {
    return (Q->numberOfItems == 0);
}

int isFullQueue(Queue* Q) {
    return (Q->numberOfItems == Q->capacity);
}

Process createProcess(int pid, int priority, int start, int duration) {
    Process p;
    p.pid = pid;
    p.priority = priority;
    p.start = start;
    p.duration = duration;
    return p;
}

