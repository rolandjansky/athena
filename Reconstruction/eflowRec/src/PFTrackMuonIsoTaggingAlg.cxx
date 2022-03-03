/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PFTrackMuonIsoTaggingAlg.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthContainers/ConstDataVector.h"
#include <memory>
#include "EventPrimitives/EventPrimitivesHelpers.h"

PFTrackMuonIsoTaggingAlg::PFTrackMuonIsoTaggingAlg(const std::string &name, ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
{
}

PFTrackMuonIsoTaggingAlg::~PFTrackMuonIsoTaggingAlg() {}

StatusCode PFTrackMuonIsoTaggingAlg::initialize()
{
  ATH_CHECK(m_inputTracksKey.initialize());
  ATH_CHECK(m_inputClustersKey.initialize());
  ATH_CHECK(m_inputVerticesKey.initialize());
  ATH_CHECK(m_outputTracksKey.initialize());
  ATH_CHECK(m_trkIsoTool.retrieve());
  ATH_CHECK(m_caloIsoTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode PFTrackMuonIsoTaggingAlg::execute(const EventContext &ctx) const
{
  auto inputTracks = SG::makeHandle(m_inputTracksKey, ctx);
  if (!inputTracks.isValid())
  {
    ATH_MSG_ERROR("Failed to retrieve " << m_inputTracksKey);
    return StatusCode::FAILURE;
  }
  auto inputClusters = SG::makeHandle(m_inputClustersKey, ctx);
  if (!inputClusters.isValid())
  {
    ATH_MSG_ERROR("Failed to retrieve " << m_inputClustersKey);
    return StatusCode::FAILURE;
  }
  auto inputVertices = SG::makeHandle(m_inputVerticesKey, ctx);
  if (!inputVertices.isValid())
  {
    ATH_MSG_ERROR("Failed to retrieve " << m_inputVerticesKey);
    return StatusCode::FAILURE;
  }
  auto outputHandle = SG::makeHandle(m_outputTracksKey, ctx);
  const xAOD::Vertex *primaryVtx = nullptr;
  for (const xAOD::Vertex *ivtx : *inputVertices)
  {
    if (ivtx->vertexType() == xAOD::VxType::PriVtx)
    {
      primaryVtx = ivtx;
      break;
    }
  }

  auto outputTracks = std::make_unique<ConstDataVector<xAOD::TrackParticleContainer>>(SG::VIEW_ELEMENTS);
  for (const xAOD::TrackParticle *itrk : *inputTracks)
  {
    if (itrk->pt() > m_minPt)
    {
      xAOD::TrackIsolation trackIsoResult;
      std::vector<xAOD::Iso::IsolationType> trackIsoTypes{xAOD::Iso::ptcone20};
      xAOD::TrackCorrection trackIsoCorr;
      trackIsoCorr.trackbitset.set(xAOD::Iso::IsolationTrackCorrection::coreTrackPtr);
      m_trkIsoTool->trackIsolation(trackIsoResult, *itrk, trackIsoTypes, trackIsoCorr, primaryVtx, nullptr, inputTracks.ptr());
      float ptCone20 = trackIsoResult.ptcones[0];
      xAOD::CaloIsolation caloIsoResult;
      xAOD::CaloCorrection caloIsoCorr;
      caloIsoCorr.calobitset.set(xAOD::Iso::IsolationCaloCorrection::coreCone);
      m_caloIsoTool->caloTopoClusterIsolation(caloIsoResult, *itrk, {xAOD::Iso::IsolationType::etcone20}, caloIsoCorr, inputClusters.ptr());
      float etConeCore = caloIsoResult.coreCorrections[xAOD::Iso::IsolationCaloCorrection::coreCone][xAOD::Iso::IsolationCorrectionParameter::coreEnergy];
      float rerr = Amg::error(itrk->definingParametersCovMatrix(), 4) / fabs(itrk->qOverP());
      float eOverP = etConeCore / itrk->pt();
      float trackIso = ptCone20 / itrk->pt();
      float ptsum = itrk->pt() + ptCone20;
      float caloIso = etConeCore / ptsum;
      float trkPtFraction = itrk->pt() / ptsum;

      bool fakeTrack = false;
      bool muonTrack = false;
      if (trackIso < m_hltTrkIsoThresh)
      {
        if (rerr > m_hltFakeTrkRErr)
          fakeTrack = true;
        else if (eOverP < m_hltIsoMuonEOverP)
          muonTrack = true;
        else if (rerr > 0.1 || (eOverP > 0.1 && rerr > 0.05))
          fakeTrack = true;
      }
      else
      {
        muonTrack = caloIso < m_hltNonIsoMuonEOverP && trkPtFraction > m_hltNonIsoMuonTrkPtFraction;
      }
      if (muonTrack || fakeTrack)
        continue;
    }
    outputTracks->push_back(itrk);
  }
  ATH_CHECK(outputHandle.put(ctx, std::move(outputTracks)) != nullptr);

  return StatusCode::SUCCESS;
}