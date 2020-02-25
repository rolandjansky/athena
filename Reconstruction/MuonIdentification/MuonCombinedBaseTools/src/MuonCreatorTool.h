/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCREATORTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCREATORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCreatorTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
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
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h" 
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "TrkSegment/SegmentCollection.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonCombinedToolInterfaces/IMuonPrintingTool.h"
#include "MuonCombinedToolInterfaces/IMuonMeanMDTdADCFiller.h"
#include "MuidInterfaces/IMuonTrackQuery.h"
#include "TrkToolInterfaces/ITrkMaterialProviderTool.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"

#include "TrackSegmentAssociationTool.h"

#include "TrkSegment/Segment.h"
#include "MuonSegment/MuonSegment.h"
#include "TrackToCalo/CaloCellCollector.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloConditions/CaloNoise.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

namespace Muon {
  class MuonSegment;
}
namespace Trk
{
  class IExtendedTrackSummaryTool;
}
namespace MuonCombined {
  class StacoTag;
  class CombinedFitTag;
  class MuGirlTag;
  class MuGirlLowBetaTag;
  class SegmentTag;
  class CaloTag;

  typedef std::vector<InDetCandidateTags> InDetCandidateTagsMap;

  class MuonCreatorTool: public AthAlgTool, virtual public IMuonCreatorTool
  {

  public:
    MuonCreatorTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonCreatorTool()=default;
  
    StatusCode initialize();

    /** IMuonCreatorTool interface: build muons from ID and MS candidates */    
    void create( const MuonCandidateCollection* muonCandidates, const InDetCandidateCollection* inDetCandidates, std::vector<const InDetCandidateToTagMap*> tagMaps,
		 OutputData& outputData ) const final;

    /** IMuonCreatorTool interface: create a muon from a muon candidate */
    xAOD::Muon* create( const MuonCandidate& candidate, OutputData& outputData ) const final;

    /** IMuonCreatorTool interface: create a muon from a muon candidate */
    xAOD::Muon* create( InDetCandidateTags& candidate, OutputData& outputData ) const final;

  private:
    void addStatisticalCombination( xAOD::Muon& muon, const InDetCandidate& candidate, const StacoTag* tag, OutputData& outputData ) const;

    void addCombinedFit( xAOD::Muon& muon, const CombinedFitTag* tag, OutputData& outputData ) const; 

    void addMuGirl( xAOD::Muon& muon, const MuGirlTag* tag, OutputData& outputData ) const;

    void addMuGirlLowBeta( xAOD::Muon& muon, const MuGirlLowBetaTag* tag, xAOD::SlowMuon* slowMuon, OutputData& outputData ) const;

    void addSegmentTag( xAOD::Muon& muon, const SegmentTag* tag ) const;
    void addCaloTag( xAOD::Muon& muon, const CaloTag* tag ) const;
    
    /** add muon candidate info to a muon, if an updateExtrapolatedTrack is provided, the routine takes ownership of the track.
        The track will be used instead of the extrapolatedTrack of the MuonCandidate. The extrapolatedTrack of the MuonCandidate will be release during the operation.
     */
    void addMuonCandidate( const MuonCandidate& candidate, xAOD::Muon& muon, OutputData& outputData, ElementLink<TrackCollection> meLink=ElementLink<TrackCollection>() ) const;

    /// function creates an element link to a track particle from the track and the TrackParticle collection.
    /// if a TrackCollection is also provided, the element link to the track will also be set
    /// takes ownership of the track
    ElementLink<xAOD::TrackParticleContainer>
      createTrackParticleElementLink( ElementLink<TrackCollection> trackLink,
				      xAOD::TrackParticleContainer& trackParticleContainer,
				      TrackCollection* trackCollection = 0) const ;

    ElementLink<xAOD::MuonSegmentContainer> createMuonSegmentElementLink( ElementLink<Trk::SegmentCollection> segLink,
									  xAOD::MuonSegmentContainer& xaodSegments,
									  Trk::SegmentCollection* muonSegmentCollection = 0) const ;

  private:
    void resolveOverlaps( const InDetCandidateCollection* inDetCandidates, const MuonCandidateCollection* muonCandidates, std::vector<const InDetCandidateToTagMap*> tagMaps,
			  InDetCandidateTagsMap& resolvedInDetCandidates,
                          std::vector<const MuonCombined::MuonCandidate*>& resolvedMuonCandidates) const;
    
    void selectStaus( const InDetCandidateCollection* inDetCandidates, InDetCandidateTagsMap& resolvedInDetCandidates, std::vector<const InDetCandidateToTagMap*> tagMaps) const;

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
    
    /// configure whether to use the updated extrapolated track for a combined fit or not
    bool m_useUpdatedExtrapolatedTrack;

    /// Number of sigma for calo cell noise cut
    float m_sigmaCaloNoiseCut;

    /// flag to print muon edm
    bool m_printSummary;
    
    /// enable filling of timing information
    bool m_fillTimingInformation;
    bool m_fillTimingInformationOnMuon;

    //associate segments to MuGirlLowBeta muons;
    bool m_segLowBeta;

    //use calo cells
    bool m_useCaloCells;

    //flag for SA muons
    bool m_doSA;

    /// copy truth links from primary track particle (or put dummy link if this is missing)
    //bool m_fillMuonTruthLinks;
    
    // helpers, managers, tools
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandle<Muon::MuonEDMPrinterTool>          m_printer;
    ServiceHandle<Muon::IMuonEDMHelperSvc>        m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };
    ToolHandle<Rec::IMuonPrintingTool>            m_muonPrinter;
    ToolHandle<Trk::IParticleCaloExtensionTool>   m_caloExtTool;
    ToolHandle<Trk::ITrackParticleCreatorTool>    m_particleCreator;
    ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiguityProcessor;
    ToolHandle<Trk::IPropagator>                  m_propagator;
    ToolHandle<xAOD::IMuonDressingTool>           m_muonDressingTool;
    ToolHandle<Rec::IMuonMomentumBalanceSignificance> m_momentumBalanceTool;
    ToolHandle<Rec::IMuonScatteringAngleSignificance> m_scatteringAngleTool; 
    ToolHandle<CP::IMuonSelectionTool>            m_selectorTool; 
    ToolHandle<xAODMaker::IMuonSegmentConverterTool>  m_muonSegmentConverterTool;
    ToolHandle<Rec::IMuonMeanMDTdADCFiller>       m_meanMDTdADCTool;
    ToolHandle<Trk::ITrkMaterialProviderTool>     m_caloMaterialProvider;
    ToolHandle<Muon::TrackSegmentAssociationTool> m_trackSegmentAssociationTool;
    ToolHandle<Rec::IMuonTrackQuery>              m_trackQuery;
    ToolHandle<Trk::IExtendedTrackSummaryTool>    m_trackSummaryTool;
    Rec::CaloCellCollector                        m_cellCollector;
    SG::ReadHandleKey<CaloCellContainer>          m_cellContainerName{this,"CaloCellContainer","AllCalo","calo cells"};
    SG::ReadCondHandleKey<CaloNoise>              m_caloNoiseKey{this,"CaloNoise","","CaloNoise object to use, or blank."};
  };

  inline void MuonCreatorTool::setP4( xAOD::Muon& muon, const xAOD::TrackParticle& tp ) const {
    muon.setP4(tp.pt(),tp.eta(),tp.phi());
  }
  
}	// end of namespace

#endif
