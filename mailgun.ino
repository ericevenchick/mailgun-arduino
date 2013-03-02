#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

// define servos
Servo pan;
Servo tilt;
const int panPin = 9;
const int tiltPin = 10;

// define trigger pin
const int triggerPin = 7;

// define time to hold trigger per dart
const int triggerTime = 280;

int panSpeed = 0;

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  // configure pins
  digitalWrite(triggerPin, LOW);
  pinMode(triggerPin, OUTPUT);
  pan.attach(panPin);
  tilt.attach(tiltPin); 
  
  // fire once on reset
  doFire();
}

void sendResponse(EthernetClient client) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<body><h1>MailGun</h1></body>");
          client.println("</html>");
}        
  
void doFire() {
  Serial.println("Firing!");
  digitalWrite(triggerPin, HIGH);
  delay(triggerTime); 
  digitalWrite(triggerPin, LOW);
  if (panSpeed == 0) {
    panSpeed = 10;
  } else {
    panSpeed = 0;
  }
}

void doPan() {
  pan.write(panSpeed); 
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // received empty line after data, end of request
        if (c == '\n' && currentLineIsBlank) {
          doFire();
          sendResponse(client);
          break;
        }
        if (c == '\n') {
          // start of new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // received character on line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
  
  doPan();
}

