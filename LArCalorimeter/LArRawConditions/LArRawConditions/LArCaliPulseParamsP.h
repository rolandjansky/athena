/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARCALIPULSEPARAMSP
#define LARRAWCONDITIONS_LARCALIPULSEPARAMSP

#include <vector> 

// persistent c-struct 
class LArCaliPulseParamsP
{
public:
    LArCaliPulseParamsP()
      : m_Tcal(-9999999), m_Fstep(-9999999), m_Offset(-9999999), m_dTimeCal(-99999), m_nCB(-999) {} ;
    LArCaliPulseParamsP(float r, float s) 
      : m_Tcal(r), m_Fstep(s), m_Offset(-9999999), m_dTimeCal(-9999999), m_nCB(-999)  {} ;
    LArCaliPulseParamsP(float r, float s, float t, float u) 
      : m_Tcal(r), m_Fstep(s), m_Offset(t), m_dTimeCal(u), m_nCB(-999)  {} ;
    LArCaliPulseParamsP(float r, float s, float t, float u ,short v) 
      : m_Tcal(r), m_Fstep(s), m_Offset(t), m_dTimeCal(u), m_nCB(v)  {} ;
    bool isEmpty() const  { return (m_Tcal < -9999 && m_Fstep < -9999) ; } ;
    float         m_Tcal;
    float         m_Fstep;
    float         m_Offset;
    float         m_dTimeCal;
    short         m_nCB;
};

     
#endif 

