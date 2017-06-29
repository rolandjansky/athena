/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FTK_RecTools/FTK_DuplicateTrackRemovalTool.h"
#include <map>
#include <vector>
#include <utility>

FTK_DuplicateTrackRemovalTool::FTK_DuplicateTrackRemovalTool(const std::string& t,
                                               const std::string& n,
                                               const IInterface*  p ):
  AthAlgTool(t,n,p),
  m_trks_nodups(NULL),
  m_uncertaintyTool("FTK_UncertaintyTool",this)
{
  declareInterface< IFTK_DuplicateTrackRemovalTool >( this );
  declareProperty( "UncertaintyTool", m_uncertaintyTool);
}

StatusCode FTK_DuplicateTrackRemovalTool::initialize() {

  StatusCode sc = AlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  m_trks_nodups = new FTK_RawTrackContainer(SG::VIEW_ELEMENTS);//we don't own the tracks, we're just going to hold them

  athenaLog << MSG::INFO << "FTK_DuplicateTrackRemovalTool initialized "<< endmsg;
  return sc;
}

StatusCode FTK_DuplicateTrackRemovalTool::finalize() {
  StatusCode sc = AlgTool::finalize();
  m_trks_nodups->clear();
  delete m_trks_nodups;
  return sc;
}

bool FTK_DuplicateTrackRemovalTool::match(const FTK_RawTrack* track, const FTK_RawTrack* oldtrack) const {
	return true; // TODO
}

FTK_RawTrackContainer* FTK_DuplicateTrackRemovalTool::removeDuplicates(const FTK_RawTrackContainer* trks){
  ATH_MSG_INFO("ACH99 - I'm in removeDuplicates!");
  m_trks_nodups->clear();
  m_trks_nodups->reserve(trks->size());
  for (unsigned int i = 0; i!=trks->size(); i++) {
	  const FTK_RawTrack *track = trks->at(i);

	  //now we should see whether this track overlaps with one (or more?) tracks already in the nodups container
	  std::vector<int> matching_oldtracks;
	  for (unsigned int e = 0; e!=m_trks_nodups->size(); e++) {
	  	  const FTK_RawTrack *oldtrack = m_trks_nodups->at(e);
	  	  if (match(track,oldtrack)) {
	  		  matching_oldtracks.push_back(e);
	  	  }
	  }

	  //if it does, either replace the (worst?) matching track with this new track, or ignore this new track, depending on which track we like better

	  m_trks_nodups->push_back((FTK_RawTrack*)track);
  }

  //maybe at the end we should do a check that no further matches exist?

  return m_trks_nodups;
}
