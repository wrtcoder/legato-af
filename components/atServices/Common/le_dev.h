/** @file le_dev.c
 *
 * Implementation of device access.
 *
 * Copyright (C) Sierra Wireless Inc. Use of this work is subject to license.
 */

#ifndef LEGATO_LE_DEV_INCLUDE_GUARD
#define LEGATO_LE_DEV_INCLUDE_GUARD

//--------------------------------------------------------------------------------------------------
/**
 * device structure
 *
 */
//--------------------------------------------------------------------------------------------------
typedef struct Device
{
    char               path[LE_ATCLIENT_PATH_MAX_BYTES]; ///< Path of the device
    uint32_t           handle;                           ///< Handle of the device.
    le_fdMonitor_Ref_t fdMonitor;                        ///< fd event monitor associated to Handle
}
Device_t;

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called when we want to read on device (or port)
 *
 * @return byte number read
 */
//--------------------------------------------------------------------------------------------------
int32_t le_dev_Read
(
    Device_t  *devicePtr,    ///< device pointer
    uint8_t   *rxDataPtr,    ///< Buffer where to read
    uint32_t   size          ///< size of buffer
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to write on device (or port)
 *
 * @return written byte number
 */
//--------------------------------------------------------------------------------------------------
int32_t le_dev_Write
(
    Device_t *devicePtr,    ///< device pointer
    uint8_t  *txDataPtr,    ///< Buffer to write
    uint32_t  size          ///< size of buffer
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to open a device (or port)
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_dev_Open
(
    Device_t *devicePtr,    ///< device pointer
    le_fdMonitor_HandlerFunc_t handlerFunc, ///< [in] Handler function.
    void* contextPtr
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to close a device (or port)
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_dev_Close
(
    Device_t *devicePtr
);

#endif
