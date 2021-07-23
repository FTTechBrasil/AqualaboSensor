#ifndef __LOGGING_H__
#define __LOGGING_H__

/*
 * This logging was taken from UIPEthernet on https://github.com/UIPEthernet/UIPEthernet.
 *
 */

#if defined(ARDUINO)
  #include <Arduino.h>
#endif

#define	LOG_NONE		   -1	  /* Logging nothing */
#define	LOG_EMERG		    0	  /* system is unusable */
#define	LOG_ALERT		    1	  /* action must be taken immediately */
#define	LOG_CRIT		    2	  /* critical conditions */
#define	LOG_ERR			    3	  /* error conditions */
#define	LOG_WARNING		  4	  /* warning conditions */
#define	LOG_NOTICE		  5	  /* normal but significant condition */
#define	LOG_INFO		    6	  /* informational */
#define	LOG_DEBUG		    7	  /* debug-level messages */
#define	LOG_DEBUG_V1		8	  /* debug-verbose-level (v) messages */
#define	LOG_DEBUG_V2		9	  /* debug-verbose-level (vv) messages */
#define	LOG_DEBUG_V3		10	/* debug-verbose-level (vvv) messages */

/**
 * To see the warnings, you have to go to 
 * File>Prefences>Compile Warning>(Choose any level diferent from None)
 * in your Arduino IDE. 
 */
#warning "You can configure LogObject and ACTLOGLEVEL in 'utility/logging.h'. More verbosity more memory usage."
//#define ACTLOGLEVEL     LOG_DEBUG_V3
#define ACTLOGLEVEL     LOG_INFO

#if ACTLOGLEVEL>LOG_NONE 
   #if defined(ARDUINO)
     #if defined(__STM32F1__) || defined(__STM32F3__) || (!defined(ARDUINO_ARCH_STM32) && defined(STM32F3)) || defined(__STM32F4__) || defined(ARDUINO_ARCH_SAM)
        #define LogObject Serial1
     #else
        #define LogObject Serial
     #endif
     #define uart_send_str(x)             print(x)
     #define uart_send_strln(x)           println(x)
     #define uart_send_dec(x)             print(x)
     #define uart_send_decln(x)           println(x)
     #define uart_send_hex(x)             print(x,HEX)
     #define uart_send_hexln(x)           println(x,HEX)
     #define uart_send_bin(x)             print(x,BIN)
     #define uart_send_binln(x)           println(x,BIN)
     #define uart_send_buf_len(buf,len)   write(buf,len)
   #endif
#endif

#endif
