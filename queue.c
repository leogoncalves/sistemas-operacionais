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
typedef enum { HIGH_PRIORITY, LOW_PRIORITY } PRIORITY;


// Define nomes para filas
typedef enum { HIGH_PRIORITY_QUEUE, LOW_PRIORITY_QUEUE, IO_QUEUE, READY_QUEUE } QUEUE_NAME;

// Define o tipo de dispositivo que solicita interrupção de IO
typedef enum { DISK, MAGNETIC_TAPE, PRINTER } DEVICE;
#define DISK_DURATION 2
#define MAGNETIC_TAPE_DURATION 4
#define PRINTER_DURATION 5


// Define o status de um processo
typedef enum { NOT_STARTED, READY, RUNNING, BLOCKED, FINISHED } STATUS;

typedef struct Process {    
    // identificador do processo
    unsigned pid;

    // PPID sempre será 1
    unsigned ppid;

    // tempo de chegada
    unsigned arrivalTime;
    
    // tempo de execucao total necessário pro processo
    unsigned totalExecutionTime; 

    // tempo do processo em processamento
    unsigned duration; 

    // tempo atualmente em cpu
    unsigned currentRunningTime;

    // Prioridade do processo
    PRIORITY priority;

    // Status do processo 
    STATUS status;

    // Lista de oprações de IO
    // momento de interrupação do processo
    unsigned IOArrivalTime; 
    
    // tipo de dispositivo
    DEVICE deviceType;
    
    // operaçao do dispositivo
    unsigned deviceOperation;

    /*
    * 0 - Interrupção não aconteceu no ciclo de relogio atual
    * 1 - Interrupção aconteceu no ciclo de relogio atual
    */
    int ioInterrupt;

    // Ponteiro para o próximo processo
    struct Process* next;
} Process;

// Estrutura de fila
typedef struct Queue {    
    Process *front, *rear;
    QUEUE_NAME name;
    int size;
} Queue;


/*
* Cria um novo processo e retorna um ponteiro para esse novo processo
*/
Process *newProcess(unsigned pid, unsigned arrivalTime, unsigned totalExecutionTime, unsigned IOArrivalTime, DEVICE deviceType);

/*
* Cria um fila vazia
*/
Queue* createQueue(QUEUE_NAME name);

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
void processStatus(STATUS processStatus);

void printQueueName(QUEUE_NAME name){
    switch(name){
        case HIGH_PRIORITY_QUEUE:
            printf("HIGH_PRIORITY_QUEUE");
            break;
        case LOW_PRIORITY_QUEUE:
            printf("LOW_PRIORITY_QUEUE");
            break;
        case IO_QUEUE:
            printf("IO_QUEUE");
            break;
        case READY_QUEUE:
            printf("READY_QUEUE");
            break;
    }
}

