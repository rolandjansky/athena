/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ElectronThresholds_h
#define ElectronThresholds_h

#include "TrigMuonEfficiency/ElectronHypothesis.h"

namespace ElectronHypoThresholds {

// EF electron
const ElectronHypothesis::EF_thresholdPair ef_el_pair[] = {
  ElectronHypothesis::EF_thresholdPair("7GeV",   7000.),
  ElectronHypothesis::EF_thresholdPair("12GeV", 12000.),
  ElectronHypothesis::EF_thresholdPair("24GeV", 24000.),
};

const ElectronHypothesis::EF_thresholdMap ef_el_map(ef_el_pair, ef_el_pair + sizeof(ef_el_pair)/sizeof(ef_el_pair[0]));
}

#endif
// eof
