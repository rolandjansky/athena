/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TRUTHHELPER_H
#define DERIVATIONFRAMEWORK_TRUTHHELPER_H

#include "TLorentzVector.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODJet/JetContainer.h"
#include <vector>

namespace DerivationFramework {

  //static bool GreaterPt(const TLorentzVector &a, const TLorentzVector &b) { return a.Pt()>b.Pt(); }

    // truth VBF filter
    bool passTruthFilter(const xAOD::TruthEventContainer* xTruthEventContainer, const xAOD::JetContainer *truthJets, double JetEtaFilter, double JetpTFilter, double MjjFilter, double PhijjFilter);

    // compute truth information
    const std::vector<float> computeTruthInfo(const xAOD::TruthEventContainer* xTruthEventContainer);
    bool isBwithWeakDK(const int pID);
    bool isDwithWeakDK(const int pID);
}
#endif
