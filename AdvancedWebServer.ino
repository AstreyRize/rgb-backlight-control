#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <Ticker.h>

int redColorAddress = 0;
int greenColorAddress = 1;
int blueColorAddress = 2;
int effectSpeedAddress = 3;
int effectAddress = 8;
int loginAddress = 10;
int passwordAddress = 30;

byte red = 255;
byte green = 255;
byte blue = 255;
float effectSpeed = 0.01;
byte effect = 0;

byte upScalePercent = 100;
bool percentIsIncrease = false;

Ticker effectTrigger;
ESP8266WebServer server ( 80 );


//////////////////////////////////////////////////
//                                              //
//                 Controllers                  //
//                                              //
//////////////////////////////////////////////////


void handleIndex() {
  String index = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'/><style>body{background-color:darkslategray}input#red[type=range]::-moz-range-track{background:linear-gradient(to right,#fff,#f00)}input#green[type=range]::-moz-range-track{background:linear-gradient(to right,#fff,#0f0)}input#blue[type=range]::-moz-range-track{background:linear-gradient(to right,#fff,#00f)}input#red[type=range]::-webkit-slider-runnable-track{background:linear-gradient(to right,#fff,#f00)}input#green[type=range]::-webkit-slider-runnable-track{background:linear-gradient(to right,#fff,#0f0)}input#blue[type=range]::-webkit-slider-runnable-track{background:linear-gradient(to right,#fff,#00f)}input[type=range]{-webkit-appearance:none;margin:18px 0;width:100%;display:block;margin-right:auto;margin-left:auto}input[type=range]:focus{outline:0}input[type=range]::-webkit-slider-runnable-track{width:100%;height:8.4px;cursor:pointer;animate:.2s;box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d;border-radius:1.3px;border:.2px solid #010101}input[type=range]::-webkit-slider-thumb{box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d;border:1px solid #000;height:36px;width:16px;border-radius:3px;background:#fff;cursor:pointer;-webkit-appearance:none;margin-top:-14px}/*!*background: #367ebd;*!*/input[type=range]::-moz-range-track{width:100%;height:8.4px;cursor:pointer;animate:.2s;box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d;border-radius:1.3px;border:.2px solid #010101}input[type=range]::-moz-range-thumb{box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d;border:1px solid #000;height:36px;width:16px;border-radius:3px;background:#fff;cursor:pointer}input[type=range]::-ms-track{width:100%;height:8.4px;cursor:pointer;animate:.2s;background:transparent;border-color:transparent;border-width:16px 0;color:transparent}input[type=range]::-ms-fill-lower{border:.2px solid #010101;border-radius:2.6px;box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d}input[type=range]::-ms-fill-upper{border:.2px solid #010101;border-radius:2.6px;box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d}input[type=range]::-ms-thumb{box-shadow:1px 1px 1px #000,0 0 1px #0d0d0d;border:1px solid #000;height:36px;width:16px;border-radius:3px;cursor:pointer}button{background-color:#4caf50;border:0;color:white;padding:20px;text-align:center;text-decoration:none;display:inline-block;font-size:16px;margin:4px 2px;cursor:pointer;border-radius:12px;width:100%;opacity:1}button:active{opacity:.5}</style></head><body><div><input id='red' type='range' onchange='changeColor()' min='0' max='255'/><br/><input id='green' type='range' onchange='changeColor()' min='0' max='255'/><br/><input id='blue' type='range' onchange='changeColor()' min='0' max='255'/><br/><br/><input id='speed' type='range' onchange='changeSpeed()' min='1' max='1000'/><br/><button id='noEffect' onclick='changeEffect(0)'>No effect</button><button id='gradient' onclick='changeEffect(1)'>Gradient</button><button id='upScale' onclick='changeEffect(2)'>Breath</button></div></body><script>function changeColor(){var e=document.getElementById('red'),t=document.getElementById('green'),n=document.getElementById('blue');if(e&&t&&n){var r=Array.from(document.getElementsByTagName('button')),a='rgb('+parseInt(e.value)+', '+parseInt(t.value)+', '+parseInt(n.value)+')';r.forEach(function(e){e.style.backgroundColor=a});var u=new XMLHttpRequest;u.open('GET','setColor?red='+e.value+'&green='+t.value+'&blue='+n.value,!0),u.send()}}function getColor(){var a=new XMLHttpRequest;a.onreadystatechange=function(){if(4===a.readyState&&200===a.status){var e=JSON.parse(a.responseText);if(!e)return;var t=document.getElementById('red'),n=document.getElementById('green'),r=document.getElementById('blue');if(!t||!n||!r)return;t.value=e.red,n.value=e.green,r.value=e.blue}},a.open('GET','getColor',!0),a.send()}function changeSpeed(){var e=document.getElementById('speed');if(e){var t=new XMLHttpRequest;t.open('GET','setEffectSpeed?effectSpeed='+e.value/1e3,!0),t.send()}}function getSpeed(){var n=new XMLHttpRequest;n.onreadystatechange=function(){if(4===n.readyState&&200===n.status){var e=JSON.parse(n.responseText);if(!e)return;var t=document.getElementById('speed');if(!t)return;t.value=1e3*e.effectSpeed}},n.open('GET','getEffectSpeed',!0),n.send()}function changeEffect(e){var t=new XMLHttpRequest;t.open('GET','/setEffect?effect='+e,!0),t.send()}getColor(),getSpeed();</script></html>";
  sendDataByPart(index);
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
  String response = String("{\"red\": ") + red + ", \"green\": " + green + ", \"blue\": " + blue + "}";
  server.send(200, "application/json", response);
}

