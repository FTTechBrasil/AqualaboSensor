/**! -----------------------------------------
 * The MIT License (MIT)
 * 
 * Copyright (c) 2021 FTTech
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * -----------------------------------------
 *
 * Read NTU Turbidity
 * 
 * This examples show how to read the NTU Turbidity Sesor from Aqualabo using
 * Modbus through RS485 and sending its data to another device trough a XBee 
 * connection. This library uses Arduino Modbus and ArduinoRS485 together with an 
 * RS485 Click made by FTTech.
 * 
 * 
 * Itens needed:
 *  - FTTech SmartNode 1S or 3S board (This also works with arduino boards)
 *  - RS485 Click
 *  - XBee Pro S3B Click
 *  - Aqualabo Turbidity Sensor
 *
 * -----------------------------------------
 */

#include "FTTech_SAMD51Clicks.h"

#include "AqualaboSensor.h"

#include "FTTech_Xbee.h"

/* *****************************************
 * GLOBAL VARIABLES
 */
#define DEBUG_SERIAL Serial
const uint32_t baud_debug = 9600; // Serial Monitor baud
const uint32_t delay_inter = 10000; // Delay between
const uint8_t print_form = 2; // Data form option on Serial Monitor .print
/******************************************/

/* *****************************************
 * MODBUS DEFINED VARIABLES
 */
#define NTU_SERIAL Serial1
uint8_t tx_pin = 1; // TX Pin for 1S board
uint8_t de_pin = 2; // DE Pin for 1S board
uint8_t re_pin = A1; // RE Pin for 1S board
const uint8_t slave_id = 1; // Slave ID
const uint32_t baud = 9600; // Baudrate for MODBUS
int result; // Store the result of connection
/******************************************/

/* *****************************************
 * XBEE DEFINED VARIABLES
 */
int BAUDRATE = 0XXXXX; // XBee baudrate
int address_HB = 0xXXXXXXXX; // Most significant bytes from destination XBee's address
int address_LB = 0xXXXXXXXX; // Least significant bytes from destination XBee's address
uint8_t payload[100] = {0}; // Creates payload, and defines its maximum size
const uint8_t xbee_id = 2; // Defines the XBee click ID within the board
FTTech_Xbee xbee(); // Create object with the serial parameter
/*****************************************/

void setup() {
    FTClicks.begin(); // If you are using FTTech boards
    FTClicks.turnON_5V(); // Power StepUp to power the ryon
    FTClicks.turnON(slave_id); // Power the RS485 Click
    FTClicks.turnON(xbee_id); // Power the XBee Click

	/* *****************************************
	 * DEBUG INIT AND PAUSES CODE IF MONITOR IS NOT OPEN
	 */
    DEBUG_SERIAL.begin(baud_debug);
    while (!DEBUG_SERIAL);
	/******************************************/

    Serial.println(F("********************************************************"));
    Serial.println(F("Aqualabo NTU Sensor (Slave)<-Modbus->FT Smart Node 1S (Master)-XBee->"));
    Serial.println(F("********************************************************"));

    /* *****************************************
     * XBEE INIT AND BEGIN
     */
    xbee.setChipID(FTClicks.ChipID);
    xbee.begin(BAUDRATE);
    /******************************************/

    /* *****************************************
     * MODBUS INIT AND BEGIN
     */
    RS485 = RS485Class(NTU_SERIAL, tx_pin, de_pin, re_pin);
    ModbusRTUClient = ModbusRTUClientClass(RS485);
    aqualaboSensorObj = AqualaboSensorClass(ModbusRTUClient);
    result = aqualaboSensorObj.begin(baud, slave_id);
    /*******************************************/

    /* ********************************************
     * TEST MODBUS MODULE INIT
     */
    if (result) {
        Serial.println("Modbus Client (Master) successfully initiated");
    } else {
        Serial.println("Modbus Client (Master) initiation failed");
    }
    /********************************************/

}

void loop() {

    //Reads sensor values and send them to defined XBee address

    aqualaboSensorObj.readNTU();

    Serial.print("Temperature: ");
    Serial.println(aqualaboSensorObj.sensorNTU.temperature, print_form);
    xbee.messageClearPayload();
    xbee.message("Temp", aqualaboSensorObj.sensorNTU.temperature);
    xbee.getOutPayload(payload);
    xbee.sendPayload(address_HB, address_LB);

    Serial.print("Turbidity NTU: ");
    Serial.println(aqualaboSensorObj.sensorNTU.turbidityNTU, print_form);
    xbee.messageClearPayload();
    xbee.message("TurbNTU", aqualaboSensorObj.sensorNTU.turbidityNTU);
    xbee.getOutPayload(payload);
    xbee.sendPayload(address_HB, address_LB);

    Serial.print("Turbidity MGL: ");
    Serial.println(aqualaboSensorObj.sensorNTU.turbidityMGL, print_form);
    xbee.messageClearPayload();
    xbee.message("TurbMGL", aqualaboSensorObj.sensorNTU.turbidityMGL);
    xbee.getOutPayload(payload);
    xbee.sendPayload(address_HB, address_LB);

    delay(delay_inter);
}