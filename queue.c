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


typedef struct Process {
    unsigned pid;
    struct Process* next;
} Process;


typedef struct Queue {    
    Process *front, *rear;
} Queue;

Process *newProcess(unsigned pid);
Queue* createQueue();
void enqueue(Queue *queue, unsigned pid);
void dequeue (Queue *queue);


int main (int argc, char **argv) {
    
    Queue* q = createQueue(); 
    enqueue(q, 10); 
    enqueue(q, 20); 
    dequeue(q); 
    dequeue(q); 
    enqueue(q, 30); 
    enqueue(q, 40); 
    enqueue(q, 50); 
    dequeue(q); 
    printf("Queue Front : %d \n", q->front->pid); 
    printf("Queue Rear : %d", q->rear->pid); 

    return 0;
}

Process *newProcess(unsigned pid) {
    Process *p = (Process*)malloc(sizeof(Process));
    p-> pid = pid;
    return p;
}

Queue* createQueue(){
    Queue *queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

void enqueue(Queue *queue, unsigned pid) {
    Process *p = newProcess(pid);

    if(queue->rear == NULL) {
        queue->front = queue->rear = p;
    }
    queue->rear->next = p;
    queue->rear = p;
}

void dequeue (Queue *queue) {
    if(queue->rear == NULL) {
        return;
    }
    Process *p = queue->front;
    queue->front = queue->front->next;

    if(queue->front == NULL) {
        queue->rear = NULL;
    }
    free(p);

}