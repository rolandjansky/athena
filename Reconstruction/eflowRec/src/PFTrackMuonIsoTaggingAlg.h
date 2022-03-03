/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_PFTRACKMUONISOTAGGINGALG_H
#define EFLOWREC_PFTRACKMUONISOTAGGINGALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"

class PFTrackMuonIsoTaggingAlg : public AthReentrantAlgorithm
{
public:
    PFTrackMuonIsoTaggingAlg(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~PFTrackMuonIsoTaggingAlg() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &ctx) const override;

private:
    /// The input track collection
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inputTracksKey{
        this, "InputTracks", "", "Input track collection"};
    /// Input cluster collection
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_inputClustersKey{
        this, "InputClusters", "", "Input cluster collection"};
    /// Input vertex collection
    SG::ReadHandleKey<xAOD::VertexContainer> m_inputVerticesKey{
        this, "InputVertices", "", "Input vertex collection"};
    /// The output tracks, excluding those identified as muons
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_outputTracksKey{
        this, "OutputTracks", "", "Output tracks, excluding those tagged as muons"};

    ToolHandle<xAOD::ITrackIsolationTool> m_trkIsoTool{
        this, "TrackIsoTool", "", "The track isolation tool"};

    ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_caloIsoTool{
        this, "CaloIsoTool", "", "The calorimeter isolation tool"};
    /** HLT track isolation threshold **/
    Gaudi::Property<float> m_hltTrkIsoThresh{this, "HLTTrackIsoThreshold", 0.1, "The HLT threshold to consider tracks as isolated"};

    /** HLT fake track relative error threshold **/
    Gaudi::Property<float> m_hltFakeTrkRErr{this, "HLTFakeTrackRErr", 0.4, "The relative error threshold for rejecting fake tracks"};

    /** HLT E over P selection for isolated muon tracks **/
    Gaudi::Property<float> m_hltIsoMuonEOverP{this, "HLTIsoMuonEOverP", 0.65, "The upper limit on E over P for identifying isolated HLT muon tracks"};

    /** HLT E over P selection for non-isolated muon tracks **/
    Gaudi::Property<float> m_hltNonIsoMuonEOverP{
        this, "HLTNonIsoMuonEOverP", 0.6, "The upper limit on E over P for identifying non-isolated HLT muon tracks"};

    /** HLT track pt fraction for non-isolated muon tracks **/
    Gaudi::Property<float> m_hltNonIsoMuonTrkPtFraction{
        this, "HLTNonIsoMuonTrkPtFraction", 0.6, "The lower limit on the track pt fraction in the central track for HLT muons"};

    Gaudi::Property<float> m_minPt{
        this, "MinPt", 10 * Gaudi::Units::GeV, "Only consider tracks above this value for muon selection (all below go into the output)"};

}; //> end class PFTrackMuonIsoTaggingAlg

#endif //> !EFLOWREC_PFTRACKMUONISOTAGGINGALG_H