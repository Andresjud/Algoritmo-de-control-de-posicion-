/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#define Motor_X 1
#define Motor_Y 2
#define Motor_Z 3

#define Eje_X MCPWM_CH_8
#define Eje_Y MCPWM_CH_6
#define Eje_Z MCPWM_CH_2

int Contador =0;


double PosM_Ycm=0;
double PosM_Xcm=0;
double PosM_Zcm=0;

double PosM_Y=0;
double PosM_X=0;
double PosM_Z=0;

int DirM_Y=0;
int DirM_X=0;
int DirM_Z=0;


int Fin_1;// el estado 0 es de activacion , 1 es desactivado
int Fin_4;//el estado 0 es de activacion , 1 es desactivado
int Fin_5;//el estado 0 es de activacion , 1 es desactivado

int Estado = 0;// 0 es Inicial ; 1 es trabjando

double Error_X = 0.0;//para el error e[0] 
double Error_Y = 0.0;//para el error e[0] 
double Error_Z = 0.0;//para el error e[0] 

double LastError_X = 0.0;
double LastError_Y = 0.0;
double LastError_Z = 0.0;

double Ref_X = 4.0; //Referencia 
double Ref          _Y = 19.0;//Referencia 
double Ref_Z = 5.0;//Referencia 

double Kp_X = 1.0;//constante proporcional
double Kp_Y = 1.0;//constante proporcional
double Kp_Z = 1.0;//constante proporcional

double Ki_X = 0.0;//constante intengradora
double Ki_Y = 0.0;//constante intengradora
double Ki_Z = 0.0;//constante intengradora

double Kd_X = 0.0;//constate derivatiba
double Kd_Y = 0.0;//constate derivatiba
double Kd_Z = 0.0;//constate derivatiba


double Control_X = 0.0; //u{0} es la salida actual  y u{1} es la salida anterior
double Control_Y = 0.0; //u{0} es la salida actual  y u{1} es la salida anterior
double Control_Z = 0.0; //u{0} es la salida actual  y u{1} es la salida anterior


double T=0.001;

double Integral_X = 0.0;
double Integral_Y = 0.0;
double Integral_Z = 0.0;

double Derivada_X = 0.0;
double Derivada_Z = 0.0;
double Derivada_Y = 0.0;




bool HOME_Y;//Home = 0 es cuando el activado el fianal de carrera
bool HOME_X;//Home = 0 es cuando el activado el fianal de carrer
bool HOME_Z;//Home = 0 es cuando el activado el fianal de carrer




// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************


void enviar_PWM (int Motor ,double control_M ){
    
    int Duty ;
    int Duty_Z;
    
    Duty= (int)(((control_M  * 100.0) + 1000.0));
    
    Duty_Z = (int)((control_M * 200.0));
    
    if (Motor == Motor_X){
        MCPWM_ChannelPrimaryDutySet(Eje_X, Duty);
    }
    else if (Motor == Motor_Y ){
        MCPWM_ChannelPrimaryDutySet(Eje_Y, Duty);
    }
    else if (Motor == Motor_Z ) { 
        MCPWM_ChannelPrimaryDutySet(Eje_Z, Duty_Z);
        
        
       
     
    if (Error_Z > 0.0){
                 
        ENABLE_2_Set();
        ENABLE_1_Clear();
        
    }
             
    else if (Error_Z < 0.0) {
                 
        ENABLE_2_Clear();
        ENABLE_1_Set(); 
    
    }  
        
    }
    if (Error_X > 10){
        Error_X=10;
    }
    if (Error_Y > 10){
        Error_Y=10;
    }
   // if (Error_Z > 10){
   //     Error_Z = 10;
    //}
    if (Error_X < -10){
        Error_X=-10;
    }
    if (Error_Y < -10){
        Error_Y=-10;
    }
    //if (Error_Z < 0){
      //  Error_Z=0;
    //}
    
   
}


void Timer1sr(){
    
    Contador = Contador + 1;
    
    if (Contador >= 100){
        
       // Control Motor eje Y
  
        enviar_PWM (Motor_Y ,Control_Y );
     
       //Control Motor eje x
             
        enviar_PWM (Motor_X ,Control_X );
                 
       //Control Motor eje z
    
        enviar_PWM (Motor_Z ,Control_Z );
      
        Contador = 0;}
}


