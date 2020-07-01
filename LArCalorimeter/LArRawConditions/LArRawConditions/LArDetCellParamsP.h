/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARDETCELLPARAMSP
#define LARRAWCONDITIONS_LARDETCELLPARAMSP

#include <vector> 

// persistent c-struct 
class LArDetCellParamsP{
public:
    LArDetCellParamsP() : m_Omega0(-9999999), m_Taur(-9999999) {} 
    LArDetCellParamsP(float o, float t)
	    : m_Omega0(o), m_Taur(t) {} 
    bool isEmpty() const { return (m_Omega0 < -9999 && m_Taur < -9999) ; } ;
    float         m_Omega0;
    float         m_Taur;
};

     
#endif 

