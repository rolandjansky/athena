/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUMUON_H_
#define  _TRIGTAUMUON_H_/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauMuon
/// Package : TauTrigPerformance
/// Author  : Olya Igonkina (University of Oregon)
/// Created : April 2006
///
/// DESCRIPTION:
///         offline muon
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TLorentzVector.h"

class TrigTauMuon: public TLorentzVector
{
 public:
  TrigTauMuon();
  TrigTauMuon(const TrigTauMuon & other);
  virtual ~TrigTauMuon();

  virtual void reset();
  int charge() const { return m_charge;}

public :
  int   m_charge;
  float m_EtCone02; // this is really EtCone02
  float m_EtCone30;
  float m_EtCone40;
  unsigned int m_author;

 private:
  ClassDef(TrigTauMuon, 2)  ;
};

#endif //TRIGTAUTAUMUON_H
