/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCALOTAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCALOTAGTOOL_H

#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTrigCaloTagExtensionTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/CaloTag.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "TrkParameters/TrackParameters.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "muonEvent/DepositInCalo.h"
#include "ICaloTrkMuIdTools/ICaloMuonLikelihoodTool.h"
#include "ICaloTrkMuIdTools/ICaloMuonScoreTool.h"
#include "ICaloTrkMuIdTools/ICaloMuonTag.h"
#include "ICaloTrkMuIdTools/ITrackDepositInCaloTool.h"
#include "ICaloTrkMuIdTools/ICaloMuonScoreONNXRuntimeSvc.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "StoreGate/ReadHandleKey.h"

#include <atomic>
#include <string>
#include <vector>

namespace MuonCombined {

  class MuonCaloTagTool: public AthAlgTool, virtual public IMuonCombinedInDetExtensionTool, virtual public IMuonCombinedTrigCaloTagExtensionTool
  {

  public:
    MuonCaloTagTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonCaloTagTool()=default;
  
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /**IMuonCombinedInDetExtensionTool interface: extend ID candidate */    
    virtual
      void extend( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, TrackCollection* combTracks, TrackCollection* meTracks, 
		   Trk::SegmentCollection* segments ) const override;

    virtual
      void extend( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap,
		   const CaloCellContainer* caloCellContainer,
		   const xAOD::CaloClusterContainer* caloClusterContainer) const override;

    virtual void extendWithPRDs(const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, IMuonCombinedInDetExtensionTool::MuonPrdData prdData,
				TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments) const override;


  private:
    
    void createMuon(const InDetCandidate & muonCandidate, const std::vector<DepositInCalo>& deposits, int tag, float likelihood, float muonScore, InDetCandidateToTagMap* tagMap) const;
    const Trk::TrackParameters* getTrackParameters(const Trk::Track* trk) const;
    bool selectTrack(const Trk::Track* trk, const Trk::Vertex* vertex) const;
    bool selectCosmic(const Trk::Track* ptcl) const;
    bool applyTrackIsolation(const xAOD::TrackParticle& tp) const;
    void showTrackInfo(const Trk::TrackParameters* par) const;
    
    // --- StoreGate keys ---
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloClusterCont{this,"CaloClusters","CaloTopoCluster","calo cluster container"};
    SG::ReadHandleKey<CaloCellContainer> m_caloCellCont{this,"CaloCells","AllCalo","calo cell container"};
    
    // --- Internal cache ---
    mutable std::atomic_int m_nTrueMuons;          //!< Counts the number true muons
    mutable std::atomic_int m_nTracksTagged;       //!< Counts the number of tracks tagged
    mutable std::atomic_int m_nMuonsTagged;        //!< Counts the number of truth muons tagged
    
    // --- Set up what to do and what not to do ---
    Gaudi::Property<bool> m_doCaloMuonTag {this, "doCaloMuonTag", true, "run CaloMuonTag Tool"};
    Gaudi::Property<bool> m_doCaloMuonScore {this, "doCaloMuonScore", true, "run CaloMuonScoreTool"};
    Gaudi::Property<bool> m_doCaloLR {this, "doCaloLR", true, "run CaloMuonLikelihoodTool"};
    Gaudi::Property<bool> m_doTrkSelection {this, "doTrkSelection", true, "This variable should be set to false when there is no primary vertex reconstructed."};
    Gaudi::Property<bool> m_doCosmicTrackSelection {this, "doCosmicTrackSelection", false, "Apply track selection for cosmics"};
    Gaudi::Property<bool> m_showCutFlow {this, "ShowCutFlow", true, "Verbose track selection and track isolation"};
    Gaudi::Property<bool> m_doTruth {this, "ShowTruth", true, "Display truth info for each analysed track"};
    Gaudi::Property<bool> m_debugMode {this, "DebugMode", false, "Switch for extra printout"};
    Gaudi::Property<bool> m_doOldExtrapolation {this, "doOldExtrapolation", false};
    Gaudi::Property<bool> m_ignoreSiAssocated {this, "IgnoreSiAssociatedCandidates", true, "If true, ignore InDetCandidates which are SiAssociated"};

