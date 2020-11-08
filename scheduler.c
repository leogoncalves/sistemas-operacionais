#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

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

/**
*
* Define o valor do QUANTUM que será utilizado pelo 
* algoritmo de Round Robin na simulação.
*
**/
#define QUANTUM 4

/**
*
* Define a duração das operações de dispositivos de I/O. 
*
**/
#define DISK_DURATION 2
#define MAGNETIC_TAPE_DURATION 4
#define PRINTER_DURATION 5

/**
*
* Define a prioridade de um processo 
* (utilizado para atribuir o processo a fila correta)
*
**/
typedef enum { HIGH_PRIORITY, LOW_PRIORITY } PRIORITY;


/**
*
* Define os tipos de dispositivo de entrada e saída disponíveis
*
**/
typedef enum { DISK = 997, MAGNETIC_TAPE = 998, PRINTER = 999 } DEVICE;

/**
*
* Define os status de um processo
*
**/
typedef enum { NOT_STARTED, READY, RUNNING, BLOCKED, FINISHED } STATUS;

/**
*
*
* Define a estrutura que iremos utiliar para definir um processo.
*
*
**/
typedef struct Process {    
    // identificador do processo
    unsigned pid;

    // PPID será sempre definido como 1 
    // simulando que o processo seja filho do init
    unsigned ppid;

    // Tempo de chegada
    unsigned arrivalTime;
    
    // Tempo de execucao total necessário pro processo
    unsigned totalExecutionTime; 

    // Tempo do processo em processamento
    unsigned duration; 

    // Tempo que o processo ficou em CPU 
    unsigned currentRunningTime;

    // Prioridade do processo
    PRIORITY priority;

    // Status do processo 
    STATUS status;

    // Momento de interrupação do processo por dispositivo de I/O
    unsigned IOArrivalTime; 
    
    // Tipo de dispositivo de I/O
    DEVICE deviceType;
    
    // Tempo de operaçao do dispositivo
    unsigned deviceOperation;

    /*
    * 0 - Interrupção não aconteceu no ciclo de relogio atual
    * 1 - Interrupção aconteceu no ciclo de relogio atual
    */
    int ioInterrupt;

    // Ponteiro para o próximo processo
    struct Process* next;
} Process;


/*
*
* Define a estrutura para uma fila circular
*/
typedef struct Queue {    
    Process *front, *rear;
    int size;
} Queue;


/*
*
* Semáfora para controle de 
* utilização de dispositivo de I/O
*
*/
typedef struct Semaphore {
    int flag;
    DEVICE deviceType;
} Semaphore;

/*
* Cria um novo processo e retorna um ponteiro para esse processo criado
*/
Process *newProcess(unsigned pid, unsigned arrivalTime, unsigned totalExecutionTime, unsigned IOArrivalTime, DEVICE deviceType);

/*
*
* Cria um fila vazia
*
*/
Queue* createQueue();

/*
*
* Adicionar um processo na fila
*
*/
void enqueue(Queue *queue, Process* p);

/*
* Remove um processo da fila e retorna o ponteiro para o processo removido;
*/
Process* dequeue (Queue *queue);

/*
* Exibe as informações de um processo
*/
void printProcess(Process *p);

/*
*
* Função auxiliar para exibir a prioridade de um processo
*
*/
void printPriority(PRIORITY priority);

/*
*
* Função auxiliar para imprimir uma fila
*
*/
void printQueue(Queue* q);

/*
*
* Função auxiliar para imprimir o tipo de
* dispositivo utilizado no processo
*/
void printDeviceType(DEVICE deviceType);

/*
*
* Função auxiliar para imprimir o status de um processo
*
*/
void processStatus(STATUS processStatus);

/*
*
* Cria uma determinada quantidade de processos com valores
* aleatórios.
*
*/
void createRandomProcess(Queue* queue, int max_process_number);

/*
*
*
*/
Semaphore* createSemaphore(DEVICE deviceType);

