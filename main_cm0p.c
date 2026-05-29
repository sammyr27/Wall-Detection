#include "project.h"
#include "stdio.h"
#include "string.h"

float distance = 0.0f; //Declaring and initializing distance float variable

void genericEventHandler(uint32_t event, void *eventParameter)
{
    switch(event)
    {
        
        // Advertise until connected
        case CY_BLE_EVT_STACK_ON:
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
        {
            Cy_BLE_GAPP_StartAdvertisement(
                CY_BLE_ADVERTISING_FAST,
                CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX
            );

            UART_PutString("Advertising...\r\n");
            break;
        }

        // BLE connected
        case CY_BLE_EVT_GATT_CONNECT_IND:
        {
            UART_PutString("BLE Connected\r\n");
            break;
        }

    case CY_BLE_EVT_GATTS_WRITE_REQ:
    {
    cy_stc_ble_gatts_write_cmd_req_param_t *writeReqParameter =
        (cy_stc_ble_gatts_write_cmd_req_param_t *)eventParameter;

    if(CY_BLE_DEVICE_INTERFACE_DEVICE_INBOUND_CHAR_HANDLE ==
       writeReqParameter->handleValPair.attrHandle)
    {
        memcpy(&distance,
               writeReqParameter->handleValPair.value.val,
               sizeof(float));

        char buffer[80];
        sprintf(buffer,
                "Received distance: %.2f cm\r\n",
                distance); // Printing out distance received from ESP

        UART_PutString(buffer);

        Cy_BLE_GATTS_WriteRsp(
            writeReqParameter->connHandle
        );
    }

    break;
}
    }
}

void bleInterruptNotify()
{
    Cy_BLE_ProcessEvents();
}

int main(void)
{
    __enable_irq();

    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR);

    UART_Start(); //UART for printing anything for debugging

    Cy_BLE_Start(genericEventHandler);

    while(Cy_BLE_GetState() != CY_BLE_STATE_ON)
    {
        Cy_BLE_ProcessEvents();
    }

    Cy_BLE_RegisterAppHostCallback(bleInterruptNotify);

    UART_PutString("BLE Ready\r\n");
    
   /* // Distance less than or equal to threshold
    if (distance <= 30.48f)
    {
    // Turn haptic feedback on here
    } 
    
    else
    {
     // Haptic feedback stays off
    } */

    for(;;)
    {
    }
}