void setEffectSpeed(){
  effectSpeed = server.arg("effectSpeed").toFloat();
  implementEffect();

  EEPROM.put(effectSpeedAddress, effectSpeed);
  EEPROM.commit();
  
  String response = String("{\"effectSpeed\": ") + String(effectSpeed, 5) + "}";
  server.send(200, "application/json", response);
}

void getEffectSpeed(){
  String response = String("{\"effectSpeed\": ") + String(effectSpeed, 5) + "}";
  server.send(200, "application/json", response);
}

void setEffect(){
  effect = server.arg("effect").toInt();
  implementEffect();
  
  String response = String("{\"effect\": ") + effect + "}";
  server.send(200, "application/json", response);
}

void implementEffect(){
    switch(effect){
    //no effect
    case 0:
      effectTrigger.detach();
      break;
    //gradient
    case 1:
      red = 255;
      green = 0;
      blue = 0;
      
      effectTrigger.detach();
      effectTrigger.attach(effectSpeed, gradient);

      EEPROM.write(effectAddress, effect);
      EEPROM.commit();
      break;
    //upScale
    case 2:
      effectTrigger.detach();
      effectTrigger.attach(effectSpeed, upScale);

      EEPROM.write(effectAddress, effect);
      EEPROM.commit();
      break;
    default:
        server.send(404, "text/plain", "Effect not found");
      break;  
  }
}

void getEffect(){
  String response = String("{\"effect\": ") + effect + "}";
  server.send(200, "application/json", response);
}

void signIn(){
  String index = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'/><style>body{background-color:darkslategray}input[type=text],[type=password],select{width:100%;padding:12px 20px;margin:8px 0;display:inline-block;border:1px solid #ccc;border-radius:4px;box-sizing:border-box}button{background-color:#4caf50;border:0;color:white;padding:20px;text-align:center;text-decoration:none;display:inline-block;font-size:16px;margin:4px 2px;cursor:pointer;border-radius:12px;width:100%;opacity:1}button:active{opacity:.5}</style></head><body><div><input id='login' type='text' maxlength='20' minlength='1' onkeyup='checkLetters(login)'/><input id='password' type='password' maxlength='20' minlength='1' onkeyup='checkLetters(password)'/><button id='signIn' onclick='signIn()'>Sign In</button></div></body><script>function signIn(){var e=document.getElementById('login'),n=document.getElementById('password');if(e&&n){var t=new XMLHttpRequest;t.open('GET','setWifiCredentials?login='+e.value+'&password='+n.value,!0),t.send()}}function checkLetters(e){e.value=e.value.replace(/[^a-z  0-9]/i,'')}</script></html>";
  sendDataByPart(index);
}

void setWifiCredentials(){
  String login = server.arg("login");
  String password = server.arg("password");

  writeStringToEEPROM(loginAddress, login);
  writeStringToEEPROM(passwordAddress, password);

  server.send(200, "text/plain", "");
}

//////////////////////////////////////////////////
//                                              //
//                  Other                       //
//                                              //
//////////////////////////////////////////////////

void sendDataByPart(String data){
  int contentSize = data.length();
  int transferred = 0;
  int packSize = 500;

  server.setContentLength(contentSize);
  server.send(200, "text/html", data.substring(transferred, transferred + packSize));

  transferred = transferred + packSize < contentSize
    ? transferred + packSize
    : contentSize;

  do{
    server.sendContent(data.substring(transferred, transferred + packSize));

    transferred = transferred + packSize < contentSize
      ? transferred + packSize
      : contentSize;
  }while(transferred < contentSize);
}

