# Message queues in disastrOS

## What ?

### Implementare un sistema IPC (Inter-Process Communication) basato su code di messaggi (msg queue) in `disastrOS` per permettere la comunicazione asincrona tra processi;
  the starting point is the version of disastrOS presented in class -
  available [here](https://gitlab.com/grisetti/sistemi_operativi_2019_20/-/tree/master/source/08_disastrOS/disastrOS_04_resources).

************************************************************************************
************************************************************************************

## How ?

#### Elenchiamo le modifiche apportate alla struttura base fornita da disastrOS:

1. In [**disastrOS_resource.h**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_resource.h) aggiungiamo il campo 'name' alla struct Resource. In seguito modifichiamo il file [**disastrOS_open_resource.c**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_open_resource.c).

2. In [**disastrOS_msg_queue.h**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_msg_queue.h) inseriamo tutte le strutture dati necessarie per implementare una coda di messaggi (allocate tramite SLAB allocator):

- Text
- Message
- Subqueue
- MsgQueue
- MsgQueuePtr

Inoltre aggiungiamo una funzione per permettere di ricercare la coda tramite il name: 'MsgQueuePtr * MsgQueuePtrList_findByName(MsgQueuePtrList *l, const char *name)'.

Ovviamente le funzioni dichiarate devono essere implementate in [**disastrOS_msg_queue.c**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_msg_queue.c), dove inseriamo anche la costante 'DSOS_MSG_QUEUE_RESOURCE' per impostare il tipo di risorsa.

3. In [**disastrOS_constants.h**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_constants.h) aggiungiamo tutta una serie di costanti che ci serviranno:

  * Macro relative ai messaggi:

    - MAX_TEXT_LEN (dimensione del messaggio scritto)
    - MAX_NUM_MESSAGES (numero massimo di messaggi che si possono scrivere)
    - MAX_NUM_MESSAGE_PER_MSG_QUEUE (numero massimo di messaggi contenuti nella msg queue)
    - MAX_NUM_PRIORITIES (numero di Subqueues nella coda di messaggi)

  * Error-code associati alle operazioni su msg queue:

    - DSOS_EMQ_CREATE
  	- DSOS_EMQ_READ
  	- DSOS_EMQ_WRITE
  	- DSOS_EMQ_CLOSE
  	- DSOS_EMQ_UNLINK
  	- DSOS_EMQ_NOFD
  	- DSOS_EMQ_NOEXIST

  * Numeri di syscalls:

    - DSOS_CALL_MQ_CREATE
  	- DSOS_CALL_MQ_OPEN
  	- DSOS_CALL_MQ_CLOSE
  	- DSOS_CALL_MQ_UNLINK
  	- DSOS_CALL_MQ_READ
  	- DSOS_CALL_MQ_WRITE

4. In [**disastrOS_syscalls.h**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_syscalls.h) vengono DICHIARATE le syscalls:
'''c
    void internal_msgQueueCreate();
   	void internal_msgQueueOpen();
   	void internal_msgQueueClose();
   	void internal_msgQueueUnlink();
   	void internal_msgQueueRead();
   	void internal_msgQueueWrite();
'''

5. Le nuove syscalls vengono IMPLEMENTATE in dei file C cosi da rendere possibile la gestione delle msg queues:

  - [**disastrOS_msgQueueCreate.c**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_msgQueueCreate.c)
   - [**disastrOS_msgQueueOpen.c**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_msgQueueOpen.c)
   - [**disastrOS_msgQueueRead.c**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_msgQueueRead.c)
   - [**disastrOS_msgQueueWrite.c**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_msgQueueWrite.c)
   - [**disastrOS_msgQueueClose.c**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_msgQueueClose.c)
   - [**disastrOS_msgQueueUnlink.c**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_msgQueueUnlink.c)


6. In [**disastrOS.c**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS.c) sono installate le nuove syscalls: vengono aggiunte al vettore 'syscall_vector' del SO, specificandone il numero di argomenti e il loro ordine in 'syscall_numarg'; viene dichiarata, inizializzata e stampata una lista di code di messaggi 'msg_queues_list'; inizializzate le nuove strutture associate alla coda:
    - Text_init();
    - Message_init();
    - Subqueue_init();
    - MsgQueue_init();
    - MsgQueuePtr_init();

 Infine implementate le syscalls:

```c
   int disastrOS_msgQueueCreate(const char *name) {
      return disastrOS_syscall(DSOS_CALL_MQ_CREATE, name);
   }
   int disastrOS_msgQueueOpen(const char *name) {
      return disastrOS_syscall(DSOS_CALL_MQ_OPEN, name);
   }
   int disastrOS_msgQueueClose(int mqdes) {
      return disastrOS_syscall(DSOS_CALL_MQ_CLOSE, mqdes);
   }
   int disastrOS_msgQueueUnlink(const char *name) {
      return disastrOS_syscall(DSOS_CALL_MQ_UNLINK, name);
   }
   int disastrOS_msgQueueRead(int mqdes, char *msg_ptr, unsigned int msg_len) {
      return disastrOS_syscall(DSOS_CALL_MQ_READ, mqdes, msg_ptr, msg_len);
   }
   int disastrOS_msgQueueWrite(int mqdes, const char *msg_ptr, unsigned msg_len, unsigned int priority) {
      return disastrOS_syscall(DSOS_CALL_MQ_WRITE, mqdes, msg_ptr, msg_len, priority);
   }
```

6. In [**disastrOS.h**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS.h) è necessario dichiarare le funzioni chiamabili dal processo:

  - **int disastrOS_msgqueueCreate(const char *name)**: 
  prende come input il nome della coda da creare e la alloca insieme alle subqueues che la compongono. Inoltre inserisce la risorsa creata nella 'resources_list'. Return 0 in caso di successo, DSOS_EMQ_CREATE altrimenti.

  - **int disastrOS_msgQueueOpen(const char *name)**: prende in input il nome della coda da aprire e alloca il descrittore associato alla coda di messaggi aperta. Il descrittore verrà inserito nella lista dei descrittori aperti associata alla coda. Return il file descriptor associato al descrittore del processo in caso di successo, altrimenti un codice di errore (DSOS_EMQ_NOFD e DSOS_EMQ_NOEXIST).

  - **int disastrOS_msgQueueClose(int mqdes)**: prende in input il descrittore 'mqdes' associato alla coda di messaggi da chiudere. Se tutte le risorse hanno chiuso il descrittore associato alla coda, allora viene eseguita [**disastrOS_msgQueueUnlink.c**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_msgQueueUnlink.c). Ritorna 0 in caso di successo, DSOS_EMQ_CLOSE altrimenti.

  - **int disastrOS_msgQueueUnlink(const char *name)**: prende in input il nome della coda di messaggi da rimuovere immediatamente. La coda verrà distrutta una volta che tutti i processi che l'hanno aperta chiudono i propri descrittori associati alla coda. Ritorna 0 in caso di successo, DSOS_EMQ_UNLINK altrimenti.

  - **int disastrOS_msgQueueRead(int mqdes, char *msg_ptr, int unsigned msg_len)**: se ci sono messaggi nella coda identificata da mqdes, rimuove quello con priorità più alta e lo inserisce nel buffer a cui punta msg_ptr tramite la funzione `char* strcpy(char *dest, const char *src)`. Viene inoltre specificata la dimensione del buffer puntato da msg_ptr (256) per poter verificare che la dimensione del messaggio da scrivere sia entro questo valore. Nel caso in cui la coda di messaggi sia vuota, il processo che ha eseguito tale funzione viene messo nella lista waiting descriptors, associata alla coda, in attesa che un altro processo inserisca un messaggio nella suddetta coda. Una volta che questo accade, il processo in waiting viene risvegliato. Questa funzione ritorna la lunghezza del messaggio letto in caso di successo, altrimenti DSOS_EMQ_READ.

  - **int disastrOS_msgQueueWrite(int mqdes, const char *msg_ptr, unsigned msg_len, unsigned int priority)**: prende in input il messaggio, puntato da msg_ptr, da inserire nella coda di messaggi identificata dal descrittore mqdes; viene specificata la lunghezza del messaggio puntato da msg_ptr e la priorità assegnata al messaggio. In particolare, viene allocato il messaggio tramite Message_alloc (vedi [*disastrOS_msgQueue.c*](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_msg_queue.c)) e inserito nella lista di messaggi associata alla sottocoda con quella priorità. Nel caso in cui la coda di messaggi sia piena, il processo che ha eseguito tale funzione viene messo in waiting descriptors e verrà risvegliato una volta rimosso un messaggio dalla coda. Ritorna la lunghezza del messaggio in caso di successo, DSOS_EMQ_WRITE altrimenti.

7. In [**disastrOS_test.c**](https://github.com/guidellarocca/progettoSO2/blob/main/disastrOS_test.c) implementiamo la comunicazione tra un Producer ed un Consumer.
Definiamo la costante 'ITERATIONS' per scegliere il numero di iterazioni che verranno eseguite.

************************************************************************************
************************************************************************************

## How-to-run ?

Per vedere il funzionamento di 'disastrOS_test', aprire terminale ed eseguire i seguenti:

* $`git clone https://github.com/guidellarocca/progettoSO2.git`
* $`cd progettoSO2`
* $`make`
* $`./disastrOS_test`

