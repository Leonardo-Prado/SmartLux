#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <esp_wifi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <string.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ListaTempoProgramado.h>
#include <Auxiliares/ManipuladorDeTempo.h>

ManipuladorDeTempo manipuladorDeTempo;
bool statusSaidas[24];
AsyncWebServer server(80);
long tempo;
bool conectado;
bool mDNSInicializado;
IPAddress myIP;
File configFile;
char wifiMode[50] = "WIFI_AP_STA";
char wifiApNome[50] = "central";
char wifiApSenha[50] = "";
char wifiSSIDNome[50] = "";
char wifiSSIDSenha[50] = "";
char configCode[] = "10000001";
bool tentandoConectar;
long tempoBuscaRede;
long tempoRelogio;
long t1 = 1;
bool falhaConexao;
short timeOutCount = 0;
hw_timer_t *timer = NULL;

//----------------------relogio-----------------//
const char *servidorNTP = "a.st1.ntp.br"; // Servidor NTP para pesquisar a hora
const int fusoHorario = -10800;           // Fuso horário em segundos (-03h = -10800 seg)
const int taxaDeAtualizacao = 1800000;    // Taxa de atualização do servidor NTP em milisegundos
WiFiUDP ntpUDP;                           // Declaração do Protocolo UDP
NTPClient timeClient(ntpUDP, servidorNTP, fusoHorario, 60000);
bool relogioIniciado = false;
String horario = "";
ListaTempoProgramado listaProg;
ListaTempoProgramado listaProgDoDia;

//---------------------função para reset watchdog----------------------//
void IRAM_ATTR resetModule()
{
  Serial.println("Reiniciando watchdog");
  esp_restart(); //reinicia o chip
}
void carregarProgramacao(String arquivo);
void trataSerial(String entradaSerial);
void entradaSerial();
bool SSIDExiste(String SSIDParaProcurar);
String trocaValor(String dado, String dadoNovo, String local);
String ipToString(IPAddress ip);
String getIp();
String setSSIDConfig(AsyncWebServerRequest *request);
String setStatusSaida(String saida);
String getStatusSaidas();
String getRedesDisponiveis();
String charToString(char *texto);
char *stringToChar(String string);
void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
bool handleFileRead(String path, AsyncWebServerRequest *request);
String getContentType(String filename);
void detalhesDaRede();
void listaDeArquivos();
File carregaConfigFile();
String carregaDadosArquivo(String dado, File file);
void mudaWifiMode(String wifiMode);
void escreveArquivo(String fileName, String dados);
void substituiEmArquivo(String dado, String dadoNovo, String fileNome);
void setWifiMode(String wifiModeRequest);
String getConfigStatus();
void criarProgramacao(String prog, String arquivo);
String tratarNovaProgramacao(AsyncWebServerRequest *request);
String getTodaProgramacao();
void getProgDoDia(ManipuladorDeTempo manipuladorDeTempo);

