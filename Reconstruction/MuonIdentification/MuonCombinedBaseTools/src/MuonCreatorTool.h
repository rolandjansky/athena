/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCREATORTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCREATORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCreatorTool.h"

#include "CaloConditions/CaloNoise.h"
#include "CaloEvent/CaloCellContainer.h"
#include "MuidInterfaces/IMuonTrackQuery.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedToolInterfaces/IMuonMeanMDTdADCFiller.h"
#include "MuonCombinedToolInterfaces/IMuonMomentumBalanceSignificance.h"
#include "MuonCombinedToolInterfaces/IMuonPrintingTool.h"
#include "MuonCombinedToolInterfaces/IMuonScatteringAngleSignificance.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegment/MuonSegment.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrackSegmentAssociationTool.h"
#include "TrackToCalo/CaloCellCollector.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkToolInterfaces/ITrkMaterialProviderTool.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/SlowMuon.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODMuonCnv/IMuonDressingTool.h"
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace MuonCombined {
class StacoTag;
class CombinedFitTag;
class MuGirlTag;
class MuGirlLowBetaTag;
class SegmentTag;
class CaloTag;

typedef std::vector<InDetCandidateTags> InDetCandidateTagsMap;

class MuonCreatorTool
  : public AthAlgTool
  , virtual public IMuonCreatorTool
{

public:
  MuonCreatorTool(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);
  ~MuonCreatorTool() = default;

  virtual StatusCode initialize() override final;

  /** IMuonCreatorTool interface: build muons from ID and MS candidates */
  virtual void create(const MuonCandidateCollection* muonCandidates,
                      const InDetCandidateCollection* inDetCandidates,
                      std::vector<const InDetCandidateToTagMap*> tagMaps,
                      OutputData& outputData) const override final;

  /** IMuonCreatorTool interface: create a muon from a muon candidate */
  virtual xAOD::Muon* create(const MuonCandidate& candidate,
                             OutputData& outputData) const override final;

  /** IMuonCreatorTool interface: create a muon from a muon candidate */
  virtual xAOD::Muon* create(InDetCandidateTags& candidate,
                             OutputData& outputData) const override final;

private:
  void addStatisticalCombination(xAOD::Muon& muon,
                                 const InDetCandidate& candidate,
                                 const StacoTag* tag,
                                 OutputData& outputData) const;

  void addCombinedFit(xAOD::Muon& muon,
                      const CombinedFitTag* tag,
                      OutputData& outputData) const;

  void addMuGirl(xAOD::Muon& muon,
                 const MuGirlTag* tag,
                 OutputData& outputData) const;

  void addMuGirlLowBeta(xAOD::Muon& muon,
                        const MuGirlLowBetaTag* tag,
                        xAOD::SlowMuon* slowMuon,
                        OutputData& outputData) const;

  void addSegmentTag(xAOD::Muon& muon, const SegmentTag* tag) const;
  void addCaloTag(xAOD::Muon& muon, const CaloTag* tag) const;

  /** add muon candidate info to a muon, if an updateExtrapolatedTrack is
     provided, the routine takes ownership of the track. The track will be used
     instead of the extrapolatedTrack of the MuonCandidate. The
     extrapolatedTrack of the MuonCandidate will be release during the
     operation.
   */
  void addMuonCandidate(
    const MuonCandidate& candidate,
    xAOD::Muon& muon,
    OutputData& outputData,
    const ElementLink<TrackCollection>& meLink = ElementLink<TrackCollection>()) const;

  /// function creates an element link to a track particle from the track and
  /// the TrackParticle collection. if a TrackCollection is also provided, the
  /// element link to the track will also be set takes ownership of the track
  ElementLink<xAOD::TrackParticleContainer> createTrackParticleElementLink(
    const ElementLink<TrackCollection>& trackLink,
    xAOD::TrackParticleContainer& trackParticleContainer,
    TrackCollection* trackCollection = 0) const;

  ElementLink<xAOD::MuonSegmentContainer> createMuonSegmentElementLink(
    const ElementLink<Trk::SegmentCollection>& segLink,
    xAOD::MuonSegmentContainer& xaodSegments,
    Trk::SegmentCollection* muonSegmentCollection = 0) const;

private:
  void resolveOverlaps(const InDetCandidateCollection* inDetCandidates,
                       const MuonCandidateCollection* muonCandidates,
                       const std::vector<const InDetCandidateToTagMap*>& tagMaps,
                       InDetCandidateTagsMap& resolvedInDetCandidates,
                       std::vector<const MuonCombined::MuonCandidate*>&
                         resolvedMuonCandidates) const;

  void selectStaus(const InDetCandidateCollection* inDetCandidates,
                   InDetCandidateTagsMap& resolvedInDetCandidates,
                   std::vector<const InDetCandidateToTagMap*> tagMaps) const;

public:
  void selectStaus(const InDetCandidateCollection* inDetCandidates,
                   std::vector<const MuonCombined::InDetCandidate*>&
                     resolvedInDetCandidates) const;

  Trk::Track* createDummyTrack(const std::vector<const Muon::MuonSegment*>& segments,
                               const Trk::Track& indetTrack) const;
  void setMuonHitCounts(xAOD::Muon& muon) const;

  bool dressMuon(xAOD::Muon& muon,
                 const xAOD::MuonSegmentContainer* segments) const;

  void addEnergyLossToMuon(xAOD::Muon& muon) const;

  void fillEnergyLossFromTrack(
    xAOD::Muon& muon,
    const std::vector<const Trk::TrackStateOnSurface*>* tsosVector) const;

  void setP4(xAOD::Muon& muon, const xAOD::TrackParticle& tp) const;

  void collectCells(xAOD::Muon& muon,
                    xAOD::CaloClusterContainer* clusterContainer,
                    Trk::CaloExtension* inputCaloExt = nullptr) const;

  void getRpcTiming(const xAOD::TrackParticle& tp,
                    std::vector<unsigned int>& rpcHitIdentifier,
                    std::vector<float>& rpcHitPositionX,
                    std::vector<float>& rpcHitPositionY,
                    std::vector<float>& rpcHitPositionZ,
                    std::vector<float>& rpcHitTime) const;
  void addRpcTiming(xAOD::Muon& muon) const;
  void addMSIDScatteringAngles(xAOD::Muon& muon) const;
  void addMSIDScatteringAngles(const xAOD::TrackParticle& track) const;
  void addSegmentsOnTrack(xAOD::Muon& muon,
                          const xAOD::MuonSegmentContainer* segments) const;
  void addAlignmentEffectsOnTrack(xAOD::TrackParticleContainer* trkCont) const;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
    this,
    "MuonIdHelperSvc",
    "Muon::MuonIdHelperSvc/MuonIdHelperSvc"
  };
  ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{
    this,
    "edmHelper",
    "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
    "Handle to the service providing the IMuonEDMHelperSvc interface"
  };

  ToolHandle<Muon::MuonEDMPrinterTool> m_printer{
    this,
    "Printer",
    "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"
  };
  ToolHandle<Rec::IMuonPrintingTool> m_muonPrinter{
    this,
    "MuonPrinter",
    "Rec::MuonPrintingTool/MuonPrintingTool"
  };
  ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtTool{
    this,
    "ParticleCaloExtensionTool",
    "Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"
  };
  ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtToolID{
    this,
    "ParticleCaloExtensionToolID",
    "Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"
  };
  ToolHandle<Trk::ITrackParticleCreatorTool> m_particleCreator{
    this,
    "TrackParticleCreator",
    "Trk::TrackParticleCreatorTool/MuonCombinedTrackParticleCreator"
  };
  ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiguityProcessor{
    this,
    "AmbiguityProcessor",
    "Trk::TrackSelectionProcessorTool/MuonAmbiProcessor"
  };
  ToolHandle<Trk::IPropagator> m_propagator{
    this,
    "Propagator",
    "Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"
  };
  ToolHandle<xAOD::IMuonDressingTool> m_muonDressingTool{
    this,
    "MuonDressingTool",
    "MuonCombined::MuonDressingTool/MuonDressingTool"
  };
  ToolHandle<Rec::IMuonMomentumBalanceSignificance> m_momentumBalanceTool{
    this,
    "MomentumBalanceTool",
    "Rec::MuonMomentumBalanceSignificanceTool/"
    "MuonMomentumBalanceSignificanceTool"
  };
  ToolHandle<Rec::IMuonScatteringAngleSignificance> m_scatteringAngleTool{
    this,
    "ScatteringAngleTool",
    "Rec::MuonScatteringAngleSignificanceTool/"
    "MuonScatteringAngleSignificanceTool"
  };
  ToolHandle<CP::IMuonSelectionTool> m_selectorTool{
    this,
    "MuonSelectionTool",
    "CP::MuonSelectionTool/MuonSelectionTool"
  };
  ToolHandle<xAODMaker::IMuonSegmentConverterTool> m_muonSegmentConverterTool{
    this,
    "MuonSegmentConverterTool",
    "Muon::MuonSegmentConverterTool/MuonSegmentConverterTool"
  };
  ToolHandle<Rec::IMuonMeanMDTdADCFiller> m_meanMDTdADCTool{
    this,
    "MeanMDTdADCTool",
    "Rec::MuonMeanMDTdADCFillerTool/MuonMeanMDTdADCFillerTool"
  };
  ToolHandle<Trk::ITrkMaterialProviderTool> m_caloMaterialProvider{
    this,
    "CaloMaterialProvider",
    "Trk::TrkMaterialProviderTool/TrkMaterialProviderTool"
  };
  ToolHandle<Muon::TrackSegmentAssociationTool> m_trackSegmentAssociationTool{
    this,
    "TrackSegmentAssociationTool",
    "Muon::TrackSegmentAssociationTool/TrackSegmentAssociationTool"
  };
  ToolHandle<Rec::IMuonTrackQuery> m_trackQuery{
    this,
    "TrackQuery",
    "Rec::MuonTrackQuery/MuonTrackQuery"
  };
  ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool{
    this,
    "TrackSummaryTool",
    "MuonTrackSummaryTool"
  };

  Rec::CaloCellCollector m_cellCollector;

  SG::ReadHandleKey<CaloCellContainer> m_cellContainerName{ this,
                                                            "CaloCellContainer",
                                                            "AllCalo",
                                                            "calo cells" };
  SG::ReadCondHandleKey<CaloNoise> m_caloNoiseKey{
    this,
    "CaloNoise",
    "",
    "CaloNoise object to use, or blank."
  };

  Gaudi::Property<bool> m_makeMSPreExtrapLink{
    this,
    "MakeTrackAtMSLink",
    false,
    "flag to decide whether or not to make link to MS track before "
    "extrapolation"
  };
  Gaudi::Property<bool> m_buildStauContainer{
    this,
    "BuildStauContainer",
    false,
    "flag to decide whether to build stau or not"
  };
  Gaudi::Property<bool> m_fillEnergyLossFromTrack{
    this,
    "FillEnergyLossFromTrack",
    true,
    "Decide whether to try to extract the calo energy loss from tracks "
  };
  Gaudi::Property<bool> m_fillAlignmentEffectsOnTrack{
    this,
    "FillAlignmentEffectsOnTrack",
    true,
    "Decide whether to add alignment effects on track to the muon (available "
    "for CB and SA tracks)"
  };
  Gaudi::Property<bool> m_fillExtraELossInfo{
    this,
    "FillExtraELossInfo",
    true,
    "Can enabled this for debugging - will add extra information not for "
    "production"
  };
  Gaudi::Property<bool> m_printSummary{ this,
                                        "PrintSummary",
                                        false,
                                        "flag to print muon edm" };
  Gaudi::Property<bool> m_useUpdatedExtrapolatedTrack{
    this,
    "UseUpdatedExtrapolatedTrack",
    true,
    "configure whether to use the updated extrapolated track for a combined "
    "fit or not"
  };
  Gaudi::Property<bool> m_fillTimingInformation{
    this,
    "FillTimingInformation",
    true,
    "enable filling of timing information"
  };
  Gaudi::Property<bool> m_fillTimingInformationOnMuon{
    this,
    "FillTimingInformationOnMuon",
    false,
    "enable filling of timing information"
  };
  Gaudi::Property<bool> m_segLowBeta{
    this,
    "AssociateSegmentsToLowBetaMuons",
    false,
    "associate segments to MuGirlLowBeta muons"
  };
  Gaudi::Property<bool> m_useCaloCells{ this, "UseCaloCells", true };
  Gaudi::Property<bool> m_doSA{ this, "MakeSAMuons", false };
  /// In case of running the muon reconstruction with LRT tracks this property
  /// removes the overlap of muons in the container in which in any case
  /// no ID track is available
  Gaudi::Property<bool> m_requireIDTracks{ this, "RequireIDTrack", false };

  Gaudi::Property<float> m_sigmaCaloNoiseCut{ this, "SigmaCaloNoiseCut", 3.4 };
};

inline void
MuonCreatorTool::setP4(xAOD::Muon& muon, const xAOD::TrackParticle& tp) const
{
  muon.setP4(tp.pt(), tp.eta(), tp.phi());
}

} // end of namespace

#endif
