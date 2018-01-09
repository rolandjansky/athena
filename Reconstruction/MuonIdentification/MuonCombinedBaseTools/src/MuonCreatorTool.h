/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCREATORTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCREATORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCreatorTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"

#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODMuon/SlowMuon.h"
#include "xAODMuonCnv/IMuonDressingTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "MuonCombinedToolInterfaces/IMuonMomentumBalanceSignificance.h"
#include "MuonCombinedToolInterfaces/IMuonScatteringAngleSignificance.h" 
#include "MuonSelectorTools/IMuonSelectionTool.h" 
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "TrkSegment/SegmentCollection.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "TrkSegment/Segment.h"
#include "MuonSegment/MuonSegment.h"
#include "TrackToCalo/CaloCellCollector.h"
#include "CaloInterface/ICaloNoiseTool.h"

namespace Muon {
  class MuonEDMPrinterTool;
  class MuonEDMHelperTool;
  class MuonSegment;
  class TrackSegmentAssociationTool;
}
namespace Trk {
  class ITrackParticleCreatorTool;
  class ITrackAmbiguityProcessorTool;
  class IPropagator;
  class ITrkMaterialProviderTool;
}
namespace Rec {
  class IMuonPrintingTool;
  class IMuonMeanMDTdADCFiller;  
  class IParticleCaloClusterAssociationTool;
  class IParticleCaloCellAssociationTool;
  class IMuonTrackQuery;
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

    /** IMuonCreatorTool interface: build muons from ID and MS candidates */    
    void create( const MuonCandidateCollection* muonCandidates, const InDetCandidateCollection* inDetCandidates, OutputData& outputData ) const final;

    /** IMuonCreatorTool interface: create a muon from a muon candidate */
    xAOD::Muon* create( const MuonCandidate& candidate, OutputData& outputData ) const final;

    /** IMuonCreatorTool interface: create a muon from a muon candidate */
    xAOD::Muon* create( const InDetCandidate& candidate, OutputData& outputData ) const final;

  private:
    void addStatisticalCombination( xAOD::Muon& muon, const InDetCandidate& candidate, const StacoTag* tag, OutputData& outputData ) const;

    void addCombinedFit( xAOD::Muon& muon, const CombinedFitTag* tag, OutputData& outputData ) const; 

    void addMuGirl( xAOD::Muon& muon, const MuGirlTag* tag, OutputData& outputData ) const;

    void addMuGirlLowBeta( xAOD::Muon& muon, MuGirlLowBetaTag* tag, xAOD::SlowMuon* slowMuon, OutputData& outputData ) const;

    void addSegmentTag( xAOD::Muon& muon, const SegmentTag* tag ) const;
    void addCaloTag( xAOD::Muon& muon, const CaloTag* tag ) const;
    
    /** add muon candidate info to a muon, if an updateExtrapolatedTrack is provided, the routine takes ownership of the track.
        The track will be used instead of the extrapolatedTrack of the MuonCandidate. The extrapolatedTrack of the MuonCandidate will be release during the operation.
     */
    void addMuonCandidate( const MuonCandidate& candidate, xAOD::Muon& muon, OutputData& outputData, const Trk::Track* updatedExtrapolatedTrack = 0 ) const;

    /// function creates an element link to a track particle from the track and the TrackParticle collection.
    /// if a TrackCollection is also provided, the element link to the track will also be set
    /// takes ownership of the track
    ElementLink<xAOD::TrackParticleContainer>
      createTrackParticleElementLink( std::unique_ptr<const Trk::Track> track, 
        xAOD::TrackParticleContainer& trackParticleContainer, 
        TrackCollection* trackCollection = 0 ) const ;

    ElementLink<xAOD::MuonSegmentContainer> createMuonSegmentElementLink( const Muon::MuonSegment & muonSegment,
        xAOD::MuonSegmentContainer& xaodSegments,
        Trk::SegmentCollection* muonSegmentCollection = 0 ) const ;

  private:
    void resolveOverlaps( const InDetCandidateCollection* inDetCandidates, const MuonCandidateCollection* muonCandidates, 
			  std::vector<const MuonCombined::InDetCandidate*>& resolvedInDetCandidates,
                          std::vector<const MuonCombined::MuonCandidate*>& resolvedMuonCandidates) const;
    
    void selectStaus( const InDetCandidateCollection* inDetCandidates, InDetCandidateCollection& resolvedInDetCandidates) const;

  public:
    void selectStaus( const InDetCandidateCollection* inDetCandidates,
                      std::vector<const MuonCombined::InDetCandidate*>& resolvedInDetCandidates) const;

    Trk::Track* createDummyTrack( std::vector<const Muon::MuonSegment*> segments, const Trk::Track& indetTrack ) const;
    void setMuonHitCounts( xAOD::Muon& muon ) const;

