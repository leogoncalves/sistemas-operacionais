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


#define QUANTUM 4

typedef enum {HIGH_PRIORITY, LOW_PRIORITY} PRIORITY;

// typedef enum {READY, } TYPE;

typedef struct Process {
    unsigned pid;
    unsigned arrivalTime; //tempo de chegada
    unsigned burstTime; //tempo de execucao
    unsigned turnaroundTime; //tempo de resposta
    unsigned waitingTime; // tempo de espera

    PRIORITY priority;
    // int type;
    struct Process* next;
} Process;


typedef struct Queue {    
    Process *front, *rear;
} Queue;

Process *newProcess(unsigned pid, unsigned arrivalTime, unsigned burstTime, unsigned turnaroundTime, unsigned waitingTime, PRIORITY priority);
Queue* createQueue();
void enqueue(Queue *queue, unsigned pid, unsigned arrivalTime, unsigned burstTime, unsigned turnaroundTime, unsigned waitingTime, PRIORITY priority);
Process* dequeue (Queue *queue);
void testQueue();

int main (int argc, char **argv) {
    
    Queue *ReadyQueue = createQueue();
    Queue *HighPriorityCPUQueue = createQueue();
    Queue *LowPriorityCPUQueue = createQueue();

    enqueue(ReadyQueue, 0, 0, 13, 0, 0, HIGH_PRIORITY);
    enqueue(ReadyQueue, 1, 4, 11, 0, 0, HIGH_PRIORITY);
    enqueue(ReadyQueue, 2, 5, 7, 0, 0, HIGH_PRIORITY);
    enqueue(ReadyQueue, 3, 7, 8, 0, 0, HIGH_PRIORITY);
    enqueue(ReadyQueue, 4, 10, 16, 0, 0, HIGH_PRIORITY);

    while( ReadyQueue->front->next != NULL ) {
        // printf("entramos no while \n");
        // Process *tmp = (Process*)malloc(sizeof(Process));
        // tmp = ReadyQueue->front;

        // if(tmp->burstTime == QUANTUM) {
        //     //excecução completa, terminar o processo
        // } else {

        // }
        Process *tmp = (Process*)malloc(sizeof(Process));
        tmp = dequeue(ReadyQueue);
        printf("%d\n", tmp->pid);
        free(tmp);

    }

    printf("sem processo na mao chefia \n");


    return 0;
}

Process *newProcess(unsigned pid, unsigned arrivalTime, unsigned burstTime, unsigned turnaroundTime, unsigned waitingTime, PRIORITY priority) {
    Process *p = (Process*)malloc(sizeof(Process));
    p->pid = pid;    
    p->arrivalTime = arrivalTime; 
    p->burstTime = burstTime; 
    p->turnaroundTime = turnaroundTime;
    p->waitingTime = waitingTime;
    p->priority = HIGH_PRIORITY;
    return p;
}

Queue* createQueue(){
    Queue *queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

void enqueue(Queue *queue, unsigned pid, unsigned arrivalTime, unsigned burstTime, unsigned turnaroundTime, unsigned waitingTime, PRIORITY priority) {
    Process *p = newProcess(pid, arrivalTime, burstTime, turnaroundTime, waitingTime, priority);

    if(queue->rear == NULL) {
        queue->front = queue->rear = p;
    }
    queue->rear->next = p;
    queue->rear = p;
}

Process* dequeue (Queue *queue) {
    if(queue->rear == NULL) {
        return;
    }
    Process *p = queue->front;
    queue->front = queue->front->next;

    if(queue->front == NULL) {
        queue->rear = NULL;
    }
    return p;

}

void testQueue(){
    Queue* q = createQueue(); 
    // enqueue(q, 10); 
    // enqueue(q, 20); 
    // dequeue(q); 
    // dequeue(q); 
    // enqueue(q, 30); 
    // enqueue(q, 40); 
    // enqueue(q, 50); 
    // dequeue(q); 
    printf("Queue Front : %d \n", q->front->pid); 
    printf("Queue Rear : %d", q->rear->pid); 
}