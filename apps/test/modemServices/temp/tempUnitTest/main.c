/**
 * This module implements the unit tests for temperature API.
 *
 * Copyright (C) Sierra Wireless Inc. Use of this work is subject to license.
 *
 */

#include "legato.h"
#include "interfaces.h"
#include "pa_temp.h"
#include "pa_temp_simu.h"
#include "le_temp_local.h"

#define NB_CLIENT 2

#define SIMU_THRESHOLD_CRITICAL "SIMU_THRESHOLD_CRITICAL"


// Task context
typedef struct
{
    uint32_t appId;
    le_thread_Ref_t                     appThreadRef;
    le_temp_ThresholdEventHandlerRef_t  eventHandler;
    char                                expectedThreshold[LE_TEMP_THRESHOLD_NAME_MAX_BYTES];
} AppContext_t;

static le_sem_Ref_t    ThreadSemaphore;
static AppContext_t AppCtx[NB_CLIENT];
static le_clk_Time_t TimeToWait ={ 0, 1000000 };
static char ExpectedThreshold[LE_TEMP_THRESHOLD_NAME_MAX_BYTES];

//--------------------------------------------------------------------------------------------------
/**
 * Synchronize test threads and tasks
 *
 */
//--------------------------------------------------------------------------------------------------
static void SynchTest( void )
{
    int i=0;

    for (;i<NB_CLIENT;i++)
    {
        LE_ASSERT(le_sem_WaitWithTimeOut(ThreadSemaphore, TimeToWait) == LE_OK);
    }
}


//-------------------------------------------------------------------------------------------------
/**
 * Threshold handler.
 */
//-------------------------------------------------------------------------------------------------
static void ThresholdEventHandler
(
    le_temp_SensorRef_t  sensorRef,
    const char*          thresholdPtr,
    void*                contextPtr
)
{
    char sensorName[LE_TEMP_SENSOR_NAME_MAX_BYTES] = {0};
    char expectedThreshold[LE_TEMP_THRESHOLD_NAME_MAX_BYTES] = {0};

    strncpy(expectedThreshold, contextPtr, sizeof(expectedThreshold));

    LE_ASSERT(le_temp_GetSensorName(sensorRef, sensorName, sizeof(sensorName)) == LE_OK);

    LE_ASSERT(!strncmp(expectedThreshold, thresholdPtr, LE_TEMP_THRESHOLD_NAME_MAX_LEN));
    LE_INFO("%s threshold event for %s sensor", thresholdPtr, sensorName);

    le_sem_Post(ThreadSemaphore);
}

//--------------------------------------------------------------------------------------------------
/**
 * Test tasks: this function handles the task and run an eventLoop
 *
 */
