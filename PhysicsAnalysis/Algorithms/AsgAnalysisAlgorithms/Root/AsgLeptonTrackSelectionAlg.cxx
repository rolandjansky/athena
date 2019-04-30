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
    declareProperty ("eventInfo", m_eventInfo, "the name of the EventInfo object to retrieve");
    declareProperty ("primaryVertices", m_primaryVertices, "the name of the PrimaryVertex container to retrieve");
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
    ANA_CHECK (m_preselection.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode AsgLeptonTrackSelectionAlg ::
  execute ()
  {
    const xAOD::EventInfo *eventInfo {nullptr};
    if (m_maxD0Significance > 0)
      ANA_CHECK (evtStore()->retrieve (eventInfo, m_eventInfo));

    const xAOD::Vertex *primaryVertex {nullptr};
    if (m_maxDeltaZ0SinTheta > 0)
    {
      const xAOD::VertexContainer *vertices {nullptr};
      ANA_CHECK (evtStore()->retrieve (vertices, m_primaryVertices));
      for (const xAOD::Vertex *vertex : *vertices)
      {
        if (vertex->vertexType() == xAOD::VxType::PriVtx)
        {
          // The default "PrimaryVertex" container is ordered in
          // sum-pt, and the tracking group recommends to pick the one
          // with the maximum sum-pt, so this will do the right thing.
          // If the user needs a different primary vertex, he needs to
          // provide a reordered primary vertex container and point
          // this algorithm to it.  Currently there is no central
          // algorithm to do that, so users will have to write their
          // own (15 Aug 18).
          if (primaryVertex == nullptr)
          {
            primaryVertex = vertex;
            break;
          }
        }
      }
    }

    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::IParticleContainer *particles = nullptr;
        ANA_CHECK (m_particlesHandle.getCopy (particles, sys));
        for (xAOD::IParticle *particle : *particles)
        {
          if (m_preselection.getBool (*particle))
          {
            m_accept.clear();
            std::size_t cutIndex {0};

            const xAOD::TrackParticle *track {nullptr};
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
                m_accept.setCutResult (cutIndex ++, fabs( d0sig ) < m_maxD0Significance);
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
        }
        return StatusCode::SUCCESS;
      });
  }
}
