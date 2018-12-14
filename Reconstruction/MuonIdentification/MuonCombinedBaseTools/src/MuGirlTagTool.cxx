/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuGirlTagTool
//  AlgTool performing in-out reconstruction using the MuGirl package.
//  A MuGirlTag is added to the InDetCandidate object.
// 
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuGirlTagTool.h"

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  MuGirlTagTool::MuGirlTagTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
        m_pMuGirlReconstruction("MuGirlNS::MuGirlRecoTool/MuGirlRecoTagTool",0)
  {
    declareInterface<IMuonCombinedInDetExtensionTool>(this);

    // Tools
    declareProperty("MuGirlReconstruction", m_pMuGirlReconstruction);
  }

  MuGirlTagTool::~MuGirlTagTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode MuGirlTagTool::initialize() {
    ATH_MSG_INFO( "Initializing MuGirlTagTool - package version " << PACKAGE_VERSION );

    if (m_pMuGirlReconstruction.retrieve().isFailure()) {
        ATH_MSG_ERROR("Could not retrieve the MuGirlreco Tool");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode MuGirlTagTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void MuGirlTagTool::extend( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, TrackCollection* combTracks, TrackCollection* meTracks, 
			      Trk::SegmentCollection* segments ) {
    if(!tagMap || !combTracks || !meTracks || !segments) ATH_MSG_DEBUG("We don't run this tool any more anyway");
    if (m_pMuGirlReconstruction->MuGirlReco(inDetCandidates).isFailure()) {
      ATH_MSG_ERROR("MuGirl reconstruction failed!");
    }
  }

  void MuGirlTagTool::extendWithPRDs( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, IMuonCombinedInDetExtensionTool::MuonPrdData prdData,
				      TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments) {
    //This is just a placeholder to ensure compilation until this tool is removed from the release: it is already deprecated
    if(prdData.mdtPrds && tagMap && combTracks && meTracks && segments){
      if (m_pMuGirlReconstruction->MuGirlReco(inDetCandidates).isFailure()) {
	ATH_MSG_ERROR("MuGirl reconstruction failed!");
      }
    }
  }

}	// end of namespace
