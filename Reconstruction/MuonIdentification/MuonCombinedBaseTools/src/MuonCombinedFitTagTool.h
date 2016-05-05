/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCOMBINEDFITTAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCOMBINEDFITTAGTOOL_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSegment/SegmentCollection.h"
#include <vector>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Rec
{
class ICombinedMuonTrackBuilder;
class IMuonTrackQuery;
class IMuonMomentumBalanceSignificance;
class IMuidMuonRecovery;
class IMuonMatchQuality;
}
namespace Trk {
  class ITrackScoringTool;
}

namespace Muon
{
  class MuonEDMPrinterTool;
}

namespace MagField {
  class IMagFieldSvc;
}

namespace MuonCombined {
  class InDetCandidate;
  class MuonCandidate;
  class IMuonTrackTagTool;
  class CombinedFitTag;

  class MuonCombinedFitTagTool: public AthAlgTool, virtual public IMuonCombinedTagTool
  {

  public:
    MuonCombinedFitTagTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonCombinedFitTagTool(void); // destructor
  
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonCombinedTagTool interface: build combined  muons from a muon and a vector of indet candidates */    
    void combine( const MuonCandidate& muonCandidate, const std::vector<InDetCandidate*>& indetCandidates );

  private:

    const Trk::Track* buildCombinedTrack(const Trk::Track& indetTrack,					     
					 const Trk::Track& spectrometerTrack,
					 const Trk::Track* extrapolatedTrack) const;
    
    bool combinedTrackQualityCheck(const Trk::Track& combinedTrack,
				   const Trk::Track& indetTrack) const;
      
    void evaluateMatchProperties(CombinedFitTag& tag, const Trk::Track& idTrack, const xAOD::TrackParticle& idTrackParticle);

    bool extrapolatedNeedsRefit(const Trk::Track& combTrack,
				const Trk::Track* extrTrack) const;

    bool bestMatchChooser(const InDetCandidate& curCandidate, const CombinedFitTag& curTag,
			  const InDetCandidate& bestCandidate, const CombinedFitTag& bestTag) const;

    void dumpCaloEloss(const Trk::Track* track, std::string txt ) const;

    // helpers, managers, tools
    ToolHandle<Muon::MuonEDMPrinterTool>              m_printer;
    ToolHandle<MuonCombined::IMuonTrackTagTool>       m_tagTool;
    ToolHandle<Rec::ICombinedMuonTrackBuilder>	      m_trackBuilder;
    ToolHandle<Rec::ICombinedMuonTrackBuilder>	      m_outwardsBuilder;
    ToolHandle<Rec::IMuonTrackQuery>		      m_trackQuery;
    ToolHandle<Rec::IMuonMomentumBalanceSignificance> m_momentumBalanceTool;
    ToolHandle<Rec::IMuidMuonRecovery>		      m_muonRecovery; 
    ToolHandle<Rec::IMuonMatchQuality>		      m_matchQuality;
    ToolHandle<Trk::ITrackScoringTool>                m_trackScoringTool;
    ServiceHandle<MagField::IMagFieldSvc>	      m_magFieldSvc;
    const AtlasDetectorID *m_DetID;

    // configuration
    double			m_badFitChi2;
    double			m_momentumBalanceCut;	// Cut on momentum balance of combined track
    double			m_indetPullCut;		// Cut on indet-combined momentum pull
    double			m_matchChiSquaredCut;	// Chi2 cut for classification as good match 

  };

}	// end of namespace

#endif // MUONCOMBINEDBASETOOLS_MUONCOMBINEDFITTAGTOOL_H


