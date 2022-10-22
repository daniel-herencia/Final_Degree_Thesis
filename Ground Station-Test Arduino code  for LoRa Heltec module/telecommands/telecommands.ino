
#include "LoRaWan_APP.h"
#include "Arduino.h"

#ifndef LoraWan_RGB
#define LoraWan_RGB 0
#endif
/*This are COMMS definitions*/
#define RF_FREQUENCY                  868000000   // 868 MHz
#define TX_OUTPUT_POWER               22          // 22 dBm
#define LORA_SPREADING_FACTOR         7//9           // [0x09 --> SF9]
#define LORA_BANDWIDTH                0           // Radio.h changes it Bandwidths[] = { LORA_BW_125, LORA_BW_250, LORA_BW_500 }      [0x04 --> 125 kHz]
#define LORA_CODINGRATE               1           // [0x01 --> CR=4/5]
//#define LORA_LOW_DATA_RATE_OPTIMIZE 0           // [0x00 --> LowDataRateOptimize OFF]
#define LORA_PREAMBLE_LENGTH          8           // CSS modulations usually have 8 preamble symbols
//#define HEADER_TYPE                 0           // [0 --> Explicit header]
//#define PAYLOAD_LENGTH              38          // [38 bytes of payload information]
//#define CRC_TYPE                    0           // [0 --> CRC OFF]  /*ARE YOU SURE??? MAYBE 4/5 0x01*/
//#define INVERT_IQ                   0           // [0 --> Standard IQ setup]
// Check CAD
// BufferBaseAddress
// LoRaSymbNumTimeout
#define TX_TIMEOUT_VALUE              3000//340         // Air time Tx
#define PACKET_LENGTH                 59          // Packet Size
/*End COMMS definitions*/

#define TX_WINDOW_TIMEOUT             13900

#define LORA_SYMBOL_TIMEOUT           0         // ??????????
#define LORA_FIX_LENGTH_PAYLOAD_ON    false
#define LORA_FIX_LENGTH_PAYLOAD_LEN   38      //???????????
#define LORA_IQ_INVERSION_ON          false   //?????


#define RX_TIMEOUT_VALUE              0     //IF NEEDED
//#define BUFFER_SIZE                   38      // Define the payload size here
#define BUFFER_SIZE                   100//40      // Define the payload size here
#define WINDOW_SIZE                   10//40

//CHECK THIS DEFINITIONS (I DO NOT KNOW IF THEY ARE CORRECT OR WHICH VALUE TO USE)
#define CAD_TIMER_TIMEOUT             3000//1000        //Define de CAD timer's timeout here
#define RX_TIMER_TIMEOUT              3000//4000        //Define de CAD timer's timeout here
#define CAD_SYMBOL_NUM                0x01
#define CAD_DET_PEAK                  22
#define CAD_DET_MIN                   10
#define CAD_TIMEOUT_MS                2000
#define NB_TRY                        10

//------------ TELECOMMANDS --------------
#define RESET2                        0x01
#define NOMINAL                       0x02
#define LOW_                          0x03
/*"CRITICAL
 (04) 0x04"
"EXIT_LOW_POWER
 (05) 0x05"
"EXIT_SURVIVAL (06)
0x06"
"EXIT_SUNSAFE (07)
0x07"
"SET_TIME
 (08) 0x08"
"SET_CONSTANT_KP
 (10) 0x0A"
"TLE
 (11) 0x0B"
"SET_GYRO_RES
(12) 0x0C"
"SEND_DATA
 (20) 0x14"
"SEND_TELEMETRY
 (21) 0x15"
"STOP_SENDING_DATA
 (22) 0x16"
"ACK_DATA
 (23) 0x17"
"SET_SF_CR
 (24) 0x18"

"SEND_CALIBRATION
 (25) 0x19"
"TAKE_PHOTO
 (30)
0x1E"
"TAKE_RF
 (40)
0x28"
"SEND_CONFIG
 (50) 0x32"
*/
//----------------------------------------



static RadioEvents_t RadioEvents;  //SHOULD THIS BE IN MAIN??? IS TO HANDLE IRQ???

byte telecommand0[] = {0xC8, 0x9D, 0x02, 0x5A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAA,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAA,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAA,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAA,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAA, 0xAA, 0xAA};