void writeStringToEEPROM(int address, String data){
  char buf[data.length() + 1];
  data.toCharArray(buf, data.length() + 1);

  for(int i = 0; i < data.length() + 1; i++){
      EEPROM.write(address + i, buf[i]);
      EEPROM.commit();
  }
}

void readStringToEEPROM(int address, char *buf, int bufLength){
  for(int i = 0; i < bufLength; i++){
      buf[i] = EEPROM.read(address + i);
  }
}


//////////////////////////////////////////////////
//                                              //
//                  Effects                     //
//                                              //
//////////////////////////////////////////////////


void gradient(){
  // Растет зеленый
  if(red == 255 && green < 255 && blue == 0){
    green++;
  }
  else if(red > 0 && green == 255  && blue == 0){
    red--;
  }
  else if(red == 0 && green == 255  && blue == 0){
    blue = 1;
  }

  // Растет синий
  else if(green == 255 && blue < 255 && red == 0){
    blue++;
  }
  else if(green > 0 && blue == 255 && red == 0){
    green--;
  }
  else if(green == 0 && blue == 255 && red == 0){
    red = 1;
  } 

  // Растет красный
  else if(blue == 255 && red < 255 && green == 0){
    red++;
  }
  else if(blue > 0 && red == 255 && green == 0){
    blue--;
  }
  else if(blue == 0 && red == 255 && green == 0){
    green = 1;
  }

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

void upScale(){
  byte upScaleRed = red * upScalePercent / 100;
  analogWrite(0, upScaleRed * 4);
  
  byte upScaleGreen = green * upScalePercent / 100;
  analogWrite(1, upScaleGreen * 4);

  byte upScaleBlue = blue * upScalePercent / 100;
  analogWrite(2, upScaleBlue * 4);

  if(percentIsIncrease){
    if(upScalePercent < 100){
      upScalePercent++;
    }
    else{
      percentIsIncrease = !percentIsIncrease;
      upScalePercent--;
    }
  }
  else{
    if(upScalePercent > 0){
      upScalePercent--;
    }
    else{
      percentIsIncrease = !percentIsIncrease;
      upScalePercent++;
    }
  }
}


//////////////////////////////////////////////////
//                                              //
//                  Setup                       //
//                                              //
//////////////////////////////////////////////////


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

void setupEffect(){
  EEPROM.get(effectSpeedAddress, effectSpeed);
  effect = EEPROM.read(effectAddress);

  implementEffect();
}

void fastBlink(){
  pinMode(LED_BUILTIN, OUTPUT);

  for(int i = 0; i < 8; i++){
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(250);
  }
}

void slowBlink(){
  pinMode(LED_BUILTIN, OUTPUT);
  
  for(int i = 0; i < 4; i++){
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(500);
  }
}

void setup ( void ) {
  EEPROM.begin(512);

  int setupButton = digitalRead(3);

  if(setupButton == HIGH){
    fastBlink();
    
    WiFi.softAPdisconnect(true);
    WiFi.softAP("ESPap");
    
    server.on ( "/", signIn);
    server.on ( "/setWifiCredentials", setWifiCredentials);
    server.onNotFound (handleNotFound);
    server.begin();
  }
  else{
    slowBlink();

    char login[20];
    readStringToEEPROM(loginAddress, login, 20);
    char password[20];
    readStringToEEPROM(passwordAddress, password, 20);
    
    WiFi.begin( login, password );

    while ( WiFi.status() != WL_CONNECTED ) {
      delay ( 500 );
    }

    server.on ( "/", handleIndex);
    server.on ( "/setColor", setColor);
    server.on ( "/getColor", getColor);
    server.on ( "/setEffect", setEffect);
    server.on ( "/getEffect", getEffect);
    server.on ( "/setEffectSpeed", setEffectSpeed);
    server.on ( "/getEffectSpeed", getEffectSpeed);
    server.onNotFound (handleNotFound);
    server.begin();

    // turn off softAP.
    WiFi.softAPdisconnect(true);
  }

  //GPIO 1 (TX) swap the pin to a GPIO.
  pinMode(1, FUNCTION_3);

  //GPIO 3 (RX) swap the pin to a GPIO.
  pinMode(3, FUNCTION_3);

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);

  setupColor();
  setupEffect();
}

void loop ( void ) {
	server.handleClient();
}
