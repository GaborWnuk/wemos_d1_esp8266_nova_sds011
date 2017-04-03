# WeMos D1 mini with Nova SDS011 air quality sensor and BMP085 barometric sensor
Simple Air Quality measure device based on WeMOS D1 mini and Nova SDS011 air quality sensor

## Description
This example shows possible implementation of Air Quality monitring device based on ESP8266 (in this case - WeMos D1 mini) and SDS011 air quality sensor which in my opinion is one of the best for our purposes, has reasonable 8000 hours of constant running lifetime (1 year!) which this example extends to 15 - 20 years (because we're using device's sleep capability and running it only for few seconds every 5 minutes) and has reasonable accuracy for reasonable price ($25).

The example assume you're using InfluxDB as your database somewhere in your wifi network and Grafana to display measurement data.

Total estimated price (if you order all the stuff from AliExpress): $21,80 (SDS011) + $3 (WeMos D1 mini) + $1,8 (BMP085) = $26,6.

## Pinout connections
### Warning - 5V vs 3.3V logic
Most of ESP's work with 3.3V logic however SDS011 is 5V Rx/Tx device, which require at least 4.7 volt to work.

However, in my case 3.3V logic didn't work so i got rid of my voltage divider. You won't find it in schematics below - it should be placed in front of D0 pin (Rx) of ESP.

ALWAYS check maximum values accepted by your microcontroler. In my case it does work perfectly, but in your case it might simply cook your device.

## Known issues
### Deep sleep
WeMos - as any other ESP8266 device - gives you ability to enter deep sleep and basically minimize power usage to absolute minimum. However i've came across weird behaviour with SDS011 being constantly on, while ESP8266 was in deep sleep. And because i'd rather keep tear and wear of SDS011 down (rated 8000 hours of constant running) - this example doesn't use deep sleep of ESP at all.

### Arduino IDE in favour of MicroPython
I absolutely love MicroPython, however i needed Software Serial support on different pins than UART available through MicroPython (as only available UART is hardware one, which is used while you program your ESP, which renders it obsolete).

# License
MIT.

Copyright (c) 2017 Gabor Wnuk.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.