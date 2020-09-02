/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    declareProperty ("nMinPixelHits", m_nMinPixelHits, "minimum number of required Pixel hits (or -1 for no cut)");
    declareProperty ("nMaxPixelHits", m_nMaxPixelHits, "minimum number of required Pixel hits (or -1 for no cut)");
    declareProperty ("nMinSCTHits", m_nMinSCTHits, "minimum number of required SCT hits (or -1 for no cut)");
    declareProperty ("nMaxSCTHits", m_nMaxSCTHits, "minimum number of required SCT hits (or -1 for no cut)");
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
    if (m_nMinPixelHits != -1 || m_nMaxPixelHits != -1)
      m_accept.addCut ("numPixelHits", "Minimum and/or maxiumum Pixel hits");
    if (m_nMinSCTHits != -1 || m_nMaxSCTHits != -1)
      m_accept.addCut ("numSCTHits", "Minimum and/or maxiumum SCT hits");

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
          asg::AcceptData acceptData (&m_accept);

          if (m_preselection.getBool (*particle))
          {
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

            acceptData.setCutResult (cutIndex ++, track != nullptr);
            if (track != nullptr)
            {
              if (m_maxD0Significance > 0)
              {
                try
                {
                  const float d0sig = xAOD::TrackingHelpers::d0significance
                    (track, eventInfo->beamPosSigmaX(), eventInfo->beamPosSigmaY(),
                    eventInfo->beamPosSigmaXY());
                  acceptData.setCutResult (cutIndex ++, fabs( d0sig ) < m_maxD0Significance);
                } catch (const std::runtime_error &) {
                  acceptData.setCutResult (cutIndex ++, false);
                }
              }
              if (m_maxDeltaZ0SinTheta > 0)
              {
                const double vertex_z = primaryVertex ? primaryVertex->z() : 0;
                const float deltaZ0SinTheta
                  = (track->z0() + track->vz() - vertex_z) * sin (particle->p4().Theta());
                acceptData.setCutResult (cutIndex ++, fabs (deltaZ0SinTheta) < m_maxDeltaZ0SinTheta);
              }
              if (m_nMinPixelHits != -1 || m_nMaxPixelHits != -1) {
                uint8_t nPixelHits;
                track->summaryValue(nPixelHits, xAOD::numberOfPixelHits);
                bool accept = true;
                if(m_nMinPixelHits != -1) {
                  accept &= nPixelHits >= m_nMinPixelHits;
                }
                if(m_nMaxPixelHits != -1) {
                  accept &= nPixelHits <= m_nMaxPixelHits;
                }
                acceptData.setCutResult (cutIndex++, accept);
              }
              if (m_nMinSCTHits != -1 || m_nMaxSCTHits != -1) {
                uint8_t nSCTHits;
                track->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
                bool accept = true;
                if(m_nMinSCTHits != -1) {
                  accept &= nSCTHits >= m_nMinSCTHits;
                }
                if(m_nMaxSCTHits != -1) {
                  accept &= nSCTHits <= m_nMaxSCTHits;
                }
                acceptData.setCutResult (cutIndex++, accept);
              }
            }
          }

          m_selectionAccessor->setBits
              (*particle, selectionFromAccept (acceptData));
        }
        return StatusCode::SUCCESS;
      });
  }
}