bool telecommand_send = false;
byte last_telecommand[4];

byte ack_telecommand[] = {0xC8, 0x9D, 0x3C};
//byte ack_telecommand[] = {0xC8, 0x9D, 0x3A};  //WRONG ACK TEST
//byte ack_telecommand[] = {0xC8, 0x9A, 0x3C};  //WRONG PIN ACK TEST



byte telecommand1[] = {0xC8, 0x9D, 0x01};
byte telecommand2[] = {0xC8, 0x9D, 0x02, 0x5A}; // 90%
//byte telecommand2[] = {0xC7, 0x9D, 0x02, 0x5A}; // 90%  //WRONG PIN TEST
byte telecommand3[] = {0xC8, 0x9D, 0x03, 0x50}; // 80%
byte telecommand4[] = {0xC8, 0x9D, 0x04, 0x41}; // 65%
byte telecommand5[] = {0xC8, 0x9D, 0x05};
byte telecommand6[] = {0xC8, 0x9D, 0x06};
byte telecommand7[] = {0xC8, 0x9D, 0x07};
byte telecommand8[] = {0xC8, 0x9D, 0x08};

byte telecommand10[] = {0xC8, 0x9D, 0x0A};
/* TLE CUBECAT 2
1 41732U 16051B 16266.30199144 .00002531 00000-0 10795-3 0 9998
2 41732 97.3734 181.4520 0015537 131.2294 349.2722 15.23731070 5728 */

byte telecommand11_1[] = {0xC8, 0x9D, 0x0B, 0x31, 0x20, 0x34, 0x31, 0x37, 0x33, 0x32, 0x55, 0x20, 0x31, 0x36, 0x30,   //aqui hay 15, incluye el pin y id
0x35, 0x31, 0x42, 0x20, 0x31, 0x36, 0x32, 0x36, 0x36, 0x2e, 0x33, 0x30, 0x31, 0x39, 0x39, 0x31, 0x34, 0x34,   //18 por linea
0x20, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x32, 0x35, 0x33, 0x31, 0x20, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2d, 0x30,
0x20, 0x31, 0x30, 0x37, 0x39, 0x35, 0x2d, 0x33, 0x20, 0x30, 0x20, 0x39, 0x39, 0x39, 0x38, 0x0a, 0x32, 0x20};   //Total primer paquete => 69 bytes (3 + 66)

byte telecommand11_2[] = {0xC8, 0x9D, 0x0B, 0x34, 0x31, 0x37, 0x33, 0x32, 0x20, 0x39, 0x37, 0x2e, 0x33, 0x37, 0x33, 0x34, 0x20, 0x31, 0x38, 0x31, 0x2e,
0x34, 0x35, 0x32, 0x30, 0x20, 0x30, 0x30, 0x31, 0x35, 0x35, 0x33, 0x37, 0x20, 0x31, 0x33, 0x31, 0x2e, 0x32, //21 bytes (3 + 18)
0x32, 0x39, 0x34, 0x20, 0x33, 0x34, 0x39, 0x2e, 0x32, 0x37, 0x32, 0x32, 0x20, 0x31, 0x35, 0x2e, 0x32, 0x33, //18 bytes
0x37, 0x33, 0x31, 0x30, 0x37, 0x30, 0x20, 0x35, 0x37, 0x32, 0x38, 0xFF}; //11 bytes. TOTAL 69 (3 + 65 + ff)

byte telecommand12[] = {0xC8, 0x9D, 0x0C};

byte telecommand20[] = {0xC8, 0x9D, 0x14, 0x00};
byte telecommand21[] = {0xC8, 0x9D, 0x15};
byte telecommand22[] = {0xC8, 0x9D, 0x16};
byte telecommand23[] = {0xC8, 0x9D, 0x17};
byte telecommand24[] = {0xC8, 0x9D, 0x18};
byte telecommand25[] = {0xC8, 0x9D, 0x19};

byte telecommand30[] = {0xC8, 0x9D, 0x1E};

byte telecommand40[] = {0xC8, 0x9D, 0x28};

byte telecommand50[] = {0xC8, 0x9D, 0x32};

