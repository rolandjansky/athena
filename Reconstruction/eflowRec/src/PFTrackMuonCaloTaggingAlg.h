/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_PFTRACKMUONCALOTAGGINGALG_H
#define EFLOWREC_PFTRACKMUONCALOTAGGINGALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "ICaloTrkMuIdTools/ICaloMuonScoreTool.h"
#include "ICaloTrkMuIdTools/ICaloMuonTag.h"
#include "ICaloTrkMuIdTools/ITrackDepositInCaloTool.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#include "GaudiKernel/SystemOfUnits.h"

/**
 * @brief Algorithm to use calorimeter information to remove potential muons
 * 
 * Outputs a reduced set of tracks, containing only those not tagged as muons
 */
class PFTrackMuonCaloTaggingAlg : public AthReentrantAlgorithm
{
public:
    PFTrackMuonCaloTaggingAlg(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~PFTrackMuonCaloTaggingAlg() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &ctx) const override;

private:
    /// The input track collection
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inputTracksKey{
        this, "InputTracks", "", "Input track collection"};
    /// The input calorimeter extension
    SG::ReadHandleKey<CaloExtensionCollection> m_caloExtensionKey{
        this, "InputCaloExtension", "", "The calorimeter extensions of the tracks"};
    /// Input cells. It's generally more efficient to pass these in this way
    SG::ReadHandleKey<CaloCellContainer> m_inputCellsKey{
        this, "InputCells", "", "Cell container to pass to the other tools"};
    /// The output tracks, excluding those identified as muons
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_outputTracksKey{
        this, "OutputTracks", "", "Output tracks, excluding those tagged as muons"};
    ToolHandle<ICaloMuonTag> m_looseMuonTagTool{
        this, "LooseTagTool", "", "Loose muon tag tool"};
    ToolHandle<ICaloMuonTag> m_tightMuonTagTool{
        this, "TightTagTool", "", "Tight muon tag tool"};
    ToolHandle<ICaloMuonScoreTool> m_muonScoreTool{
        this, "MuonScoreTool", "", "Calo muon score tool"};
    ToolHandle<ITrackDepositInCaloTool> m_depositInCaloTool{
        this, "DepositInCaloTool", "", "Deposit in calo tool"};
    Gaudi::Property<float> m_minPt{
        this, "MinPt", 10 * Gaudi::Units::GeV, "Only consider tracks above this value for muon selection (all below go into the output)"};
    Gaudi::Property<float> m_caloMuonScoreCut{
        this, "MuonScoreCut", 0.4, "Below this value, require a tight calo tag"};

}; //> end class PFTrackMuonCaloTaggingAlg

#endif //> !EFLOWREC_PFTRACKMUONCALOTAGGINGALG_H