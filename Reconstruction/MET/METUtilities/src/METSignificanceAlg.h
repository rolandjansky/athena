///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METSignificanceAlg.h

#ifndef METSignificanceAlg_H
#define METSignificanceAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"

class IMETSignificance;
class IAsgElectronLikelihoodTool;
class IAsgPhotonIsEMSelector;
namespace CP {
  class IMuonSelectionTool;
}
namespace TauAnalysisTools {
  class ITauSelectionTool;
}

namespace met {
  class METSignificanceAlg : public AthAlgorithm {

  public: 

    /// Constructor with parameters:
    METSignificanceAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~METSignificanceAlg(); 

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    METSignificanceAlg();

    bool accept(const xAOD::Electron* el);
    bool accept(const xAOD::Photon* ph);
    bool accept(const xAOD::TauJet* tau);
    bool accept(const xAOD::Muon* muon);

    std::string m_mapname;
    std::string m_corename;
    std::string m_outname;

    std::string m_softclname;
    std::string m_softtrkname;

    std::string m_jetColl;
    std::string m_eleColl;
    std::string m_gammaColl;
    std::string m_tauColl;
    std::string m_muonColl;

    bool m_doTruthLep;
    
    /// Athena configured tools
    ToolHandle<IMETSignificance> m_metsignif;

    ToolHandle<CP::IMuonSelectionTool> m_muonSelTool;
    ToolHandle<IAsgElectronLikelihoodTool> m_elecSelLHTool;
    ToolHandle<IAsgPhotonIsEMSelector>     m_photonSelIsEMTool;
    ToolHandle<TauAnalysisTools::ITauSelectionTool> m_tauSelTool;

  }; 

}

#endif
