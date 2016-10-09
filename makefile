default: assembler
assembler:
	avr-gcc -Wall -DF_CPU=16000000UL -mmcu=atmega328p -o main.elf main.S
	#avr-gcc -Wall -DF_CPU=16000000UL -mmcu=atmega328p -o main.elf foo.S
	avr-objcopy -O ihex -R .eeprom main.elf out.hex
c:
	avr-gcc -Wall -DF_CPU=16000000UL -mmcu=atmega328p -o main.elf main.c 
	#avr-gcc -Wall -DF_CPU=16000000UL -mmcu=atmega328p -o main.elf foo.c 
	avr-objcopy -O ihex -R .eeprom main.elf out.hex
arduino:
	avr-g++ -L/usr/arduino/lib -I/usr/arduino/include -Wall -DF_CPU=16000000UL -Os -mmcu=atmega328p -o main.elf main.c -larduino
upload: out.hex
	#avrdude -c arduino -p m328p -P /dev/arduino -U flash:w:out.hex
	avrdude -c arduino -p m328p -b 57600 -P /dev/seeeduino -U flash:w:out.hex

all: default upload 

