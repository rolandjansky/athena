/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONINSIDEOUTRECOTOOL_H
#define MUON_MUONINSIDEOUTRECOTOOL_H

#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonPrepRawDataProviderTools/MuonLayerHashProviderTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace Muon {
  struct MuonCandidate;
  struct MuonLayerPrepRawData;
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

    /**IMuonCombinedInDetExtensionTool interface: extend ID candidate with PRDs for segment-finding */   
    virtual void extendWithPRDs( const InDetCandidateCollection& inDetCandidates, const Muon::MdtPrepDataContainer* mdtPRDs, const Muon::CscPrepDataContainer* cscPRDs,
				 const Muon::RpcPrepDataContainer* rpdPRDs, const Muon::TgcPrepDataContainer* tgcPRDs, const Muon::sTgcPrepDataContainer* stgcPRDs,
				 const Muon::MMPrepDataContainer* mmPRDs) override;

    /**IMuonCombinedInDetExtensionTool interface: deprecated*/
    virtual void extend(const InDetCandidateCollection& inDetCandidates) override;

    /** find the best candidate for a given set of segments */
    std::pair<std::unique_ptr<const Muon::MuonCandidate>,std::unique_ptr<const Trk::Track> > 
    findBestCandidate( const xAOD::TrackParticle& indetTrackParticle, const std::vector< Muon::MuonLayerRecoData >& allLayers);

  private:
    /** handle a single candidate */
    void handleCandidate( const InDetCandidate& inDetCandidate, const Muon::MdtPrepDataContainer* mdtPRDs, const Muon::CscPrepDataContainer* cscPRDs,
			  const Muon::RpcPrepDataContainer* rpdPRDs, const Muon::TgcPrepDataContainer* tgcPRDs, const Muon::sTgcPrepDataContainer* stgcPRDs,
			  const Muon::MMPrepDataContainer* mmPRDs );

    /** add muon candidate to indet candidate */
    void addTag( const InDetCandidate& indetCandidate, const Muon::MuonCandidate& candidate, 
                 std::unique_ptr<const Trk::Track>& selectedTrack ) const;

    /** access data in layer */
    bool getLayerData( int sector, Muon::MuonStationIndex::DetectorRegionIndex regionIndex, Muon::MuonStationIndex::LayerIndex layerIndex, 
		       Muon::MuonLayerPrepRawData& layerPrepRawData, const Muon::MdtPrepDataContainer* mdtPRDs, const Muon::CscPrepDataContainer* cscPRDs,
		       const Muon::RpcPrepDataContainer* rpdPRDs, const Muon::TgcPrepDataContainer* tgcPRDs, const Muon::sTgcPrepDataContainer* stgcPRDs,
		       const Muon::MMPrepDataContainer* mmPRDs );

    /** access data in layer for a given technology */
    template<class COL>
      bool getLayerDataTech( int sector, Muon::MuonStationIndex::TechnologyIndex technology, Muon::MuonStationIndex::DetectorRegionIndex regionIndex,
			     Muon::MuonStationIndex::LayerIndex layerIndex, const Muon::MuonPrepDataContainer< COL >* input, std::vector<const COL*>& output );

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
    ToolHandle<Muon::MuonLayerHashProviderTool>      m_layerHashProvider;
    /** id pt cut */
    double m_idTrackMinPt;
    bool m_ignoreSiAssocated;
    const Muon::MdtPrepDataContainer* m_mdtPRDs;
    const Muon::CscPrepDataContainer* m_cscPRDs;
    const Muon::RpcPrepDataContainer* m_rpcPRDs;
    const Muon::TgcPrepDataContainer* m_tgcPRDs;
    const Muon::sTgcPrepDataContainer* m_stgcPRDs;
    const Muon::MMPrepDataContainer* m_mmPRDs;
  };
}



#endif
 