void setup()
{

  //---------------watchdog----------------------------//
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &resetModule, true);
  //timer, tempo (us), repetição
  timerAlarmWrite(timer, 6000000, true);
  timerAlarmEnable(timer); //habilita a interrupção

  //---------------------Iniciando Serial-----------------//
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial2.begin(115200);

  //---------------------Iniciando SPIFFS-------------//
  if (!SPIFFS.begin())
  {
    Serial.println("erro ao iniciar SPIFFS");
  }

  //------------------Carregando configuração-----------------//
  if (SPIFFS.exists("/configFile.txt"))
  {
    Serial.println("abrindo arquivo");
    Serial.flush();
    strcpy(wifiMode, carregaDadosArquivo("wifiMode=", carregaConfigFile()).c_str());
    strcpy(wifiApSenha, carregaDadosArquivo("wifiApSenha=", carregaConfigFile()).c_str());
    strcpy(wifiApNome, carregaDadosArquivo("wifiApNome=", carregaConfigFile()).c_str());
    strcpy(wifiSSIDNome, carregaDadosArquivo("wifiSSIDNome=", carregaConfigFile()).c_str());
    strcpy(wifiSSIDSenha, carregaDadosArquivo("wifiSSIDSenha=", carregaConfigFile()).c_str());
  }
  else
  {
    Serial.println("criando arquivo");
    escreveArquivo("/configFile.txt", "wifiMode=" + charToString(wifiMode) + "%");
    escreveArquivo("/configFile.txt", "wifiApNome=" + charToString(wifiApNome) + "%");
    escreveArquivo("/configFile.txt", "wifiApSenha=" + charToString(wifiApSenha) + "%");
    escreveArquivo("/configFile.txt", "wifiSSIDNome=" + charToString(wifiSSIDNome) + "%");
    escreveArquivo("/configFile.txt", "wifiSSIDSenha=" + charToString(wifiSSIDSenha) + "%");
  }

  //------------------Iniciando wifi-----------------//
  mudaWifiMode(wifiMode);

  //-----------------Iniciando wifi ModoAp-----------------//
  if (wifiApSenha == "")
    WiFi.softAP(wifiApNome);
  else
    WiFi.softAP(wifiApNome);
  myIP = WiFi.softAPIP();

  //---------Requisições-------------------//
  server.on("/upload", HTTP_GET, [](AsyncWebServerRequest *request) {
      String uploadHtml = "<html><head></head><body><form method=\"post\" enctype=\"multipart/form-data\"><input type=\"file\" name=\"name\" multiple><input class=\"button\" type=\"submit\" value=\"Upload\"></form></body></html>";
      request->send(200, "text/html", uploadHtml);
  });

  server.on("/configCode", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("configCode : " + charToString(configCode));
    request->send(200, "text/plain", configCode);
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    Serial.println("host: " + request->host());
    Serial.println("URL: " + request->url());
    Serial.println("content type: " + request->contentType());
    if (request->url().startsWith("/getWifiMode"))
      request->send(200, "text/plain", wifiMode);
    else if (request->url().startsWith("/setWifiMode"))
    {
      setWifiMode(request->url());
      request->send(200, "text/plain", "ok");
    }
    else if (request->url().startsWith("/getConfigStatus"))
    {
      String s = getConfigStatus();
      request->send(200, "text/plain", s);
    }
    else if (request->url().startsWith("/getRedesDisponiveis"))
    {
      String s = getRedesDisponiveis();
      request->send(200, "text/plain", s);
    }
    else if (request->url().startsWith("/getStatusSaidas"))
    {
      String s = getStatusSaidas();
      request->send(200, "text/plain", s);
    }
    else if (request->url().startsWith("/setStatusSaida"))
    {
      String s = setStatusSaida(request->url());
      request->send(200, "text/plain", s);
    }
    else if (request->url().startsWith("/setSSIDConfig"))
    {
      Serial.println("request = " + request->url());
      setSSIDConfig(request);
    }
    else if (request->url().startsWith("/getIp"))
    {
      request->send(200, "text/plain", getIp());
    }
    else if (request->url().startsWith("/novaProgramacao"))
    {
      request->send(200, "text/plain", tratarNovaProgramacao(request));
    }
    else if (request->url().startsWith("/getTodaProgramacao"))
    {
      request->send(200, "text/plain", getTodaProgramacao());
    }
    else if (!handleFileRead(request->url(), request))
      request->send(404, "text/plain", "404: Not Found");
  });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html").setCacheControl("max-age=30");

  //--------------recebe arquivos-------------//
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200);
  },
            onUpload);

  //----------inicia o servidor---------------//
  server.onFileUpload(onUpload);
  server.begin();

  //----------------------inicialização dos tempos de tarefas--------------------//
  tempo = millis();
  tempoBuscaRede = millis();
  tempoRelogio = millis();

  //-------------------------Carregar Programação-----------------------------//
  carregarProgramacao("/configFile.txt");
}

