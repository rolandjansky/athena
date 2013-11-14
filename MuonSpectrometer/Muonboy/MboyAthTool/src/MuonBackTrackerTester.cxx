/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"
#include "Particle/TrackParticleContainer.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MuonBackTrackerTester.h"

MuonBackTrackerTester::MuonBackTrackerTester(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
, p_OwnEDMHelper                  ( "MboyAthToolHelper" )
, p_IMuonBackTracker ( "Muon::MuonBackTracker" ) 
{

  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;
  declareProperty("MuonBackTracker" , p_IMuonBackTracker ) ;

//Declare the properties

  declareProperty( "TrackCollectionLocationIN"                 , m_TrackCollectionLocationIN           = "ConvertedMBoyMuonSpectroOnlyTracks"                ) ;
  declareProperty( "TrackCollectionLocationOUT"                , m_TrackCollectionLocationOUT          = "MuonBackTrackerTester.MuonboyTrkTrack"                ) ;
  declareProperty( "TrackParticleCollectionLocationOUT"        , m_TrackParticleCollectionLocationOUT  = "MuonBackTrackerTester.MuonboyTrackParticles"                ) ;

  declareProperty( "MboyDumpMboyBackTrackerOutputFile"      , m_MboyDumpMboyBackTrackerOutputFile      = "Out.MuonBackTrackerTester"        ) ;      

}
MuonBackTrackerTester::~MuonBackTrackerTester(){}


// Initialize
StatusCode MuonBackTrackerTester::initialize(){

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are        " );
  ATH_MSG_INFO( "= TrackCollectionLocationIN                  " << m_TrackCollectionLocationIN          );
  ATH_MSG_INFO( "= TrackCollectionLocationOUT                 " << m_TrackCollectionLocationOUT          );
  ATH_MSG_INFO( "= TrackParticleCollectionLocationOUT         " << m_TrackParticleCollectionLocationOUT          );
  ATH_MSG_INFO( "= MboyDumpMboyBackTrackerOutputFile          " << m_MboyDumpMboyBackTrackerOutputFile           );
  ATH_MSG_INFO( "================================" );


//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper ) ;

//Retrieve p_IMuonBackTracker
  if ( p_IMuonBackTracker.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonBackTracker );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonBackTracker );
  

//Set files for dumps for checks
  m_OutFile_MboyDumpMboyBackTrackerOutputFile.open(m_MboyDumpMboyBackTrackerOutputFile.c_str());
  m_OutFile_MboyDumpMboyBackTrackerOutputFile << setiosflags(std::ios::fixed);

  ATH_MSG_INFO( "Initialisation ended     " );


  return StatusCode::SUCCESS;

}

// Execute
StatusCode MuonBackTrackerTester::execute(){

  StatusCode sc = StatusCode::SUCCESS;

//Check existence of input collection
  const TrackCollection* pTrackCollectionIN ;
  sc = evtStore()->retrieve(pTrackCollectionIN, m_TrackCollectionLocationIN); 
  if (sc.isFailure()){
    ATH_MSG_ERROR( "pTrackCollectionIN not found at" << m_TrackCollectionLocationIN ) ;
    return( StatusCode::FAILURE );
  }

//Output TrackCollection
  TrackCollection* pTrackCollectionOUT        = new TrackCollection;

//BackTrack Tracks
  for (TrackCollection::const_iterator it = pTrackCollectionIN->begin(); it!=pTrackCollectionIN->end(); ++it){
    const Trk::Track* pTrack = (*it);
    Trk::Track* pTrackOUT = p_IMuonBackTracker->MuonBackTrack(pTrack) ;
    if (pTrackOUT) pTrackCollectionOUT->push_back(pTrackOUT);
  }

//Store Output TrackCollection
  sc = evtStore()->record(pTrackCollectionOUT, m_TrackCollectionLocationOUT);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not save pTrackCollectionOUT at " << m_TrackCollectionLocationOUT );
    return( StatusCode::FAILURE);
  }

//TrackParticleContainer
  Rec::TrackParticleContainer* pTrackParticleContainerOUT        = new Rec::TrackParticleContainer;
  sc = p_OwnEDMHelper->GetParticleCollection( pTrackCollectionOUT, pTrackParticleContainerOUT );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "GetParticleCollection failed for pTrackParticleContainerOUT" );
    return( StatusCode::FAILURE);
  }

  sc = evtStore()->record(pTrackParticleContainerOUT, m_TrackParticleCollectionLocationOUT);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not save pTrackParticleContainerOUT at " << m_TrackParticleCollectionLocationOUT );
    return( StatusCode::FAILURE);
  }

//Containers dump 
  DumpContainers();

  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MuonBackTrackerTester::finalize(){return StatusCode::SUCCESS;}

void MuonBackTrackerTester::DumpContainers(){

  StatusCode sc = p_OwnEDMHelper->MboyDumpTrackCollectionToolDoDump(
                                               m_TrackCollectionLocationOUT,
                                               &m_OutFile_MboyDumpMboyBackTrackerOutputFile
                                             );
  if ( sc.isFailure() ) ATH_MSG_WARNING( "DoDump failed " );

}
