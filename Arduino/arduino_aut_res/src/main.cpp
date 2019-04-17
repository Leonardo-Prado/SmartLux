#include <Arduino.h>

//------------------------cria variaveis-------------------------//
String inputString = "";
long tempo;
long tempoLeituraPortas;
bool statusSaidas[24] ={false};
bool statusEntrada[24]={false};

//-----------------prototipagem das funções--------------------//
void trataSerial(String entradaSerial);
String saidasToString();
String entradasToString();






//----------------função setup-----------------------//
void setup() {
  // initialize serial:
  Serial.begin(115200);
  Serial1.begin(115200);
  //mapeia saidas
  for(int i = 30;i<=53;i++){
    pinMode(i, OUTPUT);
    Serial.println(i);
    delay(100);
  }
  for(int i = 2;i<=27;i++){
    if(i!=18&&i!=19){
      pinMode(i, INPUT_PULLUP);
      Serial.println(i);
      delay(100);
    }
  }
  for(int i = 30;i<53;i++){
    digitalWrite(i, HIGH);
  }
  tempo = millis();
  tempoLeituraPortas = millis();
}

//----------------função loop--------------------//
void loop() {

  if(millis()-tempo>500){
    tempo = millis();
    Serial1.println(saidasToString());
    Serial.println("Saidas: "+saidasToString());
    Serial.println("entradas: "+entradasToString());
  }
  if(millis()-tempoLeituraPortas>100){
    for(int i = 30;i<=53;i++){
      if(statusSaidas[i-30])
          digitalWrite(i, LOW);
      else
          digitalWrite(i, HIGH);     
    }
    int j = 0;
    for(int i = 2;i<=27;i++){
      if(i!=18&&i!=19){
         if(digitalRead(i)==HIGH&&!statusEntrada[j])
           statusEntrada[j] = !statusEntrada[j];
         else if(digitalRead(i)==LOW&&statusEntrada[j]){
           statusEntrada[j] = !statusEntrada[j];
           statusSaidas[j] = !statusSaidas[j];
         }
         j++;
      }
    }
    tempoLeituraPortas = millis();
  }

}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent1() {
  Serial.println("evento serial");
  String entradaSerial;
  while (Serial1.available()) {
    // get the new byte:
    char inChar = (char)Serial1.read();
    // add it to the inputString:
    entradaSerial += inChar;
    Serial.println(entradaSerial);
  }
  Serial.println(entradaSerial);
  inputString = entradaSerial;
  trataSerial(entradaSerial);
  
 
}

void trataSerial(String entradaSerial){
  if(entradaSerial.startsWith("Saida")){
    int i = entradaSerial.indexOf(":")+1;
    entradaSerial.remove(0,i);
    statusSaidas[entradaSerial.toInt()] = !statusSaidas[entradaSerial.toInt()];
    Serial1.println("recebido ="+saidasToString());
  }else if(entradaSerial.startsWith("SaidaIgual")){
    int i = entradaSerial.indexOf(":")+1;
    entradaSerial.remove(0,i);
    i = entradaSerial.indexOf("=");
    int elemento = entradaSerial.substring(0,i).toInt();
    entradaSerial.remove(0,i);
    statusSaidas[elemento] = (strcmp(entradaSerial.c_str(),"1"))?true:false;
    Serial1.println("recebido ="+saidasToString());
  }
}

String saidasToString(){
  String s;
  for(int i = 0;i<23;i++){
    if(statusSaidas[i])
      s += '1';
    else
      s += '0';
  }
  return s;
}
String entradasToString(){
  String s;
  for(int i = 0;i<23;i++){
    if(statusEntrada[i])
      s += '1';
    else
      s += '0';
  }
  return s;
}