byte photo[] = {};
uint32_t air_time;
uint8_t Buffer[BUFFER_SIZE];
uint8_t reception_mode = 1;

/*
 * To avoid the variables being erased if a reset occurs, we have to store them in the Flash memory
 * Therefore, they have to be declared as a single-element array
 */
uint8_t count_packet[] = {0}; //To count how many packets have been sent (maximum WINDOW_SIZE)
uint8_t count_window[] = {0}; //To count the window number
uint8_t count_rtx[] = {0};    //To count the number of retransmitted packets
uint8_t i = 0;          //Auxiliar variable for loop

uint64_t ack;         //Information rx in the ACK (FER DESPLAÇAMENTS DSBM)
uint8_t nack_number;      //Number of the current packet to retransmit
bool nack = false;            //True when retransmition necessary
bool full_window = false;       //Stop & wait => to know when we reach the limit packet of the window

//uint8_t Buffer[BUFFER_SIZE];
//bool PacketReceived = false;
//bool RxTimeoutTimerIrqFlag = false;
char txpacket[BUFFER_SIZE]; //Creates the package


void tx_function(void);
void rx_function(void);
void packaging(void);
void resetCommsParams(void);
void OnTxDone( void );
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
void OnTxTimeout( void );
void OnRxTimeout( void );
void OnRxError( void );
void OnCadDone( bool channelActivityDetected);
static void CADTimeoutTimeoutIrq( void );
static void RxTimeoutTimerIrq( void );
//void array_to_string(uint8_t array1[], unsigned int len, char string[]);


/*
 * STATE MACHINE VARIABLES
 */
typedef enum
{
    LOWPOWER,
    RX,
    RX_TIMEOUT,
    RX_ERROR,
    TX,
    TX_TIMEOUT,
    START_CAD,
}States_t;

typedef enum
{
    CAD_FAIL,
    CAD_SUCCESS,
    PENDING,
}CadRx_t;

States_t State;

int8_t RssiValue = 0;
int8_t SnrValue = 0;

CadRx_t CadRx = CAD_FAIL;
bool PacketReceived = false;
bool RxTimeoutTimerIrqFlag = false;
int16_t RssiMoy = 0;
int8_t SnrMoy = 0;
uint16_t RxCorrectCnt = 0;
uint16_t BufferSize = BUFFER_SIZE;
uint16_t PacketCnt = 0;
uint16_t rx_counter = 0;


TimerEvent_t CADTimeoutTimer;
TimerEvent_t RxAppTimeoutTimer;

/*
 *  ---------------- END ----------------
 */

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;
  RadioEvents.CadDone = OnCadDone;
  
  Radio.Init( &RadioEvents ); //SHOULD THIS BE IN MAIN???

  Radio.SetChannel( RF_FREQUENCY );

  Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                   true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE );  //In the original example it was 3000


  //SHALL WE CARE ABOUT THE RX TIMEOUT VALUE??? IF YES, CHANGE IT IN SetRx FUNCTION
  Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );



  //Air time calculus
  air_time = Radio.TimeOnAir( MODEM_LORA , PACKET_LENGTH );

  //Flash_Read_Data( COMMS_VARIABLE , count_packet , sizeof(count_packet) );    //Read from Flash count_packet
  //Flash_Read_Data( COMMS_VARIABLE + 0x1 , count_window , sizeof(count_window) );  //Read from Flash count_window
  //Flash_Read_Data( COMMS_VARIABLE + 0x2 , count_rtx , sizeof(count_rtx) );    //Read from Flash count_rtx
  ack = 0xFFFFFFFFFFFFFFFF;
  nack = false;
  State = TX;

  //Timer used to restart the CAD
  TimerInit( &CADTimeoutTimer, CADTimeoutTimeoutIrq );
  TimerSetValue( &CADTimeoutTimer, CAD_TIMER_TIMEOUT );

  //App timmer used to check the RX's end
  TimerInit( &RxAppTimeoutTimer, RxTimeoutTimerIrq );
  TimerSetValue( &RxAppTimeoutTimer, RX_TIMER_TIMEOUT );

  //SX126xConfigureCad( CAD_SYMBOL_NUM, CAD_DET_PEAK,CAD_DET_MIN, CAD_TIMEOUT_MS);      // Configure the CAD
  Radio.StartCad(2);                                                                  // do the config and lunch first CAD
  //delay(1000); 
}


