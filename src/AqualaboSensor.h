#ifndef AqualaboSensor_h
#define AqualaboSensor_h

#include <ArduinoModbus.h>
#include <ArduinoRS485.h>


// Sensor Registers
#define WAITING_TIME_REG			0xA4
#define AVRG_PARA_REG				0xAA
#define NEW_MEAS_REG				0x01
#define MEASUREMENTS_REG			0x53



// This address will be configured as a first step
#define DEFAULT_ADDRESS			40
#define DEFAULT_WAITING_TIME	500 // in ms


/**************************************************************************!
    @brief  Aqualabo Sensor Class - Uses ModbusRTU (RS485) comunication
    Currently Supports: 
        NTU -> Turbidity and Temperature;

***************************************************************************/

class AqualaboSensorClass{
	public:
        //Constructor
        AqualaboSensorClass();
        AqualaboSensorClass(ModbusRTUClientClass& modbusRTUClient);

        // Methods
        uint8_t begin(unsigned long baudrate, uint8_t slave_address);
        uint8_t initSensor(uint8_t avg=1);
        uint8_t readWaitingTime(void);
        uint8_t readAverage(void);
        uint8_t changeAddress(uint8_t sensor_address);
        uint8_t writeAverage(uint8_t avg);
        uint8_t readNTU(void);


        struct sensorNTUVector
        {
            //! Variable: stores measured temperature in Celsius degrees
            float temperature;

            //! Variable: stores measured turbidity in NTU
            float turbidityNTU;

            //! Variable: stores measured turbidity in mg/l
            float turbidityMGL;

            //Sensor serial number variable
            char sensorSerialNumber[14];
        };
	    sensorNTUVector sensorNTU;

        union
		{
			uint16_t ints[2];
			float toFloat;
		}
		u16ItoFloat;


        // Variables
        uint16_t waitingTime;

    private:
        ModbusRTUClientClass* _modbusRTUClient = &ModbusRTUClient;

        uint8_t requestFromModbusHoldingRegisters(uint16_t register_address, uint16_t qnt_registers);
        uint8_t requestFromModbus(uint8_t modbus_function, uint16_t register_address, uint16_t qnt_registers, uint8_t retries=10);
        uint8_t readMeasures(float &parameter1, float &parameter2, float &parameter3, float &parameter4);


        uint8_t  _slave_address;

};

extern AqualaboSensorClass aqualaboSensorObj;

#endif