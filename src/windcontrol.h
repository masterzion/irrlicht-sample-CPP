#ifndef WINDCONTROL_H_INCLUDED
#define WINDCONTROL_H_INCLUDED



#include <windows.h>


typedef short _stdcall (*inpfuncPtr)(short portaddr);
typedef void _stdcall (*oupfuncPtr)(short portaddr, short datum);

HINSTANCE hLib;

short WindControlOk = 1;
inpfuncPtr inp32;
oupfuncPtr oup32;
int windcontrolport = 0x378;


void WindControlStart ( int lpt  ) {


    switch ( lpt ){
        case 2:
            windcontrolport=0x278;
            break;
        default:
            windcontrolport=0x378;
       }

     hLib = LoadLibrary("inpout32.dll");
     if (hLib == NULL) WindControlOk = 0;

     if ( WindControlOk == 1 ) {
       inp32 = (inpfuncPtr) GetProcAddress(hLib, "Inp32");
       if (inp32 == NULL)  WindControlOk = 0;
     }

     if ( WindControlOk == 1 ) {
       oup32 = (oupfuncPtr) GetProcAddress(hLib, "Out32");
       if (oup32 == NULL)  WindControlOk = 0;
     }
}



void WindControlSpeed (int val)
{
    short x;
    if ( WindControlOk == 1 ) {
      switch (val){
          case 1:
              x=0x08;
              break;
          case 2:
              x=0x0C;
              break;
          case 3:
              x=0x0E;
              break;
          case 4:
              x=0x0F;
              break;
          default:
              x=0x00;

      }

     (oup32)(windcontrolport,x);
    }
}

void WindControlStop () {
     WindControlSpeed(0);
     FreeLibrary(hLib);

}

#endif // WINDCONTROL_H_INCLUDED
