/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>                // ifstream, ofstream, fstream
#include "TrigT1RPChardware/crc8.h"

using namespace std;

crc8::crc8()
{
  // cout<<"Inizio della sessione crc8\n";
 _crci=0x00;   //crc iniziale,default="00000000"
 _pol=0x838000;    //polinomio divisore="1000000111"
 _ma=0x800000;     //maschera  
}

crc8::~crc8()
{
  // cout<<"Fine della sessione crc8\n";
}


int crc8::current()
{
 return _crci;
 //return pol;
 //return ma;
}

int crc8::calc (int dato)
{
 int i;
 int masc;
 int poli;

 _crci=_crci<<16;   //shifta il crc iniziale per metterlo davanti al dato
 dato=dato^_crci;  //mette,tramite xor,il crc davanti al dato
 masc=_ma;        //carico la maschera
 poli=_pol;       //carico il polinomio divisore

 for (i=0;i<16;i++)  
{
  if (dato & masc)   //tramite AND,individua se il MSB e' 1 e divide
  dato=dato^poli;    //divisione tra dato e polinomio
  masc=masc>>1;      //shift per bit successivo
  poli=poli>>1;      //shift per bit successivo
}
 _crci=dato;           //caricamento del crc ottenuto
 return 0;

}

