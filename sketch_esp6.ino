//#include <Serial.h> // inclusione della libreria per la comunicazione seriale
#include <SoftwareSerial.h> // inclusione della libreria per la comunicazione seriale (ovvero fra le due schede Arduino)
#define rxPin 2 // pin predisposto alla ricezione dei dati (Rx)
#define txPin 3 // pin predisposto alla trasmissione dei dati (Tx)
#define ms 300 // definizione dei millisecondi per i delay
#define An 26 // lettere presenti nell'alfabeto internazionale
#define n 100 // massimo numero di caratteri formanti una stringa per questo programma

int i,j,k,z; // indici che serviranno durante l'esecuzione del programma
String message=""; // messaggio originale
String crypted_vig_message=""; // messaggio criptato con Vigenère
String crypted_colonn_message=""; // messaggio criptato con metodo Colonnare
String verme="FROOSEVELT"; // chiave (verme) per la crittografia con Vigenère
String chiave="BUDDA"; // chiave per la crittografia con metodo Colonnare
char character; // variabile d'appoggio per un carattere
SoftwareSerial swSerial=SoftwareSerial(rxPin,txPin); // istanziazione e inizializzazione della seriale software comunicando i pin da utilizzare in ricezione ed invio dei dati

void setup() {
  // put your setup code here, to run once:
  pinMode(rxPin,INPUT); // Rx è in input
  pinMode(txPin,OUTPUT); // Tx è in output
  Serial.begin(9600); // il baud rate per la comunicazione seriale è di 9600 (baud)
  swSerial.begin(9600); // il baud rate per la comunicazione seriale software è di 9600 (baud)
}

void loop() {
  // put your main code here, to run repeatedly:
  serialReceive(); // prima compie le operazioni relative alla ricezione e l'invio dei messaggi da parte del proprio utente (ovvero dal PC al quale è connesso)
  swSerialReceive(); // poi compie le operazioni nel caso in cui siano stati ricevuti dati da parte dell'altro Arduino al quale è collegato
}

// funzione per determinare se è arrivato qualcosa sulla seriale (dunque se l'utente sta cercando di trasmettere dei dati)
void serialReceive()
{
  // se c'è qualcosa nel buffer della seriale, vuol dire che è stato ricevuto qualcosa da parte del PC al quale si è connessi
  if(Serial.available()>0)
  {
    delay(ms); // inserisco un delay prima di iniziare a comporre il messaggio con i caratteri ricevuti poiché bisogna dare il tempo alla scheda di aver ricevuto tutti i caratteri
    character=0; // pulisco la variabile per il carattere

    // ciclo per la lettura dei caratteri dal buffer della seriale e la formazione del messaggio
    do
    {
      character=Serial.read(); // leggo carattere per carattere ciò che ho ricevuto sulla seriale e lo metto sulla variabile char apposita (d'appoggio)
      message=String(message+character); // formo la stringa "message" grazie alla concatenazione di ciò che già è presente in "message" con il carattere appena letto dal buffer della seriale
    }while(Serial.available()>0); // ripeto le operazioni di lettura dal buffer finché ci sono dati da leggere
    Serial.flush(); // pulisco il buffer della seriale una volta terminata la lettura
    
    message.toUpperCase(); // tutte le lettere del messaggio vengono messe in maiuscolo (per lavorarci)
    swSerialTransmit(); // dato che il messaggio è stato composto, bisogna successivamente trasmetterlo sulla seriale software per mandarlo all'altro Arduino
  }
}

