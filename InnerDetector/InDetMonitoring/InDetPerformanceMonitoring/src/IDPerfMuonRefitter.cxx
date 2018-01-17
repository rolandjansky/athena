/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//==================================================================================

//==================================================================================
// Include files...
//==================================================================================

// This files header
#include "InDetPerformanceMonitoring/IDPerfMuonRefitter.h"

// Standard headers
#include <string>
#include "TTree.h"

// Package Headers
#include "InDetPerformanceMonitoring/PerfMonServices.h"
#include "InDetPerformanceMonitoring/ZmumuEvent.h"

//Interface Headers
#include "egammaInterfaces/IegammaTrkRefitterTool.h"
#include "TrkTrack/TrackCollection.h"

// ATLAS headers
#include "GaudiKernel/IInterface.h"
#include "StoreGate/StoreGateSvc.h"




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



StatusCode IDPerfMuonRefitter::initialize()
{


  // Setup the services
  ISvcLocator* pxServiceLocator = serviceLocator();

  if ( pxServiceLocator != NULL ) {
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




StatusCode IDPerfMuonRefitter::execute()
{

  const xAOD::MuonContainer* pxMuonContainer = PerfMonServices::getContainer<xAOD::MuonContainer>( m_container );
  if (!pxMuonContainer){
    ATH_MSG_FATAL("Unable to retrieve the muon collection"  );
    return StatusCode::FAILURE;
  }


  TrackCollection* muonTrks  = new TrackCollection(SG::OWN_ELEMENTS);
  TrackCollection* muonTrksRefit1  = new TrackCollection(SG::OWN_ELEMENTS);
  TrackCollection* muonTrksRefit2  = new TrackCollection(SG::OWN_ELEMENTS);



  xAOD::MuonContainer::const_iterator xMuonItr  = pxMuonContainer->begin();
  for ( ; xMuonItr != pxMuonContainer->end(); xMuonItr++ )
	{
	  const xAOD::Muon* muon = *xMuonItr;

    if (!muon) {
      ATH_MSG_WARNING("CB Muons missing!");
      continue;
    }
	  ++m_N_Muons;



    //const std::string region = m_xZmm.getRegion();
    const xAOD::TrackParticle* idTP =   muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);

    if (!idTP) {
       ATH_MSG_DEBUG("ID TrackParticles missing!  Skipping Muon");
       continue;
    }


    Trk::Track* defaultMuonTrk = 0;
    Trk::Track* refit1MuonTrk = 0;
    Trk::Track* refit2MuonTrk = 0;

    const xAOD::Electron* eg = 0;


    StatusCode fitStatus;
    //save default and refit track parameters
    if( idTP->track() ) {
      defaultMuonTrk = new Trk::Track(*idTP->track());
      //save tracks to storegrate	/
      muonTrks->push_back(defaultMuonTrk);

      fitStatus = m_TrackRefitter1->refitTrack( idTP->track(), eg );
      ++m_N_MuonsRefit;
      if (fitStatus == StatusCode::SUCCESS) {
        refit1MuonTrk = m_TrackRefitter1->refittedTrack();
        muonTrksRefit1->push_back(refit1MuonTrk);
      } else {
         ATH_MSG_DEBUG("Track Refit1 Failed. Skipping Muon");
         ++m_N_MuonRefitFailures;
         continue;
      }

      fitStatus = m_TrackRefitter2->refitTrack( idTP->track(), eg );
      if (fitStatus == StatusCode::SUCCESS) {
        refit2MuonTrk = m_TrackRefitter2->refittedTrack();
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
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed storing " << m_outputTracksName << endmsg;
  }
  else{
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stored "<< muonTrks->size() << " " << m_outputTracksName <<" into StoreGate" << endmsg;
  }

  sc = evtStore()->record(muonTrksRefit1, m_outputTracksName + "Refit1", false);
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed storing " << m_outputTracksName + "Refit1" << endmsg;
  } else {
		if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stored "<< muonTrksRefit1->size() << " " << m_outputTracksName  + "Refit1" <<" into StoreGate" << endmsg;
	}

  sc = evtStore()->record(muonTrksRefit2, m_outputTracksName + "Refit2", false);
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed storing " << m_outputTracksName +"Refit2" << endmsg;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stored "<< muonTrksRefit2->size() << " " << m_outputTracksName + "Refit2" <<" into StoreGate" << endmsg;
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
