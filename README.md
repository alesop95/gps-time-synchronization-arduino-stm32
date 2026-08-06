# Sincronizzazione del tempo GPS su Arduino e STM32 Nucleo

Questo repository raccoglie il lavoro sperimentale sviluppato per una tesi di laurea triennale in Ingegneria Elettronica/Informatica, con l'obiettivo di misurare e confrontare la deriva dell'orologio (clock drift) su due piattaforme embedded, usando il tempo GPS come riferimento assoluto. Le due piattaforme sono un Arduino Uno, che si affida al contatore software `millis()`, e uno STM32 Nucleo della famiglia STM32L1, che pilota invece il proprio RTC hardware con un cristallo esterno da 32.768 kHz (oscillatore LSE). Entrambe ricevono le frasi NMEA dallo stesso modulo GPS e registrano lo scarto tra il proprio orologio e il tempo GPS durante acquisizioni di piu' ore, per quantificare in modo concreto quanto un RTC hardware sia piu' stabile di un contatore software nella cronometria di lunga durata.

## Contenuto del repository

Il codice e' diviso su tre file, uno per ciascun componente della catena di misura.

`gps_time_extraction.ino` e' lo sketch Arduino: usa la libreria `SoftwareSerial` per aprire una seconda porta seriale verso il modulo GPS a 9600 baud, esegue il parsing carattere per carattere della frase `$GPGGA` per isolarne il campo orario, e stampa sulla seriale hardware (verso il PC) la coppia `<tempo_gps>;<millis_arduino>` ogni volta che intercetta una nuova frase valida.

`stm32nucleo RTC config.c` e' il firmware dello STM32 Nucleo, costruito su Contiki OS e STM32 HAL. Configura la USART3 in interrupt per ricevere le frasi NMEA, inizializza l'RTC con formato 24 ore e i prescaler asincrono e sincrono derivati dall'oscillatore LSE, sincronizza l'RTC al primo fix GPS ricevuto e da quel momento confronta periodicamente il tempo dell'RTC con i fix GPS successivi, stampando su seriale lo scarto in millisecondi insieme al timestamp GPS.

`main.py` e' il logger lato PC: uno script Python 2.7 basato su `pySerial` che apre la porta seriale (di default `COM3` a 9600 baud), legge le righe prodotte dallo sketch Arduino, calcola lo scarto tra il tempo GPS e `millis()` rispetto al primo campione ricevuto, e scrive tutto in un file `output.csv` a due colonne, sovrascritto a ogni sessione di acquisizione.

`STM32CubeL1 - MCU package.url` e' un collegamento al pacchetto STM32CubeL1 di ST Microelectronics, l'HAL usato dal firmware Nucleo.

## Come funziona la misura

La metodologia e' la stessa su entrambe le piattaforme: il modulo GPS trasmette in continuo frasi NMEA via UART a 9600 baud, la frase `$GPGGA` viene isolata e se ne estrae il campo del tempo. Sull'Arduino il confronto vero e proprio avviene offline: lo sketch si limita ad affiancare tempo GPS e `millis()` sulla seriale, ed e' lo script Python sul PC a calcolare la deriva relativa al campione iniziale e a salvarla su CSV. Sullo STM32 il confronto avviene invece a bordo: il firmware sincronizza l'RTC al primo fix GPS disponibile, poi acquisisce un nuovo fix GPS ogni 30 secondi e calcola la differenza in millisecondi tra il tempo dell'RTC (che nel frattempo scorre libero, pilotato dal solo LSE) e il tempo GPS, trasmettendo il risultato via seriale.

## Risultati

Su un'acquisizione di circa quattro ore, l'orologio software dell'Arduino (`millis()`) ha accumulato una deriva di circa 5,0 secondi, contro circa 1,6 secondi per l'RTC hardware dello STM32 pilotato dal cristallo LSE. L'RTC hardware si e' quindi dimostrato circa tre volte piu' stabile del contatore software, a conferma del motivo per cui un RTC a cristallo e' la scelta corretta per la cronometria di lunga durata in un sistema embedded.

## Stack tecnico

Arduino Uno (ATmega328P) con libreria `SoftwareSerial`; STM32 Nucleo (famiglia STM32L1) con Contiki OS e STM32 HAL, RTC pilotato da oscillatore LSE a 32.768 kHz; modulo GPS con uscita NMEA a 9600 baud; Python 2.7 con `pySerial` per l'acquisizione e la scrittura dei log CSV lato PC.

## Stato del progetto

Il codice e' quello prodotto durante il lavoro di tesi ed e' rimasto cosi' com'era a quel momento: lo script Python usa ancora la sintassi di Python 2 (per esempio `print` come istruzione), e non e' stato portato a Python 3 ne' modernizzato. Il repository va inteso come materiale sperimentale archiviato, non come uno strumento in sviluppo attivo.