/*
*
*
*/
void blockSemaphore(Semaphore* sem);

/*
*
*
*/
int checkSemaphore(Semaphore* sem);

/*
*
*
*/
void releaseSemaphore(Semaphore* sem);


/*
*
* Verifica se todos os processos terminaram de ser executados
*
*/
int allTerminatedProcess(Queue *high, Queue *low, Queue *io, Queue *ready, Process* p) {
    if(high->size == 0 && low->size == 0 && io->size == 0 && ready->size == 0 && p == NULL) {
        return 1;
    }
    return 0;
}

/*
*
* Executa o escalonador com processos pré-definidos
*
*/
void simulation_test(Queue* queue);

int main (int argc, char **argv) {
    
    /*
    * Define as filas que serão utilizadas
    */
    Queue *ReadyQueue = createQueue();
    Queue *HighPriorityCPUQueue = createQueue();
    Queue *LowPriorityCPUQueue = createQueue();
    Queue *IOQueue = createQueue();

    /*
    * Define os semáforos que serão utiliados
    */
    Semaphore* sem_DISK = createSemaphore(DISK);
    Semaphore* sem_MAGNETIC_TAPE = createSemaphore(MAGNETIC_TAPE);
    Semaphore* sem_PRINTER = createSemaphore(PRINTER);

    /*
    *
    *
    */
    int CLOCK = 0;

    /*
    * Processo em execução na CPU
    *
    */
    Process *RUNNING_PROCESS = NULL;

    /*
    * Inicializa semente utilizada para 
    * definir números aleatórios utilizados
    * na criação de novos processos
    */
    srand(time(NULL));
    
    int total_process;

    scanf("%d", &total_process);

    if(total_process == 0) {
        simulation_test(ReadyQueue);    
    } else {
        createRandomProcess(ReadyQueue, total_process);
    }
    
    while(!allTerminatedProcess(ReadyQueue, HighPriorityCPUQueue, LowPriorityCPUQueue, IOQueue, RUNNING_PROCESS)) {
        
        // Adiciona esperar no processo de 0.5s        
        // usleep(500000);
        
        printf("\n\n### CLOCK: %d ###\n\n", CLOCK);

        if(ReadyQueue->size != 0) {
            printf("TAMANHO DA FILA DE NOVOS %d\n", ReadyQueue->size);
            int size = ReadyQueue->size;
            
            for(int i = 0; i < size; i++ ){
                Process *tmp = dequeue(ReadyQueue);
                if(tmp->arrivalTime == CLOCK) {                    
                    tmp->status = READY;
                    printf("[MAIN] Processo de pid %d entrou na fila de alta prioridade \n", tmp->pid);
                    enqueue(HighPriorityCPUQueue, tmp);
                } else {
                    enqueue(ReadyQueue, tmp);
                }
            }
        }
        printf("[MAIN] VERIFICADA SE HOUVE NOVOS PROCESSOS \n");

        if(!RUNNING_PROCESS) {
            printf("[CPU] SELEÇÃO PRA CPU\n");

            if(HighPriorityCPUQueue->size != 0) {
                RUNNING_PROCESS = dequeue(HighPriorityCPUQueue);                
                printf("[CPU] Selecionado processo de pid %d da fila de alta prioridade \n", RUNNING_PROCESS->pid);
            } else if(LowPriorityCPUQueue->size != 0) {
                RUNNING_PROCESS = dequeue(LowPriorityCPUQueue);
                printf("[CPU] Selecionado processo de pid %d da fila de baixa prioridade \n", RUNNING_PROCESS->pid);
            } else {
                printf("[CPU] NENHUM PROCESSO DISPONÍVEL PARA EXECUCAO \n");
            }
        } 

        
        if(RUNNING_PROCESS) {
            printf("[MAIN] RODANDO PROCESSO DE PID %d NA CPU\n", RUNNING_PROCESS->pid);
            RUNNING_PROCESS->duration += 1;
            RUNNING_PROCESS->currentRunningTime += 1;
            
            if(RUNNING_PROCESS->duration == RUNNING_PROCESS->totalExecutionTime) {
                printf("[FINISHED_INTERRUPT] processo de pid %d FINALIZANDO\n", RUNNING_PROCESS->pid);
                RUNNING_PROCESS->status = FINISHED;
                RUNNING_PROCESS->currentRunningTime = 0;
                printf("[FINISHED_INTERRUPT] PROCESSO PID %d FINALIZADO\n", RUNNING_PROCESS->pid);
                RUNNING_PROCESS = NULL;
            } else if(RUNNING_PROCESS->duration == RUNNING_PROCESS->IOArrivalTime) {                
                if(RUNNING_PROCESS->IOArrivalTime != 0) {
                    printf("[IO_INTERRUPT] processo de pid %d interrompido por chamada de IO\n", RUNNING_PROCESS->pid);                
                    RUNNING_PROCESS->status = BLOCKED;
                    RUNNING_PROCESS->ioInterrupt = 1;
                    RUNNING_PROCESS->currentRunningTime = 0;
                    enqueue(IOQueue, RUNNING_PROCESS);
                    printf("[IO_INTERRUPT] PROCESSO ENFILEIRADO NA FILA DE IO\n");
                    RUNNING_PROCESS = NULL;
                }                
            }
            else if(RUNNING_PROCESS->currentRunningTime == QUANTUM) {
                printf("[QUANTUM_INTERRUPT] processo de pid %d interrompido por QUANTUM\n", RUNNING_PROCESS->pid);
                RUNNING_PROCESS->status = READY;
                RUNNING_PROCESS->currentRunningTime = 0;
                RUNNING_PROCESS->priority = LOW_PRIORITY;
                enqueue(LowPriorityCPUQueue, RUNNING_PROCESS);
                printf("[QUANTUM_INTERRUPT] PROCESSO ENFILEIRADO NA FILA DE BAIXA PRIORIDADE\n");
                RUNNING_PROCESS = NULL;
            } 
        }

        if(IOQueue->size != 0) {
            printf("[IO] VERIFICANDO FILA DE IO \n");
            int IOsize = IOQueue->size;
            for(int i = 0; i < IOsize; i++ ) {
                Process *tmp = dequeue(IOQueue);
                if(tmp->ioInterrupt == 0){
                    printf("[IO] ANALISANDO PROCESSO DE PID %d DA FILA DE IO\n", tmp->pid);
                    switch(tmp->deviceType){
                        case MAGNETIC_TAPE:
                            if(checkSemaphore(sem_MAGNETIC_TAPE)){
                                blockSemaphore(sem_MAGNETIC_TAPE);
                                tmp->deviceOperation += 1;
                            }
                            if(tmp->deviceOperation == MAGNETIC_TAPE_DURATION) {
                                printf("[MAGNETIC_TAPE] PROCESSO DE PID %d FINALIZOU CHAMADA DE IO\n", tmp->pid);
                                tmp->status = READY;                            
                                tmp->priority = HIGH_PRIORITY;
                                enqueue(HighPriorityCPUQueue, tmp);
                                printf("[MAGNETIC_TAPE] ENFILEIRANDO PROCESSO DE PID %d NA FILA DE ALTA PRIORIDADE\n", tmp->pid);
                            } else {
                                enqueue(IOQueue, tmp);
                                printf("[MAGNETIC_TAPE] PROCESSO DE PID %d AINDA EM CHAMADA DE IO\n", tmp->pid);
                            }
                            break;
                        case PRINTER:
                            if(checkSemaphore(sem_PRINTER)){
                                blockSemaphore(sem_PRINTER);
                                tmp->deviceOperation += 1;
                            }
                            if(tmp->deviceOperation == PRINTER_DURATION) {
                                printf("[PRINTER] PROCESSO DE PID %d FINALIZOU CHAMADA DE IO\n", tmp->pid);
                                tmp->status = READY;
                                tmp->priority = HIGH_PRIORITY;
                                enqueue(HighPriorityCPUQueue, tmp);
                                printf("[PRINTER] ENFILEIRANDO PROCESSO DE PID %d NA FILA DE ALTA PRIORIDADE\n", tmp->pid);
                            } else {
                                enqueue(IOQueue, tmp);
                                printf("[PRINTER] PROCESSO DE PID %d AINDA EM CHAMADA DE IO\n", tmp->pid);
                            }
                            break;
                        case DISK:
                            if(checkSemaphore(sem_DISK)){
                                blockSemaphore(sem_DISK);
                                tmp->deviceOperation += 1;
                            }
                            if(tmp->deviceOperation == DISK_DURATION) {
                                printf("[DISK] PROCESSO DE PID %d FINALIZOU CHAMADA DE IO\n", tmp->pid);
                                tmp->status = READY;
                                tmp->priority = LOW_PRIORITY;
                                enqueue(LowPriorityCPUQueue, tmp);
                                printf("[DISK] ENFILEIRANDO PROCESSO DE PID %d NA FILA DE BAIXA PRIORIDADE\n", tmp->pid);
                            } else {
                                enqueue(IOQueue, tmp);
                                printf("[DISK] PROCESSO DE PID %d AINDA EM CHAMADA DE IO\n", tmp->pid);
                            }
                            break;
                    }                    
                } else {
                    tmp->ioInterrupt = 0;
                    enqueue(IOQueue, tmp);
                    printf("[IO] PROCESSO DE PID %d ENTRANDO NA FILA DE IO\n", tmp->pid);

                }
                
            }
            releaseSemaphore(sem_DISK);
            releaseSemaphore(sem_MAGNETIC_TAPE);
            releaseSemaphore(sem_PRINTER);
        }

        CLOCK++;
    }
    printf("\nFIM DA SIMULACAO\n");
    return 0;
}