    Gaudi::Property<double> m_ptIsoPtRatioCut {this, "TrackPtIsoPtRatioCut", 5, "Pt isolation for a .45 cone, normalized to track pt"};
    Gaudi::Property<double> m_eIsoBarrelCut {this, "TrackEIsoBarrelCut", 15000, "Energy isolation for a .45 cone in Barrel"};
    Gaudi::Property<double> m_eIsoTransitionCut {this, "TrackEIsoTransitionCut", 8000, "Energy isolation  for a .45 cone in Barrel-EndCap transition region"};
    Gaudi::Property<double> m_eIsoEndCapCut {this, "TrackEIsoEndCapCut", 12000, "Energy isolation for a .45 cone in Endcap"};
    Gaudi::Property<double> m_eIsoPtRatioBarrelCut {this, "TrackEIsoPtRatioBarrelCut", 2.5, "Energy isolation for a .45 cone in Barrel, normalized to track pt"};
    Gaudi::Property<double> m_eIsoPtRatioTransitionCut {this, "TrackEIsoPtRatioTransitionCut", 1.25, "Energy isolation  for a .45 cone in Barrel-EndCap transition region, normalized to track pt"};
    Gaudi::Property<double> m_eIsoPtRatioEndCapCut {this, "TrackEIsoPtRatioEndCapCut", 1.6, "Energy isolation for a .45 cone in Endcap, normalized to track pt"};
    Gaudi::Property<double> m_CaloLRlikelihoodCut {this, "CaloLRLikelihoodCut", 0.5, "CaloLR likelihood ratio hard cut"};
    Gaudi::Property<double> m_CaloMuonScoreCut {this, "CaloMuonScoreCut", 0.4, "Calo muon convolutional neural network output score hard cut"};
    Gaudi::Property<double> m_trackIsoCone {this, "TrackIsoConeSize", 0.45, "Cone size for track isolation"};
    Gaudi::Property<double> m_energyIsoCone {this, "EnergyIsoConeSize", 0.4, "Cone size for energy isolation"};
    
    // --- CaloTrkMuIdTools ---
    ToolHandle<ICaloMuonLikelihoodTool>  m_caloMuonLikelihood{this,"CaloMuonLikelihoodTool","CaloMuonLikelihoodTool/CaloMuonLikelihoodTool"};
    ToolHandle<ICaloMuonScoreTool>  m_caloMuonScoreTool{this, "CaloMuonScoreTool", "CaloMuonScoreTool/CaloMuonScoreTool"};
    ServiceHandle<ICaloMuonScoreONNXRuntimeSvc>  m_caloMuonScoreONNXRuntimeSvc{this, "CaloMuonScoreONNXRuntimeSvc", "CaloMuonScoreTool/CaloMuonScoreONNXRuntimeSvc"};

    ToolHandle<ICaloMuonTag>             m_caloMuonTagLoose{this,"CaloMuonTagLoose","CaloMuonTag/CaloMuonTagLoose","CaloTrkMuIdTools::CaloMuonTag for loose tagging"}; 
    ToolHandle<ICaloMuonTag>             m_caloMuonTagTight{this,"CaloMuonTagTight","CaloMuonTag/CaloMuonTag","CaloTrkMuIdTools::CaloMuonTag for tight tagging"}; 
    ToolHandle<ITrackDepositInCaloTool>  m_trkDepositInCalo{this,"TrackDepositInCaloTool","TrackDepositInCaloTool/TrackDepositInCaloTool"};

    // --- External tools ---
    ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool{this,"TrackIsolationTool","xAOD::TrackIsolationTool"};
    ToolHandle<Trk::ITrackSelectorTool>   m_trkSelTool{this,"TrackSelectorTool","InDet::InDetDetailedTrackSelectorTool/CaloTrkMuIdAlgTrackSelectorTool"};
  };

}	// end of namespace

#endif


