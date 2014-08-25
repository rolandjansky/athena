/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCREATORTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCREATORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCreatorTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "IsolationTool/ITrackIsolationTool.h"
#include "IsolationTool/ICaloIsolationTool.h"

#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuonCnv/IMuonDressingTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "MuonCombinedToolInterfaces/IMuonMomentumBalanceSignificance.h"
#include "MuonCombinedToolInterfaces/IMuonScatteringAngleSignificance.h" 
#include "MuonSelectorTools/IMuonSelectionTool.h" 

#include "TrkSegment/SegmentCollection.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"

#include "TrkSegment/Segment.h"
#include "MuonSegment/MuonSegment.h"

namespace Muon {
  class MuonEDMPrinterTool;
  class MuonEDMHelperTool;
  class MuonSegment;
}
namespace Trk {
  class ITrackParticleCreatorTool;
  class ITrackAmbiguityProcessorTool;
  class IPropagator;
}
namespace Rec {
  class IMuonPrintingTool;
  class IMuonMeanMDTdADCFiller;  
}
namespace MuonCombined {
  class IMuonCombinedTagTool;
  class StacoTag;
  class CombinedFitTag;
  class MuGirlTag;
  class MuGirlLowBetaTag;
  class SegmentTag;
  class CaloTag;

  class MuonCreatorTool: public AthAlgTool, virtual public IMuonCreatorTool
  {

  public:
    MuonCreatorTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonCreatorTool(void); // destructor
  
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonCreatorTool interface: build muons from ID and MS candidates */    
    void create( const MuonCandidateCollection* muonCandidates, const InDetCandidateCollection* inDetCandidates,
		 xAOD::MuonContainer& container, 
		 xAOD::TrackParticleContainer* combinedTrackParticleContainer, TrackCollection* combinedTrackCollection,
		 xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection,
		 xAOD::MuonSegmentContainer* xaodSegments, Trk::SegmentCollection* muonSegmentCollection) const;

    /** create a muon from a muon candidate */
    xAOD::Muon* create( const MuonCandidate& candidate, xAOD::MuonContainer& container, 
			xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection ) const;

    /** create a muon from a muon candidate */
    xAOD::Muon* create( const InDetCandidate& candidate, xAOD::MuonContainer& container, 
			xAOD::TrackParticleContainer* combinedTrackParticleContainer, TrackCollection* combinedTrackCollection,
			xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection,
			xAOD::MuonSegmentContainer* xaodSegments, Trk::SegmentCollection* muonSegmentCollection ) const;

  private:
    void addStatisticalCombination( xAOD::Muon& muon, const InDetCandidate& candidate, const StacoTag& tag,
				    xAOD::TrackParticleContainer* combinedTrackParticleContainer,
				    xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection ) const;

    void addCombinedFit( xAOD::Muon& muon, const CombinedFitTag& tag,
			 xAOD::TrackParticleContainer* combinedTrackParticleContainer, TrackCollection* combinedTrackCollection,
			 xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection ) const;

    void addMuGirl( xAOD::Muon& muon, const MuGirlTag& tag, 
                    xAOD::TrackParticleContainer* combinedTrackParticleContainer, TrackCollection* combinedTrackCollection,
                    xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection ,
                    xAOD::MuonSegmentContainer* xaodSegments, Trk::SegmentCollection* muonSegmentCollection) const;

    void addMuGirlLowBeta( xAOD::Muon& muon, const MuGirlLowBetaTag& tag, 
                    xAOD::TrackParticleContainer* combinedTrackParticleContainer, TrackCollection* combinedTrackCollection,
                    xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, TrackCollection* extrapolatedTrackCollection ,
                    xAOD::MuonSegmentContainer* xaodSegments, Trk::SegmentCollection* muonSegmentCollection) const;

    void addSegmentTag( xAOD::Muon& muon, const SegmentTag& tag ) const;
    void addCaloTag( xAOD::Muon& muon, const CaloTag& tag ) const;
    void setP4( xAOD::Muon& muon, const xAOD::TrackParticle& tp ) const;

    void addMuonCandidate( const MuonCandidate& candidate, xAOD::Muon& muon,
			   xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer, 
			   TrackCollection* extrapolatedTrackCollection) const;

    /// function creates an element link to a track particle from the track and the TrackParticle collection.
    /// if a TrackCollection is also provided, the element link to the track will also be set
    /// takes ownership of the track
    ElementLink<xAOD::TrackParticleContainer> createTrackParticleElementLink( const Trk::Track& track, 
									      xAOD::TrackParticleContainer& trackParticleContainer, 
									      TrackCollection* trackCollection = 0 ) const ;

    ElementLink<xAOD::MuonSegmentContainer> createMuonSegmentElementLink( const Muon::MuonSegment & muonSegment,
                                                                          xAOD::MuonSegmentContainer& xaodSegments,
                                                                          Trk::SegmentCollection* muonSegmentCollection = 0 ) const ;

    void resolveOverlaps( const InDetCandidateCollection* inDetCandidates, const MuonCandidateCollection* muonCandidates, 
			  InDetCandidateCollection& resolvedInDetCandidates, MuonCandidateCollection& resolvedMuonCandidates) const;
    
    void selectStaus( const InDetCandidateCollection* inDetCandidates, InDetCandidateCollection& resolvedInDetCandidates) const;

    Trk::Track* createDummyTrack( std::vector<const Muon::MuonSegment*> segments, const Trk::Track& indetTrack ) const;
    void setMuonHitCounts( xAOD::Muon& muon ) const;

    void fillIsolation( xAOD::Muon& muon ) const;

    bool dressMuon(  xAOD::Muon& muon ) const;

    // flag to decide whether or not to make link to MS track before extrapolation
    bool m_makeMSPreExtrapLink;

    // flag to decide whether to build stau or not
    bool m_buildStauContainer;

    // helpers, managers, tools
    ToolHandle<Muon::MuonIdHelperTool>            m_idHelper;
    ToolHandle<Muon::MuonEDMPrinterTool>          m_printer;
    ToolHandle<Muon::MuonEDMHelperTool>           m_edmHelper;
    ToolHandle<Rec::IMuonPrintingTool>            m_muonPrinter;
    ToolHandle<Trk::ITrackParticleCreatorTool>    m_particleCreator;
    ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiguityProcessor;
    ToolHandle<Trk::IPropagator>                  m_propagator;
    ToolHandle<xAOD::IMuonDressingTool>           m_muonDressingTool;
    ToolHandle<xAOD::ITrackIsolationTool>         m_trackIsolationTool;
    ToolHandle<xAOD::ICaloIsolationTool>          m_caloIsolationTool;
    ToolHandle<Rec::IMuonMomentumBalanceSignificance> m_momentumBalanceTool;
    ToolHandle<Rec::IMuonScatteringAngleSignificance> m_scatteringAngleTool; 
    ToolHandle<CP::IMuonSelectionTool>            m_selectorTool; 
    ToolHandle<xAODMaker::IMuonSegmentConverterTool>  m_muonSegmentConverterTool;
    ToolHandle<Rec::IMuonMeanMDTdADCFiller>       m_meanMDTdADCTool;		
  };

  inline void MuonCreatorTool::setP4( xAOD::Muon& muon, const xAOD::TrackParticle& tp ) const {
    muon.setP4(tp.pt(),tp.eta(),tp.phi());
  }
  
}	// end of namespace

#endif


