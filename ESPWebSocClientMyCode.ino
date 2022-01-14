#include <WiFi.h>
#include <WebSocketClient.h>  // library needed for connecting to a server.

//credentials needed to connect to the WiFi network
 
const char* ssid     = "YourNetworkName";
const char* password = "YourNetworkPassword";

//specify the host to which we will connect, and the URL path to be used.
 
char path[] = "/echo";
char host[] = "demos.kaazing.com";
 
WebSocketClient webSocketClient;   // object of class WebSocketClient, which will expose the functionality needed to interact with the server.
WiFiClient client;                 //object of class WiFiClient, which will be used by the WebSocketClient object under the hood
 
void setup() {
  Serial.begin(115200);

  //establishing WiFi connection. 
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
  delay(5000);

  //Once connected to the WiFi network, we will establish a TCP connection to the host by calling the connect method of our WiFiClient.
 
  if (client.connect(host, 80)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
  }

  //establish a websocket handshake with the server via HTTP request.
  //it will send to the server the HTTP request to perform the upgrade of the connection to websocket.
  //This upgrade is needed since the websocket protocol was developed in such a way that the server can use the same port to talk with both HTTP and websocket clients

  //assign to the path and host data members of our WebSocketClient the host and path of the URL that we defined early in global variables.
 
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
  }
 
}
 
void loop() {
  String data;
 
  if (client.connected()) {
 
    webSocketClient.sendData("Info to be echoed back");               //send data to the server.
 
    webSocketClient.getData(data);                                    //recieve data from the server. Here server will just echo what we sent.
    if (data.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(data);
    }
 
  } else {
    Serial.println("Client disconnected.");
  }
 
  delay(3000);
 
}