void loop()
{
  /* if (Serial2.available()){
    Serial.println("valor na entrada Serial");
    entradaSerial();
  }*/
  timerWrite(timer, 0);
  if (millis() - tempoBuscaRede > t1)
  {
    if (!conectado && wifiSSIDNome != "" && wifiMode != "WIFI_AP")
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        if (SSIDExiste(charToString(wifiSSIDNome)))
        {
          WiFi.begin(wifiSSIDNome, wifiSSIDSenha);
          t1 = 5000;
        }
        else
          t1 = 12000;
      }
      else
        conectado = true;
    }
    else if (wifiMode != "WIFI_AP")
    {
      if (!mDNSInicializado)
      {
        //------------------------Iniciando MDNS -------------------//
        if (!MDNS.begin("central"))
          Serial.println("Erro ao configurar MDNS");
        else
        {
          Serial.println("MDNS iniciado");
          MDNS.addService("http", "tcp", 80);
          mDNSInicializado = true;
        }
      }
      else
        mDNSInicializado = true;
    }
    else
    {
      Serial.println("Rede iniciado");
      t1 = 5000;
    }
    tempoBuscaRede = millis();
    //WiFi.printDiag(Serial);
  }

  if (conectado && !relogioIniciado)
  {
    timeClient.begin();
    relogioIniciado = true;
    timeClient.update();
    manipuladorDeTempo.setTempo((timeClient.getEpochTime()));
    getProgDoDia(manipuladorDeTempo);
  }
  else if (conectado && relogioIniciado && millis() - tempoRelogio > 1000)
  {
    timeClient.update();
    horario = timeClient.getFormattedTime();
    manipuladorDeTempo.setTempo((timeClient.getEpochTime()));
    getProgDoDia(manipuladorDeTempo);
    String agoraString = String(manipuladorDeTempo.getHora())+String(manipuladorDeTempo.getMinuto());
    if(listaProgDoDia.getInicio()!=nullptr){
      bool terminou;
      TempoProgramado *temp = listaProgDoDia.getInicio();
      do{
        Serial.println(temp->horaProg);
        Serial.println(agoraString.c_str());
          if(strcmp(temp->horaProg,agoraString.c_str())==0 && manipuladorDeTempo.getSegundo()<=2){
            String menssagemSerial = "SaidaIgual :" ;
            menssagemSerial+= temp->elemento;
            menssagemSerial+= "=";
            menssagemSerial += (temp->ligar)?"1":"0";
            Serial2.println(menssagemSerial);
            Serial.println(menssagemSerial);
          }
          if(temp->proximo!=nullptr)
            temp = temp->proximo;
          else 
            terminou = true;
      }while(!terminou);
    }
    tempoRelogio = millis();
  }
}

//-----------------------envia arquivo requerido pelo cliente se o mesmo existir --------------------------------//
bool handleFileRead(String path, AsyncWebServerRequest *request)
{
  Serial.println("handleFileRead: " + path);
  // if(path.endsWith("/")) path += "index.html";           // se o caminho não for especificado envia index
  String contentType = getContentType(path); //pega o tipo de arquivo, html, css, js, pdf e etc...
  String pathWithGz = path + ".gz";          //arquivos compactados gz
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
  {                                // se o qrquivo existir normal ou compactado
    if (SPIFFS.exists(pathWithGz)) // se existir a opção compactada envia essa versão preferencialmente
      path += ".gz";
    File file = SPIFFS.open(path, "r"); // abre o arquivo
    if (!file)
    { //se o arquivo não foi aberto envia menssagem de erro e retorna
      Serial.println("Erro ao abrir o arquivo");
      return false;
    }
    else
    {
      Serial.println("arquivo aberto");
    }
    Serial.println("enviando request");
    request->send(SPIFFS, path, contentType); // envia arquivo ao cliente
    Serial.println("fechando arquivo");
    file.close(); // fecha o arquivo
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);
  return false; // se arquivo nao existe retorna
}

//-----------------Cliente requisita o upload de um arquivo para o sistema---------------------//

