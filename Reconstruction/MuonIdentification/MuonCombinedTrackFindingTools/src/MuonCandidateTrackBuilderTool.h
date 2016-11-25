/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONCANDIDATETRACKBUILDER_H
#define MUON_MUONCANDIDATETRACKBUILDER_H

#include "MuonCombinedToolInterfaces/IMuonCandidateTrackBuilderTool.h"

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Rec {
  class ICombinedMuonTrackBuilder;
}

namespace Muon {

  class IMuonSegmentTrackBuilder;
  class MuonEDMPrinterTool;
  class MuonIdHelperTool;
  struct MuonCandidate;

  class MuonCandidateTrackBuilderTool : virtual public IMuonCandidateTrackBuilderTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonCandidateTrackBuilderTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonCandidateTrackBuilderTool();
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonCandidateTrackBuilderTool interface: buildCombinedTrack */   
    const Trk::Track* buildCombinedTrack( const Trk::Track& idTrack, const MuonCandidate& candidate ) const;

  private:

    ToolHandle<IMuonSegmentTrackBuilder>       m_muonTrackBuilder; 
    ToolHandle<MuonEDMPrinterTool>             m_printer;
    ToolHandle<MuonIdHelperTool>               m_idHelper;
    bool                                       m_reOrderMeasurements;
    ToolHandle<Rec::ICombinedMuonTrackBuilder> m_trackFitter;
  };
}



#endif
 

