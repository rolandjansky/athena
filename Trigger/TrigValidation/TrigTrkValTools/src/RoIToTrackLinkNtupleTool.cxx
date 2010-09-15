/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// RoIToTrackLinkNtupleTool.cxx
//   Source file for class RoIToTrackLinkNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Pavel.Ruzicka -at- cern.ch
///////////////////////////////////////////////////////////////////

#include "TTree.h"
//Gaudi

#include "GaudiKernel/ITHistSvc.h"
#include "TrigTrkValTools/RoIToTrackLinkNtupleTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

const std::string Trk::RoIToTrackLinkNtupleTool::trkParticleTool = "Rec::TrackParticle";
const std::string Trk::RoIToTrackLinkNtupleTool::trigInDetTrackTool= "TrigInDetTrack";

// constructor
Trk::RoIToTrackLinkNtupleTool::RoIToTrackLinkNtupleTool(
   const std::string& t,
    const std::string& n,
    const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_eventLinkTree(0),
  m_trackIndexBegin(std::vector<int>()),
  m_nTracksPerRoI(std::vector<int>())
{
  declareInterface<IRoIPropertyNtupleTool>(this);
  declareProperty("trackToolType",  m_trackToolType = trkParticleTool, "Tool which links following containers:"+trkParticleTool+", "+trigInDetTrackTool);
}

// destructor
Trk::RoIToTrackLinkNtupleTool::~RoIToTrackLinkNtupleTool() {}

StatusCode Trk::RoIToTrackLinkNtupleTool::initialize() {
  if(checkToolType().isFailure()) {
    ATH_MSG_FATAL ("Invalid value of trackToolType: "<< m_trackToolType<<"does not match to any valid values: " <<trkParticleTool<<", "<<trigInDetTrackTool);
    return StatusCode::FAILURE;
                                  }

  ATH_MSG_DEBUG ("initialise successful in "<<name());
  return AthAlgTool::initialize();
}

StatusCode Trk::RoIToTrackLinkNtupleTool::finalize() {
  ATH_MSG_DEBUG ("finalize successfull in "<<name());
  return StatusCode::SUCCESS;
}

void Trk::RoIToTrackLinkNtupleTool::registerTrackCollections
( std::vector<std::string> collections) const {
  m_trackCollections = collections;
  m_trackIndexBegin.resize(m_trackCollections.size());
  m_nTracksPerRoI.resize(m_trackCollections.size());
}

StatusCode Trk::RoIToTrackLinkNtupleTool::addNtupleItems( TTree* tree, const std::string cbnt_prefix ) const {
  if (!tree) return StatusCode::FAILURE;
  //-----------------
  // add items
  m_eventLinkTree = tree;
  for (unsigned int tColIndex = 0; tColIndex<m_trackCollections.size(); tColIndex++) {
    m_eventLinkTree->Branch((cbnt_prefix+m_trackCollections[tColIndex]+"IndexBegin").c_str(),
                            &(m_trackIndexBegin.at(tColIndex)),
                            (m_trackCollections[tColIndex]+" IndexBegin/I").c_str());
    m_eventLinkTree->Branch((cbnt_prefix+"n"+m_trackCollections[tColIndex]+"PerRoI").c_str(),
                              &(m_nTracksPerRoI.at(tColIndex)),
                              (m_trackCollections[tColIndex]+"PerRoI/b").c_str());
  }
  
  return StatusCode::SUCCESS;
}

void Trk::RoIToTrackLinkNtupleTool::setTrackTreeIndices
( unsigned int tColIndex, 
  int indexBegin,
  int nTracksPerRoI ) const {
  m_trackIndexBegin[tColIndex] = indexBegin;
  m_nTracksPerRoI[tColIndex] = nTracksPerRoI;
  if (m_nTracksPerRoI[tColIndex]==0) m_trackIndexBegin[tColIndex]=0;
  ATH_MSG_VERBOSE ("Track Index Begin: "<<m_trackIndexBegin[tColIndex]<<" Number of Tracks per RoI "<<m_nTracksPerRoI[tColIndex] );
}

StatusCode Trk::RoIToTrackLinkNtupleTool::fillRoIData( const TrigRoiDescriptor* , bool ) const {
  return StatusCode::SUCCESS;
}

StatusCode Trk::RoIToTrackLinkNtupleTool::resetVariables( ) const {
 for(unsigned int tColIndex = 0; tColIndex < m_trackIndexBegin.size(); tColIndex++) {
    m_trackIndexBegin[tColIndex] = 0;
    m_nTracksPerRoI[tColIndex]   = 0;
  }

  return StatusCode::SUCCESS;
}

StatusCode Trk::RoIToTrackLinkNtupleTool::checkToolType() const {
  if( (m_trackToolType != trkParticleTool) && (m_trackToolType != trigInDetTrackTool) ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

inline bool Trk::RoIToTrackLinkNtupleTool::isTrkParticleLinkRoI() const{
  if (m_trackToolType == trkParticleTool) return true;
  return false;
}

inline bool Trk::RoIToTrackLinkNtupleTool::isTrigInDetTrackLinkRoI() const{
  if (m_trackToolType == trigInDetTrackTool) return true;
  return false;
}
