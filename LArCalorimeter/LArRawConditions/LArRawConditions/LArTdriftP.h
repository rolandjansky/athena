/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARTDRIFTP
#define LARRAWCONDITIONS_LARTDRIFTP

#include <vector> 

// persistent c-struct 
class  LArTdriftP{
public:
    LArTdriftP(): m_Tdrift(-9999999) {} 
    LArTdriftP(float d): m_Tdrift(d) {} 
    bool isEmpty() const { return m_Tdrift < -9999 ; } ;
    float         m_Tdrift;
};

     
#endif 

