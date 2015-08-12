/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONINSIDEOUTRECOTOOL_H
#define MUON_MUONINSIDEOUTRECOTOOL_H

#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace Muon {

  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
  class IMuonSystemExtensionTool;
  class IMuonLayerSegmentFinderTool;
  class IMuonLayerSegmentMatchingTool;
  class IMuonLayerAmbiguitySolverTool;
  class IMuonCandidateTrackBuilderTool;
  class IMuonRecoValidationTool;
  class ITrackAmbiguityProcessorTool;
}

namespace Rec {
  class ICombinedMuonTrackBuilder;
}

namespace Trk {
  class ITrackAmbiguityProcessorTool;
}

namespace MuonCombined { 

  class MuonInsideOutRecoTool : virtual public MuonCombined::IMuonCombinedInDetExtensionTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonInsideOutRecoTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonInsideOutRecoTool();
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonCombinedInDetExtensionTool interface: extend ID candidate */   
    void extend( const InDetCandidateCollection& inDetCandidates ) const;

  private:
    /** handle a single candidate */
    void handleCandidate( const InDetCandidate& inDetCandidate ) const;

    /** tool handles */
    ToolHandle<Muon::MuonIdHelperTool>               m_idHelper; 
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer; 
    ToolHandle<Muon::IMuonSystemExtensionTool>       m_muonSystemExtentionTool;
    ToolHandle<Muon::IMuonLayerSegmentFinderTool>    m_segmentFinder;
    ToolHandle<Muon::IMuonLayerSegmentMatchingTool>  m_segmentMatchingTool;
    ToolHandle<Muon::IMuonLayerAmbiguitySolverTool>  m_ambiguityResolver;
    ToolHandle<Muon::IMuonCandidateTrackBuilderTool> m_candidateTrackBuilder;
    ToolHandle<Muon::IMuonRecoValidationTool>        m_recoValidationTool;
    ToolHandle<Rec::ICombinedMuonTrackBuilder>       m_trackFitter;
    ToolHandle<Trk::ITrackAmbiguityProcessorTool>    m_trackAmbibuityResolver;
    
  };
}



#endif
 