int main ( void )
{
    GPIO_Initialize();
    SYS_Initialize ( NULL );///* Initialize all modules *////* Se define el duty y el canal de la PWM.
    QEI1_Initialize();///* Initialize el QEA 
    QEI1_Start();
    QEI2_Start();
    QEI4_Start();
    QEI3_Start();
    MCPWM_Start();
    
    HOME_Y = 0;    HOME_X = 0;
    HOME_Z = 0;
    Estado = 0;
     
     
    while ( true )
    {
      
        
        
       PosM_Y = QEI1_PositionGet();
       PosM_X = QEI2_PositionGet();
       PosM_Z = QEI3_PositionGet();
       
       PosM_Ycm =((PosM_Y)/((double)802));//De cm a pulsos
       PosM_Xcm =((PosM_X)/((double)802));//De cm a pulsos
       PosM_Zcm =((PosM_Z)/((double)802));//De cm a pulsos
       
       
       //---------------------------- Error -------------------------------------------//
       
       Error_X = (Ref_X-PosM_Xcm);//Calculo de Error eje x
       Error_Y = (Ref_Y-PosM_Ycm);//Calculo de Error eje y
       Error_Z = (Ref_Z-PosM_Zcm);//Calculo de Error eje z
       
       LastError_X = Error_X;
       LastError_Y = Error_Y;
       LastError_Z = Error_Z;
       
      // rateError = (error - lastError)/elapsedTime;
       
       //---- Calcular los coeficiontes de la ecuaci?n de diferencia del eje X ---//
       
       Kd_X = 0.0;
       Kp_X = 1.0;
       Ki_X = 0.0;
       
       Integral_X = (Integral_X * (Error_X * T));
       Derivada_X = (Error_X -  LastError_X/T);
       
       if (Integral_X > 9.9){
           Integral_X =10;
       }
       else if (Integral_X < -9.9){
           Integral_X = -10;
       }
       
       
      
      //---- Calcular los coeficiontes de la ecuaci?n de diferencia del eje Y ---//
       
       Kd_Y = 0.4;
       Kp_Y = 1.2;
       Ki_Y = 0.4;
       
       Integral_Y = Integral_Y * (Error_Y * T);
       Derivada_Y = (Error_Y - LastError_Y/T);
       
       if (Integral_Y > 9.9){
           Integral_Y =10;
       }
       else if (Integral_Y < -9.9){
           Integral_Y = -10;
       }
       
      
       //---- Calcular los coeficiontes de la ecuaci?n de diferencia del eje Z ---//
        
       Kd_Z = 0.0;
       Kp_Z = 1.0;
       Ki_Z = 0.0;
       
       Integral_Z = Integral_Z * (Error_Z * T);
       Derivada_Z = (Error_Z - LastError_X/T);
       
       if (Integral_Z > 9.9){
           Integral_Z =10;
       }
       else if (Integral_Z < -9.9){
           Integral_Z = -10;
       }
       
      
       
       //----------------------- PID ---------------------------------//
       
       Control_X = Error_X * Kp_X + Ki_X * Integral_X;
       Control_Y = Error_Y * Kp_Y + Ki_Y * Integral_Y;
       Control_Z = Error_Z * Kp_Z + Ki_Z * Integral_Z;
       
       
       
      
       if(H1_Get()) {HOME_Y= false;} else {HOME_Y=true;}   
       if(H4_Get()) {HOME_X= false ;} else {HOME_X=true;}  
       if(H5_Get()) {HOME_Z= true ;} else {HOME_Z=false;} 
       
       
        
     if (Estado == 0){
         
             INDI_0_Clear();
             INDI_1_Clear();
            
             // Si todos estan en HOME, termino de iniciar y pasa al estado de control
             if (HOME_Y && HOME_X && HOME_Z){
             
                 PosM_X = 0;
                 PosM_Y = 0;
                 PosM_Z = 0;
                 
                 QEI1_PositionCountSet(0);
                 QEI2_PositionCountSet(0);
                 QEI3_PositionCountSet(0);
                 
                 PosM_Ycm =0.0;
                 PosM_Xcm =0.0;
                 PosM_Zcm =0.0;
          
                      
                 Estado = 1;
                
   
             }
            else {
                // llevar todos los motores al HOME 
                if (!(HOME_X)){
                 MCPWM_ChannelPrimaryDutySet(Eje_X, 500);
                   
                }
                else{
                 MCPWM_ChannelPrimaryDutySet(Eje_X, 1000);
                }
                    
                
                if(!(HOME_Y)){
                    MCPWM_ChannelPrimaryDutySet(Eje_Y, 500);
                    
                }
                 else{
                    MCPWM_ChannelPrimaryDutySet(Eje_Y, 1000);
                 }
                
                if (!(HOME_Z)){
                   ENABLE_2_Clear();
                   ENABLE_1_Set(); 
                   MCPWM_ChannelPrimaryDutySet(Eje_Z, 1000);
                    
                    }
                else {
                    MCPWM_ChannelPrimaryDutySet(Eje_Z, 0);
                    ENABLE_1_Clear();
                    ENABLE_2_Clear();
                   
                }
                
             }
             
             
         }
       else if (Estado == 1){
             INDI_0_Set();
             INDI_1_Clear();
             
             
             
                     
             if ( PosM_Xcm >= 2.0 && PosM_Ycm >= 2.0 && !(HOME_Z)){
                 
                
                 MCPWM_ChannelPrimaryDutySet(Eje_X, 1000);
                 MCPWM_ChannelPrimaryDutySet(Eje_Y, 1000); 
                 MCPWM_ChannelPrimaryDutySet(Eje_Z, 0);
                 ENABLE_1_Clear();
                 ENABLE_2_Clear();
                 
                 CORETIMER_DelayMs (1000);
                 
                 QEI1_PositionCountSet(0);
                 QEI2_PositionCountSet(0);
                 QEI3_PositionCountSet(0);
                 
                 PosM_Ycm =0.0;
                 PosM_Xcm =0.0;
                 PosM_Zcm =0.0;
                 
                 PosM_X = 0;
                 PosM_Y = 0;
                 PosM_Z = 0;
                
                 
                 MCPWM_ChannelPrimaryDutySet(Motor_Y, 1050);
                 MCPWM_ChannelPrimaryDutySet(Motor_X, 1050); 
                 MCPWM_ChannelPrimaryDutySet(Motor_Z, 100);
                 ENABLE_1_Clear();
                 ENABLE_2_Set();
                 
                 Estado = 2;
                 TMR2_CallbackRegister( Timer1sr, (uintptr_t) NULL);
                 TMR2_Start();
                 Contador = 0;
                 
             }
             else{
                 if (PosM_Xcm == 1.0){
                   MCPWM_ChannelPrimaryDutySet(Eje_X, 1000);
                   
                 }
                 else {
                     MCPWM_ChannelPrimaryDutySet(Eje_X, 1200);
                     
                 }
                 
                 if (PosM_Ycm == 1.0){
                     MCPWM_ChannelPrimaryDutySet(Eje_Y, 1000);
                    
                 }
                 else {
                     MCPWM_ChannelPrimaryDutySet(Eje_Y, 1200);
                    
                 }
                 if(PosM_Zcm >= 1.0){
                     ENABLE_1_Clear();
                     ENABLE_2_Clear();
                     MCPWM_ChannelPrimaryDutySet(Eje_Z, 0);
                    
                 }
                 else {
                     ENABLE_1_Clear();
                     ENABLE_2_Set();
                     MCPWM_ChannelPrimaryDutySet(Eje_Z, 200);
                    
                     
                 }
             }
         }
       
     else if(Estado == 2) {
          
             INDI_1_Set();
             INDI_0_Clear();   
             
             if (Error_X <=1.5 && Error_Y <=1.5 && Error_Z <=1.5){
                 Estado = 4;
                 Contador = 0;
                 
             }
        
     
             
             
             if (HOME_X){
                 Estado = 3;
             }
             if (HOME_Y){
                 Estado = 3;
             }
              if (HOME_Z){
                 Estado = 3;
             }
     }
       
     else if (Estado == 3 ){
         
             INDI_0_Set();
             INDI_1_Set();
             enviar_PWM (Motor_X ,1000);
             enviar_PWM (Motor_Y ,1000);
             MCPWM_Stop();
                   
     }
       
             
       
       
       
     
     else if (Estado == 4){
         
         
         Ref_X = 10.0;

         Ref_Y = 10.0;
 
         Ref_Z = 3.0;

         
                 
                 
              if (HOME_X){
                 Estado = 3;
             }
             if (HOME_Y){
                 Estado = 3;
             }
              if (HOME_Z){
                 Estado = 3;
             }
     }
      
    
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