void loop() {
    // put your main code here, to run repeatedly:
    Radio.IrqProcess( );
    switch( State )
    {
        case RX_TIMEOUT:
        {
            //RxTimeoutCnt++;
            //State = START_CAD;
            //Radio.Rx( RX_TIMEOUT_VALUE );  //Basic RX code
            //DelayMs(1); //Basic RX code
            Radio.Standby;
            State = LOWPOWER;
        break;
        }
        case RX_ERROR:
        {
            //RxErrorCnt++;
            PacketReceived = false;
            //State = START_CAD;
            //Radio.Rx( RX_TIMEOUT_VALUE );  //Basic RX code
            //DelayMs(1); //Basic RX code
            Radio.Standby;
            State = LOWPOWER;
        break;
        }
        case RX:
        {
            if( PacketReceived == true )
            {
                PacketReceived = false;     // Reset flag
                RxCorrectCnt++;         // Update RX counter
        #if(FULL_DBG)
          //printf( "Rx Packet n %d\r\n", PacketCnt );
        #endif
                rx_counter = rx_counter + 1;
                //Serial.print(Buffer[2],HEX);
                //Serial.println(" Position 3 Received");
                //Serial.print(last_telecommand[2],HEX);
                //Serial.println(" Last telecommand send");
                //if (rx_counter == 5){
                //Serial.print(Buffer[0],HEX);
                //Serial.println(" Position 0 Rx");
                Serial.println();
                Serial.print("Received paquet number ");
                Serial.println(Buffer[2],DEC);
                //for (int i = 3; i < BUFFER_SIZE; i = i + 1) {
                for (int i = 0; i < BUFFER_SIZE; i = i + 1) {
                  Serial.print("0x");
                  Serial.print(Buffer[i],HEX);
                  Serial.print(", ");
                  if (((i-2)%16 == 0) && i != 3){
                    Serial.println();
                  }
                }
                if(Buffer[2] == 0x41){
                  Serial.println("ERROR MESSAGE RECEIVED");
                }
                else if (Buffer[2] == 0x44){
                  Serial.println("BEACON MESSAGE RECEIVED");
                }
                else if (Buffer[2] == last_telecommand[2]){
                  delay(100); //Optimum value
                  //delay(1000);
                  Radio.Send( ack_telecommand, 3);
                  Serial.println("ACK SEND");
                  delay(100);
                  Radio.Send( ack_telecommand, 3);  //discomment this line
                  Serial.println("ACK SEND");
                }
                else if (Buffer[0] == 'E' && Buffer[1] == 'N' && Buffer[2] == 'D'){
                  reception_mode = 0;
                  delay(1000);
                }
                State = LOWPOWER;
            }
            else
            {
                if (CadRx == CAD_SUCCESS)
                {
                  //PUT HERE THE CODE TO WITHDRAW THE INFO FROM THE BUFFER

                    //channelActivityDetectedCnt++;   // Update counter
          #if(FULL_DBG)
                      printf( "Rxing\r\n");
          #endif
                    RxTimeoutTimerIrqFlag = false;
                    TimerReset(&RxAppTimeoutTimer); // Start the Rx's's Timer
                    //Radio.Rx( RX_TIMEOUT_VALUE );
                }
                else
                {
                    TimerStart(&CADTimeoutTimer);   // Start the CAD's Timer
                }
                Radio.Rx( RX_TIMEOUT_VALUE );  //Basic RX code
                delay(1); //Basic RX code
                State = LOWPOWER;
            }
            break;
        }
        case TX:
        {
            printf("Send Packet n %d \r\n",PacketCnt);
            if( PacketCnt == 0xFFFF)
            {
                PacketCnt = 0;
            }
            else
            {
                PacketCnt ++;
            }
            //Send Frame
            delay(300);
            //Serial.println("I'm going to tx");
            //tx_function();
            //Radio.Send( telecommand11_1, sizeof(telecommand11_1) );   //TEST SEND TLE
            //delay(500);//TLE
            //Radio.Send( telecommand11_2, sizeof(telecommand11_2) );   //TEST SEND TLE

            Radio.Send( telecommand20, sizeof(telecommand20) ); //TEST SEND DATA
            //Radio.Send( telecommand2, sizeof(telecommand2) ); //TEST TELECOMMAND 2 (LOW)
            Serial.print("Telecommand TX: ");
            for (int i = 0; i < sizeof(telecommand2); i = i + 1) {
              Serial.print(telecommand20[i],HEX);
              Serial.print(" ");
            }
            Serial.println();
            delay(300); //Optimum delay for SF = 7 is 300 ms
            //delay(1000); //Force error paquet => timeout before 2nd telecommand rx
            Radio.Send( telecommand20, sizeof(telecommand20) ); //TEST SEND DATA
            //Radio.Send( telecommand2, sizeof(telecommand2) ); //TEST TELECOMMAND 2 (LOW)
            delay(100);
            Serial.print("Telecommand TX: ");
            for (int i = 0; i < sizeof(telecommand2); i = i + 1) {
              Serial.print(telecommand20[i],HEX);
              last_telecommand[i] = telecommand2[i];
              Serial.print(" ");
            }
            Serial.println();
            telecommand_send = true;
            reception_mode = 1;
            State = LOWPOWER;
            Buffer[0] = 'H';
            break;
        }
        case TX_TIMEOUT:
        {
            State = LOWPOWER;
            break;
        }
        /*case START_CAD:
        {
            //i++;    // Update NbTryCnt
            TimerStop(&RxAppTimeoutTimer);  // Stop the Rx's Timer
            // Trace for debug
            if(CadRx == CAD_FAIL)
            {
        #if(FULL_DBG)
                printf("No CAD detected\r\n");
        #endif
            }
            CadRx = CAD_FAIL;           // Reset CAD flag
            delay(100);     //Add a random delay for the PER test => CHECK THIS WARNING
      #if(FULL_DBG)
              printf("CAD %d\r\n",i);
      #endif
            Radio.StartCad(2);          //StartCad Again
            State = LOWPOWER;
        break;
        }*/
        case LOWPOWER:
        default:
            if( reception_mode == 1){
              State = RX;
            } else{
              State = TX;
            }
            // Set low power
            //State = TX;
            break;
    }
    TimerLowPowerHandler( );
        // Process Radio IRQ
    //Radio.IrqProcess( );
}





