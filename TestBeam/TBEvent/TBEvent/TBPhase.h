//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBPHASE
#define TBEVENT_TBPHASE
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Phase for an event.
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "AthenaKernel/CLASS_DEF.h"
#include <math.h>



class TBPhase
{
 public:
  

  static const int unknown = -99999 ;
  static const short sunknown = -999 ;

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////
    
  TBPhase() : m_phase(unknown) , m_phaseind(sunknown) , m_dTtoWAC(unknown) {};
  TBPhase(float phase,short phaseind) : m_phase(phase),m_phaseind(phaseind) , m_dTtoWAC(unknown) {};
  TBPhase(float phase,short phaseind,float dTtoWAC) : m_phase(phase),m_phaseind(phaseind),m_dTtoWAC(dTtoWAC) {};

  ~TBPhase() {};
  
  ///////////////////////
  // Data Manipulation //
  ///////////////////////
  float getPhase() const {return m_phase;}
  short getPhaseind() const {return m_phaseind;}
  float getdTtoWACSigned() const {return m_dTtoWAC;}
  float getdTtoWAC() const {return fabs(m_dTtoWAC);}

 private:
    
  //////////////////
  // Private Data //
  //////////////////

  float m_phase;
  short m_phaseind;
  float m_dTtoWAC;


};

CLASS_DEF( TBPhase , 127061164 , 1 )
#endif
