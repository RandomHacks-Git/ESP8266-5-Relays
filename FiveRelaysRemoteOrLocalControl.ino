//Visit my youtube channel RandomHacks at https://goo.gl/yP90fX
//You may use this code and modify it as you wish. If you publish it in your website make sure to provide a link to my youtube channel.
//Some snips of the code are based on the code found here: http://www.arduinesp.com/wifiwebserver


#include <ESP8266WiFi.h>

const char* ssid = "network_name"; //set ssid
const char* password = "network_password";  //set password
IPAddress ip(192, 168, 1, 73);  //set ip
IPAddress gateway(192,168,1,1); //set gateway
IPAddress subnet(255,255,255,0); //set subnet mask
WiFiServer server(80); //set port 
int refreshRate= 0; //refresh rate of the webpage in seconds (0=disabled)

int relayPins[5]= {13, 12, 14, 4, 5}; //Initializes the relayPin variables and assigns the corresponding GPIO pin numbers
boolean status[5]={LOW, LOW, LOW, LOW, LOW}; //Used to keep track of the status of each relayPin, starts LOW 
int buttonDelay=300; //set delay between accepted button presses, 300ms seems to work the best for me
unsigned long  buttonLastPressed = 0; //Used later to keep track of the time a button was last pressed
unsigned long timeout=0; //used in a while loop so the program does not get stuck, probably not needed
int analog=0; //variable that stores value read by the analog pin
boolean didFail=0; //in case the wireless connection fails changes to 1 and later is used to prevent the esp from constantly trying to reconnect to wifi since I still want to use the buttons even if there is no wireless available
unsigned long failTime=0; //used to keep track of the time when the wireless connection failed


void wifiConnect(){ //connects to wifi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);//connects to wifi
  WiFi.config(ip, gateway, subnet); //sets static ip, gateway and subnet
  unsigned long previousMillis=millis(); 
  while (WiFi.status() != WL_CONNECTED && millis() - previousMillis < 5000) { //prints a dot each 500ms until the wireless is connected, if there is no connection for 5 seconds it continues the program
    delay(500);                                              //this is important since you still want the buttons to work even if for some reason there is no wireless available
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED){ //prints stuff bellow on serial when connected
    Serial.println("");
    Serial.println("WiFi connected");   
    server.begin();// Start the server
    Serial.println("Server started");
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());// Prints the IP address
    Serial.println("/");
    didFail=0;}
  else{
    Serial.println("Connection error!"); //prints connection error if it wasn't able to connect to wireless
    failTime = millis(); //sets failTime to current time
    didFail=1;
  }
}

boolean debounce (unsigned long buttonLastPressed){ //This function is used to debounce the buttons, returns 1 if the passed time between the last pressed button 
  if( millis() - buttonLastPressed > buttonDelay ){ //and now is greater than the value set earlier with the buttonDelay variable, else it returns 0
    return 1;}
  else{ 
    return 0;}
}

void setup() {
 
{
   for(int i=0; i<5; i++)
   {
       pinMode(relayPins[i], OUTPUT); // Set the mode to OUTPUT
       digitalWrite(relayPins[i], LOW);//starting pins low 
   }
}
  
  Serial.begin(115200); //starts serial port and delays 10ms before continuing
  delay(10);
  
  wifiConnect();// Connect to WiFi network
}

