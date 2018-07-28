#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

const char *ssid = "Astrey";
const char *password = "474643756hammerman";

int redColorAddress = 0;
int greenColorAddress = 1;
int blueColorAddress = 2;

byte red;
byte green;
byte blue; 

ESP8266WebServer server ( 80 );

void handleIndex() {
  String index = "<html><head><style>body{background-color:darkslategrey}input#red[type=range]::-moz-range-track{background:linear-gradient(to right,#fff,#f00)}input#green[type=range]::-moz-range-track{background:linear-gradient(to right,#fff,#0f0)}input#blue[type=range]::-moz-range-track{background:linear-gradient(to right,#fff,#00f)}input[type=range]{-webkit-appearance:none;margin:18px 0;width:100%;display:block;margin-right:auto;margin-left:auto}input[type=range]:focus{outline:0}input[type=range]::-webkit-slider-runnable-track{width:100%;height:8.4px;cursor:pointer;animate:.2s;box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d;border-radius:1.3px;border:.2px solid #010101}input[type=range]::-webkit-slider-thumb{box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d;border:1px solid #000;height:36px;width:16px;border-radius:3px;background:#fff;cursor:pointer;-webkit-appearance:none;margin-top:-14px}input[type=range]::-moz-range-track{width:100%;height:8.4px;cursor:pointer;animate:.2s;box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d;border-radius:1.3px;border:.2px solid #010101}input[type=range]::-moz-range-thumb{box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d;border:1px solid #000;height:36px;width:16px;border-radius:3px;background:#fff;cursor:pointer}input[type=range]::-ms-track{width:100%;height:8.4px;cursor:pointer;animate:.2s;background:transparent;border-color:transparent;border-width:16px 0;color:transparent}input[type=range]::-ms-fill-lower{border:.2px solid #010101;border-radius:2.6px;box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d}input[type=range]::-ms-fill-upper{border:.2px solid #010101;border-radius:2.6px;box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d}input[type=range]::-ms-thumb{box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d;border:1px solid #000;height:36px;width:16px;border-radius:3px;background:#fff;cursor:pointer}p{color:black;text-align:center}</style></head><body><div><p>Red</p><input id='red' type='range' onchange='changeColor(this)' min='0' max='255'/><br/><p>Green</p><input id='green' type='range' onchange='changeColor(this)' min='0' max='255'/><br/><p>Blue</p><input id='blue' type='range' onchange='changeColor(this)' min='0' max='255'/></div></body><script>function changeColor(){console.log('changeColor >>');var e=document.getElementById('red'),o=document.getElementById('green'),n=document.getElementById('blue');if(e&&o&&n){var l='#'+parseInt(e.value).toString(16)+parseInt(o.value).toString(16)+parseInt(n.value).toString(16);document.body.style.backgroundColor=l,console.log('color?red='+e.value+'&green='+o.value+'&blue='+n.value);var t=new XMLHttpRequest;t.open('GET','color?red='+e.value+'&green='+o.value+'&blue='+n.value,!0),t.send()}}changeColor();</script></html>";
  int contentSize = index.length();
  int transferred = 0;
  int packSize = 500;

  server.setContentLength(contentSize);
	server.send(200, "text/html", index.substring(transferred, transferred + packSize));

  transferred = transferred + packSize < contentSize
    ? transferred + packSize
    : contentSize;

  do{
    server.sendContent(index.substring(transferred, transferred + packSize));

    transferred = transferred + packSize < contentSize
      ? transferred + packSize
      : contentSize;
  }while(transferred < contentSize);
}

void setColor(){
  red = server.arg("red").toInt();
  green = server.arg("green").toInt();
  blue = server.arg("blue").toInt();

  if(red >= 0 || red <= 255){
    analogWrite(0, red * 4);
  }

  if(green >= 0 || green <= 255){
    analogWrite(1, green * 4);
  }

  if(blue >= 0 || blue <= 255){
    analogWrite(2, blue * 4);
  }

  EEPROM.write(redColorAddress, red);
  EEPROM.write(greenColorAddress, green);
  EEPROM.write(blueColorAddress, blue);
  EEPROM.commit();

  server.send(200, "text/plain", "");
}

void getColor(){
  String response = String("{ red: ") + red + ", green: " + green + ", blue: " + blue + "}";
  server.send(200, "application/json", response);
}

void handleNotFound() {
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for ( uint8_t i = 0; i < server.args(); i++ ) {
		message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
	}

	server.send ( 404, "text/plain", message );
}

void setupColor(){
  red = EEPROM.read(redColorAddress);
  green = EEPROM.read(greenColorAddress);
  blue = EEPROM.read(blueColorAddress);

  if(red >= 0 || red <= 255){
    analogWrite(0, red * 4);
  }

  if(green >= 0 || green <= 255){
    analogWrite(1, green * 4);
  }

  if(blue >= 0 || blue <= 255){
    analogWrite(2, blue * 4);
  }
}

void setup ( void ) {
  EEPROM.begin(512);
    
  //GPIO 1 (TX) swap the pin to a GPIO.
  pinMode(1, FUNCTION_3);

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);

  setupColor();

	WiFi.begin ( ssid, password );
	Serial.println ( "" );

	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
	}

	server.on ( "/", handleIndex);
	server.on ( "/setColor", setColor);
  server.on ( "/getColor", getColor);
	server.onNotFound (handleNotFound);
	server.begin();
}

void loop ( void ) {
	server.handleClient();
}
