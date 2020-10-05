/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARNOISEP
#define LARRAWCONDITIONS_LARNOISEP

#include <vector> 

// persistent c-struct 
class LArNoiseP 
{
public:
    LArNoiseP() : m_Noise(-9999999) {} 
    LArNoiseP(float n) : m_Noise(n) {} 
    bool isEmpty() const { return m_Noise < -9999 ; } ;
    float         m_Noise;
};

#endif 

