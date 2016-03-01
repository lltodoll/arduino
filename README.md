# arduino

arduino-temp-greenhouse
A simple greenhouse controller based on arduino

Scretch controls the temperature in greenhouse over 433 mhz transmitter.
The wireless socket turns on / off, if the temperature under / over a specifier temperatur (DAY_TARGET_TEMP +- DIVIATION_TEMP / NIGHT_TARGET_TEMP +- DIVIATION_TEMO).

ldr value > 500 = DAY
ldr value < 500 = NIGHT

LDR INPUT PIN = A0
RF 433 TRANSMITTER PIN = 10
DS18S20 PIN = 2