void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{ // upload de um novo arquivo para o sistema de arquivos SPIFFS, usado para atualização durante o desenvolvimento
  //estrutura de dados com a descrição do arquivo
  struct uploadRequest
  {
    uploadRequest *next;
    AsyncWebServerRequest *request;
    File uploadFile;
    uint32_t fileSize;
    uploadRequest()
    {
      next = NULL;
      request = NULL;
      fileSize = 0;
    }
  };
  static uploadRequest uploadRequestHead;
  uploadRequest *thisUploadRequest = NULL;

  if (!index)
  {
    String toFile = filename;
    if (request->hasParam("dir", true))
    {
      AsyncWebParameter *p = request->getParam("dir", true);
      Serial.println("dir param: " + p->value());
      toFile = p->value();
      if (!toFile.endsWith("/"))
        toFile += "/";
      toFile += filename;
    }
    if (!toFile.startsWith("/"))
      toFile = "/" + toFile;

    if (SPIFFS.exists(toFile))
      SPIFFS.remove(toFile);
    thisUploadRequest = new uploadRequest;
    thisUploadRequest->request = request;
    thisUploadRequest->next = uploadRequestHead.next;
    uploadRequestHead.next = thisUploadRequest;
    thisUploadRequest->uploadFile = SPIFFS.open(toFile, "w");
    Serial.println("Upload: START, filename: " + toFile);
  }
  else
  {
    thisUploadRequest = uploadRequestHead.next;
    while (thisUploadRequest->request != request)
      thisUploadRequest = thisUploadRequest->next;
  }

  if (thisUploadRequest->uploadFile)
  {
    for (size_t i = 0; i < len; i++)
    {
      thisUploadRequest->uploadFile.write(data[i]);
    }
    thisUploadRequest->fileSize += len;
  }

  if (final)
  {
    thisUploadRequest->uploadFile.close();
    Serial.print("Upload: END, Size: ");
    Serial.println(thisUploadRequest->fileSize);
    uploadRequest *linkUploadRequest = &uploadRequestHead;
    while (linkUploadRequest->next != thisUploadRequest)
      linkUploadRequest = linkUploadRequest->next;
    linkUploadRequest->next = thisUploadRequest->next;
    delete thisUploadRequest;
  }
}

String getContentType(String filename)
{
  if (filename.endsWith(".htm"))
    return "text/html";
  else if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".xml"))
    return "text/xml";
  else if (filename.endsWith(".pdf"))
    return "application/x-pdf";
  else if (filename.endsWith(".zip"))
    return "application/x-zip";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

