| ESP8266 Pin | OLED Pin |
| ----------- | -------- |
| **GND**     | GND      |
| **3V**      | VCC      |
| **D1**      | SCL      |
| **D2**      | SDA      |


| ESP8266 Pin | Button Terminal |
| ----------- | --------------- |
| **GND**     | Terminal 1      |
| **D6**      | Terminal 2      |

| ESP8266 Pin | LED Connection                       |
| ----------- | ------------------------------------ |
| **3V**      | LED **+** (anode)                    |
| **GND**     | → 10Ω resistor → LED **-** (cathode) |


      +-----------------------------+
      |        ESP8266 (NodeMCU)   |
      |                             |
      |     [D1] ----> SCL --------+-----+
      |     [D2] ----> SDA --------|-----|--> OLED 0.96"
      |     [3V] ----> VCC --------|-----|
      |     [GND] ---> GND --------+-----+
      |                             |
      |     [D6] ----> PUSH BUTTON  o====o <--- GND
      |                             |
      |                             |
      |     [3V] ----> LED (+)      |
      |             (YELLOW LED)    |
      |     [GND] --> [10Ω Resistor]---> LED (-)
      +-----------------------------+

Legend:
  o====o  → Push button terminals
  --->    → Wire connection
  [ ]     → Pin label or component
