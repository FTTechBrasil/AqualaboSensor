/***********************************************************************
 * Includes
 ***********************************************************************/
#include "AqualaboSensor.h"
#include "utility/logging.h"


/***********************************************************************
 * Class contructors
 ***********************************************************************/	
AqualaboSensorClass::AqualaboSensorClass()
{
}

AqualaboSensorClass::AqualaboSensorClass(ModbusRTUClientClass& modbusRTUClient) :
	_modbusRTUClient(&modbusRTUClient)
{
}


/***********************************************************************
 * Public Methods of the Class
 ***********************************************************************/

/**!
 * @name begin()
 * @brief initialize modbus object
 * @param void
 * @returns uint8_t "1" if no error, "0" if error
*/
uint8_t AqualaboSensorClass::begin(unsigned long baudrate, uint8_t slave_address)
{
	#if ACTLOGLEVEL>=LOG_DEBUG_V3
    	LogObject.uart_send_strln(F("AqualaboSensorClass::begin(unsigned long baudrate, uint8_t slave_address) DEBUG_V3:Function started"));
  	#endif

    _slave_address = slave_address;
	waitingTime = DEFAULT_WAITING_TIME;

    // start the Modbus RTU client
    if (_modbusRTUClient->begin(baudrate)) {
		#if ACTLOGLEVEL>=LOG_NOTICE
			LogObject.uart_send_strln(F("Modbus Client (Master) successfully initialized"));
		#endif
    }else{
		#if ACTLOGLEVEL>=LOG_ALERT
			LogObject.uart_send_strln(F("Couldn't initialize Modbus Client (Master), check wiring and server (slave) address"));
		#endif
		return 0;
	}

    return initSensor();
}


/**!
 * @name initSensor()
 * @brief Get the necessary waiting time to get measures after asking for them
 * @param avg: has to be between 1 and 50
 * @returns uint8_t "1" if no error, "0" if error
*/
uint8_t AqualaboSensorClass::initSensor(uint8_t avg)
{
	#if ACTLOGLEVEL>=LOG_DEBUG_V3
    	LogObject.uart_send_strln(F("AqualaboSensorClass::initSensor(void) DEBUG_V3:Function started"));
  	#endif
	
	//Get the necessary waiting time to get measures after asking for them
	if(!readWaitingTime())
	{
		return 0;
	}
	
	//Configure average
	if(writeAverage(avg))
	{
		if(readAverage() != avg)
		{
			#if ACTLOGLEVEL>=LOG_WARNING
				LogObject.uart_send_strln(F("Error configuring average"));
			#endif
			return 0;
		}
	}
	else
	{
		#if ACTLOGLEVEL>=LOG_WARNING
			LogObject.uart_send_strln(F("Communication error writing average"));
		#endif

		return 0;
	}
	
	#if ACTLOGLEVEL>=LOG_DEBUG_V3
		LogObject.uart_send_strln(F("Sensor succefully initialized"));
	#endif
	return 1;	
}


/**!
 * @name readWaitingTime()
 * @brief Get the necessary waiting time to get measures after asking for them
 * @param void
 * @returns uint8_t "1" if no error, "0" if error
*/
uint8_t AqualaboSensorClass::readWaitingTime(void)
{	
	#if ACTLOGLEVEL>=LOG_DEBUG_V3
    	LogObject.uart_send_strln(F("AqualaboSensorClass::readWaitingTime(void) DEBUG_V3:Function started"));
  	#endif

	if (requestFromModbusHoldingRegisters(WAITING_TIME_REG, 1))	{
        waitingTime = _modbusRTUClient->read();
		
		#if ACTLOGLEVEL>=LOG_DEBUG
			LogObject.uart_send_str(F("waitingTime: "));
			LogObject.uart_send_dec(waitingTime);
			LogObject.uart_send_strln(F("[ms]"));
		#endif
		
		return 1;
	}
	
	// If no response from the slave, print an error message.
	#if ACTLOGLEVEL>=LOG_WARNING
		LogObject.uart_send_str(F("Communication error reading waiting time"));
	#endif
	
	return 0;

}