void detalhesDaRede()
{
  Serial.println("");
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void listaDeArquivos()
{
  String str = "";
  File dir = SPIFFS.open("/");
  File file = dir.openNextFile();
  while (file)
  {
    str += file.name();
    str += " - ";
    str += file.size();
    str += "\r\n";
    file = dir.openNextFile();
  }
  Serial.print(str);
}

File carregaConfigFile()
{
  File f = SPIFFS.open("/configFile.txt", "r");
  return f;
}

String carregaDadosArquivo(String dado, File file)
{
  Serial.println("dados para buscar: " + dado);
  int i = 0;
  int j = 0;
  bool terminou = false;
  String s = "";
  String l;
  for (i = 0; i < file.size(); i++)
  {
    char m = (char)file.read();
    l = l + m;
    if (dado[j] == m)
    {
      j++;
    }
    else
      j = 0;
    if (j == dado.length())
    {
      while (!terminou)
      {
        char c = (char)file.read();
        if (c != '%')
        {
          s = s + c;
        }
        else
          terminou = true;
      }
      break;
    }
  }
  file.close();
  Serial.println("Dados de retorno: " + s);
  return s;
}

void mudaWifiMode(String wifiMode)
{
  if (wifiMode == "WIFI_AP")
    WiFi.mode(WIFI_AP);
  else if (wifiMode == "WIFI_STA")
    WiFi.mode(WIFI_STA);
  else
    WiFi.mode(WIFI_AP_STA);
}

void escreveArquivo(String fileName, String dados)
{
  File file = SPIFFS.open(fileName, "a");
  if (file)
  {
    Serial.println("escrever em arquivo" + dados);
    file.print(dados);
    file.close();
  }
  else
    Serial.println("erro ao abrir arquivo");
}

void substituiEmArquivo(String dado, String dadoNovo, String fileNome)
{
  File file = SPIFFS.open(fileNome, "r");
  int i = 0;
  String s;
  for (i = 0; i < file.size(); i++)
  {
    s = s + (char)file.read();
  }
  file.close();
  Serial.println("config atual: " + s);
  s = trocaValor(dado, dadoNovo, s);
  file = SPIFFS.open(fileNome, "w");
  Serial.println("novas configurações: " + s);
  file.print(s);
  file.close();
}

void setWifiMode(String wifiModeRequest)
{
  if (!wifiModeRequest.startsWith("/setWifiMode="))
    strcpy(wifiMode, wifiModeRequest.c_str());
  else
  {
    int i = wifiModeRequest.indexOf("=") + 1;
    wifiModeRequest.remove(0, i);
    Serial.println(wifiModeRequest);
    strcpy(wifiMode, wifiModeRequest.c_str());
  }
  mudaWifiMode(wifiMode);
  substituiEmArquivo("wifiMode=", wifiMode, "/configFile.txt");
}

String charToString(char *texto)
{
  int i = 0;
  String s = "";
  for (i = 0; i < strlen(texto); i++)
  {
    s = s + texto[i];
  }
  Serial.println(s);
  return s;
}
char *stringToChar(String string)
{
  char c[string.length()];
  strcpy(c, string.c_str());
  Serial.print("stringToChar: ");
  Serial.println(c);
  return c;
}

String getConfigStatus()
{
  String configStatus = "";
  File file = SPIFFS.open("/configFile.txt", "r");
  for (int i = 0; i < file.size(); i++)
  {
    configStatus = configStatus + (char)file.read();
  }
  Serial.println("config status: " + configStatus);
  return configStatus;
}

String getRedesDisponiveis()
{
  String redes = "";
  int n = WiFi.scanNetworks();
  String potencia = "";
  String conectado = "";
  if (n > 0)
  {
    for (int i = 0; i < n; ++i)
    {
      String imagem = "img_wifi_";
      if (WiFi.RSSI(i) > -55)
        imagem = imagem + '4';
      else if (WiFi.RSSI(i) <= -55 && WiFi.RSSI(i) > -65)
        imagem = imagem + '3';
      else if (WiFi.RSSI(i) <= -65 && WiFi.RSSI(i) > -85)
        imagem = imagem + '2';
      else if (WiFi.RSSI(i) <= -85 && WiFi.RSSI(i) > -90)
        imagem = imagem + '1';
      else
        imagem = imagem + '1';
      if (WiFi.encryptionType(i) < 7)
        imagem = imagem + "_bar_lock.png";
      else
        imagem = imagem + ".png";
      if (WiFi.SSID() == WiFi.SSID(i))
        redes = redes + "<li onclick=\"showDialogModal('" + imagem + "','" + WiFi.SSID(i).c_str() + "','conectado')\" id=\"idRedeWifiDisponivel" + i + "\" class=\"redeWifiDisponivel\"><div style=\"display:block\"><div><img class=\"wifiIcon\"src=\"" + imagem + "\" /></div><div><a>" + WiFi.SSID(i) + "</a><br/><span id=\"statusConexao\">Conectado</span></div></div></li>";
      else
        redes = redes + "<li onclick=\"showDialogModal('" + imagem + "','" + WiFi.SSID(i).c_str() + "','desconectado')\" id=\"idRedeWifiDisponivel" + i + "\" class=\"redeWifiDisponivel\"><div style=\"display:block\"><div><img class=\"wifiIcon\"src=\"" + imagem + "\" /></div><div><a>" + WiFi.SSID(i) + "</a><br/><span id=\"statusConexao\"></span></div></div></li>";
    }
  }
  Serial.println(redes);
  return redes;
}

String getStatusSaidas()
{
  String s;
  for (int a = 0; a < 24; a++)
  {
    if (statusSaidas[a])
      s = s + '1';
    else
      s = s + '0';
  }
  Serial.println(s);
  return s;
}

String setStatusSaida(String saida)
{
  String s;
  int i = saida.indexOf("=") + 1;
  saida.remove(0, i);
  Serial.println("modificar saida :" + saida);
  Serial2.println("Saida :" + saida);
  if (statusSaidas[saida.toInt()])
    s = s + '1';
  else
    s = s + '0';
  return s;
}

String setSSIDConfig(AsyncWebServerRequest *request)
{
  AsyncWebParameter *nome = request->getParam(0);
  AsyncWebParameter *senha = request->getParam(1);
  Serial.print("nome : ");
  Serial.println(nome->value().c_str());
  Serial.print("senha : ");
  Serial.println(senha->value().c_str());
  char nomeAntigo[50];
  strcpy(nomeAntigo, wifiSSIDNome);
  char senhaAntiga[50];
  strcpy(senhaAntiga, wifiSSIDSenha);
  Serial.println("desconectar");
  WiFi.disconnect();
  long timeConn = millis();
  int count = 0;
  Serial.println("tentar conectar");
  while (!WiFi.begin(nome->value().c_str(), senha->value().c_str()) && count < 5)
  {
    if (millis() - timeConn > 500)
    {
      Serial.println("tentativa " + count);
      timeConn = millis();
      count++;
    }
  }
  if (WiFi.status() != WL_CONNECTED)
    count = 10;
  if (count >= 5)
  {
    Serial.println("falha na conexão");
    WiFi.begin(nomeAntigo, senhaAntiga);
    while (WiFi.status() != WL_CONNECTED)
    {
    }
    return "falha na conexão, verifique a senha.";
  }
  else
  {
    Serial.println("conexao concluida");
    substituiEmArquivo("wifiSSIDNome=", wifiSSIDNome, "/configFile.txt");
    substituiEmArquivo("wifiSSIDSenha=", wifiSSIDSenha, "/configFile.txt");
    return "ok";
  }
}
String getIp()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("nao conectado");
    return " A central não está conectada";
  }
  else
  {
    Serial.println("conectado");
    return ipToString(WiFi.localIP());
  }
}

