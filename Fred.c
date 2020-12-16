/*
 * File:   Simon.c
 * Author: camil
 *
 * Created on 16 de diciembre de 2020, 10:34
 */

#include <htc.h>
#include <xc.h>
//#include <eeprom.h>
#define Secuencia_Tamano 512
#define EEPROM_SEED_ADRESS 0
#define EEPROM_SOUND_EBABLE_ADRESS 1
#define EEPROM_START 0xF000
#define EEPROM_WRITE
#define EEPROM_READ

 
// Variables globales
char Secuencia[Secuencia_Tamano];               // Secuencia a seguir. Tamano = Tama�o
unsigned int SecuenciaApuntador = 0;  			// Posicion actual en la secuencia
unsigned int UsuarioSecuenciaApuntador = 0;
unsigned char estaJugando = 0;
 
unsigned char estaSonidoActivo = 1;
unsigned int Frecuencias[] = {440,493,523,587};
 
void GenerarProximo();
void _Reset();
 
void GameOver()
{
     PORTD = 0x00;
     __delay_ms(500);
     _Reset();
}

void GeneradorNumerosRandom()
{
     unsigned short seed=0;
     //seed = EEPROM_read(EEPROM_SEED_ADRESS);
     srand(seed);
     seed++;
     //EEPROM_write(EEPROM_SEED_ADRESS,seed);
}
 
// Reinicia el juego (se llama una vez cuando se enciende el pic o cuando el jugador se equivoca)
void _Reset()
{
     GeneradorNumerosRandom();
     estaJugando = 0;
     GenerarProximo();
}
 
void Limpiar()	// Apaga todos los LED
{
     PORTD = 0x00;
}
 
void Mostrar(int numero)
{
     Limpiar();
     PORTD = PORTD | (0b00010000 << numero);	// Enmascaramiento para encender el LED solicitado
}
 
void RepetirSecuencia()
{
     for(int i=0; i < SecuenciaApuntador; i++)
     {
          Limpiar();
          __delay_ms(250);
          Mostrar(Secuencia[i]);
          if(estaSonidoActivo)
                            //Frecuencias[BotonPresionado],250;
                            Mostrar(Secuencia[i]);
          else
              __delay_ms(250);
     }
}
 
void GenerarProximo()
{
     int RandomNumber;
     RandomNumber = rand()%4;
     Secuencia[SecuenciaApuntador] = RandomNumber;
     SecuenciaApuntador++;
     __delay_ms(500);
     RepetirSecuencia();
     __delay_ms(500);
     Limpiar();
     estaJugando = 1;
     UsuarioSecuenciaApuntador = 0;
}

 
void PresionadoDeBotones(char a, char b, char c, char d)
{
     int BotonPresionado = 0;
     
     if(a || b || c || d)    // Si el usuario presiona algun pulsador
     {
          if(a)
               BotonPresionado = 0;
          if(b)
               BotonPresionado = 1;
          if(c)
               BotonPresionado = 2;
          if(d)
               BotonPresionado = 3;
 
          if(estaJugando)
          {
            Mostrar(BotonPresionado);
            if(estaSonidoActivo)
                Frecuencias[BotonPresionado],250;
                //printf("se supone que se prende el led");
            else
                __delay_ms(250);
            Limpiar();
            if(BotonPresionado == Secuencia[UsuarioSecuenciaApuntador])
            {
                 // Si el usuario no se equivoca
                 UsuarioSecuenciaApuntador++;
                 if(UsuarioSecuenciaApuntador == SecuenciaApuntador)
                 {
                     // Si el usuario recuerda la secuencia completa
                     estaJugando = 0;
                     UsuarioSecuenciaApuntador = 0;
                     GenerarProximo();					// Agrega un nuevo numero a la secuencia
                 }
            }
            else
            {
                // Si el usuario se equivoca
                GameOver();
            }
        }
     }
}
 
void CargarConfiguracion()
{
     unsigned short SoundEnabled = 0;
     //SoundInit = EEPROM_read(EEPROM_SOUND_EBABLE_ADRESS);
     //SoundInit = EEPROM_SOUND_EBABLE_ADRESS;
     estaSonidoActivo = SoundEnabled;
}
 
void main(void) {
     int i=0;
     bit pa,pb,pc,pd;   // Estado actual de los pulsadores
     bit oa,ob,oc,od;   // Estado anterior de los pulsadores
	 
	 // Inicialización
     oa=ob=oc=od=0;
     pa=pb=pc=pd=0;
 
     TRISD = 0x0F; 		// Entradas (pulsadores): RD0 - RD3. Salidas (LEDs): RD4 - RD7
     PORTD = 0x00;		// Se inicializa todo en estado bajo, es decir, apagado.
     
     CargarConfiguracion();
     
     //Sound_init(&PORTB,0);
     _Reset();     		// Llama a la función _Reset() una vez antes de iniciar para realizar configuraciones adicionales.
     
     while(1)			// Ciclo infinito. Implementacionn de la maquina de estados.
     {
             pa=pb=pc=pd=0;
             // Pulsador 1
             if(LATBbits(&PORTD,0,1,0) && !oa)
             {
                     oa = 1;
                     pa = 1;
             }
             if(!LATBbits(&PORTD,0,1,0))
             {
                     oa = 0;
                     pa = 0;
             }
             
             // Pulsador 2
             if(LATBbits(&PORTD,1,1,0) && !ob)
             {
                     ob = 1;
                     pb = 1;
             }
             if(!LATBbits(&PORTD,1,1,0))
             {
                     ob = 0;
                     pb = 0;
             }
             
             // Pulsador 3
             if(LATBbits(&PORTD,2,1,0) && !oc)
             {
                     oc = 1;
                     pc = 1;
             }
             if(!LATBbits(&PORTD,2,1,0))
             {
                     oc = 0;
                     pc = 0;
             }
             
             // Pulsador 4
             if(LATBbits(&PORTD,3,1,0) && !od)
             {
                     od = 1;
                     pd = 1;
             }
             if(!LATBbits(&PORTD,3,1,0))
             {
                     od = 0;
                     pd = 0;
             }      
             // Se analizan los botones presionados
             PresionadoDeBotones(pa,pb,pc,pd);
     }
}