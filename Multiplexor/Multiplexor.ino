// ACTUALIZACION ABRIL 2024 PARA CONTROL REMOTO POR COMUNICACION SERIAL: FRANCISCO CASTILLO / FRANCISCO BENEGAS


// Constants
const int CH_A = 2;   //8
const int CH_B = 3;   //9
const int CH_C = 4;  //10
const int Pin_Rojo = 5;  // 11 // LED
const int Pin_Verde = 6; // 12  //LED
const int COM_IN = 7; //7
const int Rele_3 = 8;  //AZUL      // RELÉ 3
const int Rele_2y4 = 9;  //VIOLETA   // RELÉS 2 Y 4 EN PARALELO
const int Rele_1 = 10;  //ROJO     // RELÉ 1
const int Rele_6 = 11;  //NARANJA  // RELÉ 6
const int Rele_5y7 = 12;  //AMARILLO // RELÉS 5 Y 7 EN PARALELO

int remoteButton; // variable para el boton "presionado" remotamente

// Arrays
int CHA[8] = {0, 1, 0, 1, 0, 1, 0, 1};
int CHB[8] = {0, 0, 1, 1, 0, 0, 1, 1};
int CHC[8] = {0, 0, 0, 0, 1, 1, 1, 1};
int LED[8] = {5, 4, 3, 6, 1, "X", 2, "X"};  //'X' xq hay 2 que no utilizamos
int BTN[8] = {2, 3, 4, 1, 5, "X", 6, "X"};
int Estado[8] = {0, 0, 0, 0, 0, 0, 0, 0};    //6 u 8
bool PreviousButtonState[8] = {0, 0, 0, 0, 0, 0, 0, 0};
bool buttonState;
 
int corr = 0;
int Volt_A = 0;
int Volt_B = 0;
int Rem = 0;

char mensaje[40];

long baudRate = 115200;

void setup() {
  pinMode(CH_A, OUTPUT);
  pinMode(CH_B, OUTPUT);
  pinMode(CH_C, OUTPUT);
  pinMode(COM_IN, INPUT);
  pinMode(Pin_Rojo, OUTPUT);
  pinMode(Pin_Verde, OUTPUT);
  pinMode(Rele_3, OUTPUT);
  pinMode(Rele_2y4, OUTPUT);
  pinMode(Rele_1, OUTPUT);
  pinMode(Rele_6, OUTPUT);
  pinMode(Rele_5y7, OUTPUT);
  Serial.begin(baudRate);
  Serial.setTimeout(0); //FC: Esto lo que hace es que el Serial.parseInt no se quede mucho tiempo esperando data

  corr = 0;
  Volt_A = 0;
  Volt_B = 0;
  //FC: Estado del 1 al 6 son el estado de los leds. 1: verde, 2: rojo, 3: parpareando
  Estado[1] = 1; // RST
  Estado[2] = 1; // SGs
  Estado[3] = 0; // M1
  Estado[4] = 0; // M2
  Estado[5] = 0; // M3
  Estado[6] = 1; // R/L
}

