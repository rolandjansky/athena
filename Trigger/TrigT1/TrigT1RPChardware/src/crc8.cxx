/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPChardware/crc8.h"

#include <fstream>  // ifstream, ofstream, fstream
#include <iostream>

using namespace std;

crc8::crc8() {
    // cout<<"Inizio della sessione crc8\n";
    m_crci = 0x00;     // crc iniziale,default="00000000"
    m_pol = 0x838000;  // polinomio divisore="1000000111"
    m_ma = 0x800000;   // maschera
}

crc8::~crc8() {
    // cout<<"Fine della sessione crc8\n";
}

int crc8::current() {
    return m_crci;
    // return pol;
    // return ma;
}

int crc8::calc(int dato) {
    int i;
    int masc;
    int poli;

    m_crci = m_crci << 16;  // shifta il crc iniziale per metterlo davanti al dato
    dato = dato ^ m_crci;   // mette,tramite xor,il crc davanti al dato
    masc = m_ma;            // carico la maschera
    poli = m_pol;           // carico il polinomio divisore

    for (i = 0; i < 16; i++) {
        if (dato & masc)         // tramite AND,individua se il MSB e' 1 e divide
            dato = dato ^ poli;  // divisione tra dato e polinomio
        masc = masc >> 1;        // shift per bit successivo
        poli = poli >> 1;        // shift per bit successivo
    }
    m_crci = dato;  // caricamento del crc ottenuto
    return 0;
}