/**!
 * @name:	writeAverage()
 * @description: Configure the average value (1-50)
 * @param : avg
 * @returns: uint8_t "1" if no error, "0" if error
*/
uint8_t AqualaboSensorClass::writeAverage(uint8_t avg)
{
	#if ACTLOGLEVEL>=LOG_DEBUG_V3
    	LogObject.uart_send_strln(F("AqualaboSensorClass::writeAverage(void) DEBUG_V3:Function started"));
  	#endif

	uint8_t retries = 0;
	int8_t result = 0;

	if (avg > 50) avg = 50;

	while (!result && (retries < 5))
	{
		result = _modbusRTUClient->holdingRegisterWrite(_slave_address, AVRG_PARA_REG, avg);
		if(!result){
			#if ACTLOGLEVEL>=LOG_DEBUG
				LogObject.uart_send_str(F("Write avr holding register failed! Attempt: "));
				LogObject.uart_send_dec(retries);
				LogObject.uart_send_str(F(" Error: "));
				LogObject.uart_send_decln(_modbusRTUClient->lastError());
			#endif
			delay(100);
			retries++;
		}else{
			#if ACTLOGLEVEL>=LOG_DEBUG_V3
				LogObject.uart_send_strln(F("Succefully wrote avr holding register"));
			#endif
			return 1;	
		}
	}

	// If no response from the slave, print an error message.
	#if ACTLOGLEVEL>=LOG_WARNING
		LogObject.uart_send_str(F("Communication error when trying to write avr register"));
		LogObject.uart_send_str(F(" Error: "));
		LogObject.uart_send_decln(_modbusRTUClient->lastError());
	#endif
	
	return 0;	
}


/**!
 * @name:	readAverage()
 * @description: Configure the average value (1-50)
 * @param : avg
 * @returns: uint8_t average value (1-50) if OK, 0 if error
*/
uint8_t AqualaboSensorClass::readAverage(void)
{	
	#if ACTLOGLEVEL>=LOG_DEBUG_V3
    	LogObject.uart_send_strln(F("AqualaboSensorClass::readAverage(void) DEBUG_V3:Function started"));
  	#endif

	uint8_t average = 0;

	if (requestFromModbusHoldingRegisters(AVRG_PARA_REG, 1))	{
        average = (uint8_t)_modbusRTUClient->read();
		
		#if ACTLOGLEVEL>=LOG_DEBUG
			LogObject.uart_send_str(F("average: "));
			LogObject.uart_send_dec(average);
			LogObject.uart_send_strln(F(" "));
		#endif
		
		return average;
	}
	
	// If no response from the slave, print an error message.
	#if ACTLOGLEVEL>=LOG_WARNING
		LogObject.uart_send_str(F("Communication error reading avarage"));
		LogObject.uart_send_str(F(" Error: "));
		LogObject.uart_send_decln(_modbusRTUClient->lastError());
	#endif
	
	return average;
}



/**!
 * @name:	readNTU()
 * @description: Reads the sensor data
 * @param : void
 * @returns: uint8_t '1' if OK, 0 if error
*/
uint8_t AqualaboSensorClass::readNTU(void)
{
	#if ACTLOGLEVEL>=LOG_DEBUG_V3
    	LogObject.uart_send_strln(F("AqualaboSensorClass::readNTU(void) DEBUG_V3:Function started"));
  	#endif

	// Initialize variables
	sensorNTU.temperature = 0;
	sensorNTU.turbidityNTU = 0;
	float parameter3_dummy = 0; //turbidityFNU is always equal to turbidityNTU
	sensorNTU.turbidityMGL = 0;

	uint8_t retries = 0;
	int8_t result = 0;
	uint8_t validMeasure = 0;

	while (!validMeasure && (retries < 3))
	{
		result = readMeasures(sensorNTU.temperature,
							sensorNTU.turbidityNTU,
							parameter3_dummy,
							sensorNTU.turbidityMGL);

		if ((sensorNTU.temperature != 0)
		|| (sensorNTU.turbidityNTU != 0)
		|| (parameter3_dummy 	   != 0)
		|| (sensorNTU.turbidityMGL != 0))
		{
			validMeasure = 1;
		}
		else
		{
			delay(1000);
		}
		retries++;
	}

	return result;
}


