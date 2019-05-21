/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_MERGEMCTOOL_H
#define DERIVATIONFRAMEWORK_MERGEMCTOOL_H

#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODJet/JetContainer.h"
#include "TLorentzVector.h"
#include "TH1D.h"

namespace DerivationFramework {

  class MergeMCTool : public ::AthHistogramAlgorithm {

    public: 
    MergeMCTool(const std::string& name, ISvcLocator* pSvcLocator);

      ~MergeMCTool();

      StatusCode  initialize();
      virtual StatusCode execute();
      StatusCode  finalize();

    private:

      static bool GreaterPt(const TLorentzVector &a, const TLorentzVector &b) { return a.Pt()>b.Pt(); }
      bool passTruthFilter(const xAOD::TruthEventContainer* xTruthEventContainer, const xAOD::JetContainer *truthJets, double JetEtaFilter, double JetpTFilter, double MjjFilter, double PhijjFilter);
      
      // for the flavour filtering
      float getPTV(const xAOD::TruthEventContainer* xTruthEventContainer);
      bool passFlavour(const xAOD::TruthEventContainer* xTruthEventContainer, int &flav);
      bool isBwithWeakDK(const int pID) const;
      bool isDwithWeakDK(const int pID) const;

      int32_t                        m_debugplots;
  }; 
}

#endif
