# How to use this

### 0. Reminder: For new projects

If you're creating a new project using this methodology, here's a reminder of how to do it

First, install platformio

Then, Search for the embedded device that you're targeting to

```bash
$ platformio boards | grep ${the_one_youre_looking_for}
```

Use the board code (This project is using "nanoatmega328")
and initialize the project targeting CLion IDE

```bash
$ platformio init --ide clion --board nanoatmega328
```

Put the code in the `src` directory. Here's a hello world example for `src/helloworld.cpp`

```cpp
#include <Arduino.h>

int counter = 1;

void setup() {
    Serial.begin(9200);
}

void loop() {
    Serial.println("Hello World!");
    Serial.print("Counter = ");
    Serial.println(counter);
    delay(1000);
    counter++;
}
```

Finally, load the project using **CLion** IDE. Compile it using the hammer icon.

### 1. Compiling

To compile, just hit the hammer icon in IDE.

### 2. Load into the device

just execute

```bash
$ pio run -t upload
```

### 3. To attach a console

```bash
$ pio device monitor -b 9200 # change the baudrate accordingly
```

## Troubleshooting

If you have problem by opening the usb device, find which device is and change its owner

```bash
$ # Assuming the device is /dev/ttyUSB0 and user is langelog
$ sudo chown langelog /dev/ttyUSB0
```
