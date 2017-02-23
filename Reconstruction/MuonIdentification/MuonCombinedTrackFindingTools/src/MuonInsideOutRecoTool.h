/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONINSIDEOUTRECOTOOL_H
#define MUON_MUONINSIDEOUTRECOTOOL_H

#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "MuonLayerEvent/MuonLayerRecoData.h"

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace Muon {
  struct MuonCandidate;
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
  class Track;
  class ITrackAmbiguityProcessorTool;
}

static const InterfaceID IID_MuonInsideOutRecoTool("MuonCombined::MuonInsideOutRecoTool",1,0);

namespace MuonCombined { 

  class MuonInsideOutRecoTool : virtual public MuonCombined::IMuonCombinedInDetExtensionTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonInsideOutRecoTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonInsideOutRecoTool();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonInsideOutRecoTool; }

    /**IMuonCombinedInDetExtensionTool interface: extend ID candidate */   
    virtual void extend( const InDetCandidateCollection& inDetCandidates ) override;

    /** find the best candidate for a given set of segments */
    std::pair<std::unique_ptr<const Muon::MuonCandidate>,std::unique_ptr<const Trk::Track> > 
    findBestCandidate( const xAOD::TrackParticle& indetTrackParticle, const std::vector< Muon::MuonLayerRecoData >& allLayers);

  private:
    /** handle a single candidate */
    void handleCandidate( const InDetCandidate& inDetCandidate );

    /** add muon candidate to indet candidate */
    void addTag( const InDetCandidate& indetCandidate, const Muon::MuonCandidate& candidate, 
                 std::unique_ptr<const Trk::Track>& selectedTrack ) const;


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
    /** id pt cut */
    double m_idTrackMinPt;
    bool m_ignoreSiAssocated;
  };
}



#endif
 


