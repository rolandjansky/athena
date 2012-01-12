/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUELECTRON_H_
#define  _TRIGTAUELECTRON_H_
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauElectron
/// Package : TauTrigPerformance
/// Author  : Olya Igonkina (University of Oregon)
/// Created : April 2006
///
/// DESCRIPTION:
///         offline electron
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TLorentzVector.h"

class TrigTauElectron: public TLorentzVector
{
 public:
  TrigTauElectron();
  TrigTauElectron(const TrigTauElectron & other);
  virtual ~TrigTauElectron();

  virtual void reset();
  int charge() const { return m_charge;}

  int isEM() const { return m_isEM;}
  

public :
  int   m_charge;
  int   m_isEM;
  float m_EoverP;
  float m_EtCone02; // this is really EtCone20
  float m_EtCone30;
  float m_EtCone40;
  unsigned int m_author;

 private:
  ClassDef(TrigTauElectron, 2)  ;
};

#endif //TRIGTAUTAUELECTRON_H
