/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMMISSIONEVENT_COMTIME
#define COMMISSIONEVENT_COMTIME
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Information on trigger timing for ATLAS commissioning
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "CLIDSvc/CLASS_DEF.h"
#include "CLHEP/Vector/ThreeVector.h"

class ComTime
{
 public:
 
  ComTime() : m_trigCounterTTCTime((double)0.), m_trigCounterTime((double)0.), 
    m_trigCounterPosition(CLHEP::Hep3Vector((double)0.,(double)0.,(double)0.)),
    m_cosThetaDirection(CLHEP::Hep3Vector((double)0.,(double)0.,(double)0.)) {}
  
  ComTime(double TTCTime) : m_trigCounterTTCTime(TTCTime),  m_trigCounterTime((double)0.),
    m_trigCounterPosition(CLHEP::Hep3Vector((double)0.,(double)0.,(double)0.)),
    m_cosThetaDirection(CLHEP::Hep3Vector((double)0.,(double)0.,(double)0.)) {}
  
  ComTime(double TTCTime, double time) : m_trigCounterTTCTime(TTCTime),  m_trigCounterTime(time),
    m_trigCounterPosition(CLHEP::Hep3Vector((double)0.,(double)0.,(double)0.)), 
    m_cosThetaDirection(CLHEP::Hep3Vector((double)0.,(double)0.,(double)0.)) {}
  
  ComTime(double TTCTime, double time, CLHEP::Hep3Vector& pos, CLHEP::Hep3Vector& mom) : m_trigCounterTTCTime(TTCTime),
    m_trigCounterTime(time), m_trigCounterPosition(pos), m_cosThetaDirection(mom) {}


  ~ComTime() {};
  
  void SetTTCTime(double TTCTime)   {m_trigCounterTTCTime=TTCTime;}
  void SetTime(double time)         {m_trigCounterTime=time;}
  void SetPosition(CLHEP::Hep3Vector pos)  {m_trigCounterPosition=pos;}
  void SetDirection(CLHEP::Hep3Vector dir) {m_cosThetaDirection=dir;}

  double getTTCTime() const {return m_trigCounterTTCTime;}
  double getTime()  const {return m_trigCounterTime;}

  CLHEP::Hep3Vector GetCounterPosition() const {return m_trigCounterPosition;}
  CLHEP::Hep3Vector GetcosThetaDirection() const {return m_cosThetaDirection;}

 private:
    
  double m_trigCounterTTCTime;
  double m_trigCounterTime;
  CLHEP::Hep3Vector m_trigCounterPosition;
  CLHEP::Hep3Vector m_cosThetaDirection;

};

CLASS_DEF(ComTime,229624403,1)
#endif
