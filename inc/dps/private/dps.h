/*
 *******************************************************************
 *
 * Copyright 2016 Intel Corporation All rights reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#ifndef _DPS_INTERNAL_H
#define _DPS_INTERNAL_H

#include <dps/dps.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Map keys for CBOR serialization of DPS messages
 */
#define DPS_CBOR_KEY_PORT           1   /* uint */
#define DPS_CBOR_KEY_CANCEL         2   /* bool */
#define DPS_CBOR_KEY_TTL            3   /* int */
#define DPS_CBOR_KEY_PUB_ID         4   /* uint */
#define DPS_CBOR_KEY_SEQ_NUM        5   /* bool */
#define DPS_CBOR_KEY_ACK_REQ        6   /* bool */
#define DPS_CBOR_KEY_BLOOM_FILTER   7   /* bstr */
#define DPS_CBOR_KEY_INBOUND_SYNC   8   /* bool */
#define DPS_CBOR_KEY_OUTBOUND_SYNC  9   /* bool */
#define DPS_CBOR_KEY_NEEDS         10   /* bstr */
#define DPS_CBOR_KEY_INTERESTS     11   /* bstr */

#define DPS_SECS_TO_MS(t)   ((uint64_t)(t) * 1000ull)
#define DPS_MS_TO_SECS(t)   ((uint32_t)((t) / 1000ull))

/**
 * Address type
 */
typedef struct _DPS_NodeAddress {
    struct sockaddr_storage inaddr;
} DPS_NodeAddress;

/**
 * For managing data that has been received
 */
typedef struct _DPS_RxBuffer {
    uint8_t* base;   /**< base address for buffer */
    uint8_t* eod;    /**< end of data */
    uint8_t* rxPos;  /**< current read location in buffer */
} DPS_RxBuffer;

/**
 * Initialize a receive buffer
 *
 * @param buffer    Buffer to initialized
 * @param storage   The storage for the buffer. The storage cannot be NULL
 * @param size      The size of the storage
 *
 * @return   DPS_OK or DP_ERR_RESOURCES if storage is needed and could not be allocated.
 */
DPS_Status DPS_RxBufferInit(DPS_RxBuffer* buffer, uint8_t* storage, size_t size);

/**
 * Free resources allocated for a buffer and nul out the buffer pointers.
 *
 * @param buffer    Buffer to free
 */
void DPS_RxBufferFree(DPS_RxBuffer* buffer);

/*
 * Clear receive buffer fields
 */
#define DPS_RxBufferClear(b) do { (b)->base = (b)->rxPos = (b)->eod = NULL; } while (0)

/*
 * Data available in a receive buffer
 */
#define DPS_RxBufferAvail(b)  ((uint32_t)((b)->eod - (b)->rxPos))

/**
 * For managing data to be transmitted
 */
typedef struct _DPS_TxBuffer {
    uint8_t* base;  /**< base address for buffer */
    uint8_t* eob;   /**< end of buffer */
    uint8_t* txPos; /**< current write location in buffer */
} DPS_TxBuffer;

/**
 * Initialize a transmit buffer
 *
 * @param buffer    Buffer to initialized
 * @param storage   The storage for the buffer. If the storage is NULL storage is allocated.
 * @param size      Current size of the buffer
 *
 * @return   DPS_OK or DP_ERR_RESOURCES if storage is needed and could not be allocated.
 */
DPS_Status DPS_TxBufferInit(DPS_TxBuffer* buffer, uint8_t* storage, size_t size);

/**
 * Free resources allocated for a buffer and nul out the buffer pointers.
 *
 * @param buffer    Buffer to free
 */
void DPS_TxBufferFree(DPS_TxBuffer* buffer);

/**
 * Add data to a transmit buffer
 *
 * @param buffer   Buffer to append to
 * @param storage  The data to append
 * @param len      Length of the data to append
 *
 * @return   DPS_OK or DP_ERR_RESOURCES if there not enough room in the buffer
 */
DPS_Status DPS_TxBufferAppend(DPS_TxBuffer* buffer, const uint8_t* data, size_t len);

/*
 * Clear transmit buffer fields
 */
#define DPS_TxBufferClear(b) do { (b)->base = (b)->txPos = (b)->eob = NULL; } while (0)

/*
 * Space left in a transmit buffer
 */
#define DPS_TxBufferSpace(b)  ((uint32_t)((b)->eob - (b)->txPos))

/*
 * Number of bytes that have been written to a transmit buffer
 */
#define DPS_TxBufferUsed(b)  ((uint32_t)((b)->txPos - (b)->base))

/**
 * Convert a transmit buffer into a receive buffer. Note that this
 * aliases the internal storage so care must be taken to avoid a
 * double free.
 *
 * @param txBuffer   A buffer containing data
 * @param rxBuffer   Receive buffer struct to be initialized
 */
void DPS_TxBufferToRx(DPS_TxBuffer* txBuffer, DPS_RxBuffer* rxBuffer);

/**
 * Convert a receive buffer into a transmit buffer. Note that this
 * aliases the internal storage so care must be taken to avoid a
 * double free.
 *
 * @param rxBuffer   A buffer containing data
 * @param txBuffer   Transmit buffer struct to be initialized
 */
void DPS_RxBufferToTx(DPS_RxBuffer* rxBuffer, DPS_TxBuffer* txBuffer);

/**
 * Print the current subscriptions
 */
void DPS_DumpSubscriptions(DPS_Node* node);

#ifdef __cplusplus
}
#endif

#endif