/*
* Implementação das funções 
* utilizadas para o desenvolvimento 
* do simulador
*
*/

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

Queue* createQueue(){
    Queue *queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    queue->size = 0;
    return queue;
}

void enqueue(Queue *queue, Process *p) {

    if(queue->rear == NULL) {
        queue->front = queue->rear = p;
        queue->front->next = queue->rear;
    } else {
        p->next = queue->front;
        queue->rear->next = p;
        queue->rear = p;
    }    
    queue->size += 1;
}

void printQueue(Queue* q) {
    int size = q->size;
    Process *p = q->front;
    for(int i=0; i<size; i++) {
        printf("POSICAO %d: %d -> %d \n", i, p->pid, p->next->pid);
        p = p->next;
    }
    if(q->size == 0 && q->front != NULL) {
        printf("POSICAO NULL: %d \n", q->front->pid);
    }
}

Process* dequeue (Queue *queue) {
    Process *p;
    if(queue->rear == NULL) {
        return NULL;
    } else {
        p = queue->front;
        queue->front = queue->front->next;
        queue->rear->next = queue->front;
        queue->size -= 1;
        if(queue->size == 0) {
            queue->front = queue->rear = NULL;
        }
    }
    return p;

}

void printProcess(Process *p) {  
    printf("| PID | PPID | ArT | TET | DUR | CRT | PRIORITY        | STATUS      | IO_AT | DEVICE TYPE   | DEVICE OPEREATION |\n");
    printf("| %3d ", p->pid);
    printf("| %3d  ", p->ppid);
    printf("| %3d ", p->arrivalTime);
    printf("| %3d ", p->totalExecutionTime);
    printf("| %3d ", p->duration);
    printf("| %3d ", p->currentRunningTime);
    printPriority(p->priority);
    processStatus(p->status);
    printf("| %3d   ", p->IOArrivalTime);
    printDeviceType(p->deviceType);
    printf("| %3d               |", p->deviceOperation);
    printf("\n");
}