// funzione per determinare se è arrivato qualcosa sull'rxPin (pin di ricezione della SoftwareSerial), dunque se si deve procedere a mostrare un messaggio ricevuto da un altro Arduino
void swSerialReceive()
{
  // se c'è qualcosa nel buffer della seriale software, vuol dire che è stato ricevuto qualcosa da parte dell'altro Arduino
  if(swSerial.available()>0)
  {
    delay(ms); // inserisco un delay prima di iniziare a comporre il messaggio con i caratteri ricevuti poiché bisogna dare il tempo alla scheda di aver ricevuto tutti i caratteri
    character=0; // pulisco la variabile per il carattere

    // ciclo per la lettura dei caratteri dal buffer della seriale software e la formazione del messaggio
    do
    {
      character=swSerial.read(); // leggo carattere per carattere ciò che ho ricevuto sulla seriale software e lo metto sulla variabile char apposita (d'appoggio)
      crypted_colonn_message=String(crypted_colonn_message+character); // la stringa formata è quella di "crypted_colonn_message" poiché il messaggio ricevuto si suppone essere stato proprio previamente criptato tramite metodo della Trasposizione Colonnare (in ultimo, dopo il metodo di Vigenère)
    }while(swSerial.available()>0); // ripeto le operazioni di lettura dal buffer finché ci sono dati da leggere
    swSerial.flush(); // pulisco il buffer della SoftwareSerial una volta terminata la lettura
    
    crypted_colonn_message.toUpperCase(); // tutte le lettere del messaggio vengono messe in maiuscolo (per lavorarci)
    serialTransmit(); // dato che il messaggio è stato composto, bisogna successivamente trasmetterlo sulla seriale per mostrarlo sul monitor seriale del pc al quale si è connessi
  }
}

// funzioine richiamata se si è ricevuto un messaggio da un altro Arduino (ricezione su SoftwareSerial) e dunque bisogna inviarlo al computer per mostrarlo a video
void serialTransmit()
{
  //decrypt_Colonnar(); // decripto il messaggio ricevuto con il metodo della Trasposizione Colonnare
  decrypt_Vigenere(); // decripto ancora il messaggio ricevuto (e già criptato una volta) con il metodo di Vigènere
  
  Serial.println("MESSAGGIO RICEVUTO: " + crypted_colonn_message); // innanzitutto stampo il messaggio ricevuto così com'è (ovvero ancora completamente criptato)
  Serial.println("CHIAVE DECRIPTING COLONNARE: " + chiave); // stampo la chiave di crypting/decrypting usata per il metodo Colonnare
  Serial.println("CHIAVE DECRIPTING VIGENERE: " + verme); // stampo la chiave di crypting/decrypting usata per il metodo di Vigenère
  Serial.println("DECRIPTATO CON METODO COLONNARE: " + crypted_vig_message); // poi stampo il messaggio decriptato una volta (col metodo Colonnare)
  Serial.println("DECRIPTATO CON VIGENERE, MESSAGGIO ORIGINALE: " + message); // infine stampo il messaggio decriptato per la seconda volta (col metodo di Vigenère): dunque il messaggio originale
  message=""; // pulisco la stringa del messaggio dato che l'ho trasmesso
  crypted_vig_message=""; // pulisco la stringa criptata con Vigenère
  crypted_colonn_message=""; // pulisco la stringa criptata con metodo Colonnare
}

// fuinzione richiamata se si è ricevuto un messaggio dalla seriale, dunque si procede alla trasmissione del messaggio (con SoftwareSerial) ad un altro Arduino
void swSerialTransmit()
{
  //crypt_Vigenere(); // cripto il messaggio da inviare con il metodo di Vigènere
  crypted_vig_message=message; // messaggio che è già criptato
  crypt_Colonnar(); // cripto (nuovamente) il messaggio da inviare con il metodo della Trasposizione Colonnare

  // eseguo un ciclo per inviare un carattere alla volta all'altro Arduino, tramite la seriale software
  /*
  for(i=0;i<crypted_colonn_message.length();i++)
  {
    swSerial.print(crypted_colonn_message.charAt(i)); // "stampa" (invio) della lettera i-esima dell messaggio
  }
  //swSerial.println(); // invio un carattere per andare a "linea nuova"
  */
  
  // stampa dei messaggi per il "riuscito invio"
  Serial.println("MESSAGGIO ORIGINALE: " + message); // stampa del messaggio originale che si voleva trasmettere
  Serial.println("CHIAVE CRIPTING VIGENERE: " + verme); // stampo la chiave di crypting/decrypting usata per il metodo di Vigenère
  Serial.println("CHIAVE CRIPTING COLONNARE: " + chiave); // stampo la chiave di crypting/decrypting usata per il metodo Colonnare
  Serial.println("CRIPTATO CON VIGENERE: " + crypted_vig_message); // stampa del messaggio criptato una volta (col metodo di Vigenère)
  Serial.println("CRIPTATO CON METODO COLONNARE E INVIATO: " + crypted_colonn_message); // stampa del messaggio completamente criptato (quindi anche col metodo della Trasposizione Colonnare)
  message=""; // pulisco la stringa del messaggio dato che l'ho trasmesso
  crypted_vig_message=""; // pulisco la stringa criptata con Vigenère
  crypted_colonn_message=""; // pulisco la stringa criptata con metodo Colonnare
}

