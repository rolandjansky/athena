/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUJETL1OBJECT_H
#define TRIGTAUJETL1OBJECT_H
//=====================================================================
//  TrigTauJetL1Object - contains JetL1 variables
//
//  author : M. Pilar Casado (casado@ifae.es) 03.2007
//=====================================================================


#include "TLorentzVector.h"


class TrigTauJetL1Object : public TLorentzVector
{
 public :
  TrigTauJetL1Object();
  TrigTauJetL1Object(const TrigTauJetL1Object & other);

  virtual ~TrigTauJetL1Object();
  virtual void reset();

  //----------- public data members
  float m_eta;
  float m_phi;
  float m_ET4x4;
  float m_ET6x6;
  float m_ET8x8;

 private:
  ClassDef(TrigTauJetL1Object, 2)  ;
};

#endif //TRIGTAUJETL1OBJECT_H
