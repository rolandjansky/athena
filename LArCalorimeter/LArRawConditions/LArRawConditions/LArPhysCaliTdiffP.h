/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARPHYSCALITDIFFP
#define LARRAWCONDITIONS_LARPHYSCALITDIFFP

#include <vector> 

class LArPhysCaliTdiffP
{

public: 
    LArPhysCaliTdiffP() : m_Tdiff(-9999999) {} 
    LArPhysCaliTdiffP(float t) 
	    : m_Tdiff(t) {} 
    bool isEmpty() const { return m_Tdiff < -9999 ; } ;
    float         m_Tdiff;
};

#endif 

