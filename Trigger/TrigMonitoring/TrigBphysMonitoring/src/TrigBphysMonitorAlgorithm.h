/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBPHYSMONITORING_TRIGBPHYSMONITORALGORITHM_H
#define TRIGBPHYSMONITORING_TRIGBPHYSMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODTrigBphys/TrigBphysContainer.h" 
#include "xAODTrigBphys/TrigBphys.h"

#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"

class TrigBphysMonitorAlgorithm : public AthMonitorAlgorithm {
public:
  TrigBphysMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigBphysMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  
private:
  SG::ReadHandleKeyArray<xAOD::TrigBphysContainer> m_TrigBphysContainerKeys;
  
  SG::ReadHandleKey<xAOD::MuonContainer> m_offlineMuonCollectionKey{this, "offlineMuonCollectionKey", "Muons"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_offlineIDTrackCollectionKey{this, "offlineIDTrackCollectionKey", "InDetTrackParticles"};
  SG::ReadHandleKey<xAOD::VertexContainer> m_offlinePvCollectionKey {this,"offlinePvCollectionKey","PrimaryVertices"};
  
  Gaudi::Property<std::vector<std::string>> m_ContainerNames{this, "ContainerNames", {}};
  
  Gaudi::Property<std::vector<std::string>> m_ChainNames_MuMu{this, "ChainNames_MuMu", {}};
  Gaudi::Property<std::vector<std::string>> m_ChainNames_MuMuX{this, "ChainNames_MuMuX", {}};
  Gaudi::Property<std::vector<std::string>> m_ChainNames_ElEl{this, "ChainNames_ElEl", {}};
  
  Gaudi::Property<double> m_dimuMassLower_prefit{this, "dimuMassLower_prefit", 0.};
  Gaudi::Property<double> m_dimuMassUpper_prefit{this, "dimuMassUpper_prefit", 16000.};
  Gaudi::Property<double> m_dimuMassLower_postfit{this, "dimuMassLower_postfit", 0.};
  Gaudi::Property<double> m_dimuMassUpper_postfit{this, "dimuMassUpper_postfit", 15000.};
  Gaudi::Property<double> m_dimuChi2Cut{this, "dimuChi2Cut", 20.};
  
  Gaudi::Property<double> m_deltaRMatchingThreshold{this, "deltaRMatchingThreshold", 0.05};
  
  ToolHandle<InDet::VertexPointEstimator> m_vertexPointEstimator {this, "VertexPointEstimator", "", "tool to find starting point for the vertex fitter"};
  ToolHandle<Trk::TrkVKalVrtFitter> m_vertexFitter {this, "VertexFitter", "", "VKalVrtFitter tool to fit tracks into the common vertex"};
  ToolHandle<Trk::V0Tools> m_v0Tools {this, "V0Tools", "", "V0 tools to calculate things like Lxy"}; /// For lxy etc
  
  StatusCode fillContainers(const EventContext& ctx) const;
  StatusCode fillContainerHists(const EventContext& ctx, const SG::ReadHandleKey<xAOD::TrigBphysContainer>& trigBphysContainerKey) const;
  
  StatusCode fillChains(const EventContext& ctx) const;
  StatusCode fillDimuonChainHists(const EventContext& ctx, const std::string& chainName) const;
  StatusCode fillBmumuxChainHists(const EventContext& ctx, const std::string& chainName) const;
  StatusCode fillDielectronChainHists(const EventContext& ctx, const std::string& chainName) const;
  StatusCode fillChainGenericHists(const EventContext& /*ctx*/, const ToolHandle<GenericMonitoringTool>& currentMonGroup, const std::string& chainName) const;
  StatusCode fillBphysObjectHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const ElementLink<xAOD::TrigBphysContainer>& bphysLink, const std::string& objStr) const;
  StatusCode fillTrigLeptonHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const ElementLink<xAOD::TrigBphysContainer>& bphysLink, const std::string& name_prefix) const;
  StatusCode fillTrigBmumuxTrkHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const ElementLink<xAOD::TrigBphysContainer>& bphysLink, UInt_t tracksStartFrom = 2) const;
  StatusCode fillDiTracksHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, 
                               const std::vector<ElementLink<xAOD::TrackParticleContainer> >& tpLinkVector,
                               const std::string& name_prefix) const;
  StatusCode fillTracksHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, 
                           const std::vector<ElementLink<xAOD::TrackParticleContainer> >& tpLinkVector, 
                           const std::string& prefix, 
                           bool separateHists = false, 
                           UInt_t offsetIndex = 0) const;
  StatusCode fillTrkHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const xAOD::TrackParticle* trk, const std::string& name_prefix) const;
  
  StatusCode fillOfflineDimuons(const EventContext& ctx, const std::vector<std::unique_ptr<xAOD::Vertex>>& dimuonContainer) const;
  StatusCode fillOfflineDimuonHists(const EventContext& /*ctx*/, const std::string& dimuonMonGroupName, const std::vector<std::unique_ptr<xAOD::Vertex>>& dimuonContainer) const;
  StatusCode fillVertexHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const xAOD::Vertex* vertex, const std::string& objStr) const;
  
  StatusCode buildDimuons(const EventContext& ctx, std::vector<std::unique_ptr<xAOD::Vertex>>& vxContainer) const;
  std::unique_ptr<xAOD::Vertex> dimuonFit(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2) const;
  double dimuonMass(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2) const;
  std::vector<const xAOD::Vertex*> GetGoodPVs(const xAOD::VertexContainer* pvContainer) const;
  const xAOD::Vertex* getPvForDimuon_lowestA0(const xAOD::Vertex* vtx, const std::vector<const xAOD::Vertex*>& PVs) const;
  
  bool matchDimuon(const xAOD::Vertex* dimuonVertex, const std::string& chainName) const;
  bool matchDimuon(const xAOD::Vertex* dimuonVertex, const ElementLink<xAOD::TrigBphysContainer>& bphysLink) const;
  bool matchTrackParticles(const xAOD::TrackParticle* trk1, const xAOD::TrackParticle* trk2) const;
  
};
#endif
