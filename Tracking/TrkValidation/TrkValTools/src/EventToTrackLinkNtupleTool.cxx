/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// EventToTrackLinkNtupleTool.cxx
//   Source file for class EventToTrackLinkNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////

#include "TTree.h"
//Gaudi
// #include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ITHistSvc.h"
// #include "GaudiKernel/ISvcLocator.h"
// #include "GaudiKernel/PropertyMgr.h"
#include "TrkValTools/EventToTrackLinkNtupleTool.h"
#include "CommissionEvent/ComTime.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

  const std::string Trk::EventToTrackLinkNtupleTool::trackTypeName = "Trk::Track";
  const std::string Trk::EventToTrackLinkNtupleTool::trkParticleTypeName = "Rec::TrackParticle";
  const std::string Trk::EventToTrackLinkNtupleTool::trkParticleTrigTypeName= "Rec::TrackParticle_Trig";
  const std::string Trk::EventToTrackLinkNtupleTool::inDetTrackTrigTypeName= "InDetTrack_Trig";

// constructor
Trk::EventToTrackLinkNtupleTool::EventToTrackLinkNtupleTool(
   const std::string& t,
    const std::string& n,
    const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_collectionType(trackTypeName),
  m_eventLinkTree(0),
  m_trackIndexBegin(std::vector<int>()),
  m_nTracksPerEvent(std::vector<int>()),
  m_genParticleIndexBegin(0),
  m_nGenParticlesPerEvent(0)
{
  declareInterface<IEventPropertyNtupleTool>(this);
  declareProperty("CollectionType",m_collectionType, "Type of collection which associated with this instance ["
		  +trackTypeName+", "+trkParticleTypeName+", "+trkParticleTrigTypeName+", "+inDetTrackTrigTypeName+"]");
}

// destructor
Trk::EventToTrackLinkNtupleTool::~EventToTrackLinkNtupleTool() {}

StatusCode Trk::EventToTrackLinkNtupleTool::initialize() {
  if(checkCollectionType().isFailure()) return StatusCode::FAILURE;
  ATH_MSG_DEBUG ("initialise successful in "<<name());
  return AthAlgTool::initialize();
}

StatusCode Trk::EventToTrackLinkNtupleTool::finalize() {
  ATH_MSG_DEBUG ("finalize successfull in "<<name());
  return StatusCode::SUCCESS;
}

void Trk::EventToTrackLinkNtupleTool::registerTrackCollections
( std::vector<std::string> collections, bool doTruth) const {
  m_trackCollections = collections;
  m_doTruth = doTruth;
  m_trackIndexBegin.resize(m_trackCollections.size());
  m_nTracksPerEvent.resize(m_trackCollections.size());
}

StatusCode Trk::EventToTrackLinkNtupleTool::addNtupleItems( TTree* tree, const std::string cbnt_prefix ) const {
  if (!tree) return StatusCode::FAILURE;
  //-----------------
  // add items
  m_eventLinkTree = tree;
  for (unsigned int tColIndex = 0; tColIndex<m_trackCollections.size(); tColIndex++) {
    m_eventLinkTree->Branch((cbnt_prefix+m_trackCollections[tColIndex]+"IndexBegin").c_str(),
                            &(m_trackIndexBegin.at(tColIndex)) );
    m_eventLinkTree->Branch((cbnt_prefix+"n"+m_trackCollections[tColIndex]+"PerEvent").c_str(),
			    &(m_nTracksPerEvent.at(tColIndex)) );
  }
  if ( m_doTruth ){
    m_eventLinkTree->Branch((cbnt_prefix+"GenParticlesIndexBegin").c_str(),   &m_genParticleIndexBegin );
    m_eventLinkTree->Branch((cbnt_prefix+"nGenParticlesPerEvent").c_str(),    &m_nGenParticlesPerEvent );
    }
  return StatusCode::SUCCESS;
}

void Trk::EventToTrackLinkNtupleTool::setTrackTreeIndices
( unsigned int tColIndex, 
  int indexBegin,
  int nTracksPerEvent ) const {
  ATH_MSG_DEBUG ("start setTrackTreeIndices");
  ATH_MSG_VERBOSE ("Collection index "<<tColIndex<<"\tm_trackIndexBegin\t"<< m_trackIndexBegin.size() );
  m_trackIndexBegin[tColIndex] = indexBegin;
  m_nTracksPerEvent[tColIndex] = nTracksPerEvent;
  if (m_nTracksPerEvent[tColIndex]==0) m_trackIndexBegin[tColIndex]=0;
}

void Trk::EventToTrackLinkNtupleTool::setGenParticleTreeIndices
( int indexBegin,
  int nGenParticlesPerEvent) const {
  m_genParticleIndexBegin = indexBegin;
  m_nGenParticlesPerEvent = nGenParticlesPerEvent;
}

StatusCode Trk::EventToTrackLinkNtupleTool::fillEventData( ) const {
  return StatusCode::SUCCESS;
}

StatusCode Trk::EventToTrackLinkNtupleTool::resetVariables( ) const {
  return StatusCode::SUCCESS;
}

StatusCode Trk::EventToTrackLinkNtupleTool::checkCollectionType() const {
  if(( m_collectionType != trackTypeName) && (m_collectionType != trkParticleTypeName) && (m_collectionType != trkParticleTrigTypeName) && (m_collectionType != inDetTrackTrigTypeName)) {
                    ATH_MSG_ERROR ("Recieved collection type: "<< m_collectionType 
				   <<"does not correspond to any of following types:"<< trackTypeName 
				   <<", "<<trkParticleTypeName <<", "<<trkParticleTrigTypeName
				   <<", "<<inDetTrackTrigTypeName);
				   
		    return StatusCode::FAILURE;
                                                                                                      }
  return StatusCode::SUCCESS;
}