    bool dressMuon(  xAOD::Muon& muon ) const;

    void addEnergyLossToMuon( xAOD::Muon& muon ) const;

    void fillEnergyLossFromTrack(xAOD::Muon& muon, const std::vector<const Trk::TrackStateOnSurface*>* tsosVector) const;

    void setP4( xAOD::Muon& muon, const xAOD::TrackParticle& tp ) const;

    void collectCells( xAOD::Muon& muon, xAOD::CaloClusterContainer* clusterContainer ) const;

    void getRpcTiming(const xAOD::TrackParticle& tp,
		      std::vector<unsigned int>& rpcHitIdentifier,
		      std::vector<float>& rpcHitPositionX,
		      std::vector<float>& rpcHitPositionY,
		      std::vector<float>& rpcHitPositionZ,
		      std::vector<float>& rpcHitTime) const;
    void addRpcTiming( const xAOD::TrackParticle& track ) const;
    void addRpcTiming( xAOD::Muon& muon ) const;
    void addMSIDScatteringAngles(xAOD::Muon& muon) const;
    void addMSIDScatteringAngles(const xAOD::TrackParticle& track) const;
    void addSegmentsOnTrack( xAOD::Muon& muon ) const;
    void addAlignmentEffectsOnTrack( xAOD::TrackParticleContainer* trkCont ) const;

    /// flag to decide whether or not to make link to MS track before extrapolation
    bool m_makeMSPreExtrapLink;

    /// flag to decide whether to build stau or not
    bool m_buildStauContainer;
    
    /// Decide whether to try to extract the calo energy loss from tracks 
    bool m_fillEnergyLossFromTrack;

    /// Decide whether to add alignment effects on track to the muon (available for CB and SA tracks)
    bool m_fillAlignmentEffectsOnTrack;
    
    /// Can enabled this for debugging - will add extra information not for production
    bool m_fillExtraELossInfo;
    
    /// Since the Calo information can come from various sources, make sure that we don't overwrite once 'best' source added.
    mutable bool m_haveAddedCaloInformation;
    
    /// configure whether to use the updated extrapolated track for a combined fit or not
    bool m_useUpdatedExtrapolatedTrack;

    /// Flag to apply noise cut to calo cells around muons
    bool m_applyCaloNoiseCut;
      
    /// Number of sigma for calo cell noise cut
    float m_sigmaCaloNoiseCut;

    /// flag to print muon edm
    bool m_printSummary;
    
    /// enable filling of timing information
    bool m_fillTimingInformation;
    bool m_fillTimingInformationOnMuon;

    //associate segments to MuGirlLowBeta muons;
    bool m_segLowBeta;

    /// copy truth links from primary track particle (or put dummy link if this is missing)
    //bool m_fillMuonTruthLinks;
    
    // helpers, managers, tools
    ToolHandle<Muon::MuonIdHelperTool>            m_idHelper;
    ToolHandle<Muon::MuonEDMPrinterTool>          m_printer;
    ToolHandle<Muon::MuonEDMHelperTool>           m_edmHelper;
    ToolHandle<Rec::IMuonPrintingTool>            m_muonPrinter;
    ToolHandle<Trk::IParticleCaloExtensionTool>   m_caloExtTool;
    ToolHandle<Trk::ITrackParticleCreatorTool>    m_particleCreator;
    // FIXME mutable
    mutable ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiguityProcessor;
    ToolHandle<Trk::IPropagator>                  m_propagator;
    ToolHandle<xAOD::IMuonDressingTool>           m_muonDressingTool;
    ToolHandle<Rec::IMuonMomentumBalanceSignificance> m_momentumBalanceTool;
    ToolHandle<Rec::IMuonScatteringAngleSignificance> m_scatteringAngleTool; 
    ToolHandle<CP::IMuonSelectionTool>            m_selectorTool; 
    ToolHandle<xAODMaker::IMuonSegmentConverterTool>  m_muonSegmentConverterTool;
    ToolHandle<Rec::IMuonMeanMDTdADCFiller>       m_meanMDTdADCTool;
    ToolHandle <ICaloNoiseTool>                   m_caloNoiseTool; 
    ToolHandle<Trk::ITrkMaterialProviderTool>     m_caloMaterialProvider;
    ToolHandle<Muon::TrackSegmentAssociationTool> m_trackSegmentAssociationTool;
    ToolHandle<Rec::IMuonTrackQuery>              m_trackQuery;
    Rec::CaloCellCollector                        m_cellCollector;
    std::string                                   m_cellContainerName;
  };

  inline void MuonCreatorTool::setP4( xAOD::Muon& muon, const xAOD::TrackParticle& tp ) const {
    muon.setP4(tp.pt(),tp.eta(),tp.phi());
  }
  
}	// end of namespace

#endif
