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

// Define o quantum utilizado no algoritmo de RR
#define QUANTUM 4

// Define a prioridade de um processo (usado para atribuir o processo a fila correta)
typedef enum { HIGH_PRIORITY = 1, LOW_PRIORITY = 0 } PRIORITY;

// Define o tipo de dispositivo que solicita interrupção de IO
typedef enum { DISK = 98, MAGNETIC_TAPE = 99, PRINTER = 100 } DEVICE;

typedef enum { 
    READ_DISK, WRITE_DISK, 
    READ_MAGNETIC_TAPE, WRITE_MAGNETIC_TAPE, 
    READ_PRINTER, WRITE_PRINTER,
} DEVICE_OPERATION;

// Define o status de um processo
typedef enum { NOT_STARTED, READY, RUNNING, BLOCKED } STATUS;

typedef struct Process {    
    // identificador do processo
    unsigned pid;

    // PPID sempre será 1
    unsigned ppid;

    // tempo de chegada
    unsigned arrivalTime;
    
    // tempo de execucao
    unsigned burstTime; 

    /*
    * Turnaround sempre calculado com base 
    * no Tempo total gasto pelo processo menos o 
    * Tempo de chegada do processo na fila de prontos 
    * pela primeira vez
    */
    unsigned turnaroundTime;

    // tempo de espera
    unsigned waitingTime;

    // Prioridade do processo
    PRIORITY priority;

    // Status do processo 
    STATUS status;

    // Lista de oprações de IO
    // momento de interrupação do processo
    unsigned IOArrivalTime; 
    
    // tipo de dispositivo
    DEVICE type;
    
    //status (iniciado, rodando, terminado)
    DEVICE_OPERATION operation;

    // Ponteiro para o próximo processo
    struct Process* next;
} Process;

// Estrutura de fila
typedef struct Queue {    
    Process *front, *rear;
} Queue;


/*
* Cria um novo processo e retorna um ponteiro para esse novo processo
*/
Process *newProcess(unsigned pid, unsigned arrivalTime, unsigned burstTime, unsigned turnaroundTime, unsigned waitingTime, PRIORITY priority);

/*
* Cria um fila vazia
*/
Queue* createQueue();

/*
* Enfileira um processo em uma fila
*/
void enqueue(Queue *queue, Process* p);

/*
* Remove o processo do final da fila e o retorna;
*/
Process* dequeue (Queue *queue);

/*
* Teste para checar funcionamento da fila
*/
void testQueue();

/*
* Exibe as informações de um processo
*/
void printProcess(Process *p);

/*
* Prints
*/
void printPriority(PRIORITY priority);
void printDeviceType(DEVICE deviceType);
void IODeviceOperation(DEVICE_OPERATION operation);
void processStatus(STATUS processStatus);


int main (int argc, char **argv) {
    
    Queue *ReadyQueue = createQueue();
    Queue *HighPriorityCPUQueue = createQueue();
    Queue *LowPriorityCPUQueue = createQueue();
    Queue *IOQueue = createQueue();

    Process *p0 = newProcess(0, 0, 13, 0, 0, HIGH_PRIORITY);
    Process *p1 = newProcess(1, 4, 11, 0, 0, HIGH_PRIORITY);
    Process *p2 = newProcess(2, 5, 7, 0, 0, HIGH_PRIORITY);
    Process *p3 = newProcess(3, 7, 8, 0, 0, HIGH_PRIORITY);
    Process *p4 = newProcess(4, 10, 16, 0, 0, HIGH_PRIORITY);

    printf("#### DADOS DO PROCESSO ####\n");
    printProcess(p0);
    printProcess(p1);
    printProcess(p2);
    printProcess(p3);
    printProcess(p4);


    enqueue(ReadyQueue, p0);
    enqueue(ReadyQueue, p1);
    enqueue(ReadyQueue, p2);
    enqueue(ReadyQueue, p3);
    enqueue(ReadyQueue, p4);

    

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

void enqueue(Queue *queue, Process *p) {

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

void enqueueProcessAfterIO(Queue *highPriority, Queue *lowPriority, Process* p) {
    switch(p->type) {
        case(DISK):
            enqueue(lowPriority, p);
            break;
        case(MAGNETIC_TAPE):
        case(PRINTER):
            enqueue(highPriority, p);
            break;
    }
}

void printProcess(Process *p) {
    
    printf("PID\t PPID\t ARRIVAL_TIME\t BURST_TIME\t TURNAROUND_TIME\t WAITING_TIME\t PRIORIDADE\t STATUS\t IO_ARRIVAL_TIME\t DEVICE_TYPE\t OPERATION_TYPE\t  \n");
    printf("%d\t", p->pid);
    printf("%d\t", p->ppid);
    printf("%d\t", p->arrivalTime);
    printf("%d\t", p->burstTime);
    printf("%d\t", p->turnaroundTime);
    printf("%d\t", p->waitingTime);
    printPriority(p->priority);
    processStatus(p->status);
    printf("%3d\t", p->IOArrivalTime);
    printDeviceType(p->type);
    IODeviceOperation(p->operation);
}

void printPriority(PRIORITY priority) {
    switch(priority) {
        case HIGH_PRIORITY:
            printf("ALTA PRIORIDADE \t");
            break;
        case LOW_PRIORITY:
            printf("ALTA PRIORIDADE \t");
            break;
        default:
            printf("-\t");
            break;
    }
};

void printDeviceType(DEVICE deviceType){
    switch(deviceType) {
        case DISK:
            printf("DISCO");
            break;
        case MAGNETIC_TAPE:
            printf("FITA MAGNÉTICA");
            break;
        case PRINTER:
            printf("IMPRESSORA");
            break;
    }
};

void IODeviceOperation(DEVICE_OPERATION operation){
    switch(operation) {
        case READ_DISK:
            printf("LEITURA DO DISCO \t");
            break;
        case WRITE_DISK:
            printf("ESCRITA NO DISCO \t");
            break;
        case READ_MAGNETIC_TAPE:
            printf("LEITURA DA FITA MAGNÉTICA \t");
            break;
        case WRITE_MAGNETIC_TAPE:
            printf("ESCRITA NA FITA MAGNÉTICA \t");
            break;
        case READ_PRINTER:
            printf("LEITURA DA IMPRESSORA \t");
            break;
        case WRITE_PRINTER:
            printf("ESCRITA DA IMPRESSORA \t");
            break;
        default:
            printf("-\t");
            break;
    }
};

void processStatus(STATUS processStatus){
    switch(processStatus) {
        case NOT_STARTED:
            printf("NAO INICIADO \t");
            break;
        case READY:
            printf("PRONTO \t");
            break;
        case RUNNING:
            printf("EXECUTANDO \t");
            break;
        case BLOCKED:
            printf("BLOQUEADO \t");
            break;
        default:
            printf("-\t");
            break;
    }
};
