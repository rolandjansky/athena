/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////// 
/// Name    : TauToolAthenaExample.cxx
/// Package : offline/PhysicsAnalysis/TauID/TauTools
/// Authors : Dugan O'Neil and Jyothsna Rani
/// Created : September 2007
///
/// DESCRIPTION:
/// Top-level example of how to call tauTools from Athena
/// TruthHandler, RecoHandler and MathcingTools classes present in TauTools package have 
/// a very general interface, requiring only containers of tauJets or truth as input. 
/// So, we provide this example of how to access event information and pass it to tauTools from Athena. Other
/// examples which do the same thing for other analysis formats should also 
/// be available in this CVS package.
///
/// INSTRUCTIONS: Posted on TauTools Wiki page
///               https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TauAnalysis
/// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "TauTools/TauToolAthenaExample.h"

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

TauToolAthenaExample::TauToolAthenaExample(const std::string& name, 
					   ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator) {
  
  /** Container names and the cuts */
  declareProperty("MCParticleContainer", 
		  m_truthParticleContainerName = "SpclMC",   
		  "Truth particle container");
  declareProperty("TauJetContainer",     
		  m_taujetContainerName = "TauRecContainer", 
		  "Tau container");
  declareProperty("DeltaRCut",           
		  m_deltaRMatchCut = 0.2,
		  "delta R match cut");

}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

TauToolAthenaExample::~TauToolAthenaExample() {}

////////////////////////////////////////////////////////////////////////////////////
/// initialize

StatusCode TauToolAthenaExample::initialize() {

  m_event_number = 0;

  msg(MSG::DEBUG) << "Initializing TauToolAthenaExample" << endreq;

  ////////////////////////////////////////////////////////////////////////
  // 1: Create histograms to be filled in
  ////////////////////////////////////////////////////////////////////////
  tauexample.CreateHistos();

  return StatusCode::SUCCESS;
}		 


///////////////////////////////
/// Execute - on event by event

StatusCode TauToolAthenaExample::execute() {

  ////////////////////////////////////////////////////////////////////////
  // 2: Get the container keys to fetch from store gate
  ////////////////////////////////////////////////////////////////////////
  /** get the MC truth particle AOD container from StoreGate */
  const TruthParticleContainer*  mcpartTES = 0;
  StatusCode sc = evtStore()->retrieve( mcpartTES, m_truthParticleContainerName );
  if( sc.isFailure()  ||  !mcpartTES ) {
    msg(MSG::WARNING) << "No AOD MC truth particle container with name " 
		      << m_truthParticleContainerName << " found in TDS"
		      << endreq;
    return StatusCode::FAILURE;
  }
  msg(MSG::DEBUG) << " MC Truth Container Successfully Retrieved" << endreq;
  
  /** get the AOD TauJet container from TES */
  const Analysis::TauJetContainer* tauRecTES = 0;
  sc = evtStore()->retrieve( tauRecTES, m_taujetContainerName );
  if( sc.isFailure()  ||  !tauRecTES ) {
    msg(MSG::WARNING)
          << "No reconstructed tau container with name " 
	  <<  m_taujetContainerName << " found in TDS"
          << endreq;
     return StatusCode::FAILURE;
  }
  msg(MSG::DEBUG) << " Reco TauRec Container Successfully Retrieved" << endreq;

  tauexample.setdeltaRMatchCut(m_deltaRMatchCut);

  ////////////////////////////////////////////////////////////////////////
  // 3: Loop over the events and call the efficiency/resolution 
  // methods for each event
  ////////////////////////////////////////////////////////////////////////
  tauexample.getTauRecProngEfficiency(tauRecTES, mcpartTES);
  tauexample.getTauRecResolution(tauRecTES, mcpartTES);

  m_event_number++;

  return StatusCode::SUCCESS;
}


//////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode TauToolAthenaExample::finalize() {

  ////////////////////////////////////////////////////////////////////////
  // 4: Finalize and dump the histograms in ROOT file
  ////////////////////////////////////////////////////////////////////////
  tauexample.finalizeProng();
  tauexample.finalizeResolution();

  // uncomment the below lines if you want to store all the histos in a single ROOT file
  // otherwise the histos are stored seperately in two different ROOT files for each method.
  //TFile *exrootfile = new TFile("TauToolExamples.root","RECREATE");
  //tauexample.StoreHistos(exrootfile);

  msg(MSG::INFO) << "finalize()" << endreq;

  return StatusCode::SUCCESS;
  
}

///////////////////////////////
/// Clear - clear members
StatusCode TauToolAthenaExample::clear() {

  return StatusCode::SUCCESS;
}

