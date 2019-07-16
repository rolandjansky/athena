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

  class MergeMCAna : public ::AthHistogramAlgorithm {

    public: 
    MergeMCAna(const std::string& name, ISvcLocator* pSvcLocator);

      ~MergeMCAna();

      StatusCode  initialize();
      virtual StatusCode execute();
      StatusCode  finalize();

    private:

      float getPTV(const xAOD::TruthEventContainer* xTruthEventContainer);
      int32_t                        m_debugplots;
  }; 
}

#endif
