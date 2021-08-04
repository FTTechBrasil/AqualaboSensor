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
 * Modbus through RS485. This library uses Arduino Modbus and ArduinoRS485 together
 * with an RS485 Click made by FTTech.
 * 
 * 
 * Itens needed:
 *  - FTTech SmartNode Board (This also works with arduino boards)
 *  - RS485 Click
 *  - Arqualabo Turbidity Sensor
 *
 * -----------------------------------------
*/

#include "FTTech_SAMD51Clicks.h"
#include "AqualaboSensor.h"

#define DEBUG_SERIAL Serial
#define NTU_SERIAL Serial3
static const uint8_t rs485_click = 3;

uint8_t tx_pin = 42;
uint8_t de_pin = A9;
uint8_t re_pin = A5;
// ID do escravo (slave_id) e baudrate da comunicacao
const uint8_t slave_id = 1;
const uint32_t baud = 9600;
// resultado da comunicacao modbus
int result;

void setup() {
  FTClicks.begin();
  FTClicks.turnON_5V();   // Power StepUp to power the ryon
  FTClicks.turnON(rs485_click);   // Power the RS485 Click

  DEBUG_SERIAL.begin(9600);
  while(!DEBUG_SERIAL)
    ;

  Serial.println(F("********************************************************"));
  Serial.println(F("FT Smart Node 3S (Master) <-> Aqualabo NTU Sensor (Slave)"));
  Serial.println(F("********************************************************"));


  RS485 = RS485Class(NTU_SERIAL, tx_pin, de_pin, re_pin);
  ModbusRTUClient = ModbusRTUClientClass(RS485);

  aqualaboSensorObj = AqualaboSensorClass(ModbusRTUClient);

  result = aqualaboSensorObj.begin(baud, slave_id);
  if (result) {
    Serial.println("Cliente Modbus (Master) inicializado com sucesso");
  }
  else{
    Serial.println("Falha ao inicializar o cliente (Master) Modbus");
  }


}

void loop() {
  aqualaboSensorObj.readNTU();

  Serial.print("Temperature: ");
  Serial.println(aqualaboSensorObj.sensorNTU.temperature, 2);

  Serial.print("turbidity NTU: ");
  Serial.println(aqualaboSensorObj.sensorNTU.turbidityNTU, 2);

  Serial.print("turbidity MGL: ");
  Serial.println(aqualaboSensorObj.sensorNTU.turbidityMGL, 2);

  delay(10000);
}
