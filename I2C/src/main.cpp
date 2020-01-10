#include <Arduino.h>
#define FreqCPU 16000000
#define PV 1
#define BAUD 9600
#define UBRR FreqCPU/16/BAUD - 1

void UART_init(unsigned int ubrr){ //8 data bits, 1 stop bit, 9600 baud rate, no parity bit
UCSR0A = (0<<U2X0) ; //modo assincrono normal
UCSR0B = (1<<RXEN0)|(1<<TXEN0);
UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
UBRR0H = (unsigned char)(ubrr>>8); //baud rate
UBRR0L = (unsigned char)ubrr;
}
void I2C_init(void) {
TWSR = 0x00; //status register
TWBR = 72; //I2C with 100 kHz
TWCR = (1<<TWEN); //this bit enables TWI operation and activates TWI interface
//when TWEN is 1, TWI takes control over the I/O pins connected to the SCL and SDA pins
}

void I2C_stop(void){
//TWINT is the interrupt flag; TWSTO is the stop condition bit; TWEN to activate TWI interface
TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void start_with_addr(uint8_t addr) {
//Send a start condition; TWSTA
TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
while (!(TWCR & (1<<TWINT)));
TWDR = addr;
TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
while (!(TWCR & (1<<TWINT)));
}

void I2C_write(uint32_t data){
TWDR = data;
TWCR = (1<<TWINT)|(1<<TWEN);
while (!(TWCR & (1<<TWINT)));
}

uint8_t I2C_read(void){//I2C read ack function
TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA); //Enable TWI, generation of ack and set interrupt high
while (!(TWCR & (1<<TWINT)));
return TWDR; //Return received data
}
uint8_t I2C_read2(void){//I2C read ack function
TWCR=(1<<TWEN)|(1<<TWINT); //Enable TWI, generation of ack and set interrupt high
while (!(TWCR & (1<<TWINT)));
return TWDR; //Return received data
}

void configuration(void){
/*Pointer bits:
0000 = RFU, Reserved for Future Use (Read-Only register)
0001 = Configuration register (CONFIG)
0101 = Temperature register (TA)
1000 = Resolution register
*/
start_with_addr(0x30);
I2C_write(0x01); // Write CONFIG register
I2C_write(0x60); //write data
I2C_stop(); // send STOP command
}

float readT(void){
start_with_addr(0x30);
I2C_write(0x05);
I2C_stop();
start_with_addr(0x31);
uint8_t T = I2C_read();
uint8_t T2 = I2C_read2();
return T,T2;

/*float temp = T & 0x0FFF;
temp /= 16.0;
if (T & 0x1000) temp -= 256;
return temp;*/
}

void setup() {
// put your setup code here, to run once:
UART_init(UBRR);
I2C_init();
configuration();
    Serial.println("Temp (C)");
}

float Temperature;
void loop() {
uint8_t T1,T2=readT();

    
Temperature = 0.062 * T2 + 13.91;
Serial.println(Temperature);
delay(2500);
}