///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ORMETMakerAlg.h

#ifndef ORMETMakerAlg_H
#define ORMETMakerAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "METMakerAlg.h"
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
  //class ORMETMakerAlg : public AthAlgorithm {
  class ORMETMakerAlg : public METMakerAlg {

  public: 

    /// Constructor with parameters:
    ORMETMakerAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    virtual ~ORMETMakerAlg(); 

    /// Athena algorithm's Hooks
    virtual StatusCode  initialize() override;
    virtual StatusCode  execute() override;
    virtual StatusCode  finalize() override;

  private: 

    /// Default constructor:
    ORMETMakerAlg();

    bool accept(const xAOD::Electron* el);
    bool accept(const xAOD::Photon* ph);
    bool accept(const xAOD::TauJet* tau);
    bool accept(const xAOD::Muon* muon);

    //std::string m_softclname;
    //std::string m_softtrkname;
    std::string m_soft;

    //In release 21 need to replace the names of the containers by the appropriate data handles
    /*SG::ReadHandleKey<xAOD::ElectronContainer>      m_ElectronContainerKey;
    SG::ReadHandleKey<xAOD::PhotonContainer>        m_PhotonContainerKey;
    SG::ReadHandleKey<xAOD::TauJetContainer>        m_TauJetContainerKey;
    SG::ReadHandleKey<xAOD::MuonContainer>          m_MuonContainerKey;
    SG::ReadHandleKey<xAOD::JetContainer>           m_JetContainerKey;

    SG::ReadHandleKey<xAOD::MissingETContainer>           m_CoreMetKey;

    SG::WriteHandleKey<xAOD::MissingETContainer> m_metKey;
    SG::ReadHandleKey<xAOD::MissingETAssociationMap> m_metMapKey;*/
    SG::ReadHandleKey<xAOD::MissingETAssociationMap> m_ORMetMapKey;

    SG::WriteHandleKey<xAOD::PFOContainer> m_chargedPFOContainerWriteHandleKey{this,"PFOChargedOutputName","OverlapRemovedCHSChargedParticleFlowObjects","WriteHandleKey for charged PFO"}; //jetOR
    SG::WriteHandleKey<xAOD::PFOContainer> m_neutralPFOContainerWriteHandleKey{this,"PFONeutralOutputName","OverlapRemovedCHSNeutralParticleFlowObjects","WriteHandleKey for charged PFO"}; //jetOR
    SG::WriteHandleKey<xAOD::PFOContainer> m_PFOContainerWriteHandleKey{this,"PFOOutputName","OverlapRemovedCHSParticleFlowObjects","WriteHandleKey for PFO"}; //jetOR


SG::ReadHandleKey<xAOD::PFOContainer> m_inPFOKey{this, "InPFOKey", "", "ReadHandleKey for modified  PFlow Objects"};
SG::WriteHandleKey<xAOD::PFOContainer> m_outPFOKey{this, "OutPFOKey", "", "WriteHandleKey for modified PFlow Objects"};





    //bool m_doTruthLep;
    bool m_doRetrieveORconstit;
    bool m_retainMuonConstit;
    bool m_doORMet;


    double m_electronPT;
    double m_muonPT;
    double m_photonPT;
    double m_tauPT;

    double m_electronETA;
    double m_muonETA;
    double m_photonETA;
    double m_tauETA;

    //bool m_selectElectrons;

    //bool m_usePromptElectrons;

    //bool m_doBadMuon;
    bool m_useUnmatched;
    bool m_doJVT;

    
    /// Athena configured tools
    /*ToolHandle<IMETMaker> m_metmaker;

    ToolHandle<CP::IMuonSelectionTool> m_muonSelTool;
    ToolHandle<IAsgElectronLikelihoodTool> m_elecSelLHTool;
    ToolHandle<IAsgPhotonIsEMSelector>     m_photonSelIsEMTool;
    ToolHandle<TauAnalysisTools::ITauSelectionTool> m_tauSelTool;*/

  }; 

}

#endif

