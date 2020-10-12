/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "TestMCASTTool.h"
#include <cmath>

namespace CP {

TestMCASTTool::TestMCASTTool( const std::string& name, ISvcLocator* svcLoc ) :
  AthAlgorithm( name, svcLoc ),
  m_MCaSTool( "CP::MuonCalibrationAndSmearingTool/MuonCalibrationAndSmearingTool", this )
{
  declareProperty( "Output", m_Output = "MCaST_Debug.root" );
  declareProperty( "SGKey", m_sgKey = "Muons" );
  declareProperty( "MuonCalibrationAndSmearingTool", m_MCaSTool );

  m_DebugFile = nullptr;
  m_DebugTree = nullptr;
  m_Combined = nullptr;
  m_InnerDet = nullptr;
  m_MSExtr = nullptr;
  m_MSOnlyExtr = nullptr;
}

StatusCode TestMCASTTool::initialize() {
  ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "SGKey = " << m_sgKey );
  ATH_MSG_DEBUG( "MuonCalibrationAndSmearingTool   = " << m_MCaSTool );
  ATH_CHECK( m_MCaSTool.retrieve() );

  const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
  m_sysList = CP::make_systematics_vector(recommendedSystematics);
  m_sysList.push_back(CP::SystematicSet());

  for(std::vector<CP::SystematicSet>::iterator sysItr = m_sysList.begin() + 1; sysItr != m_sysList.end(); ++sysItr) {
    std::string syst_name = ( *sysItr ).name();
    m_sysNames.push_back(syst_name);
  }

  std::vector<std::string> empty_list;

  m_Combined = std::make_unique<MMCTest::TrackInfo>("CB", m_sysNames);
  m_InnerDet = std::make_unique<MMCTest::TrackInfo>("ID", m_sysNames);
  m_MSExtr = std::make_unique<MMCTest::TrackInfo>("ME", m_sysNames);
  m_MSOnlyExtr = std::make_unique<MMCTest::TrackInfo>("MSOE", empty_list);

  m_DebugFile = new TFile( m_Output.c_str(), "RECREATE", "Smearing and non-Smearing of Muons" );
  m_DebugTree = new TTree( "CorrectionsTree", "This Tree contains the information of the muon after smearing effects" );

  m_DebugTree->Branch("Muon_Sel_Category_Raw", &m_SelCategoryRaw);
  m_DebugTree->Branch("Muon_Sel_Category", &m_SelCategory);

  m_Combined->Register(m_DebugTree);
  m_InnerDet->Register(m_DebugTree);
  m_MSExtr->Register(m_DebugTree);
  m_MSOnlyExtr->Register(m_DebugTree);

  return StatusCode::SUCCESS;
}

StatusCode TestMCASTTool::execute() {

  //---\\---// Retrieving EventInfo
  const xAOD::EventInfo* m_EvtInfo = nullptr;
  ATH_CHECK( evtStore()->retrieve( m_EvtInfo, "EventInfo" ) );
  ATH_MSG_DEBUG( "Event Number: " << m_EvtInfo->eventNumber() );

  //---\\---// Retrieving muons from container
  const xAOD::MuonContainer* muons = 0;
  ATH_CHECK( evtStore()->retrieve( muons, m_sgKey ) );
  ATH_MSG_DEBUG( "Number of muons: " << muons->size() );

  //---\\---// Looping over muons
  xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
  xAOD::MuonContainer::const_iterator mu_end = muons->end();

  xAOD::MuonContainer* mymuons = new xAOD::MuonContainer;
  ATH_CHECK( evtStore()->record(mymuons,"CalibMuons") );
  xAOD::MuonAuxContainer* mymuonsaux = new xAOD::MuonAuxContainer;
  ATH_CHECK( evtStore()->record(mymuonsaux,"CalibMuonsAux.") );
  mymuons->setStore(mymuonsaux);

  m_Combined->Reset();
  m_InnerDet->Reset();
  m_MSExtr->Reset();
  m_MSOnlyExtr->Reset();

  ATH_MSG_DEBUG( "Calibrating muon" ); 

  for(; mu_itr != mu_end; ++mu_itr) {

    //---\\---// Simple preselection
    if((*mu_itr)->muonType() != xAOD::Muon::Combined) continue;  

    const xAOD::TrackParticle* cbTrack = (*mu_itr)->trackParticle(xAOD::Muon::CombinedTrackParticle);
    if(cbTrack) m_Combined->Fill(cbTrack);

    const xAOD::TrackParticle* idTrack = (*mu_itr)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    if(idTrack) m_InnerDet->Fill(idTrack);

    const xAOD::TrackParticle* meTrack = nullptr;
    try {
      meTrack = (*mu_itr)->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
    } catch (SG::ExcBadAuxVar& b) { meTrack = (*mu_itr)->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle); }
    if(meTrack) m_MSExtr->Fill(meTrack);

    const xAOD::TrackParticle* msoeTrack = nullptr;
    try {
      msoeTrack = (*mu_itr)->trackParticle(xAOD::Muon::MSOnlyExtrapolatedMuonSpectrometerTrackParticle);
    } catch (SG::ExcBadAuxVar& b) { msoeTrack = (*mu_itr)->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle); }
    if(msoeTrack) m_MSOnlyExtr->Fill(msoeTrack);

    //---\\---// Calibrating muon
    ATH_MSG_DEBUG( "Calibrating muon" ); 
    for(std::vector<CP::SystematicSet>::iterator sysItr = m_sysList.begin() + 1; sysItr != m_sysList.end(); ++sysItr) {
      if( m_MCaSTool->applySystematicVariation(*sysItr) != CP::SystematicCode::Ok) {
        continue;
      }
      xAOD::Muon* mu = 0;
      if(m_MCaSTool->correctedCopy(**mu_itr, mu) == CP::CorrectionCode::Error) {
        ATH_MSG_WARNING( "Failed to correct the muon!" );
        continue;
      }

      std::string syst_name = ( *sysItr ).name();

      m_SelCategoryRaw = mu->auxdata<int>("raw_MCaST_Category");
      m_SelCategory = mu->auxdata<int>("MCaST_Category");
      m_Combined->SetCalibPt(mu->pt(), syst_name);
      m_InnerDet->SetCalibPt(mu->auxdata<float>("InnerDetectorPt"), syst_name);
      m_MSExtr->SetCalibPt(mu->auxdata<float>("MuonSpectrometerPt"), syst_name);
      delete mu;
    }

    m_DebugTree->Fill();
  }

  return StatusCode::SUCCESS;
}

StatusCode TestMCASTTool::finalize() {
  m_DebugFile->Write();
  return StatusCode::SUCCESS;
}

} // namespace CP