int main (int argc, char **argv) {
    
    Queue *ReadyQueue = createQueue(READY_QUEUE);
    Queue *HighPriorityCPUQueue = createQueue(HIGH_PRIORITY_QUEUE);
    Queue *LowPriorityCPUQueue = createQueue(LOW_PRIORITY_QUEUE);
    Queue *IOQueue = createQueue(IO_QUEUE);
    int CLOCK = 0;
    Process *RUNNING_PROCESS = NULL;

    Process *p0 = newProcess(0, 0, 13, 3, DISK);
    Process *p1 = newProcess(1, 4, 11, 5, PRINTER);
    Process *p2 = newProcess(2, 5, 7, 2, MAGNETIC_TAPE);
    Process *p3 = newProcess(3, 7, 8, 4, MAGNETIC_TAPE);
    Process *p4 = newProcess(4, 8, 16, 11, DISK);

    enqueue(ReadyQueue, p0);
    enqueue(ReadyQueue, p1);
    enqueue(ReadyQueue, p2);
    enqueue(ReadyQueue, p3);
    enqueue(ReadyQueue, p4);

    while(1) {
        printf("### CLOCK: %d ###\n", CLOCK);

        printf("TAMANHO DA FILA DE PRONTOS %d\n", ReadyQueue->size);
        if(ReadyQueue->size != 0) {
            int size = ReadyQueue->size;
            for(int i = 0; i <= size; i++ ){
                Process *tmp = dequeue(ReadyQueue);
                if(tmp->arrivalTime == CLOCK) {                    
                    tmp->status = READY;
                    printf(" ENTROU \n ");
                    processStatus(tmp->status);
                    printf(" \n ");
                    enqueue(HighPriorityCPUQueue, tmp);
                } else {
                    enqueue(ReadyQueue, tmp);
                }
            }
        }
        printf("[LOG] Passamos pela fila de novos \n");

        if(!RUNNING_PROCESS) {
            printf("[MAIN] SELEÇÃO PRA CPU\n");
            if(HighPriorityCPUQueue->size != 0) {
                RUNNING_PROCESS = dequeue(HighPriorityCPUQueue);                
            } else if(LowPriorityCPUQueue->size != 0) {
                RUNNING_PROCESS = dequeue(LowPriorityCPUQueue);
            }
        } 

        
        if(RUNNING_PROCESS) {
            printf("[MAIN] RODANDO CPU\n");
            RUNNING_PROCESS->duration += 1;
            RUNNING_PROCESS->currentRunningTime += 1;
            
            if(RUNNING_PROCESS->duration == RUNNING_PROCESS->IOArrivalTime) {
                printf("[IO_INTERRUPT] DURATION == IO_ARRIVAL_TIME\n");
                RUNNING_PROCESS->status = BLOCKED;
                RUNNING_PROCESS->ioInterrupt = 1;
                RUNNING_PROCESS->currentRunningTime = 0;
                printf("[IO_INTERRUPT] PROCESSO SERA ENFILEIRADO NA FILA DE IO\n");
                enqueue(IOQueue, RUNNING_PROCESS);
                printf("[IO_INTERRUPT] PROCESSO ENFILEIRADO NA FILA DE IO\n");
                RUNNING_PROCESS = NULL;
            } else if(RUNNING_PROCESS->currentRunningTime == QUANTUM) {
                printf("[QUANTUM_INTERRUPT] CURRENT RUNNING TIME == QUANTUM\n");
                RUNNING_PROCESS->status = READY;
                RUNNING_PROCESS->currentRunningTime = 0;
                RUNNING_PROCESS->priority = LOW_PRIORITY;
                printf("[QUANTUM_INTERRUPT] PROCESSO SERA ENFILEIRADO NA FILA DE BAIXA PRIORIDADE\n");
                enqueue(LowPriorityCPUQueue, RUNNING_PROCESS);
                printf("[QUANTUM_INTERRUPT] PROCESSO ENFILEIRADO NA FILA DE BAIXA PRIORIDADE\n");
                RUNNING_PROCESS = NULL;
            } else if(RUNNING_PROCESS->duration == RUNNING_PROCESS->totalExecutionTime) {
                printf("[FINISHED_INTERRUPT] DURATION == TOTAL_EXECUTION_TIME\n");
                RUNNING_PROCESS->status = FINISHED;
                RUNNING_PROCESS->currentRunningTime = 0;
                printf("[FINISHED_INTERRUPT] PROCESSO PID %d FINALIZADO", RUNNING_PROCESS->pid);
                RUNNING_PROCESS = NULL;
            }
        }

        printf("[MAIN] TAMANHO DA FILA DE IO %d\n", IOQueue->size);
        if(IOQueue->size != 0) {
            int IOsize = IOQueue->size;
            for(int i = 0; i < IOsize; i++ ) {
                printf("[IO] VAMOS DESENFILEIRAR PROCESSO DA FILA DE IO\n");                
                Process *tmp = dequeue(IOQueue);
                printf("\n[IO] DESENFILEIRAMOS PROCESSO DA FILA DE IO\n");
                if(tmp->ioInterrupt == 0){
                    tmp->deviceOperation += 1;

                    switch(tmp->deviceType){
                        case MAGNETIC_TAPE:
                            if(tmp->deviceOperation == MAGNETIC_TAPE_DURATION) {
                                printf("[MAGNETIC_TAPE_DURATION] DEVICE_OPERATION == MAGNETIC_TAPE_DURATION");
                                tmp->status = READY;
                                RUNNING_PROCESS->priority = HIGH_PRIORITY;
                                printf("[MAGNETIC_TAPE_DURATION] VAMOS ENFILEIRAR PROCESSO TMP NA FILA DE ALTA");
                                enqueue(HighPriorityCPUQueue, tmp);
                                printf("[MAGNETIC_TAPE_DURATION] ENFILEIRAMOS PROCESS TMP NA FILA DE ALTA");
                            } else {
                                printf("[MAGNETIC_TAPE_DURATION] VAMOS ENFILEIRAR PROCESSO TMP NA FILA DE IO");
                                enqueue(IOQueue, tmp);
                                printf("[MAGNETIC_TAPE_DURATION] ENFILEIRAMOS PROCESSO TMP NA FILA DE IO");
                            }
                            break;
                        case PRINTER:
                            if(tmp->deviceOperation == PRINTER_DURATION) {
                                printf("[PRINTER_DURATION] DEVICE_OPERATION == PRINTER_DURATION");
                                tmp->status = READY;
                                RUNNING_PROCESS->priority = HIGH_PRIORITY;
                                printf("[PRINTER_DURATION] VAMOS ENFILEIRAR O PROCESSO NA FILA DE ALTA");
                                enqueue(HighPriorityCPUQueue, tmp);
                                printf("[PRINTER_DURATION] ENFILEIRAMOS O PROCESSO NA FILA DE ALTA");
                            } else {
                                printf("[PRINTER_DURATION] VAMOS ENFILEIRAR O PROCESSO NA FILA DE IO");
                                enqueue(IOQueue, tmp);
                                printf("[PRINTER_DURATION] ENFILEIRAMOS O PROCESSO NA FILA DE IO");
                            }
                            break;
                        case DISK:
                            if(tmp->deviceOperation == DISK_DURATION) {
                                printf("[DISK_DURATION] DEVICE_OPERATION == DISK_DURATION");
                                tmp->status = READY;
                                RUNNING_PROCESS->priority = LOW_PRIORITY;
                                printf("[DISK_DURATION] VAMOS ENFILEIRAR O PROCESSO NA FILA DE BAIXA");
                                enqueue(LowPriorityCPUQueue, tmp);
                                printf("[DISK_DURATION] ENFILEIRAMOS O PROCESSO NA FILA DE BAIXA");
                            } else {
                                printf("[DISK_DURATION] VAMOS ENFILEIRAR O PROCESSO NA FILA DE IO");
                                enqueue(IOQueue, tmp);
                                printf("[DISK_DURATION] ENFILEIRAMOS O PROCESSO NA FILA DE IO");
                            }
                            break;
                    }                    
                } else {
                    tmp->ioInterrupt = 0;
                    enqueue(IOQueue, tmp);

                }
                
            }
        }

        CLOCK++;
    }
    printf("FIM\n");

    // printf("#### DADOS DO PROCESSO ####\n");
    // printProcess(p0);
    // printProcess(p1);
    // printProcess(p2);
    // printProcess(p3);
    // printProcess(p4);
    return 0;
}

