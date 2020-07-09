/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMINBIASP
#define LARRAWCONDITIONS_LARMINBIASP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
*/


class LArMinBiasP
{
public: 

    LArMinBiasP() : m_MinBiasRMS(-9999999) {} 
    LArMinBiasP(float rms) : m_MinBiasRMS(rms) {} 
    bool isEmpty() const { return m_MinBiasRMS < -9999 ; } ;
    float m_MinBiasRMS;

};

#endif 

