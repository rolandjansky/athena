/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUJET_H_
#define  _TRIGTAUJET_H_
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauJet
/// Package : TauTrigPerformance
/// Author  : S. Demers (based on TrigTauElecton...)
/// Created : February 2009
///
/// DESCRIPTION:
///         offline jet
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TLorentzVector.h"

class TrigTauJet: public TLorentzVector
{
 public:
  TrigTauJet();
  virtual ~TrigTauJet();

  virtual void reset();
  float weight() const { return m_weight;}

  bool isGood() const;
  bool isBad() const;
  bool isUgly() const;
  
public :
  float m_weight;       // flavour tag weight
  
  float m_emJESfactor;  // JES factor at EM scale
  
  int             m_jetn90cells;        // # cells carrying >90% energy 
  int             m_jetn90constituents; // # constituents carrying >90% energy
  float           m_jetQuality;         // jet quality
  float           m_jetTimeClusters;    // cluster timing
  float           m_jetTimeCells;       // cell timing
  float           m_jetEMf;             // EM fraction
  float           m_jetHECf;            // efrac in hadronic end cap
  float           m_jetTG3f;            // efrac in TileGap3
  float           m_jetCorf;            // efrac in dead cells (BCH_CORR_CELL)

 private:
  ClassDef(TrigTauJet, 3)  ;
};

#endif //TRIGTAUJET_H
