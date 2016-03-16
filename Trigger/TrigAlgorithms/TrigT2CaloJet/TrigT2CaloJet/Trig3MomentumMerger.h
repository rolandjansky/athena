/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_TRIG3MOMENTUMMERGER_H 
#define TRIGT2CALOJET_TRIG3MOMENTUMMERGER_H

#include "TrigT2CaloJet/GridsHandler.h"
#include "TrigT2CaloJet/EtaPhiSampleHash.h"
#include "TrigT2CaloJet/HashedTrig3Momentum.h"

/*! 
  @brief Merge Trig3Momentum to a finite granularity

  Merge some input Trig3Momentum collection into an output
  Trig3Momentum collection with a predefined granularity.

  April 2012
  davide.gerbaudo@gmail.com
*/

class Trig3MomentumMerger {
 public:
  Trig3MomentumMerger();
  bool mergeCells(const Vt3m &input, Vt3m &output);
 private:
  unsigned int expectedLength(const Vt3m &input);
 private:
  EtaPhiSampleHash m_hashMap;
  GridsHandler m_gridsHandler;
};

#endif
