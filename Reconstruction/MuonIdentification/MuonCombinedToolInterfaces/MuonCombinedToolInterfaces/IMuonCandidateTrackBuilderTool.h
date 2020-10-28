/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONCANDIDATETRACKBUILDERTOOL
#define MUON_IMUONCANDIDATETRACKBUILDERTOOL

#include "GaudiKernel/IAlgTool.h"
#include <vector>

#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonLayerEvent/MuonCandidate.h"

static const InterfaceID IID_IMuonCandidateTrackBuilderTool("Muon::IMuonCandidateTrackBuilderTool",1,0);

namespace Trk{
  class Track;
}

namespace Muon {

  /** Interface for a tool to build tracks out of a MuonCandidate  */
  class IMuonCandidateTrackBuilderTool : virtual public IAlgTool {     
  public:
    /** IAlgTool interface */
    static const InterfaceID& interfaceID();

    /** @brief build a track out of a MuonCandidate */
    virtual Trk::Track* buildCombinedTrack( const Trk::Track& idTrack, const MuonCandidate& candidate ) const = 0;

    void cleanUp() const {};

  };
 
  inline const InterfaceID& IMuonCandidateTrackBuilderTool::interfaceID()
  {
    return IID_IMuonCandidateTrackBuilderTool;
  }


} // end of name space

#endif // IMuonCandidateTrackBuilderTool_H