String ipToString(IPAddress ip)
{
  String s = "";
  for (int i = 0; i < 4; i++)
    s += i ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

String trocaValor(String dado, String dadoNovo, String local)
{
  String s;
  int i = 0;
  int j = 0;
  bool terminou = false;
  for (i = 0; i < local.length(); i++)
  {
    if (dado[j] == local[i])
    {
      j++;
    }
    else
      j = 0;
    if (j == dado.length())
    {
      while (!terminou)
      {
        i++;
        char c = local[i];
        if (c != '%')
        {
          s = s + c;
        }
        else
          terminou = true;
      }
      break;
    }
  }
  local.replace(s, dadoNovo);
  Serial.println(local);
  return local;
}
bool SSIDExiste(String SSIDParaProcurar)
{
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++)
  {
    if (WiFi.SSID(i) == SSIDParaProcurar)
      return true;
  }
  return false;
}

void entradaSerial()
{
  Serial.println("evento serial");
  String entradaSerial;
  while (Serial2.available())
  {
    // get the new byte:
    char inChar = (char)Serial2.read();
    // add it to the inputString:
    entradaSerial += inChar;
  }
  Serial.println(entradaSerial);
  trataSerial(entradaSerial);
}

void trataSerial(String entradaSerial)
{
  if (entradaSerial.startsWith("recebido"))
  {
    int i = entradaSerial.indexOf("=") + 1;
    entradaSerial.remove(0, i);
    for (i = 0; i < entradaSerial.length(); i++)
    {
      if (entradaSerial[i] == '1')
        statusSaidas[i] = true;
      else
        statusSaidas[i] = false;
    }
  }
}

