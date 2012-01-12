/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUTRIGROI_H
#define TRIGTAUTRIGROI_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauRoI
/// Package : TauTrigPerformNtuple
/// Author  : Olya Igonkina (University of Oregon)
/// Created : March 2007
///
/// DESCRIPTION:
///           info for trigger EDMs candidates _other_ than taus
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TObject.h"
#include "TLorentzVector.h"
#include "TrigTauPerformNtuple/TrigTauFlags.h"

class TrigTauRoI: public TLorentzVector
{
 public :
  TrigTauRoI()  {reset();};
  virtual ~TrigTauRoI() {};

  void reset() {m_tags.reset(); m_roiWord=0;};

  bool isJet() const {return m_tags.isJet(); } ;
  bool isMuon() const {return m_tags.isMuon(); };
  bool isElectron() const {return m_tags.isElectron(); };
  bool isTau() const {return m_tags.isTau(); }; 

  bool isTau_allDetectors() const {return m_tags.isTau_allDetectors(); };
  bool isTau_CaloOnly() const {return m_tags.isTau_CaloOnly(); };
  bool isTau_IDOnly() const {return m_tags.isTau_IDOnly(); };


  TrigTauFlags m_tags;
  long int     m_roiWord;



 private:
  ClassDef(TrigTauRoI, 3)  ;
};

#endif // TRIGTAUTRIGROI_H