// funzione per il crypting con il metodo di Vigenère
void crypt_Vigenere()
{
  // cifratura del messaggio
  for(i=0;i<message.length();i++)
  {
    int appoggio=(int)message.charAt(i);
    int appoggioV=(int)verme.charAt(i%verme.length());
    char cryptedChar=(char)((((appoggio-65)+(appoggioV-65))%An)+65);
    crypted_vig_message=String(crypted_vig_message+cryptedChar); // prima di ogni operazione tolgo il valore 65 (in quanto in ASCII la A ÃƒÆ’Ã‚Â¨ 65) per operare con le vere poiszioni alfabetiche delle lettere (ovvero a partire da 0) e poi lo riaggiungo per ottenere la corrispondente lettera in ASCII (le operazioni descritte vengono effettuate prima, per chiarezza di lettura del codice)
  }
}

// funzione per il crypting con il metodo della Trasposizione Colonnare
void crypt_Colonnar()
{
  char ch=' '; // carattere singolo di appoggio
  int iApp; // intero d'appoggio
  char app[2][chiave.length()+1]; // matrice di caratteri d'appoggio (nella prima riga ci sono le lettere, nella seconda la loro posizione (rispetto all'ordine alfabetico delle lettere della chiave), dunque la matrice avrÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â  tante colonne quante sono le lettere componenti la chiave di cifratura (aggiungo +1 per il carattere terminatore)
  //chiave.toCharArray(app[0],strlen(app[0]));
  
  Serial.println("Lettere nel vettore");
  // copio il contenuto di chiave nella prima riga di app
  for(i=0;i<chiave.length();i++)
  {
    app[0][i]=chiave.charAt(i);
    Serial.println(app[0][i]);
  }
  // aggiungo i caratteri terminatori
  app[0][chiave.length()]='\0';
  app[1][chiave.length()]='\0';
  delay(1000);
  
  
  // ordino le lettere di app alfabeticamente (in ordine crescente)
  Serial.println(chiave.length());
  delay(1000);
  for(i=0;i<chiave.length();i++)
  {
    for(j=0;j<chiave.length()-1;j++)
    {
      Serial.println("lettera j esima");
      Serial.println(app[0][j]);
      Serial.println("lettera j+1 esima");
      Serial.println(app[0][j+1]);
      Serial.println("lettera ch");
      Serial.println(ch);
      if(app[0][j]>app[0][j+1])
      {
        ch=app[0][j];
        app[0][j]=app[0][j+1];
        app[0][j+1]=ch;
      }
      Serial.println("nuova lettera j esima");
      Serial.println(app[0][j]);
    }
  }
  
  Serial.println("Lettere ORDINATE nel vettore");
  /* verifico che in app ci siano le giuste lettere */
  for(i=0;i<strlen(app[0]);i++)
  {
    Serial.println(app[0][i]);
  }
  
  delay(1000);
  
  // attribuisco un numero per ogni lettera ordinata della chiave (nella seconda riga della matrice app)
  for(i=0;i<strlen(app[0]);i++)
  {
    app[1][i]=char(i+49); // i numeri andranno da 1 alla lunghezza di app (strlen(app)), i che è un numero verrà convertito a carattere aggiungendo il valore 49 (in ASCII è il carattere '1')
    Serial.println(app[1][i]);
  }
  
  delay(1000);
  
  // riordino le lettere di app secondo l'ordine iniziale (della chiave)
  for(i=0;i<chiave.length();i++)
  {
    // ricerco la lettera corrispondente della chiave (chiave.charAt(i)) nella matrice d'appoggio (app[0][j]), dunque partendo dalla Iesima posizione
    for(j=i;j<strlen(app[0]);j++)
    {
      if(app[0][j]==chiave.charAt(i))
      {
        // scambio la posizione delle lettere e la posizione dei rispettivi numeri di posizione delle lettere
        ch=app[0][i];
        iApp=app[1][i];
        app[0][i]=app[0][j];
        app[1][i]=app[1][j];
        app[0][j]=ch;
        app[1][j]=iApp;
        
        break; // esco dal ciclo for interno perchÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â© ho trovato la corrispondenza
      }
    }
  }
  
  // stampa dei caratteri e delle loro posizioni ordinate
  for(i=0;i<strlen(app[0]);i++)
  {
    Serial.println(app[0][i]);
    Serial.println(app[1][i]);
    Serial.println();
  }
  
  delay(1000);
  
  // creazione della tabella contenente la frase da cifrare
  // per il numero delle righe, devo controllare de la divisione fra il numero delle lettere della frase da cifrare e quello della chiave di cifratura dia resto 0: se non lo dÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â  (resto !=0) allora il numero di righe sarÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â  il resto della divisione +1
  if((crypted_vig_message.length()%chiave.length())!=0)
  {
    i=(int)(crypted_vig_message.length()/chiave.length());
    i++;
  }
  else
  {
    i=crypted_vig_message.length()/chiave.length();
  }
  char tabella[i][chiave.length()+1]; // istanziazione della tabella nella quale verrÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â  operata la trasposizione colonnare (aggiungo +1 per i caratteri terminatori)
  Serial.println(char(i+48));
  Serial.println(char(chiave.length()+1+48));
  delay(1000);
  
  int n_rows=i; // salvo su una variabile di appoggio il numero delle righe della tabella per utilizzarlo in seguito
  
  // da ora "i" conterrÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â  il numero di righe presenti in "tabella"
  
  // riempimento della tabella con le lettere della frase da cifrare
  z=0;
  for(k=0;k<i;k++)
  {
    // il ciclo per le colonne si ripete fino a strlen(tabella[0]-1) perché l'ultimo carattere deve essere quello terminatore
    for(j=0;j<chiave.length();j++)
    {
      // controllo se l'indice del carattere da estrarre dalla frase da cifrare non abbia superato la lunghezza della stringa stessa
      if(z<crypted_vig_message.length())
      {
        tabella[k][j]=crypted_vig_message.charAt(z);
        z++;
      }
      else // se l'ha superato, vuol dire che non si hanno piÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¹ caratteri nella stringa, dunque procedo al riempimento degli spazi rimanenti nella tabella con un carattere riempitivo (il dollaro: '$')
      {
        tabella[k][j]='$';
      }
      tabella[k][chiave.length()]='\0'; // nell'ultimo spazio (i) di tutte le righe di tabella metto un carattere terminatore
      
      Serial.println(tabella[k][j]);
    }
    Serial.println("riga");
  }
  delay(5000);
  
  char vettore[chiave.length()+1]; // predispongo un vettore lungo quanto la chiave (più 1 per il carattere terminatore) nel quale copio ogni volta il contenuto della seconda riga della matrice originale (app). I confronti verranno quindi effettuati con questo vettore, che verrà ogni volta re-inizializzato in quanto le reali posizioni della tabella originale (app) non devoo andare perse.
  for(k=0;k<chiave.length();k++)
  {
    Serial.println("Sono entrato in questo ciclo per la copia della matrice nel vettore");
    vettore[k]=app[1][k];
    Serial.println(vettore[k]);
  }
  vettore[chiave.length()]='\0'; // metto il carattere terminatore a fine vettore
  
  // creazione della stringa cifrata
  for(i=0;i<chiave.length();i++)
  {
    /*char vettore[chiave.length()+1]; // predispongo un vettore lungo quanto la chiave (più 1 per il carattere terminatore) nel quale copio ogni volta il contenuto della seconda riga della matrice originale (app). I confronti verranno quindi effettuati con questo vettore, che verrà ogni volta re-inizializzato in quanto le reali posizioni della tabella originale (app) non devoo andare perse.
    for(k=0;k<chiave.length();k++)
    {
      Serial.println("Sono entrato in questo ciclo per la copia della matrice nel vettore");
      vettore[k]=app[1][k];
      Serial.println(vettore[k]);
    }
    vettore[chiave.length()]='\0'; // metto il carattere terminatore a fine vettore*/
    
    /*Serial.println("------------------");
    Serial.println("------------------");
    Serial.println("Sono entrato nel ciclo");*/
    // ricerca della prima lettera (in ordine alfabetico) della chiave di crittografia
    j=0; // indice per i caratteri della chiave
    z=0; // z contiene la posizione Jesima del valore piÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¹ piccolo del vettore di posizioni delle lettere ordinate alfabeticamente della chiave
    do
    {
      Serial.println(vettore[j]);
      Serial.println(vettore[z]);
      delay(000);
      if(vettore[j]<vettore[z])
      {
        z=j; // se il valore confrontato ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¨ piÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¹ piccolo registro la sua posizione nella variabile z ciÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â² consentirÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â  di trovare il numero della colonna da prendere in considerazione per la ricavazione dei caratteri da prendere per formare la stringa cifrata
        //Serial.println("Ho scambiato");
      }
      j++;
    }while(j<chiave.length()); // eseguo l'operazione per tutti i caratteri di "chiave"
    /*Serial.println("print di z");
    Serial.println(z);*/
    delay(000);
    vettore[z]=char(chiave.length()+1+48); // una volta trovato il valore minimo assegno a quella posizione un valore piÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¹ alto del numero degli elementi della stringa "chiave" stessa: in questo modo una lettera (e dunque la relativa posizione) non potranno essere presi piÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¹ di una volta in considerazione
    /*Serial.println("Nuovo valore del vettore di z");
    Serial.println(vettore[z]);
    delay(5000);*/
    
    /*Serial.println("Il valore di n_rows e");
    Serial.println(n_rows);
    delay(5000);*/
    // concatenazione delle lettere della colonna specificata da z (ovvero in ordine numerico) alla stringa
    for(k=0;k<n_rows;k++)
    {
      //Serial.println(tabella[k][z]);
      crypted_colonn_message=String(crypted_colonn_message+tabella[k][z]);
      /*Serial.println(crypted_colonn_message);
      delay(1000);*/
    }
    //delay(100000);
  }
}

