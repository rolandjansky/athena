/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUL1OBJECT_H
#define TRIGTAUL1OBJECT_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL1Object.h
/// Package : TrigTauPerformNtuple
/// Author  : Olya Igonkina
/// Created : January 2006
///
/// DESCRIPTION:
///           contains LVL1 variables from EMTAURoI
///           The names of variables are taken from L1EmTauObject (12.0.1 release)
///           all energies given are TRANSVERSE energies
///           and EmCore contains deposition in EM and HAD part!
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TLorentzVector.h"

class TrigTauL1Object : public TLorentzVector
{
 public :
  TrigTauL1Object();
  TrigTauL1Object(const TrigTauL1Object & other);

  virtual ~TrigTauL1Object();
  virtual void reset();

  virtual int charge() const { return 0;}

  float et4x4() { return Et(); };
  float et2x2() { return m_EmCore; };
  float etStandard() { return m_TauEnergy; };


  //----------- public data members
  // Et() corresponds to the total energy EmCore+EmIsol+HdIsol

  float m_EmEnergy;
  float m_TauEnergy;
  float m_EmIsol;
  float m_HdIsol;
  float m_EmCore; // this variable contains EM+Had core (according to Alan)
  float m_HdCore;
  int   m_EmThresh;
  int   m_TauThresh;
  long int   m_roiWord;
  
 private:
  ClassDef(TrigTauL1Object, 3)  ;
};

#endif //TRIGTAUTAUOBJECT_H
