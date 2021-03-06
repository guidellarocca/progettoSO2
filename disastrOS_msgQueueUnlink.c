#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_msg_queue.h"
#include "disastrOS_descriptor.h"


void internal_msgQueueUnlink() {

    const char *name = (const char *) running->syscall_args[0];

    // search tramite il nome
    MsgQueuePtr *mqPtr = MsgQueuePtrList_findByName(&msg_queues_list, name);
    MsgQueue *mqdesc = NULL;

    if (mqPtr != NULL) {
        mqdesc = mqPtr->msgQueuePtr;
    }

    if (!mqdesc) {
        printf("ERROR Message queue with name '%s' doesn't exist right now! It has already been unlinked!\n\n", name);
        running->syscall_retvalue = DSOS_EMQ_UNLINK;
        return;
    }

    // La coda verrà distrutta una volta che tutti i processi che l'hanno aperta chiudono
    // i propri descrittori associati alla coda
    if (mqdesc->resource.descriptors_ptrs.size > 0) {
        printf("ERROR The message queue '%s' has opened descriptors!\n", name);
        running->syscall_retvalue = 0;
        return;
    }

    List_detach(&msg_queues_list, (ListItem *) mqPtr);
    List_detach(&resources_list, (ListItem *) mqdesc);

    if (MsgQueuePtr_free(mqPtr) != 0) {
        disastrOS_debug("ERROR Failed to deallocate the message queue ptr for the message queue '%s'!\n", name);
        running->syscall_retvalue = DSOS_EMQ_UNLINK;
        return;
    }

    if (MsgQueue_free(mqdesc) != 0) {
        disastrOS_debug("ERROR Failed to deallocate the message queue '%s'!\n", name);
        running->syscall_retvalue = DSOS_EMQ_UNLINK;
        return;
    }

    disastrOS_debug("Message queue with name '%s' destroyed!\n", name);
    running->syscall_retvalue = 0;
}