void loop() {

  ////////// CONTROL REMOTO POR COMUNICACION SERIAL //////////

      if (Serial.available() > 0) {  // FC: Serial avalaible se fija si hay data en el puerto serial y devuelve el numero de caracteres que encuentra (ej: si envio 123, devuelve 3)
        
        remoteButton = Serial.parseInt();  // FC: leo el entero disponible en la comunicacion serial, si hay un terminador entonces lo saca, por ej: envio 5'\n' -> devuelve 5

        // FC: luego de haber leido el entero, tiro todo lo demas que haya en el puerto serial, por ejemplo, si envio 5'\n', 
        // con la linea anterior leo el 5 y en esta linea tiro el '\n', esto es para que el if no se ejecute dos veces, que es lo que provocaba que los leds queden apagadas por un ratito
        while (Serial.available() > 0) Serial.read();  


        //FC: si el boton "apretado" remotamente no es el 6, entonces forza el modo remoto (si es el 6 entonces opera normalmente)
        if (remoteButton != 6) {
        Rem = 1; // activo el modo REMoto
        Estado[6] = 2; // pongo el led R/L en rojo
        }

        buttonOperation(remoteButton);
      }
  ////////////////////////////////////////////////////////////
  
    // Barrido de búsqueda
    for (int N = 0; N < 8; N++) {

      // Verifica el estado del botón correspondiente
      if (BTN[N] != "X") {    //SALTEAR CUANDO DICE X

        //FC: estas tres lineas por lo que entiendo son pines que van al integrado y configuran el estado de cual boton se va a leer a continuacion (es un multiplexor de botones o algo asi)
        digitalWrite(CH_A, CHA[N]);
        digitalWrite(CH_B, CHB[N]);
        digitalWrite(CH_C, CHC[N]);

        //FC: aca es donde efectivamente se lee el estado del pin a partir de un pin comun y usando el multiplexor de botones
        buttonState = digitalRead(COM_IN);
        
        if (buttonState != PreviousButtonState[BTN[N]]) {    //FC: si el estado del boton cambio (lo presione o deje de presionar), entonces continua

          if (buttonState == HIGH && (Rem == 0 || BTN[N] == 6 )) {  //FC: si (el boton esta presionado) y (rem == 0 o estamos viendo el boton 6) entonces segui 
          buttonOperation(BTN[N]);
          }
        PreviousButtonState[BTN[N]] = buttonState;
        delay(10);

        }

        ledsRefresh(LED[N]);
      }
    }


  //ACTUALIZO RELES:
  //TENSION EN LOCK IN 1:
  if (Volt_B == 0){  //MIDO ΔV_SG1
    digitalWrite(Rele_1, LOW);
    }

  else {             //MIDO ΔV_SG2
    digitalWrite(Rele_1, HIGH);
    }

    //TENSION EN LOCK IN 2:
    if (Volt_A == 0){  //MIDO ΔV_SG2
      digitalWrite(Rele_3, HIGH);
      digitalWrite(Rele_2y4, LOW);
      }

    if (Volt_A == 1){  //MIDO ΔV_R1
      digitalWrite(Rele_3, HIGH);
      digitalWrite(Rele_2y4, HIGH);
      }
  
    if (Volt_A == 2){  //MIDO ΔV_R2
      digitalWrite(Rele_3, LOW);
      digitalWrite(Rele_2y4, LOW);
      }

    if (Volt_A == 3){  //MIDO ΔV_R3
      digitalWrite(Rele_3, LOW);
      digitalWrite(Rele_2y4, HIGH);
      }

    //CORRIENTE DEL LOCK IN 2:
    if (corr == 1){    //I_R1
      digitalWrite(Rele_5y7, LOW);
      digitalWrite(Rele_6, HIGH);
      }
    if (corr == 2){    //I_R2
      digitalWrite(Rele_5y7, HIGH);
      digitalWrite(Rele_6, LOW);
      }
    if (corr == 3){    //I_R3
      digitalWrite(Rele_5y7, HIGH);
      digitalWrite(Rele_6, HIGH);
      }
    if (corr == 0){    //NO HAY CORRIENTE
      digitalWrite(Rele_5y7, LOW);
      digitalWrite(Rele_6, LOW);
      }

}

// FC: actualizacion del estado de los leds

void ledsRefresh(int NLED) {

  if (Estado[NLED] == 1 || Estado[NLED] == 3 && (millis() / 500) % 2 == 0){
        delay(4);
        digitalWrite(Pin_Verde, HIGH);
        delay(1);
        digitalWrite(Pin_Verde, LOW);
        }
  if (Estado[NLED] == 2 || Estado[NLED] == 3 && (millis() / 500) % 2 == 1){
        digitalWrite(Pin_Rojo, HIGH);
        delay(5);
        digitalWrite(Pin_Rojo, LOW);
        }

}