/**!
 * @name readMeasures()
 * @brief Returns all the measures of the sensor
 * @param pameter1: will be filled with read value
 * @param pameter2: will be filled with read value
 * @param pameter3: will be filled with read value
 * @param pameter4: will be filled with read value
 * @returns uint8_t "1" if no error, "0" if error
*/
uint8_t AqualaboSensorClass::readMeasures(float &parameter1, float &parameter2, float &parameter3, float &parameter4)
{
	#if ACTLOGLEVEL>=LOG_DEBUG_V3
    	LogObject.uart_send_strln(F("AqualaboSensorClass::readMeasures(float &parameter1, float &parameter2, float &parameter3, float &parameter4) DEBUG_V3:Function started"));
  	#endif

	uint8_t retries = 0;
	int8_t result = 0;

	
	while (!result && (retries < 5))
	{
		// 0x000F means thar we want 4 parameters -> 00000000 00001111, each one represents a position
		result = _modbusRTUClient->holdingRegisterWrite(_slave_address, NEW_MEAS_REG, 0x000F);
		if(!result){
			#if ACTLOGLEVEL>=LOG_DEBUG
				LogObject.uart_send_str(F("Write number of desired parameters failed! Attempt: "));
				LogObject.uart_send_dec(retries);
				LogObject.uart_send_str(F(" Error: "));
				LogObject.uart_send_decln(_modbusRTUClient->lastError());
			#endif
			delay(100);
			retries++;
		}else{
			#if ACTLOGLEVEL>=LOG_DEBUG_V3
				LogObject.uart_send_strln(F("Succefully wrote number of desired parameters"));
			#endif
		}
	}

	// Important delay
	delay(waitingTime + 10);
	
	if (result)
	{
		result = requestFromModbusHoldingRegisters(MEASUREMENTS_REG, 0x08);

		if (result)
		{
			u16ItoFloat.ints[1] = _modbusRTUClient->read();
			u16ItoFloat.ints[0] = _modbusRTUClient->read();
			parameter1 = u16ItoFloat.toFloat;

			u16ItoFloat.ints[1] = _modbusRTUClient->read();
			u16ItoFloat.ints[0] = _modbusRTUClient->read();
			parameter2 = u16ItoFloat.toFloat;
			
			u16ItoFloat.ints[1] = _modbusRTUClient->read();
			u16ItoFloat.ints[0] = _modbusRTUClient->read();
			parameter3 = u16ItoFloat.toFloat;

			u16ItoFloat.ints[1] = _modbusRTUClient->read();
			u16ItoFloat.ints[0] = _modbusRTUClient->read();
			parameter4 = u16ItoFloat.toFloat;
		}
		else
		{
			// If no response from the slave, print an error message.
			#if ACTLOGLEVEL>=LOG_WARNING
				LogObject.uart_send_str(F("Communication error reading sensor values"));
				LogObject.uart_send_str(F(" Error: "));
				LogObject.uart_send_decln(_modbusRTUClient->lastError());
			#endif
			
			return 0;
		}
	}
	else
	{
		#if ACTLOGLEVEL>=LOG_WARNING
			LogObject.uart_send_str(F("Write number of desired parameters failed! Attempt: "));
			LogObject.uart_send_dec(retries);
			LogObject.uart_send_str(F(" Error: "));
			LogObject.uart_send_decln(_modbusRTUClient->lastError());
		#endif

		return 0;
	}

	return 1;
}



/**!
 * @name requestFromModbusHoldingRegisters()
 * @brief request from modbus a number of holding registers
 * @param register_address: address of the first disered address
 * @param qnt_registers: number of registers to be requested
 * @returns uint8_t "1" if no error, "0" if error
*/
uint8_t AqualaboSensorClass::requestFromModbusHoldingRegisters(uint16_t register_address, uint16_t qnt_registers)
{
	#if ACTLOGLEVEL>=LOG_DEBUG_V3
    	LogObject.uart_send_strln(F("AqualaboSensorClass::requestFromModbusHoldingRegisters(void) DEBUG_V3:Function started"));
  	#endif
	  
	return requestFromModbus(HOLDING_REGISTERS, register_address, qnt_registers);
}

/**!
 * @name requestFromModbus()
 * @brief request from modbus
 * @param modbus_function: desired modbus function, like holding_registers or coil
 * @param register_address: address of the first disered address
 * @param qnt_registers: number of registers to be requested
 * @returns uint8_t "1" if no error, "0" if error
*/
uint8_t AqualaboSensorClass::requestFromModbus(uint8_t modbus_function, uint16_t register_address, uint16_t qnt_registers, uint8_t retries)
{	
	#if ACTLOGLEVEL>=LOG_DEBUG_V3
    	LogObject.uart_send_strln(F("AqualaboSensorClass::requestFromModbus(void) DEBUG_V3:Function started"));
  	#endif

	uint8_t _retries = 0;
	int8_t result = 0;
	
	while (!result && (_retries < retries))	{
		result = _modbusRTUClient->requestFrom(_slave_address, modbus_function, register_address, qnt_registers);
		if(!result){
			#if ACTLOGLEVEL>=LOG_DEBUG
				LogObject.uart_send_str(F("Resquest from Modbus Server failed! Attempt: "));
				LogObject.uart_send_dec(_retries);
				LogObject.uart_send_str(F(" Error: "));
				LogObject.uart_send_decln(_modbusRTUClient->lastError());			
			#endif
			delay(100);
			retries++;
		}else{
			#if ACTLOGLEVEL>=LOG_DEBUG_V3
				LogObject.uart_send_strln(F("Got Modbus message"));
			#endif
		}
	}

	return result;
}

// instantiate static
AqualaboSensorClass aqualaboSensorObj;
