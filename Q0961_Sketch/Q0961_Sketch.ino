/*
     CÓDIGO:	  Q0961
     AUTOR:		  BrincandoComIdeias
     APRENDA: 	https://cursodearduino.net/
     SKETCH:    ESP32 Com Display Touch
     DATA:		  31/10/2022
*/

// INCLUSÃO DE BIBLIOTECA
#include <Nextion.h>   // https://github.com/itead/ITEADLIB_Arduino_Nextion
#include <DHT.h>       // DHT sensor library by Adafruit

//INSTALAR A BIBLIOTECA EspSoftwareSerial by Dirk Kaar

// DEFINIÇÃO DE PINOS
#define pinRele 13

#define DHTPIN 27
#define DHTTYPE DHT22

// INSTANCIANDO OBJETOS
DHT dht(DHTPIN, DHTTYPE);

NexPage page0    = NexPage(0, 0, "page0");
NexPage page1    = NexPage(1, 0, "page1");
NexPage page2    = NexPage(2, 0, "page2");

NexDSButton    bt0        = NexDSButton(2, 1, "bt0");
NexGauge       medidor    = NexGauge(2, 2, "z0");
NexProgressBar termometro = NexProgressBar(2, 3, "j0");
NexNumber      nUmid      = NexNumber(2, 6, "n0");
NexNumber      nTemp      = NexNumber(2, 7, "n1");

//Lista dos objetos que podem enviar comandos ao Arduino (para o Arduino ficar ouvindo no Loop)
NexTouch *nex_listen_list[] = 
{
    &bt0,
    NULL
};

//Memoria para funcionamento da biblioteca Nextion
char buffer[100] = {0};

void setup() {
  nexInit();
  dbSerial.begin(9600);
  delay(500);

  // TROCA PARA A PAGINA 1
  page1.show();
  delay(5000);

  // TROCA PARA A PAGINA 2
  page2.show();

  // INICIA O SENSOR E CONFIGURA PINO DO RELE
  dht.begin();
  pinMode(pinRele, OUTPUT);

  // CONFIGURA A FUNÇÃO EXECUTADA QUANDO TOCAR O BOTÃO
  bt0.attachPush(bt0PushCallback, &bt0);
  dbSerial.println("Fim do Setup");
}

void loop() {
  static float umidade;
  static float temperatura;
  static float umidadeAnt;
  static float temperaturaAnt;
  static bool primeiraVez = true;

  //Fica ouvindo o display para receber comandos
  nexLoop(nex_listen_list);

  //A cada minuto faz leitura do sensor
  static unsigned long delaySensor;
  if (((millis() - delaySensor) > 60000) || (primeiraVez)) {
    primeiraVez = false;
    delaySensor = millis();
    
    umidade = dht.readHumidity();
    temperatura = dht.readTemperature();

    if (isnan(umidade) || isnan(temperatura)) {
      //Erro de leitura
      dbSerial.println("Erro de Leitura do DHT");
    }
  }

  //Se mudou a umidade, atualiza display
  if ((umidade != umidadeAnt)) { 
    //Atualiza os números do display
    nUmid.setValue(umidade);
 
    //Atualiza Controles do Display
    umidade = (int)umidade * 10;          // MULTIPLICA POR 10 PARA O MAP
    medidor.setValue(map(umidade, 0, 1000, 0, 180));
  }
  umidadeAnt = umidade;

  //Se mudou a temperatura, atualiza display
  if ((temperatura != temperaturaAnt)) { 
    //Atualiza os números do display
    nTemp.setValue(temperatura);

    //Atualiza Controles do Display
    temperatura = (int)temperatura * 10;  // MULTIPLICA POR 10 PARA O MAP
    termometro.setValue(map(temperatura, -200, 570, 23, 90));
  }
  temperaturaAnt = temperatura;
}

void bt0PushCallback(void *ptr)
{
    uint32_t dual_state;
    NexDSButton *btn = (NexDSButton *)ptr;
    dbSerial.println("Comando recebido do 'b0'");
    dbSerial.print("ptr=");
    dbSerial.println((uint32_t)ptr); 
    memset(buffer, 0, sizeof(buffer));

    //Verifica o estado do botão no display
    bt0.getValue(&dual_state);
    if(dual_state) 
    {
        digitalWrite(pinRele, HIGH);
    } else {
        digitalWrite(pinRele, LOW);
    }
}
