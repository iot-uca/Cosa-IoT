/*
  Cosmos device

  Circuit:
   Ethernet shield attached to pins 10, 11, 12, 13

  created 18 Dec 2019
  by Francisco Ortiz Peñaloza
  modified 25 Junio 2020
  based on work by Tom Igoe and Adrian McEwen

*/

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(192, 168, 0, 3); // numeric IP for Google (no DNS)
//char server[] = "www.google.com";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

// Variables to measure time between requests
unsigned long lastPostStateTime = 0; // last time you connected to the server, in milliseconds
const unsigned long postingStateInterval = 10 * 1000; //delay between updates, in milliseconds

unsigned long lastCommandExecutionTime = 0;
const unsigned long executingCommandInterval = 30 * 1000;

const unsigned char COOLING = 3;

typedef enum {
  COMMAND_NONE = 'N',
  COMMAND_COOLING_ON = 'O',
  COMMAND_COOLING_OFF = 'F'
} COMMAND;

void postState() {

  // close any connection before send a new request
  client.stop();

  Serial.print("Posting status to ");
  Serial.print(server);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 8090)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());

    // build response body
    String requestBody = "[{\"data_stream\":\"temperature\", \"value\":" + String (random(20, 25)) + "}, {\"data_stream\":\"is_cooling_on\", \"value\":" + String(digitalRead(COOLING)) + "}]";
    String contentLenght = "Content-Length: " + String (requestBody.length());

    // Make a HTTP request:
    client.println("POST /data-points HTTP/1.1");
    client.println("Host: 192.168.0.3:8090");
    client.println("Content-Type: application/json;version=1.0.0");
    client.println(contentLenght.c_str());
    client.println("Accept: */*");
    client.println("Connection: close");
    client.println();
    client.println(requestBody.c_str());
    client.println();

    // note the time that the connection was made
    lastPostStateTime = millis();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // initialize COOLING as an output pin
  pinMode(COOLING, OUTPUT);

  inializeEthernet();
}

void loop() {

  COMMAND command = readCommand();
  executeCommand(command);

  if (millis() - lastPostStateTime > postingStateInterval) {
    postState();
  }

  if (millis() - lastCommandExecutionTime > executingCommandInterval) {
    getNextCommand();
  }

}
