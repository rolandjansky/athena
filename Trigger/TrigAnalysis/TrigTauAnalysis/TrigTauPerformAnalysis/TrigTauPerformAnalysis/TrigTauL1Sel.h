/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL1Sel.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to apply L1 selection to performance ntuple objects
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _TRIGTAUL1SEL_H
#define _TRIGTAUL1SEL_H

#include "TrigTauPerformAnalysis/TrigTauHistogram.h"

class TrigTauL1Object;

// Bits defining pattern returned by TrigTauL1Sel::cutWord()
const unsigned long L1CUT_TAUCLUS = 0x01;
const unsigned long L1CUT_EMISOL = 0x02;
const unsigned long L1CUT_HDISOL = 0x04;
const unsigned long L1CUT_EMCORE = 0x08;
const unsigned long L1CUT_HDCORE = 0x10;
const unsigned long L1CUT_EMENER = 0x20;

//
// L1 trigger selector
//

class TrigTauL1Sel {
 public:
  TrigTauL1Sel();
  TrigTauL1Sel(const TrigTauL1Sel& other);
  virtual ~TrigTauL1Sel();

  void setCuts(float emE, 
               float tauE, 
               float EmI=0., 
               float HdI=0., 
               float EmCore=0., 
               float hdCore=0.);
  void removeEmIsol();

  void print() const;

  bool passed(const TrigTauL1Object& tau) const;

  // Return bits indicating cuts failed
  unsigned long cutWord(const TrigTauL1Object& tau) const;

  void setDebug( bool newval) { _debug = newval;};
  void setHistDebug(TrigTauHistogram::HISTTYPE type=TrigTauHistogram::PTHIST);

  TrigTauHistogram * histManager() { return m_histManager;};

 private:
  float cut_EmEnergy;
  float cut_TauEnergy;
  float cut_EmIsol;  
  float cut_HdIsol;  
  float cut_EmCore;  
  float cut_HdCore;

  bool _debug;

  TrigTauHistogram * m_histManager;

};

#endif
