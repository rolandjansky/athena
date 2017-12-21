///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMakerAlg.h

#ifndef METMakerAlg_H
#define METMakerAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"

#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"


class IMETMaker;
class IAsgElectronLikelihoodTool;
class IAsgPhotonIsEMSelector;
namespace CP {
  class IMuonSelectionTool;
}
namespace TauAnalysisTools {
  class ITauSelectionTool;
}

namespace met {
  class METMakerAlg : public AthAlgorithm {

  public: 

    /// Constructor with parameters:
    METMakerAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~METMakerAlg(); 

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    METMakerAlg();

    bool accept(const xAOD::Electron* el);
    bool accept(const xAOD::Photon* ph);
    bool accept(const xAOD::TauJet* tau);
    bool accept(const xAOD::Muon* muon);

    std::string m_corename;

    std::string m_softclname;
    std::string m_softtrkname;

    //In release 21 need to replace the names of the containers by the appropriate data handles
    SG::ReadHandleKey<xAOD::ElectronContainer>      m_ElectronContainerKey;
    SG::ReadHandleKey<xAOD::PhotonContainer>        m_PhotonContainerKey;
    SG::ReadHandleKey<xAOD::TauJetContainer>        m_TauJetContainerKey;
    SG::ReadHandleKey<xAOD::MuonContainer>          m_MuonContainerKey;
    SG::ReadHandleKey<xAOD::JetContainer>           m_JetContainerKey;

    std::string m_eleColl;
    std::string m_gammaColl;
    std::string m_tauColl;
    std::string m_jetColl;
    std::string m_muonColl;



    SG::WriteHandleKey<xAOD::MissingETContainer> m_metKey;

    SG::ReadHandle<xAOD::MissingETAssociationMap> m_metMap;


    bool m_doTruthLep;
    
    /// Athena configured tools
    ToolHandle<IMETMaker> m_metmaker;

    ToolHandle<CP::IMuonSelectionTool> m_muonSelTool;
    ToolHandle<IAsgElectronLikelihoodTool> m_elecSelLHTool;
    ToolHandle<IAsgPhotonIsEMSelector>     m_photonSelIsEMTool;
    ToolHandle<TauAnalysisTools::ITauSelectionTool> m_tauSelTool;

  }; 

}

#endif
