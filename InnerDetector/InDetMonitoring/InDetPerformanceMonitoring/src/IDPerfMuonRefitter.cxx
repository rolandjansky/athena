/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


//==================================================================================

//==================================================================================
// Include files...
//==================================================================================

// This file's header
#include "InDetPerformanceMonitoring/IDPerfMuonRefitter.h"


// Need containers
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

//Interface Headers
#include "TrkTrack/TrackCollection.h"

// ATLAS headers
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/IInterface.h"


//==================================================================================
// Public Methods
//==================================================================================
IDPerfMuonRefitter::IDPerfMuonRefitter(const std::string& name,
  ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_TrackRefitter1(""),
  m_TrackRefitter2(""),
  m_N_Muons(0),
  m_N_MuonsRefit(0),
  m_N_MuonRefitFailures(0),
  m_container(PerfMonServices::MUON_COLLECTION)
{
  // Properties that are set from the python scripts.
  declareProperty("OutputTracksName", m_outputTracksName = "IDMuonTracks");
  declareProperty("ReFitterTool1",    m_TrackRefitter1, "ToolHandle for track fitter implementation");
  declareProperty("ReFitterTool2",    m_TrackRefitter2, "ToolHandle for track fitter implementation");

}


IDPerfMuonRefitter::~IDPerfMuonRefitter()
{}


StatusCode IDPerfMuonRefitter::initialize ATLAS_NOT_THREAD_SAFE () // Thread unsafe PerfMonServices class is used.
{
  // Setup the services
  ISvcLocator* pxServiceLocator = serviceLocator();
  if ( pxServiceLocator ) {
    StatusCode xSC = PerfMonServices::InitialiseServices( pxServiceLocator );
    if ( !xSC.isSuccess() )
    {
      ATH_MSG_FATAL("Problem Initializing PerfMonServices");
    }
  }
  // Retrieve fitter
  if (m_TrackRefitter1.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_TrackRefitter1 );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool" << m_TrackRefitter1 );
  }
  // Retrieve the second fitter
  if (m_TrackRefitter2.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_TrackRefitter2 );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool" << m_TrackRefitter2 );
  }
  return StatusCode::SUCCESS;
}




StatusCode IDPerfMuonRefitter::execute ATLAS_NOT_THREAD_SAFE () // Thread unsafe PerfMonServices class is used.
{
  const xAOD::MuonContainer* pxMuonContainer = PerfMonServices::getContainer<xAOD::MuonContainer>( m_container );
  if (!pxMuonContainer){
    ATH_MSG_FATAL("Unable to retrieve the muon collection"  );
    return StatusCode::FAILURE;
  }
  TrackCollection* muonTrks  = new TrackCollection(SG::OWN_ELEMENTS);
  TrackCollection* muonTrksRefit1  = new TrackCollection(SG::OWN_ELEMENTS);
  TrackCollection* muonTrksRefit2  = new TrackCollection(SG::OWN_ELEMENTS);
  for (const auto & muon : *pxMuonContainer){
    if (!muon) {
      ATH_MSG_WARNING("CB Muons missing!");
      continue;
    }
	  ++m_N_Muons;
    const xAOD::TrackParticle* idTP =   muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    if (!idTP) {
       ATH_MSG_DEBUG("ID TrackParticles missing!  Skipping Muon");
       continue;
    }
    Trk::Track* defaultMuonTrk{};
    Trk::Track* refit1MuonTrk{};
    Trk::Track* refit2MuonTrk{};
    const xAOD::Electron* eg{};
    StatusCode fitStatus;
    //save default and refit track parameters
    if( idTP->track() ) {
      defaultMuonTrk = new Trk::Track(*idTP->track());
      //save tracks to storegrate	/
      muonTrks->push_back(defaultMuonTrk);
      IegammaTrkRefitterTool::Cache cache1{}; 
      cache1.electron=eg; 
      fitStatus = m_TrackRefitter1->refitTrack( Gaudi::Hive::currentContext(),idTP->track(), cache1 );
      ++m_N_MuonsRefit;
      if (fitStatus == StatusCode::SUCCESS) {
        refit1MuonTrk = cache1.refittedTrack.release();
        muonTrksRefit1->push_back(refit1MuonTrk);
      } else {
         ATH_MSG_DEBUG("Track Refit1 Failed. Skipping Muon");
         ++m_N_MuonRefitFailures;
         continue;
      }
      IegammaTrkRefitterTool::Cache cache2{}; 
      cache2.electron=eg; 
      fitStatus = m_TrackRefitter2->refitTrack(Gaudi::Hive::currentContext(),idTP->track(), cache2 );
      if (fitStatus == StatusCode::SUCCESS) {
        refit2MuonTrk = cache2.refittedTrack.release();
        muonTrksRefit2->push_back(refit2MuonTrk);
      } else {
        ATH_MSG_DEBUG("Track Refit2 Failed. Skipping Muon");
        continue;
      }
    }
  }// End loop over muons
  //Store information into storegate
  StatusCode sc = evtStore()->record(muonTrks, m_outputTracksName, false);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Failed storing " << m_outputTracksName);
  } else{
    ATH_MSG_DEBUG( "Stored "<< muonTrks->size() << " " << m_outputTracksName <<" into StoreGate" );
  }
  sc = evtStore()->record(muonTrksRefit1, m_outputTracksName + "Refit1", false);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Failed storing " << m_outputTracksName + "Refit1" );
  } else {
		ATH_MSG_DEBUG( "Stored "<< muonTrksRefit1->size() << " " << m_outputTracksName  + "Refit1" <<" into StoreGate");
	}
  sc = evtStore()->record(muonTrksRefit2, m_outputTracksName + "Refit2", false);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Failed storing " << m_outputTracksName +"Refit2" );
  } else {
    ATH_MSG_DEBUG( "Stored "<< muonTrksRefit2->size() << " " << m_outputTracksName + "Refit2" <<" into StoreGate" );
  }
  return StatusCode::SUCCESS;
}


StatusCode IDPerfMuonRefitter::finalize()
{
  ATH_MSG_INFO("***************************************************");
  ATH_MSG_INFO("**************** IDPerfMuonRefitter ***************");
  ATH_MSG_INFO("***************************************************");
  ATH_MSG_INFO(m_N_Muons << "\t\t Muons inspected" );
  ATH_MSG_INFO(m_N_MuonsRefit << "\t\t Muons refit" );
  ATH_MSG_INFO(m_N_MuonRefitFailures << "\t\t Muons refit failures" );
  ATH_MSG_INFO("***************************************************");
  ATH_MSG_INFO("***************************************************");
  return StatusCode::SUCCESS;
}
