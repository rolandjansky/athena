#ifndef TRIGHLTJETHYPOUNITTESTS_MAKEHYPOJETS_H
#define TRIGHLTJETHYPOUNITTESTS_MAKEHYPOJETS_H

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"


HypoJetVector makeHypoJets(const std::vector<double>& etas);

// HypoJetVector makeHypoJets(const std::vector<MockJetWithLorentzVector*>& gj);

#endif