void printPriority(PRIORITY priority) {
    switch(priority) {
        case HIGH_PRIORITY:
            printf("| ALTA PRIORIDADE ");
            break;
        case LOW_PRIORITY:
            printf("| BAIXA PRIORIDADE ");
            break;
    }
};

void printDeviceType(DEVICE deviceType){
    switch(deviceType) {
        case DISK:
            printf("| DISCO         ");
            break;
        case MAGNETIC_TAPE:
            printf("| FITA MAGNÉTICA");
            break;
        case PRINTER:
            printf("| IMPRESSORA    ");
            break;
    }
};

void processStatus(STATUS processStatus){
    switch(processStatus) {
        case NOT_STARTED:
            printf("| NAO INICIADO");
            break;
        case READY:
            printf("| PRONTO      ");
            break;
        case RUNNING:
            printf("| EXECUTANDO  ");
            break;
        case BLOCKED:
            printf("| BLOQUEADO   ");
            break;
        case FINISHED:
            printf("| FINALIZADO  ");
            break;
    }
};

Semaphore* createSemaphore(DEVICE deviceType){
    Semaphore* sem = (Semaphore*)malloc(sizeof(Semaphore));
    sem->flag = 1;
    sem->deviceType = deviceType;
    
    return sem;
}

void blockSemaphore(Semaphore* sem){
    sem->flag = 0;
}