void loop() {
  
  if (WiFi.status() != WL_CONNECTED && !didFail){ //if wireless is down and the connection did not fail previously retries to connect
    wifiConnect();
  }
  if (WiFi.status() != WL_CONNECTED && didFail && millis()-failTime > 15000){//if wireless is down and it did fail to connect previously and it failed more than 15 seconds ago retries to connect
    wifiConnect();
  }
  
  analog=analogRead(A0);

   if (debounce(buttonLastPressed) && analog > 100 && analog < 250){ //if the debounce function returns 1 and the analog read is between 10 and 150 
       status[0] = !status[0]; //toggles the status1 variable
       digitalWrite(relayPins[0], status[0]); //sets the pin according to the status1 variable
       buttonLastPressed = millis(); //sets the buttonLastPressed variable to the current time
       Serial.println("\nButton 1 pressed: \t"); 
       Serial.println(analog); //prints the read value by the analog pin, used for debugging
  }

   if (debounce(buttonLastPressed) && analog > 300 && analog < 480){
        status[1] = !status[1];
        digitalWrite(relayPins[1], status[1]);
        buttonLastPressed = millis();
        Serial.println("\nButton 2 pressed: \t"); 
        Serial.println(analog);
  }
  
   if (debounce(buttonLastPressed) && analog > 500 && analog < 650){
        status[2] = !status[2];
        digitalWrite(relayPins[2], status[2]);
        buttonLastPressed = millis();
        Serial.println("\nButton 3 pressed: \t"); 
        Serial.println(analog);
  }
   
   if (debounce(buttonLastPressed) && analog > 700 && analog < 850){
        status[3] = !status[3];
        digitalWrite(relayPins[3], status[3]);
        buttonLastPressed = millis();
        Serial.println("\nButton 4 pressed: \t"); 
        Serial.println(analog);
  }
   
   if (debounce(buttonLastPressed) && analog > 900){
        status[4] = !status[4];
        digitalWrite(relayPins[4], status[4]);
        buttonLastPressed = millis();
        Serial.println("\nButton 5 pressed: \t"); 
        Serial.println(analog);
  }


  
  WiFiClient client = server.available(); // Check if a client has connected, start the loop again if no client 
  if (!client) {
    delay(5);
    return; //goes back to the start of the loop, nothing past this runs unless a client connected
  }

  
  Serial.println("New Client");
  while(!client.available()&& timeout < 300){ // Wait until the client sends some data in case there is no data send in 300ms continues the program, this assures that the program does not get stuck here
    delay(1);
    timeout=millis()-timeout;
  }
  timeout=0;
  
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
   
  // Match the request
 
  if (request.indexOf("/relay1=ON") != -1) {
    digitalWrite(relayPins[0], HIGH);
    status[0] = HIGH;
  } 
  if (request.indexOf("/relay1=OFF") != -1){
    digitalWrite(relayPins[0], LOW);
    status[0] = LOW;
  }

   if (request.indexOf("/relay2=ON") != -1) {
    digitalWrite(relayPins[1], HIGH);
    status[1] = HIGH;
  } 
  if (request.indexOf("/relay2=OFF") != -1){
    digitalWrite(relayPins[1], LOW);
    status[1] = LOW;
  }
   if (request.indexOf("/relay3=ON") != -1) {
    digitalWrite(relayPins[2], HIGH);
    status[2] = HIGH;
  } 
  if (request.indexOf("/relay3=OFF") != -1){
    digitalWrite(relayPins[2], LOW);
    status[2] = LOW;
  }
   if (request.indexOf("/relay4=ON") != -1) {
    digitalWrite(relayPins[3], HIGH);
    status[3] = HIGH;
  } 
  if (request.indexOf("/relay4=OFF") != -1){
    digitalWrite(relayPins[3], LOW);
    status[3] = LOW;
  }
   if (request.indexOf("/relay5=ON") != -1) {
    digitalWrite(relayPins[4], HIGH);
    status[4] = HIGH;
  } 
  if (request.indexOf("/relay5=OFF") != -1){
    digitalWrite(relayPins[4], LOW);
    status[4] = LOW;
  }
  if (request.indexOf("/allRelays=ON") != -1){
    for (int i=0; i<5; i++){
      digitalWrite(relayPins[i], HIGH);
      status[i] = HIGH;
    }
  }
    if (request.indexOf("/allRelays=OFF") != -1){
    for (int i=0; i<5; i++){
      digitalWrite(relayPins[i], LOW);
      status[i] = LOW;
    }
  }

  if (request.indexOf("?refresh=") != -1){
    int firstNumber = request.lastIndexOf("=") + 1;
    int lastNumber = request.length()- 8;
    String temporary = request.substring(firstNumber, lastNumber);
    refreshRate = temporary.toInt();
  }
 
  // Return the response HTML
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("<!DOCTYPE HTML>");

  if (refreshRate > 0){ //sets webpage refresh rate to default (disabled) or any other requested value
    client.println("<META HTTP-EQUIV=\"refresh\" CONTENT=\"");
    client.print(refreshRate);
    client.print("; URL=http://");
    client.print(WiFi.localIP());
    client.print("\">");
  }
 
  client.println("<html> <title>Relay Remote Control</title> <h1 style=\"text-align: center;\">Relay Remote Control</h1> <hr /> <table style=\"height: 208px; width: 211px; margin-left: auto; margin-right: auto;\"> <tbody> <tr style=\"height: 23px;\"> <td style=\"width: 63.3px; height: 23px; text-align: center;\"><span style=\"text-decoration: underline;\"><span style=\"color: #333333; text-decoration: underline;\"><strong>Name</strong></span></span></td> <td style=\"width: 66.2333px; height: 23px; text-align: center;\"><span style=\"text-decoration: underline;\"><span style=\"color: #333333; text-decoration: underline;\"><strong>Status</strong></span></span></td> <td style=\"width: 80.4667px; height: 23px; text-align: center;\"><span style=\"text-decoration: underline;\"><span style=\"color: #333333; text-decoration: underline;\"><strong>Function</strong></span></span></td> </tr>");

  for (int i=0; i<5; i++){ //used to print status and function of each pin
  if (status[i]==LOW){
    client.println("<tr style=\"height: 23px;\"><td style=\"width: 63.3px; height: 23px; text-align: center;\"><span style=\"color: #000000;\"><strong>Relay");
    client.print(i+1);
    client.print("</strong></span></td><td style=\"width: 66.2333px; height: 23px; text-align: center;\"><span style=\"background-color: #ff0000;\">&nbsp;&nbsp; OFF&nbsp; &nbsp; </span></td> <td style=\"width: 80.4667px; height: 23px; text-align: center;\"><strong><span style=\"text-decoration: underline;\"><span style=\"color: #339966; text-decoration: underline;\"><a href=\"/relay");
    client.print(i+1);
    client.print("=ON\">Turn ON</a></span></span></strong></td></tr>");
  }
  else{
    client.println("<tr style=\"height: 23px;\"><td style=\"width: 63.3px; height: 23px; text-align: center;\"><span style=\"color: #000000;\"><strong>Relay");
    client.print(i+1);
    client.print("</strong></span></td><td style=\"width: 66.2333px; height: 23px; text-align: center;\"><span style=\"background-color: #339966;\">&nbsp;&nbsp;&nbsp; ON&nbsp; &nbsp;&nbsp; </span></td><td style=\"width: 80.4667px; height: 23px; text-align: center;\"><strong><span style=\"text-decoration: underline;\"><span style=\"color: #ff0000; text-decoration: underline;\"><a href=\"/relay");
    client.print(i+1);
    client.print("=OFF\">Turn OFF</a></span></span></strong></td></tr>");
  }
}
  
  client.println("</tbody> </table> <p style=\"text-align: center;\">Turn all: <a href=\"/allRelays=ON\">ON</a> <a href=\"/allRelays=OFF\">OFF</a> <br> </p> <form style= \"width:450px; margin-left: auto; margin-right: auto; text-align: center;\" method=\"GET\"> Refresh rate in seconds: <input style=\"width:50px;\" title=\"Please enter a number\" type=\"number\" name=\"refresh\"/> <input type=\"submit\" value=\"SET\" /> </form> <br> <hr /> <h5 style=\"text-align: center;\"><a title=\"RandomHacks youtube channel\" href=\"https://goo.gl/yP90fX\" target=\"_blank\">Click here to check out my youtube channel!</a></h5> </html>");
 
 
  Serial.println("Client disconnected");
  Serial.println("");
  
}

