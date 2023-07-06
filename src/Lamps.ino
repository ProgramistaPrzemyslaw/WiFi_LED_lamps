#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
// Replace with your network credentials
const char* ssid     = "ssis";
const char* password = "pass";

// Set web server port number to 80
WiFiServer server(80);

// Decode HTTP GET value
String redString = "0";
String greenString = "0";
String blueString = "0";
int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;

// Variable to store the HTTP req  uest
String header;
String WarmWhiteState = "off";
String ColdWhiteState = "off";
// Red, green, and blue pins for PWM control
const int Freq = 22000;
const int redPin = 15;     // 13 corresponds to GPIO13
const int greenPin = 13;   // 12 corresponds to GPIO12
const int bluePin = 12;    // 14 corresponds to GPIO14
const int WarWhitePin = 14;
const int ColdWhitePin = 16;
const int output = 2;
// Setting PWM bit resolution
const int resolution = 256;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  analogWriteFreq(Freq);
  // configure LED PWM resolution/range and set pins to LOW
 // analogWriteRange(resolution);
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
  pinMode(WarWhitePin,OUTPUT);
  pinMode(ColdWhitePin,OUTPUT);
  digitalWrite(WarWhitePin,LOW); 
  digitalWrite(ColdWhitePin,LOW);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {            // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
        

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">");
            client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>");
            client.println("</head><body><div class=\"container\"><div class=\"row\"><h1>WiFi Lamp</h1></div>");
            client.println("<a class=\"btn btn-primary btn-lg\" href=\"#\" id=\"change_color\" role=\"button\">Change Color</a> ");
            client.println("<input class=\"jscolor {onFineChange:'update(this)'}\" id=\"rgb\"></div>");
            client.println("<script>function update(picker) {document.getElementById('rgb').innerHTML = Math.round(picker.rgb[0]) + ', ' +  Math.round(picker.rgb[1]) + ', ' + Math.round(picker.rgb[2]);");
            client.println("document.getElementById(\"change_color\").href=\"?r\" + Math.round(picker.rgb[0]) + \"g\" +  Math.round(picker.rgb[1]) + \"b\" + Math.round(picker.rgb[2]) + \"&\";}</script></body></html>");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            


client.println("<body>");
client.println("<p>Warm White Brightness</p>");
client.println("<p><span id=\"textWarmSliderValue\">%WSLIDERVALUE%</span></p>");
client.println("<p><input type=\"range\" onchange=\"updateWarmSliderPWM(this)\" id=\"WarmSlider\" min=\"0\" max=\"100\" value=\"%WSLIDERVALUE%\" step=\"1\" class=\"slider\"></p>");
client.println("<script>");
client.println("function updateWarmSliderPWM(element) {");
client.println("var WarmValue = document.getElementById(\"WarmSlider\").value;");
client.println("document.getElementById(\"textWarmSliderValue\").innerHTML =  WarmValue;");
client.println("console.log(WarmValue);");
client.println("var xhr = new XMLHttpRequest();");
client.println("xhr.open(\"GET\", \"/warm-slider?value=\"+ WarmValue, true);");
client.println("xhr.send();");
client.println("}");
client.println("</script>");
client.println("</body>");
            

client.println("<body>");
client.println("<p>Cold White Brightness</p>");
client.println("<p><span id=\"textColdSliderValue\">%CSLIDERVALUE%</span></p>");
client.println("<p><input type=\"range\" onchange=\"updateColdSliderPWM(this)\" id=\"ColdSlider\" min=\"0\" max=\"100\" value=\"%CSLIDERVALUE%\" step=\"1\" class=\"slider\"></p>");
client.println("<script>");
client.println("function updateColdSliderPWM(element) {");
client.println("var ColdValue = document.getElementById(\"ColdSlider\").value;");
client.println("document.getElementById(\"textColdSliderValue\").innerHTML =  ColdValue;");
client.println("console.log(ColdValue);");
client.println("var xhr = new XMLHttpRequest();");
client.println("xhr.open(\"GET\", \"/cold-slider?value=\"+ ColdValue, true);");
client.println("xhr.send();");
client.println("}");
client.println("</script>");
client.println("</body>");     

            client.println("");
            // The HTTP response ends with another blank line
            

            // Request sample: /?r201g32b255&
            // Red = 201 | Green = 32 | Blue = 255
            if(header.indexOf("GET /?r") >= 0 ||header.indexOf("GET /13/on?r") >= 0 ||header.indexOf("GET /13/off?r") >=0||header.indexOf("GET /15/on?r") >= 0 ||header.indexOf("GET /15/off?r") >= 0 ) {
              pos1 = header.indexOf('r');
              pos2 = header.indexOf('g');
              pos3 = header.indexOf('b');
              pos4 = header.indexOf('&');
              redString = header.substring(pos1+1, pos2);
              greenString = header.substring(pos2+1, pos3);
              blueString = header.substring(pos3+1, pos4);
              Serial.println(redString.toInt());
              Serial.println(greenString.toInt());
              Serial.println(blueString.toInt());
              analogWrite(redPin, 255-redString.toInt());
              analogWrite(greenPin, 255-greenString.toInt());
              analogWrite(bluePin, 255-blueString.toInt()); 
            }

          
            client.println("</body></html>");
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