void releaseSemaphore(Semaphore* sem){
    sem->flag = 1;
}

int checkSemaphore(Semaphore* sem){
    return sem->flag;
}

int randomInRange(int min, int max) {
    int random = rand();
    return random % (max + 1 - min) + min;
}

void createRandomProcess(Queue* queue, int max_process_number) {
        
    // int number_of_process = randomInRange(1, max_process_number);
    printf("TOTAL DE PROCESSOS A SEREM CRIADOS: %d \n", max_process_number);
    for(int i = 0; i < max_process_number ; i++) {
        int arrival_time = randomInRange(0, 20);
        int total_execution_time = randomInRange(4,10);
        int io_arrival_time = randomInRange(0, total_execution_time);

        DEVICE deviceType = 0;        
        // magic number to device type enum
        if(io_arrival_time) {
            int randomDeviceType = randomInRange(DISK, PRINTER);
            switch(randomDeviceType){
                case DISK:
                    deviceType = DISK;
                    break;
                case MAGNETIC_TAPE:
                    deviceType = MAGNETIC_TAPE;
                    break;
                case PRINTER:
                    deviceType = PRINTER;
                    break;
            }
        }

        if(io_arrival_time == 0) {
            printf("PROCESS DE PID %d POSSUI IO ARRIVAL TIME IGUAL A 0 \n", i+1);
        }

        Process *tmp = newProcess(i+1, arrival_time, total_execution_time, io_arrival_time, deviceType);
        enqueue(queue, tmp);
    }
}

void simulation_test(Queue* queue) {
    Process *p0 = newProcess(1, 0, 13, 3, DISK);
    Process *p1 = newProcess(2, 4, 11, 5, PRINTER);
    Process *p2 = newProcess(3, 5, 7, 2, MAGNETIC_TAPE);
    Process *p3 = newProcess(4, 7, 8, 4, MAGNETIC_TAPE);
    Process *p4 = newProcess(5, 8, 16, 11, DISK);
    
    printProcess(p0);
    enqueue(queue, p0);
    printf("\n");
    
    printProcess(p1);
    enqueue(queue, p1);
    printf("\n");
    
    printProcess(p2);
    enqueue(queue, p2);
    printf("\n");
    
    printProcess(p3);
    enqueue(queue, p3);
    printf("\n");
    
    printProcess(p4);
    enqueue(queue, p4);
    printf("\n");
}