// FC: funcion que determina el cambio de estado al apretar un botton determinado. Esto antes formaba parte del codigo, lo meti dentro de una funcion para no duplicarlo al agregar el control remoto.

void buttonOperation(int button) {

  switch(button){   //FC: dependiendo de que boton se apreto va a un caso u otro y hace la tarea correspondiente

    //Case 1,2,3,4,5,6 corresponden a los 6 botones
    case 1:   //DEFAULT     
      corr = 0;
      Volt_A = 0;
      Volt_B = 0;
      Estado[1] = 1;
      Estado[2] = 1;
      Estado[3] = 0;
      Estado[4] = 0;
      Estado[5] = 0;
      
      break;

    case 2:  //EL EXCEPCIONAL MIDE SG2 EN LOS 2 LOCK INS, SINO VOLVES AL DEFAULT.          
      Volt_B = (Volt_B + 1) % 2;
      Estado[1] = 1 - Volt_B;
      Estado[2] = Volt_B + (Volt_A == 0 ? 1 : 0);
      break;

    case 3:
      if (Volt_A == 1) {
        if (corr == 1){
          corr = 0;
          Estado[3] = 1;    //1 = VERDE
          }
        else{
          corr = 1;
          Estado[3] = 3;    //3 = VERDE+ROJO (TENSIÓN Y CORRIENTE EN EL MISMO CANAL)
          Estado[4] = 0;
          Estado[5] = 0;
          }
        }
      else {
        //O sea, Volt_A == 0,2,3:
        Estado[2 + Volt_A] = Estado[2 + Volt_A]-1;  //Apago el que estaba prendido (o lo dejo en solo corriente)
        Volt_A = 1;
        Estado[3] = Estado[3]+1; 
        }
      break;


    case 4:
      if (Volt_A == 2) {
        if (corr == 2){
          corr = 0;
          Estado[4] = 1;    //1 = VERDE
          }
        else{
          corr = 2;
          Estado[3] = 0;    
          Estado[4] = 3;    //3 = VERDE+ROJO (TENSIÓN Y CORRIENTE EN EL MISMO CANAL)
          Estado[5] = 0;
          }
        }
      else {
        //O sea, Volt_A == 0,2,3:
        Estado[2 + Volt_A] = Estado[2 + Volt_A]-1;  //Apago el que estaba prendido (o lo dejo en solo corriente)
        Volt_A = 2;
        Estado[4] = Estado[4]+1;
        }
      break;

    case 5:
      if (Volt_A == 3) {
        if (corr == 3){
          corr = 0;
          Estado[5] = 1;    //1 = VERDE
          }
        else{
          corr = 3;
          Estado[3] = 0;    
          Estado[4] = 0;    
          Estado[5] = 3;    //3 = VERDE+ROJO (TENSIÓN Y CORRIENTE EN EL MISMO CANAL)
          }
        }
      else {
        //O sea, Volt_A == 0,2,3:
        Estado[2 + Volt_A] = Estado[2 + Volt_A]-1;  //Apago el que estaba prendido (o lo dejo en solo corriente)
        Volt_A = 3;
        Estado[5] = Estado[5]+1;
        }
      break;

    case 6:
      //FC: si se apreta el boton 6 entonces rem=0->1 / rem=1->0, si rem == 1 entonces el led queda rojo y apretar los demas botones no cumple las condiciones del if anterior, por lo que
      // no hacen nada (el panel queda bloqueado = control remoto). Si el estado 6 (R/L) ya estaba en rojo (rem == 1) entonces volver a a pretarlo hace rem == 0, el led se pone en verde (control local)
      // y se puede volver a operar apretando los botones
      Rem = (Rem + 1)%2;
      Estado[6] = Rem + 1;
      break;

  }

}