//CAD: CHANNEL ACTIVITY DETECTED

void tx_function(void){
  //configuration();
  if (!full_window)
  {
    packaging(); //Start the TX by packaging all the data that will be transmitted
    //SX126xSetPayload(); //Aquesta fa el writebuffer, sha de posar direccions com a la pag 48 del datasheet
    Radio.Send( Buffer, BUFFER_SIZE );
    //Serial.println("Packet Tx");
    //array_to_string(Buffer, BUFFER_SIZE, txpacket);
    /*
    for (unsigned int i = 0; i < BUFFER_SIZE; i++)
    {
      Serial.print(Buffer[i],HEX);
      Serial.print(" ");
    }*/
    Serial.printf("\r\nTX packet \"%s\" \r\n",Buffer);
    Serial.println();
    //txpacket[BUFFER_SIZE*3] = '\0';
    //Serial.println(strlen(txpacket));
  }
  Serial.print("Airtime: ");
  Serial.print(air_time,DEC);
  Serial.println();
  if (full_window){
      reception_mode = 1;
      //Radio.Standby;
      State = RX;
      full_window = false;
      delay(300);
      Serial.println("Vuelvo a RX");
  }

  //delay(air_time);//DELETE THIS LINE => TIMER NECESSARY
  //delay(300);
}


/* I THINK THAT THIS FUNCTION IS NOT NEEDED*/
void rx_function(void){
  //Radio.Rx( RX_TIMEOUT_VALUE );

};