void carregarProgramacao(String arquivo)
{
  File file = SPIFFS.open(arquivo, "r");
  bool terminou = false;
  String s = "prog";
  String dadosProg = "";
  int i = 0;
  do 
  {
    dadosProg = carregaDadosArquivo(s + i + "=", carregaConfigFile());
    if (dadosProg != "")
    {
      //remove tudo até o igual
      int a = dadosProg.indexOf("=") + 1;
      dadosProg.remove(0, a);
      //separa os valores
      a = dadosProg.indexOf("-");
      char dataProg[10];
      strcpy(dataProg, dadosProg.substring(0, a).c_str());
      dadosProg.remove(0, a + 1);
      a = dadosProg.indexOf("-");
      char horaProg[10];
      strcpy(horaProg, dadosProg.substring(0, a).c_str());
      dadosProg.remove(0, a + 1);
      a = dadosProg.indexOf("-");
      char diasRepetir[25];
      strcpy(diasRepetir, dadosProg.substring(0, a).c_str());
      dadosProg.remove(0, a + 1);
      a = dadosProg.indexOf("-");
      String repetir = dadosProg.substring(0, a);
      dadosProg.remove(0, a + 1);
      a = dadosProg.indexOf("-");
      String lig = dadosProg.substring(0, a);
      dadosProg.remove(0, a + 1);
      a = dadosProg.indexOf("-");
      long elemento = dadosProg.substring(0, a).toInt();
      dadosProg.remove(0, a + 1);
      char nomeElemento[50];
      strcpy(nomeElemento, dadosProg.c_str());

      bool repet;
      if (repetir == "true")
        repet = true;
      else
        repet = false;
      bool ligar;
      if (lig == "true")
        ligar = true;
      else
        ligar = false;
      listaProg.addItem(dataProg, horaProg, repet, ligar, elemento, nomeElemento,diasRepetir);
    }
    else
    {
      terminou = true;
    }
    i++;
  } while (!terminou);
}

void criarProgramacao(String prog, String arquivo)
{
  Serial.println("programação a inserir" + prog);
  int i = 0;
  bool terminou;
  String dadosProg = "";
  String s = "prog";
  do
  {
    Serial.println("verificar se existe para inserir.");
    dadosProg = carregaDadosArquivo(s + i + "=", carregaConfigFile());
    if (dadosProg != "")
    {
      i++;
      terminou = false;
    }
    else
    {
      terminou = true;
    }
  } while (!terminou);
  escreveArquivo(arquivo, s + i + "=" + prog + "%");
  listaProg.apagarProg();
  carregarProgramacao(arquivo);
}

String tratarNovaProgramacao(AsyncWebServerRequest *request)
{
  Serial.println("tratarNovaProgramacao");
  String s = "";
  s = request->getParam(0)->value();
  Serial.println(s);
  criarProgramacao(s, "/configFile.txt");
  return "ok";
}

String getTodaProgramacao()
{
  String s = "";
  if (listaProg.getInicio() != nullptr)
  {
    TempoProgramado *prog = listaProg.getInicio();
    do
    {
      s += String(prog->dataProg) + "-" + String(prog->horaProg) + "-" + prog->repetir + "-" + prog->ligar + "-" + prog->elemento + "-" + String(prog->nomeElemento) + "%";
      Serial.println("programação : " + s);
      Serial.println("nomeElemento : " + String(prog->nomeElemento));
      if (prog->proximo != nullptr)
        prog = prog->proximo;
      else
        return s;
    } while (prog->proximo != nullptr);
  }
  return s;
}
void getProgDoDia(ManipuladorDeTempo manipuladorDeTempo){
  char dia[10];
  char diaSemana[4];
  strcpy(diaSemana,manipuladorDeTempo.getDiaDaSemanaStringLow().c_str());
  String a = "";
  String b = "";
  if(manipuladorDeTempo.getDia()<10)
    a = "0";
  if(manipuladorDeTempo.getMes()+1<10)
    b = "0";
  strcpy(dia,String(a+String(manipuladorDeTempo.getDia()) +b+ String(manipuladorDeTempo.getMes()+1)+String(manipuladorDeTempo.getAno())).c_str());
  Serial.print("Dia: ");
  Serial.println(dia);
  Serial.println(diaSemana);
  listaProgDoDia = listaProg.buscarTodos(dia,diaSemana);  
  Serial.print("resultado: ");
  if(listaProgDoDia.getInicio()!=nullptr){
    bool terminou = false;
    TempoProgramado *temp = listaProgDoDia.getInicio();
    while(!terminou){
      Serial.println(temp->dataProg);
      Serial.println(temp->diasRepetir);
      if(temp->proximo == nullptr)
        terminou = true;
    }
    Serial.println("concluido;");
  }
}