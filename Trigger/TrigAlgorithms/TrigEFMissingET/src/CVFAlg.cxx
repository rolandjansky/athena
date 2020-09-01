/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "CVFAlg.h"
#include "StoreGate/DecorKeyHelpers.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteDecorHandle.h"
#include "AthContainers/ConstDataVector.h"

// NB: We should try and get something like the faster binned phi approach used
// in the PFlow construction. Of course, we might *really* want to just find a
// way of sharing that information? (for the track <-> cluster association)
namespace
{
  bool fastDR(
      double eta,
      double phi,
      const IExtendTrackToLayerTool::TrackExtension &track,
      double dr,
      double dr2)
  {
    double dEta = std::abs(eta - track.eta);
    if (dEta > dr)
      return false;
    double dPhi = std::abs(phi - track.phi);
    if (dPhi > dr)
      return false;
    return (dEta * dEta + dPhi * dPhi) < dr2;
  }
} // namespace

namespace HLT
{
  namespace MET
  {
    CVFAlg::CVFAlg(const std::string &name, ISvcLocator *pSvcLocator)
        : AthReentrantAlgorithm(name, pSvcLocator)
    {
    }

    StatusCode CVFAlg::initialize()
    {
      ATH_MSG_DEBUG("Initializing " << name());
      CHECK(m_extensionTool.retrieve());
      if (!m_trackSelTool.empty())
      {
        CHECK(m_trackSelTool.retrieve());
        m_useTrackSelTool = true;
      }
      CHECK(m_tvaTool.retrieve());
      CHECK(m_inputClusterKey.initialize());
      CHECK(m_inputTrackKey.initialize());
      CHECK(m_inputVertexKey.initialize());
      if (m_outputCVFKey.key().find(".") == std::string::npos)
        m_outputCVFKey = m_inputClusterKey.key() + "." + m_outputCVFKey.key();
      else if (SG::contKeyFromKey(m_outputCVFKey.key()) != m_inputClusterKey.key())
      {
        ATH_MSG_ERROR("CVF key does not match input cluster key!");
        return StatusCode::FAILURE;
      }
      CHECK(m_outputCVFKey.initialize());
      return StatusCode::SUCCESS;
    }

    StatusCode CVFAlg::execute(const EventContext &ctx) const
    {
      using TrackExtension = IExtendTrackToLayerTool::TrackExtension;

      auto clusters = SG::makeHandle(m_inputClusterKey, ctx);
      if (!clusters.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_inputClusterKey);
        return StatusCode::FAILURE;
      }
      auto tracks = SG::makeHandle(m_inputTrackKey, ctx);
      if (!tracks.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_inputTrackKey);
        return StatusCode::FAILURE;
      }
      auto vertexHandle = SG::makeHandle(m_inputVertexKey, ctx);
      if (!vertexHandle.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_inputVertexKey);
        return StatusCode::FAILURE;
      }
      auto decCVF = SG::makeHandle<float>(m_outputCVFKey, ctx);

      // Find the primary vertex
      const xAOD::Vertex *priVtx = nullptr;
      std::vector<const xAOD::Vertex *> vertices;
      vertices.reserve(vertexHandle->size());
      for (const xAOD::Vertex *vtx : *vertexHandle)
      {
        if (!priVtx && vtx->vertexType() == xAOD::VxType::PriVtx)
          priVtx = vtx;
        vertices.push_back(vtx);
      }
      if (!priVtx)
      {
        // If we fail to find a primary vertex then we give every cluster a
        // CVF value of -1
        for (const xAOD::CaloCluster *iclus : *clusters)
          decCVF(*iclus) = -1;
        return StatusCode::SUCCESS;
      }

      const xAOD::TrackParticleContainer *filteredTracks = nullptr;
      ConstDataVector<xAOD::TrackParticleContainer> viewTracks(SG::VIEW_ELEMENTS);
      if (m_useTrackSelTool)
      {
        for (const xAOD::TrackParticle *track : *tracks)
          if (m_trackSelTool->accept(*track))
            viewTracks.push_back(track);
        filteredTracks = viewTracks.asDataVector();
      }
      else
        filteredTracks = tracks.ptr();

      // To speed things up later, check the maximum positive and negative range of
      // the track etas. This means we can avoid too many loops and dR comparisons
      // later on.
      double minTrkEta = std::numeric_limits<double>::infinity();
      double maxTrkEta = -std::numeric_limits<double>::infinity();
      ATH_MSG_DEBUG("Building extrapolation");
      std::vector<std::tuple<bool, const xAOD::TrackParticle *, TrackExtension>> extensions;
      extensions.reserve(filteredTracks->size());
      for (const xAOD::TrackParticle *track : *filteredTracks)
      {
        TrackExtension ext = m_extensionTool->extendTrack(ctx, *track);
        if (!ext.isValid())
          continue;
        minTrkEta = std::min(minTrkEta, ext.eta);
        maxTrkEta = std::max(maxTrkEta, ext.eta);
        bool isPV;
        if (m_useCompatible)
          isPV = m_tvaTool->isCompatible(*track, *priVtx);
        else
          isPV = m_tvaTool->getUniqueMatchVertex(*track, vertices) == priVtx;
        extensions.push_back(std::make_tuple(isPV, track, ext));
      }

      double DR2 = m_clusterMatchDR.value() * m_clusterMatchDR.value();
      for (const xAOD::CaloCluster *iclus : *clusters)
      {
        double eta = iclus->eta();
        if (iclus->e() ||
            eta - m_clusterMatchDR > maxTrkEta ||
            eta + m_clusterMatchDR < minTrkEta)
        {
          decCVF(*iclus) = -1;
          continue;
        }

        double phi = iclus->phi();
        double totalSum = 0;
        double PVSum = 0;
        for (const std::tuple<bool, const xAOD::TrackParticle *, TrackExtension> &ext : extensions)
        {
          // match the cluster to the track
          if (!fastDR(eta, phi, std::get<2>(ext), m_clusterMatchDR, DR2))
            continue;
          totalSum += std::get<1>(ext)->pt();
          if (std::get<0>(ext))
            PVSum += std::get<1>(ext)->pt();
        }
        decCVF(*iclus) = totalSum == 0 ? -1 : PVSum / totalSum;
      }
      return StatusCode::SUCCESS;
    }
  } // namespace MET
} // namespace HLT