//--------------------------------------------------------------------------------------------------
static void* AppHandler
(
    void* ctxPtr
)
{
    AppContext_t * appCtxPtr = (AppContext_t*) ctxPtr;

    LE_DEBUG("App id: %d", appCtxPtr->appId);

    // Check bad parameter
    appCtxPtr->eventHandler = le_temp_AddThresholdEventHandler(NULL, &ExpectedThreshold);
    LE_ASSERT(appCtxPtr->eventHandler == NULL);

    // Subscribe to temperature event handler
    appCtxPtr->eventHandler=le_temp_AddThresholdEventHandler(ThresholdEventHandler,&ExpectedThreshold);
    LE_ASSERT(appCtxPtr->eventHandler != NULL);

    // Semaphore is used to synchronize the task execution with the core test
    le_sem_Post(ThreadSemaphore);

    le_event_RunLoop();
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove temperature event handlers
 *
 */
//--------------------------------------------------------------------------------------------------
static void RemoveHandler
(
    void* param1Ptr,
    void* param2Ptr
)
{
    AppContext_t * appCtxPtr = (AppContext_t*) param1Ptr;

    le_temp_RemoveThresholdEventHandler( appCtxPtr->eventHandler );

    // Semaphore is used to synchronize the task execution with the core test
    le_sem_Post(ThreadSemaphore);
}

//--------------------------------------------------------------------------------------------------
/**
 * Thread running an event loop for le_temp and pa_temp_simu (useful for event report)
 *
 */
//--------------------------------------------------------------------------------------------------
static void* TempThread
(
    void* ctxPtr
)
{
    pa_temp_Init();

    le_temp_Init();

    le_sem_Post(ThreadSemaphore);

    le_event_RunLoop();
}

//--------------------------------------------------------------------------------------------------
/**
 * Create and start a thread to run le_temp and pa_temp_simu (useful for event report)
 *
 */
//--------------------------------------------------------------------------------------------------
void Testle_Temp_Init
(
    void
)
{
    // Create a semaphore to coordinate the test
    ThreadSemaphore = le_sem_Create("HandlerSem", 0);

    le_thread_Start(le_thread_Create("PaTempThread", TempThread, NULL));

    le_sem_Wait(ThreadSemaphore);
}

//--------------------------------------------------------------------------------------------------
/**
 * Test APIs with bad parameters
 *
 * Tested API:
 *  - le_temp_GetPlatformTemperature
 *  - le_temp_GetPlatformThresholds
 *  - le_temp_GetRadioTemperature
 *  - le_temp_GetRadioThresholds
 *  - le_temp_SetPlatformThresholds
 *  - le_temp_SetRadioThresholds
 *
 * exit if failed
 *
 */
//--------------------------------------------------------------------------------------------------
void Testle_Temp_TestBadParameters
(
    void
)
{
    le_temp_SensorRef_t simuSensorRef = (le_temp_SensorRef_t)0xdeadbeef;
    char sensorName[LE_TEMP_SENSOR_NAME_MAX_BYTES] = {0};
    int32_t temp = 0;

    pa_tempSimu_SetReturnCode(LE_OK);

    LE_ASSERT(le_temp_GetSensorName(simuSensorRef, sensorName, sizeof(sensorName)) == LE_FAULT);
    LE_ASSERT(le_temp_GetTemperature(simuSensorRef, &temp) == LE_FAULT);
    LE_ASSERT(le_temp_SetThreshold(simuSensorRef, SIMU_THRESHOLD_CRITICAL, temp) == LE_FAULT);
    LE_ASSERT(le_temp_GetThreshold(simuSensorRef, SIMU_THRESHOLD_CRITICAL, &temp) == LE_FAULT);

    simuSensorRef = le_temp_Request(PA_SIMU_TEMP_SENSOR);
    LE_ASSERT(le_temp_GetSensorName(simuSensorRef, NULL, sizeof(sensorName)) == LE_FAULT);
    LE_ASSERT(le_temp_GetTemperature(simuSensorRef, NULL) == LE_FAULT);
    LE_ASSERT(le_temp_SetThreshold(simuSensorRef, NULL, temp) == LE_FAULT);
    LE_ASSERT(le_temp_GetThreshold(simuSensorRef, SIMU_THRESHOLD_CRITICAL, NULL) == LE_FAULT);
    LE_ASSERT(le_temp_GetThreshold(simuSensorRef, NULL, &temp) == LE_FAULT);
}

//--------------------------------------------------------------------------------------------------
/**
 * Test APIs with bad return code
 *
 * Tested API:
 *  - le_temp_GetPlatformTemperature
 *  - le_temp_GetPlatformThresholds
 *  - le_temp_GetRadioTemperature
 *  - le_temp_GetRadioThresholds
 *  - le_temp_SetPlatformThresholds
 *  - le_temp_SetRadioThresholds
 *
 * exit if failed
 *
 */
//--------------------------------------------------------------------------------------------------
void Testle_Temp_TestBadReturnCode
(
    void
)
{
    char sensorName[LE_TEMP_SENSOR_NAME_MAX_BYTES] = {0};
    int32_t temp = 0;

    le_temp_SensorRef_t simuSensorRef = le_temp_Request(PA_SIMU_TEMP_SENSOR);

    pa_tempSimu_SetReturnCode(LE_FAULT);
    LE_ASSERT(le_temp_GetSensorName(simuSensorRef, sensorName, sizeof(sensorName)) == LE_FAULT);
    LE_ASSERT(le_temp_GetTemperature(simuSensorRef, &temp) == LE_FAULT);
    LE_ASSERT(le_temp_SetThreshold(simuSensorRef, SIMU_THRESHOLD_CRITICAL, temp) == LE_FAULT);
    LE_ASSERT(le_temp_GetThreshold(simuSensorRef, SIMU_THRESHOLD_CRITICAL, &temp) == LE_FAULT);
    LE_ASSERT(le_temp_StartMonitoring() == LE_FAULT);
}

//--------------------------------------------------------------------------------------------------
/**
 * Test APIs with correct parameters
 *
 * Tested API:
 *  - le_temp_GetPlatformTemperature
 *  - le_temp_GetPlatformThresholds
 *  - le_temp_GetRadioTemperature
 *  - le_temp_GetRadioThresholds
 *  - le_temp_SetPlatformThresholds
 *  - le_temp_SetRadioThresholds
 *
 * exit if failed
 */
//--------------------------------------------------------------------------------------------------
void Testle_Temp_TestCorrectUsage
(
    void
)
{
    char sensorName[LE_TEMP_SENSOR_NAME_MAX_BYTES] = {0};
    int32_t temp = 0;

    pa_tempSimu_SetReturnCode(LE_OK);

    le_temp_SensorRef_t simuSensorRef = le_temp_Request(PA_SIMU_TEMP_SENSOR);

    LE_ASSERT(le_temp_GetSensorName(simuSensorRef, sensorName, sizeof(sensorName)) == LE_OK);
    LE_ASSERT(le_temp_GetTemperature(simuSensorRef, &temp) == LE_OK);
    LE_ASSERT(temp == PA_SIMU_TEMP_DEFAULT_TEMPERATURE);

    LE_ASSERT(le_temp_GetThreshold(simuSensorRef, SIMU_THRESHOLD_CRITICAL, &temp) == LE_OK);
    LE_ASSERT(temp == PA_SIMU_TEMP_DEFAULT_HI_CRIT);

    LE_ASSERT(le_temp_SetThreshold(simuSensorRef, SIMU_THRESHOLD_CRITICAL, 0) == LE_OK);
    LE_ASSERT(le_temp_GetThreshold(simuSensorRef, SIMU_THRESHOLD_CRITICAL, &temp) == LE_OK);
    LE_ASSERT(temp == 0);

    LE_ASSERT(le_temp_StartMonitoring() == LE_OK);
}

//--------------------------------------------------------------------------------------------------
/**
 * Test the temperature event handler
 *
 * API tested:
 *  le_temp_AddThresholdEventHandler
 *  call of the handler
 *
 */
//--------------------------------------------------------------------------------------------------
static void Testle_Temp_AddHandlers
(
    void
)
{
    int i;

    // int app context
    memset(AppCtx, 0, NB_CLIENT*sizeof(AppContext_t));

    // Start tasks: simulate multi-user of le_sim
    // each thread subcribes to state handler using le_sim_AddNewStateHandler
    for (i=0; i < NB_CLIENT; i++)
    {
        char string[20]={0};
        snprintf(string,20,"app%dhandler", i);
        AppCtx[i].appId = i;
        AppCtx[i].appThreadRef = le_thread_Create(string, AppHandler, &AppCtx[i]);
        le_thread_Start(AppCtx[i].appThreadRef);
    }

    // Wait that the tasks have started before continuing the test
    SynchTest();

    strncpy(ExpectedThreshold, SIMU_THRESHOLD_CRITICAL, sizeof(ExpectedThreshold)-1);

    pa_tempSimu_TriggerEventReport(SIMU_THRESHOLD_CRITICAL);

    // wait the handlers' calls
    SynchTest();

    // Check that no more call of the semaphore
    LE_ASSERT(le_sem_GetValue(ThreadSemaphore) == 0);
}

//--------------------------------------------------------------------------------------------------
/**
 * Test the temperature remove handlers
 *
 * API tested:
 *  le_temp_RemoveThresholdEventHandler
 *
 */
//--------------------------------------------------------------------------------------------------
static void Testle_Temp_RemoveHandlers
(
    void
)
{
    int i;

    // Remove handlers: add le_temp_RemoveThresholdEventHandler to the eventLoop of tasks
    for (i=0; i<NB_CLIENT; i++)
    {
        le_event_QueueFunctionToThread( AppCtx[i].appThreadRef,
                                        RemoveHandler,
                                        &AppCtx[i],
                                        NULL );
    }

    // Wait for the tasks
    SynchTest();

    // trigger an event report
    strncpy(ExpectedThreshold, SIMU_THRESHOLD_CRITICAL, sizeof(ExpectedThreshold)-1);

    pa_tempSimu_TriggerEventReport(SIMU_THRESHOLD_CRITICAL);

    // Wait for the semaphore timeout to check that handlers are not called
    LE_ASSERT(le_sem_WaitWithTimeOut(ThreadSemaphore, TimeToWait) == LE_TIMEOUT);
}

//--------------------------------------------------------------------------------------------------
/**
 * main of the test
 *
 */
//--------------------------------------------------------------------------------------------------
COMPONENT_INIT
{
    // To reactivate for all DEBUG logs
   le_log_SetFilterLevel(LE_LOG_DEBUG);

    Testle_Temp_Init();

    LE_INFO("======== Start UnitTest of TEMP API ========");

    LE_INFO("======== Test invalid parameters ========");
    Testle_Temp_TestBadParameters();

    LE_INFO("======== Test failed return code ========");
    Testle_Temp_TestBadReturnCode();

    LE_INFO("======== Test correct usage ========");
    Testle_Temp_TestCorrectUsage();

    LE_INFO("======== Test AddHandlers ========");
    Testle_Temp_AddHandlers();

    LE_INFO("======== Test RemoveHandlers ========");
    Testle_Temp_RemoveHandlers();

    LE_INFO("======== UnitTest of TEMP API ends with SUCCESS ========");
    exit(0);
}
