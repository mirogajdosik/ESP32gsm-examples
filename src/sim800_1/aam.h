#ifndef AMM_H
#define AMM_H


#define XIP_SIZE          20
#define XUSER_SIZE        20
#define XPASS_SIZE        20
#define XTELEF_SIZE       20
#define XNAME_SIZE        32
#define XFROM_SIZE        32
#define XTO_SIZE          32
#define XSUBJECT_SIZE     32
#define XRECIPIENT_SIZE   32
#define XSMSTIME_SIZE     32

//#define XSIZE20       20
//#define XSIZE32       32
/*=================================================================
  AMSG
*/
#define AMSG_MAX        20
//#define AMSG_TELEF_SIZE 20
//#define AMSG_FROM_SIZE  64
//#define AMSG_TO_SIZE    64
//#define AMSG_SUBJ_SIZE  64
#define AMSG_MSG_SIZE   400
//#define AMSG_SMSTIME_SIZE 32

#define AMSG_TYP_FROM_SMS   1
#define AMSG_TYP_FROM_MAIL  2
#define AMSG_TYP_FROM_MQTT  3
#define AMSG_TYP_FROM_EVENT 4
#define AMSG_TYP_TO_SMS     1
#define AMSG_TYP_TO_MAIL    2
#define AMSG_TYP_TO_MQTT    4
#define AMSG_TYP_TO_FILE    8

struct amsg {
  bool enabled;
  bool alloc;
  bool subjForMe;
  bool okTelef;
  uint8_t typTO;
  uint8_t typFROM;
  char telef[XTELEF_SIZE];
  char from[XFROM_SIZE];
  char to[XTO_SIZE];
  char subject[XSUBJECT_SIZE];
  char msg[AMSG_MSG_SIZE];
  char smsTime[XSMSTIME_SIZE];
};

extern struct amsg AMSG[];


/*=================================================================
  AFILE
*/
#define FILEDEF_SMSRD  0
#define FILEDEF_SMSWR  1
#define FILEDEF_MSG    2
#define FILEDEF_EVENT  3
#define FILEDEFmax     4
struct file_def {
  const char fname[20];
  size_t maxSize;
};



/*=================================================================
  AVAL
*/
#define VPTYP_NONE    0
#define VPTYP_BOOL    1
#define VPTYP_UI8     2
#define VPTYP_UI16    3
#define VPTYP_UI32    4
#define VPTYP_STR     5
#define VPTYP_FNGET   10
#define VPTYP_FNPOST  11

struct value_pop {
  const char key[10];
  uint8_t typ;
  uint8_t size;
  uint8_t toEEP;
  uint8_t cosi;
  void *p;
  const char *vdef;
  
};


/*=================================================================
  EVENT 
*/
#define EVT_MAX  4
#define EVT_DEST_MAX  2

#define EVT_PAR_SMS   1
#define EVT_PAR_MAIL  2
#define EVT_PAR_MQTT  4
#define EVT_PAR_FILE  8
#define EVT_PAR_DST1  0x10
#define EVT_PAR_DST2  0x20
#define EVT_PAR_TST   0x40
#define EVT_PAR_INV   0x80

#define EVT_PIN_TEST    100
#define EVT_PIN_POWER   101
#define EVT_PIN_75P     102
#define EVT_PIN_50P     103
#define EVT_PIN_25P     104
#define EVT_PIN_32      32
#define EVT_PIN_33      33
#define EVT_PIN_34      34
#define EVT_PIN_35      35


struct evt_dst {
  char telef[XTELEF_SIZE];
  char recipient[XRECIPIENT_SIZE];
};
struct evt_evt {
  uint8_t par1;     //0-sms, 1-mail, 2-mqtt, 3-dst1, 4-dst2, 6-test, 7-inverzia
  uint8_t func;
  uint8_t actDelay;
  uint8_t cosi;
  uint16_t vmin;
  uint16_t vmax;
  char subject[XSUBJECT_SIZE];
  char msg[20];
};
struct evt_defin {
  struct evt_dst dst[EVT_DEST_MAX];
  struct evt_evt evt[EVT_MAX];
};


struct evt_ram {
  uint16_t cnt_change;
  int old_stay;
  long last_act;  
};
extern struct evt_defin EVTDEF;
extern const struct value_pop TABVP_EVT[];
extern struct evt_ram EVTRAM[];

extern byte batReg[];

/*=================================================================
  SMS 
*/
#define MGSMSenabled  1
#define MGSMStoMAIL   2
#define MGSMStoMQTT   4
#define MGSMStoFILE   8
#define MGSMSdebRD1   0x10
#define MGSMSdebWR1   0x20

struct mgsms {
  uint8_t cfg;
  uint8_t replys;
  uint16_t maxSMSday;
  uint16_t resend;
  uint16_t rerecv;
  char recipient[XRECIPIENT_SIZE];
  char subject[XSUBJECT_SIZE];
};
extern struct mgsms MGSMS;
extern bool SMSforceRead;


#endif // AMM_H
