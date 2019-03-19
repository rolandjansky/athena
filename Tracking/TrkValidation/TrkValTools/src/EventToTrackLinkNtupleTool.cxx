/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ITHistSvc.h"
#include "TrkValTools/EventToTrackLinkNtupleTool.h"
#include "CommissionEvent/ComTime.h"

  const std::string Trk::EventToTrackLinkNtupleTool::s_trackTypeName = "Trk::Track";
  const std::string Trk::EventToTrackLinkNtupleTool::s_trkParticleTypeName = "Rec::TrackParticle";
  const std::string Trk::EventToTrackLinkNtupleTool::s_trkParticleTrigTypeName= "Rec::TrackParticle_Trig";
  const std::string Trk::EventToTrackLinkNtupleTool::s_inDetTrackTrigTypeName= "InDetTrack_Trig";

// constructor
Trk::EventToTrackLinkNtupleTool::EventToTrackLinkNtupleTool(
   const std::string& t,
    const std::string& n,
    const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_collectionType(s_trackTypeName),
  m_eventLinkTree(nullptr),
  m_doTruth{},
  m_trackIndexBegin(std::vector<int>()),
  m_nTracksPerEvent(std::vector<int>()),
  m_genParticleIndexBegin(0),
  m_nGenParticlesPerEvent(0)
{
  declareInterface<IEventPropertyNtupleTool>(this);
  declareProperty("CollectionType",m_collectionType, "Type of collection which associated with this instance ["
		  +s_trackTypeName+", "+s_trkParticleTypeName+", "+s_trkParticleTrigTypeName+", "+s_inDetTrackTrigTypeName+"]");
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
  if(( m_collectionType != s_trackTypeName) && (m_collectionType != s_trkParticleTypeName) && (m_collectionType != s_trkParticleTrigTypeName) && (m_collectionType != s_inDetTrackTrigTypeName)) {
                    ATH_MSG_ERROR ("Recieved collection type: "<< m_collectionType 
				   <<"does not correspond to any of following types:"<< s_trackTypeName 
				   <<", "<<s_trkParticleTypeName <<", "<<s_trkParticleTrigTypeName
				   <<", "<<s_inDetTrackTrigTypeName);
				   
		    return StatusCode::FAILURE;
                                                                                                      }
  return StatusCode::SUCCESS;
}