Process *newProcess(
    unsigned pid, 
    unsigned arrivalTime, 
    unsigned totalExecutionTime, 
    unsigned IOArrivalTime, 
    DEVICE deviceType) {

    Process *p = (Process*)malloc(sizeof(Process));
    
    p->pid = pid;
    p->ppid = 1;    
    p->arrivalTime = arrivalTime; 
    p->totalExecutionTime = totalExecutionTime; 
    p->duration = 0; 
    p->currentRunningTime = 0;
    p->IOArrivalTime = IOArrivalTime;
    p->priority = HIGH_PRIORITY;
    p->deviceType = deviceType;
    p->status = NOT_STARTED;
    p->deviceOperation = 0;
    p->ioInterrupt = 0;

    printf("[LOG] Processo de PID %d iniciado\n", pid);
    return p;
}

Queue* createQueue(QUEUE_NAME name){
    Queue *queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    queue->size = 0;
    queue->name = name;
    return queue;
}

void enqueue(Queue *queue, Process *p) {

    if(queue->rear == NULL) {
        queue->front = queue->rear = p;
    }
    queue->rear->next = p;
    queue->rear = p;
    queue->size += 1;
}

Process* dequeue (Queue *queue) {
    if(queue->rear == NULL) {
        return NULL;
    }
    Process *p = queue->front;
    queue->front = queue->front->next;

    if(queue->front == NULL) {
        queue->rear = NULL;
    }
    queue->size -= 1;
    return p;

}

void printProcess(Process *p) {  
    // printf("pid , ppid , arrivalTime , totalExecutionTime , duration , currentRunningTime , priority , status , IOArrivalTime , type , deviceOperation \n");
    printf("%d ", p->pid);
    printf("%d ", p->ppid);
    printf("%d ", p->arrivalTime);
    printf("%d ", p->totalExecutionTime);
    printf("%d ", p->duration);
    printf("%d ", p->currentRunningTime);
    printPriority(p->priority);
    processStatus(p->status);
    printf("%d ", p->IOArrivalTime);
    printDeviceType(p->deviceType);
    printf("%d ", p->deviceOperation);
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

void processStatus(STATUS processStatus){
    switch(processStatus) {
        case NOT_STARTED:
            printf("NAO INICIADO");
            break;
        case READY:
            printf("PRONTO");
            break;
        case RUNNING:
            printf("EXECUTANDO");
            break;
        case BLOCKED:
            printf("BLOQUEADO");
            break;
        case FINISHED:
            printf("FINALIZADO");
            break;
    }
};