void packaging(void){
  //NACK packets at the beginnig of the next window

  if (nack)
  {
    while(i<sizeof(ack) && nack_number != i-1)
    { 
      //function to obtain the packets to retx
      if(!((ack >> i) & 1)) //When position of the ack & 1 != 1 --> its a 0 --> NACK
      {
        
        nack_number = i;  //Current packet to rtx
        //Packet from last window => count_window - 1
        //Flash_Read_Data( PHOTO_ADDR + (count_window[0]-1)*WINDOW_SIZE*BUFFER_SIZE + (nack_number)*BUFFER_SIZE , Buffer , sizeof(Buffer) );  //Direction in HEX
        for (int i = 0; i < BUFFER_SIZE; i = i + 1) {
          Buffer[i] = photo[(count_window[0]-1)*WINDOW_SIZE*BUFFER_SIZE + (nack_number)*BUFFER_SIZE + i];
        }
        
        count_rtx[0]++;
      }
      i++;
    }
    if (i==sizeof(ack)){
      i=0;
    }
    else if (nack_number == i-1){
      i++;
    }
  }
  else //no NACKS
  {
    //Serial.println("Posiciones memoria enviadas: ");
    //Flash_Read_Data( PHOTO_ADDR + count_window[0]*WINDOW_SIZE*BUFFER_SIZE + (count_packet[0]-count_rtx[0])*BUFFER_SIZE , Buffer , sizeof(Buffer) ); //Direction in HEX
    for (int i = 0; i < BUFFER_SIZE; i = i + 1) {
      Buffer[i] = photo[count_window[0]*WINDOW_SIZE*BUFFER_SIZE + (count_packet[0]-count_rtx[0])*BUFFER_SIZE + i];
      Serial.print(count_window[0]*WINDOW_SIZE*BUFFER_SIZE + (count_packet[0]-count_rtx[0])*BUFFER_SIZE + i,DEC);
      Serial.print(" ");
    }
    //Serial.println("Salgo del for");
    if (count_packet[0] < WINDOW_SIZE-1)
    {
      count_packet[0]++;
    }
    else
    {
      Serial.println("Ventana Acabada");
      reception_mode = 1;
      count_packet[0] = 0;
      //count_window[0]++;
      count_window[0]=0;
      full_window = true;
    }
  }
};


/*This function is called when a new photo is stored in the last photo position*/
void resetCommsParams(void){
  count_packet[0] = 0;
  count_window[0] = 0;
  count_rtx[0]  = 0;
}


/*
 * FUNCTIONS OBTAINED FROM EXAMPLE MAIN.C
 */


void OnTxDone( void )
{
    Radio.Standby( );
    State = TX;
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    Radio.Standby( );
    BufferSize = size;
    memcpy( Buffer, payload, BufferSize );
    RssiValue = rssi;
    SnrValue = snr;
    PacketReceived = true;
    RssiMoy = (((RssiMoy * RxCorrectCnt) + RssiValue) / (RxCorrectCnt + 1));
    SnrMoy = (((SnrMoy * RxCorrectCnt) + SnrValue) / (RxCorrectCnt + 1));
    //Serial.printf("\r\nreceived packet \"%s\" with rssi %d , length %d\r\n",Buffer,rssi,size);
    State = RX;
}

void OnTxTimeout( void )
{
    Radio.Standby( );
    State = TX_TIMEOUT;
    Serial.println("OnTxTimeout");
}

void OnRxTimeout( void )
{
    Radio.Standby( );
    if( RxTimeoutTimerIrqFlag )
    {
        State = RX_TIMEOUT;
    }
    else
    {
    #if(FULL_DBG)
          printf(".");
    #endif
        Radio.Rx( RX_TIMEOUT_VALUE );   //  Restart Rx
        //SymbTimeoutCnt++;               //  if we pass here because of Symbol Timeout
        State = LOWPOWER;
    }
    Serial.println("OnRxTimeout");
}

void OnRxError( void )
{
    Radio.Standby( );
    State = RX_ERROR;
    Serial.println("OnRxError");
}

void OnCadDone( bool channelActivityDetected)
{
    Radio.Standby( );

    if( channelActivityDetected == true )
    {
        CadRx = CAD_SUCCESS;
    }
    else
    {
        CadRx = CAD_FAIL;
    }
    State = RX;
    Serial.println("OnCadDone");
}

static void CADTimeoutTimeoutIrq( void )
{
    Radio.Standby( );
    //State = START_CAD;
    State = RX;
    Serial.println("CADTimeoutIrq");
}

static void RxTimeoutTimerIrq( void )
{
    RxTimeoutTimerIrqFlag = true;
}
