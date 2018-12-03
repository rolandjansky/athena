/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// UpgradePerformanceFunctionsAlg.cxx
// Base class
#include "UpgradePerformanceFunctions/UpgradePerformanceFunctionsAlg.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODJet/JetContainer.h"

// For shallow copy containers
#include "xAODCore/ShallowCopy.h"

// For finding calibration files
#include "PathResolver/PathResolver.h"

// For output of histograms
#include "TH1.h"
#include "TFile.h"

// For configuration
#include "TString.h"
#include "TEnv.h"


//**********************************************************************

UpgradePerformanceFunctionsAlg::UpgradePerformanceFunctionsAlg(const std::string& name,
                           ISvcLocator* pSvcLocator )
  : ::AthAnalysisAlgorithm( name, pSvcLocator )
  , m_UpgradePerformanceFunctions("")
  , m_Nevts(0)
{
  // asg Tool Handles must be dealt with differently
  m_UpgradePerformanceFunctions.declarePropertyFor( this, "UpgradePerformanceFunctions", "The UpgradePerformanceFunctions instance" );
}

//**********************************************************************
UpgradePerformanceFunctionsAlg::~UpgradePerformanceFunctionsAlg() { }

//**********************************************************************

StatusCode UpgradePerformanceFunctionsAlg::initialize() {

  ATH_MSG_INFO("Initializing " << name() << "...");

  m_UpgradePerformanceFunctions.setTypeAndName("Upgrade::UpgradePerformanceFunctions/UpgradePerformanceFunctions");
  ATH_CHECK(m_UpgradePerformanceFunctions.retrieve());
  ATH_MSG_INFO("Retrieved tool: " << m_UpgradePerformanceFunctions->name() );

  //-- Book histograms here if needed

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode UpgradePerformanceFunctionsAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  ATH_MSG_INFO("Time per event:          (CPU / WALL)");
  ATH_MSG_INFO("  Entire loop           - " << m_clock0.CpuTime() * 1e3 / m_Nevts << " / " << m_clock0.RealTime() * 1e3 / m_Nevts << " ms");
  ATH_MSG_INFO("  First event           - " << m_clock1.CpuTime() * 1e3 << " / " << m_clock1.RealTime() * 1e3 << " ms");
  ATH_MSG_INFO("  Excluding first event - " << m_clock2.CpuTime() * 1e3 / (m_Nevts - 1) << " / " << m_clock2.RealTime() * 1e3 / (m_Nevts - 1) << " ms");
  
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode UpgradePerformanceFunctionsAlg::execute() {
  ATH_MSG_VERBOSE("Executing " << name() << "...");
  m_clock0.Start(false);
  if (m_Nevts == 0) {
    m_clock1.Start(false);
  } else {
    m_clock2.Start(false);
  }

  const xAOD::EventInfo* evtInfo(nullptr);
  ATH_CHECK( evtStore()->retrieve(evtInfo) );
  bool isSim = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);

  if( !isSim ) {
    ATH_MSG_ERROR( "You are trying to smear a data file. Exiting." );
    return StatusCode::FAILURE;
  }

  //Retrieve the truth containers
  const xAOD::JetContainer* truthJets(nullptr);
  ATH_CHECK( evtStore()->retrieve(truthJets, "AntiKt4TruthJets") );
  ATH_MSG_DEBUG( "Number of jets: " << truthJets->size() );

  //----- Jets
  for(auto jet : *truthJets) {
    ATH_MSG_DEBUG( "Jet pT " << jet->pt() * 1e-3 );
    ATH_MSG_DEBUG( "Smeared jet pT " << m_UpgradePerformanceFunctions->getJetSmearedEnergy(jet->pt(), jet->eta(), true) );
  }

  m_clock0.Stop();
  if (m_Nevts == 0) {
    m_clock1.Stop();
  } else {
    m_clock2.Stop();
  }
  ++m_Nevts;

  return StatusCode::SUCCESS;
}

//**********************************************************************


