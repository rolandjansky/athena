/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_MERGEMCTOOL_H
#define DERIVATIONFRAMEWORK_MERGEMCTOOL_H

//#include "AthenaBaseComps/AthAlgTool.h"
//#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "AthenaBaseComps/AthHistogramAlgorithm.h"
//#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
//#include "GaudiKernel/ToolHandle.h"
#include "xAODJet/JetContainer.h"
#include "TLorentzVector.h"
#include "TH1D.h"

namespace DerivationFramework {

  class MergeMCTool : public ::AthHistogramAlgorithm {
    //class MergeMCTool : public AthAnalysisAlgorithm {
    //class MergeMCTool : public AthAlgTool, public IAugmentationTool {

    public: 
    //MergeMCTool( const std::string& t, const std::string& n, const IInterface* p );
    MergeMCTool(const std::string& name, ISvcLocator* pSvcLocator);

      ~MergeMCTool();

      StatusCode  initialize();
      virtual StatusCode execute();
      StatusCode  finalize();

      //virtual StatusCode addBranches() const;


    private:

      static bool GreaterPt(const TLorentzVector &a, const TLorentzVector &b) { return a.Pt()>b.Pt(); }
      bool passTruthFilter(const xAOD::JetContainer *truthJets, double JetEtaFilter, double JetpTFilter, double MjjFilter, double PhijjFilter);

      //int32_t                        m_runNumber;
  }; 
}

#endif
