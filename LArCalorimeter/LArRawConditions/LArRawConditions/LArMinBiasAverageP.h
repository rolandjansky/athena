/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMINBIASAVERAGEP
#define LARRAWCONDITIONS_LARMINBIASAVERAGEP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
*/


class LArMinBiasAverageP
{
public: 

    LArMinBiasAverageP() : m_MinBiasAverage(-9999999) {} 
    LArMinBiasAverageP(float average) : m_MinBiasAverage(average) {} 
    bool isEmpty() const { return m_MinBiasAverage < -9999 ; } ;
    float m_MinBiasAverage;

};

#endif 

