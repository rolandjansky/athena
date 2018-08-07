/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgAnalysisAlgorithms/AsgLeptonTrackSelectionAlg.h>

#include <xAODEgamma/Electron.h>
#include <xAODEventInfo/EventInfo.h>
#include <xAODMuon/Muon.h>
#include <xAODTracking/TrackParticlexAODHelpers.h>
#include <xAODTracking/VertexContainer.h>

//
// method implementations
//

namespace CP
{
  AsgLeptonTrackSelectionAlg ::
  AsgLeptonTrackSelectionAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("maxD0Significance", m_maxD0Significance, "maximum d0 significance (or 0 for no cut)");
    declareProperty ("maxDeltaZ0SinTheta", m_maxDeltaZ0SinTheta, "maximum Delta z0 sin theta (or 0 for no cut)");
    declareProperty ("selectionDecoration", m_selectionDecoration, "the decoration for the asg selection");
  }



  StatusCode AsgLeptonTrackSelectionAlg ::
  initialize ()
  {
    if (m_selectionDecoration.empty())
    {
      ANA_MSG_ERROR ("no selection decoration name set");
      return StatusCode::FAILURE;
    }
    ANA_CHECK (makeSelectionAccessor (m_selectionDecoration, m_selectionAccessor));

    if (m_maxD0Significance < 0 || !std::isfinite (m_maxD0Significance))
    {
      ATH_MSG_ERROR ("invalid value of maxD0Significance: " << m_maxD0Significance);
      return StatusCode::FAILURE;
    }
    if (m_maxDeltaZ0SinTheta < 0 || !std::isfinite (m_maxDeltaZ0SinTheta))
    {
      ATH_MSG_ERROR ("invalid value of maxDeltaZ0SinTheta: " << m_maxDeltaZ0SinTheta);
      return StatusCode::FAILURE;
    }

    m_accept.addCut ("trackRetrieval", "whether the track retrieval failed");
    if (m_maxD0Significance > 0)
      m_accept.addCut ("maxD0Significance", "maximum D0 significance cut");
    if (m_maxDeltaZ0SinTheta > 0)
      m_accept.addCut ("maxDeltaZ0SinTheta", "maximum Delta z0 sin theta cut");

    m_systematicsList.addHandle (m_particlesHandle);
    ANA_CHECK (m_systematicsList.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode AsgLeptonTrackSelectionAlg ::
  execute ()
  {
    const xAOD::EventInfo *eventInfo {};
    if (m_maxD0Significance > 0)
      ANA_CHECK (evtStore()->retrieve (eventInfo, "EventInfo"));

    const xAOD::Vertex *primaryVertex {};
    if (m_maxDeltaZ0SinTheta > 0)
    {
      const xAOD::VertexContainer *vertices {};
      ANA_CHECK (evtStore()->retrieve (vertices, "PrimaryVertices"));
      for (const xAOD::Vertex *vertex : *vertices)
      {
        if (vertex->vertexType() == xAOD::VxType::PriVtx)
        {
          /// \todo If there is more than one primary vertex
          /// available, this will pick up the primary vertex
          /// (semi-)randomly.  The tracking group recommends to pick
          /// the one with the maximum sum-pt, but I don't have any
          /// example of that, and anyways we probably want to make
          /// sure we pick it up once and provide it centrally.
          /// Again, I'm not quite sure how to do that.  This should
          /// probably be addressed at some point in the future (06
          /// Aug 18).
          if (primaryVertex == nullptr)
            primaryVertex = vertex;
        }
      }
    }

    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::IParticleContainer *particles = nullptr;
        ANA_CHECK (m_particlesHandle.getCopy (particles, sys));
        for (xAOD::IParticle *particle : *particles)
        {
          m_accept.clear();
          std::size_t cutIndex {};

          const xAOD::TrackParticle *track {};
          if (const xAOD::Muon *muon = dynamic_cast<xAOD::Muon*>(particle))
            track = muon->primaryTrackParticle();
          else if (const xAOD::Electron *electron = dynamic_cast<xAOD::Electron*>(particle))
            track = electron->trackParticle();
          else
          {
            ANA_MSG_ERROR ("failed to cast input to electron or muon");
            return StatusCode::FAILURE;
          }

          m_accept.setCutResult (cutIndex ++, track != nullptr);
          if (track != nullptr)
          {
            if (m_maxD0Significance > 0)
            {
              const float d0sig = xAOD::TrackingHelpers::d0significance
                (track, eventInfo->beamPosSigmaX(), eventInfo->beamPosSigmaY(),
                 eventInfo->beamPosSigmaXY());
              m_accept.setCutResult (cutIndex ++, d0sig < m_maxD0Significance);
            }
            if (m_maxDeltaZ0SinTheta > 0)
            {
              const double vertex_z = primaryVertex ? primaryVertex->z() : 0;
              const float deltaZ0SinTheta
                = (track->z0() + track->vz() - vertex_z) * sin (particle->p4().Theta());
              m_accept.setCutResult (cutIndex ++, fabs (deltaZ0SinTheta) < m_maxDeltaZ0SinTheta);
            }
          }

          m_selectionAccessor->setBits
            (*particle, selectionFromAccept (m_accept));
        }
        return StatusCode::SUCCESS;
      });
  }
}