// funzione per il decrypting con il metodo di Vigenère
void decrypt_Vigenere()
{
  // decifratura del messaggio
  for(i=0;i<crypted_vig_message.length();i++)
  {
    int appoggio=(int)crypted_vig_message.charAt(i);
    int appoggioV=(int)verme.charAt(i%verme.length());
    
    // se la sottrazione della lettera Iesima del verme dalla lettera Iesima della frase da decifrare ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¨ > o = a 0 allora si procede normalmente
    if((crypted_vig_message.charAt(i)-verme[i%verme.length()])>=0)
    {
      char decryptedChar=(char)(((appoggio-65)-(appoggioV-65))+65);
      message=String(message+decryptedChar);
    }
    else // altrimenti (se ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¨ < di 0) si aggiunge il numero di caratteri dell'alfabeto usato (internazionale in questo caso: An=26) prima di procedere con le altre operazioni
    {
      char decryptedChar=(char)((((appoggio-65)-(appoggioV-65))+An)+65);
      message=String(message+decryptedChar);
    }
  }
}

// funzione per il decrypting con il metodo della Trasposizione Colonnare
void decrypt_Colonnar()
{
  char ch=' '; // carattere singolo di appoggio
  int iApp; // intero d'appoggio
  
  char app[2][chiave.length()]; // matrice di caratteri d'appoggio (nella prima riga ci sono le lettere, nella seconda la loro posizione (rispetto all'ordine alfabetico delle lettere della chiave), dunque la matrice avrÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â  tante colonne quante sono le lettere componenti la chiave di cifratura
  chiave.toCharArray(app[0],strlen(app[0])); // copio il contenuto di chiave nella prima riga di app
  
  // ordino le lettere di app alfabeticamente (in ordine crescente)
  for(i=0;i<strlen(app[0]);i++)
  {
    for(j=0;j<strlen(app[0])-1;j++)
    {
      if(app[0][j]>app[0][j+1])
      {
        ch=app[0][j];
        app[0][j]=app[0][j+1];
        app[0][j+1]=ch;
      }
    }
  }
  
  // attribuisco un numero per ogni lettera ordinata della chiave (nella seconda riga della matrice app)
  for(i=0;i<strlen(app[0]);i++)
  {
    app[1][i]=i+1; // i numeri andranno da 1 alla lunghezza di app (strlen(app))
  }
  
  // riordino le lettere di app secondo l'ordine iniziale (della chiave)
  for(i=0;i<chiave.length();i++)
  {
    // ricerco la lettera corrispondente della chiave (chiave.charAt(i)) nella matrice d'appoggio (app[0][j]), dunque partendo dalla Iesima posizione
    for(j=i;j<strlen(app[0]);j++)
    {
      if(app[0][j]==chiave.charAt(i))
      {
        // scambio la posizione delle lettere e la posizione dei rispettivi numeri di posizione delle lettere
        ch=app[0][i];
        iApp=app[1][i];
        app[0][i]=app[0][j];
        app[1][i]=app[1][j];
        app[0][j]=ch;
        app[1][j]=iApp;
        
        break; // esco dal ciclo for interno perchÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â© ho trovato la corrispondenza
      }
    }
  }
  
  // creazione della tabella contenente la stringa da decriptare
  // ricavo il numero delle colonne
  k=crypted_colonn_message.length()/chiave.length();
  char tabella[chiave.length()][k]; // creo una tabella di tante righe quanti sono i caratteri della chiave e tante colonne quanto il quoziente fra il numero di caratteri della stringa da decifrare e quelli della chiave
  
  // riempimento della tabella con i caratteri della stringa da decifrare
  z=0;
  for(i=0;i<chiave.length();i++)
  {
    for(j=0;j<k;j++)
    {
      tabella[i][j]=crypted_colonn_message.charAt(z); // non opero alcun controllo sul superamento dell'indice z in quanto l'utilizzo del carattere riempitivo ('$') permette di avere un numero di lettere del messaggio da decifrare proprio pari al numero di righe * il numero delle colonne (e dunque riempire esattamente tutti gli "spazi" senza mai "sforare")
      z++;
    }
  }
  
  // ricavo la stringa decifrata
  char app2[2][strlen(app[0])]; // creazione di una seconda matrice d'appoggio che verrÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â  usata in seguito
  for(i=0;i<strlen(tabella[0]);i++) // ripeto il ciclo per un numero di volte pari al numero di colonne della tabella
  {
    for(k=0;k<chiave.length();k++) // ripeto il ciclo un numero di volte pari al numero di lettere della chiave
    {
      // prima di compiere operazioni sulla matrice contenente le lettere con i valori numerici ordinati del verme, copio tutta la matrice in un'altra di appoggio (per lavorare su quest'ultima e non "sporcare" quella originale)
      for(j=0;j<2;j++)
      {
        for(z=0;z<strlen(app[0]);z++)
        {
          app2[j][z]=app[j][z]; // copia
        }
      }
      
      // ricerca della prima lettera (in ordine alfabetico) della chiave di crittografia
      j=0; // indice per i caratteri della chiave
      z=0; // z contiene la posizione Jesima del valore piÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¹ piccolo del vettore di posizioni delle lettere ordinate alfabeticamente della chiave
      do
      {
        if(app2[1][j]<app2[1][z])
        {
          z=j; // se il valore confrontato ÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¨ piÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¹ piccolo registro la sua posizione nella variabile z ciÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â² consentirÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â  di trovare il numero della colonna da prendere in considerazione per la ricavazione dei caratteri da prendere per formare la stringa cifrata
        }
      }while(j<chiave.length()); // eseguo l'operazione per tutti i caratteri di "chiave"
      app2[1][z]=chiave.length()+1; // una volta trovato il valore minimo assegno a quella posizione un valore piÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¹ alto del numero degli elementi della stringa "chiave" stessa: in questo modo una lettera (e dunque la relativa posizione) non potranno essere presi piÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Â¹ di una volta in considerazione
    
      // concatenazione delle lettere della riga specificata (z) alla colonna I-esima
      crypted_vig_message=String(crypted_vig_message+tabella[z][i]);
    }
  }
}
