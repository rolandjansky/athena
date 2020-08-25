/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCOMBINEDFITTAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCOMBINEDFITTAGTOOL_H

#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSegment/SegmentCollection.h"
#include "xAODTracking/VertexContainer.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuidInterfaces/IMuonTrackQuery.h"
#include "MuidInterfaces/IMuidMuonRecovery.h"
#include "MuidInterfaces/IMuonMatchQuality.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTool.h"
#include "MuonCombinedToolInterfaces/IMuonMomentumBalanceSignificance.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <string>
#include <vector>

namespace MuonCombined {
  class InDetCandidate;
  class MuonCandidate;
  class CombinedFitTag;
  class InDetCandidateToTagMap;

  class MuonCombinedFitTagTool: public AthAlgTool, virtual public IMuonCombinedTagTool {

  public:
    MuonCombinedFitTagTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonCombinedFitTagTool()=default;
  
    virtual StatusCode initialize() override;

    /**IMuonCombinedTagTool interface: build combined  muons from a muon and a vector of indet candidates */    
    virtual 
      void combine( const MuonCandidate& muonCandidate, const std::vector<const InDetCandidate*>& indetCandidates, InDetCandidateToTagMap& tagMap,
		    TrackCollection* combTracks, TrackCollection* METracks) const override;

  private:

    Trk::Track* buildCombinedTrack(const Trk::Track& indetTrack,					     
				   const Trk::Track& spectrometerTrack,
				   const Trk::Track* extrapolatedTrack) const;
    
    bool combinedTrackQualityCheck(Trk::Track& combinedTrack,
				   const Trk::Track& indetTrack) const;
      
    Trk::Track* evaluateMatchProperties(Trk::Track* combinedTrack, CombinedFitTag& tag, 
					const Trk::Track& idTrack, const xAOD::TrackParticle& idTrackParticle) const;

    bool extrapolatedNeedsRefit(const Trk::Track& combTrack,
				const Trk::Track* extrTrack) const;

    bool bestMatchChooser(const InDetCandidate& curCandidate, const CombinedFitTag& curTag, Trk::Track& curTrack, Trk::Track* curMETrack,
			  const InDetCandidate& bestCandidate, const CombinedFitTag& bestTag, Trk::Track& bestTrack, Trk::Track* bestMETrack) const;

    void dumpCaloEloss(Trk::Track* track, std::string txt ) const;

    void dumpCaloEloss(const Trk::Track* track, std::string txt ) const;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    ToolHandle<Muon::MuonEDMPrinterTool> m_printer {this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
    ToolHandle<MuonCombined::IMuonTrackTagTool> m_tagTool {this, "MuonTrackTagTool", "MuonCombined::MuonTrackTagTestTool/MuonTrackTagTestTool"};
    ToolHandle<Rec::ICombinedMuonTrackBuilder> m_trackBuilder {this, "TrackBuilder", ""};
    ToolHandle<Rec::ICombinedMuonTrackBuilder> m_outwardsBuilder {this, "OutwardsTrackBuilder", ""};
    ToolHandle<Rec::IMuonTrackQuery> m_trackQuery {this, "TrackQuery", "Rec::MuonTrackQuery/MuonTrackQuery"};
    ToolHandle<Rec::IMuonMomentumBalanceSignificance> m_momentumBalanceTool {this, "MomentumBalanceTool", "Rec::MuonMomentumBalanceSignificanceTool/MuonMomentumBalanceSignifTool"};
    ToolHandle<Rec::IMuidMuonRecovery> m_muonRecovery {this, "MuonRecovery", ""};
    ToolHandle<Rec::IMuonMatchQuality> m_matchQuality {this, "MatchQuality", "Rec::MuonMatchQuality/MuonMatchQuality"};
    ToolHandle<Trk::ITrackScoringTool> m_trackScoringTool {this, "TrackScoringTool", "Muon::MuonTrackScoringTool/MuonTrackScoringTool"};

    Gaudi::Property<double> m_badFitChi2 {this, "BadFitChi2", 2.5};
    Gaudi::Property<double> m_momentumBalanceCut {this, "MomentumBalanceCut", 6};
    Gaudi::Property<double> m_indetPullCut {this, "IndetPullCut", 6};
    Gaudi::Property<double> m_matchChiSquaredCut {this, "MatchChiSquaredCut", 30};

    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};

    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexKey { this, "VertexContainer", "PrimaryVertices", "primary vertex container" };
  };

}	// end of namespace

#endif // MUONCOMBINEDBASETOOLS_MUONCOMBINEDFITTAGTOOL_H


