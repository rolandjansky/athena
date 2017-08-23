/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// Thin_vtxTrk.cxx
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// This is a trivial example of an implementation of a thinning tool
// which removes all ID tracks which do not pass a user-defined cut

#include "DerivationFrameworkBPhys/Thin_vtxTrk.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODBPhys/BPhysHypoHelper.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::Thin_vtxTrk::Thin_vtxTrk(const std::string& t, const std::string& n, const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
  m_ntot(0),
  m_npass(0),
  m_acceptanceR(-1.),  // Do not add tracks within a cone from the vertex by default
  m_noFlags(false),
  m_nVtxTot(0),
  m_nVtxPass(0)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  
  declareProperty("ThinningService", m_thinningSvc);
  
  declareProperty("TrackParticleContainerName", m_trackParticleContainerName = "InDetTrackParticles");
  declareProperty("VertexContainerNames"      , m_vertexContainerNames);
  declareProperty("PassFlags"                 , m_passFlags);
  declareProperty("AcceptanceRadius"          , m_acceptanceR);
  declareProperty("IgnoreFlags"                   , m_noFlags);
  declareProperty("ApplyAnd"                  , m_and = false);
  declareProperty("ApplyAndForTracks"         , m_trackAnd = false);
  declareProperty("ThinTracks"                , m_thinTracks = true);
}

// Destructor
DerivationFramework::Thin_vtxTrk::~Thin_vtxTrk() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::Thin_vtxTrk::initialize()
{
  // Decide which collections need to be checked for ID TrackParticles
  ATH_MSG_VERBOSE("initialize() ...");
  
  if (m_trackParticleContainerName=="") {
    ATH_MSG_FATAL("No inner detector track collection provided for thinning.");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Using " << m_trackParticleContainerName << "as the source collection for inner detector track particles");
  }
  
  if (m_vertexContainerNames.empty()) {
    ATH_MSG_FATAL("No vertex collections provided for thinning.");
    return StatusCode::FAILURE;
  } else {
    for(std::vector<std::string>::iterator itr = m_vertexContainerNames.begin(); itr!=m_vertexContainerNames.end(); ++itr) {
      ATH_MSG_INFO("Using " << *itr << " as the source vertex collection for inner detector track particles");
    }
  }
  
  if( m_noFlags){
    ATH_MSG_INFO("IgnoreFlags is set, all vertices in the container will be kept");
  }

  if( ! m_noFlags){
    if (m_passFlags.empty()) {
      ATH_MSG_FATAL("No pass flags provided for thinning.");
      return StatusCode::FAILURE;
    } else {
      for(std::vector<std::string>::iterator itr = m_passFlags.begin(); itr!=m_passFlags.end(); ++itr) {
        ATH_MSG_INFO("Vertices must pass the \"" << *itr << "\" selection");
      }
    }
  }
  
  if (m_acceptanceR > 0.) {
      ATH_MSG_INFO("Extra tracks must be within cone of "<<m_acceptanceR<<" from vertex candidate.");
  }

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::Thin_vtxTrk::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
  ATH_MSG_INFO("Processed "<< m_nVtxTot <<" vertices, "<< m_nVtxPass<< " were retained ");
  
  return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::Thin_vtxTrk::doThinning() const
{
  // Retrieve main TrackParticle collection
  const xAOD::TrackParticleContainer* importedTrackParticles;
  CHECK( evtStore()->retrieve(importedTrackParticles,m_trackParticleContainerName) );
  
  // Check the event contains tracks
  unsigned int nTracks = importedTrackParticles->size();
  if (nTracks==0) return StatusCode::SUCCESS;
  
  // Set up a trackMask with the same entries as the full TrackParticle collection
  std::vector<bool> trackMask;
  trackMask.assign(nTracks,false); // default: don't keep any tracks
  m_ntot += nTracks;
  
  // Retrieve vertex containers
  for(std::vector<std::string>::const_iterator itr = m_vertexContainerNames.begin(); itr!=m_vertexContainerNames.end(); ++itr) {
    // retieve vertex 
    xAOD::VertexContainer* vertexContainer;
    CHECK( evtStore()->retrieve(vertexContainer, *itr) );
    std::vector<bool> vtxMask;
    vtxMask.assign(vertexContainer->size(), false); // default: don't keep any vertices
    
    // loop over vertices
    int k = 0;
    for(xAOD::VertexContainer::iterator vtxItr = vertexContainer->begin(); vtxItr!=vertexContainer->end(); ++vtxItr, ++k) {
      xAOD::Vertex* vtx = *vtxItr;
      m_nVtxTot++;
      
      // check if the vertex passed the required selections criteria
      bool passed = false;
      for(std::vector<std::string>::const_iterator flagItr = m_passFlags.begin(); flagItr!=m_passFlags.end(); ++flagItr) {
        SG::AuxElement::Accessor<Char_t> flagAcc(*flagItr);
        if(flagAcc.isAvailable(*vtx) && flagAcc(*vtx) != 0) {
          passed = true;
          break;
        }
      } // end of loop over flags
      
      if(passed || m_noFlags) {
        // vertex passed the selection
    	vtxMask[k] = true;
    	m_nVtxPass++;

        // Add tracks according to DR selection
        if(m_acceptanceR > 0.){

          // determine the sum of the tracks at vertex as centre for the cone
          TLorentzVector centreCandidate;
          for(uint j=0; j<vtx->nTrackParticles(); ++j) {
	    centreCandidate += vtx->trackParticle(j)->p4();
          }

          for(uint i=0; i<nTracks; ++i) {
	    if(!trackMask[i]) {  // do this only for tracks that haven't been selected, yet
	      const xAOD::TrackParticle* track = (*importedTrackParticles)[i];
	      if(centreCandidate.DeltaR(track->p4()) < m_acceptanceR) trackMask[i]= true;
	    }
          }
        }// end adding tracks according to DR selection

        if(m_thinTracks) {
          // loop over all tracks
          for(uint i=0; i<nTracks; ++i) {
            if(!trackMask[i]) {  // do this only for tracks that haven't been selected, yet
              const xAOD::TrackParticle* track = (*importedTrackParticles)[i];
              // loop over tracks at vertex
              for(uint j=0; j<vtx->nTrackParticles(); ++j) {
                if(vtx->trackParticle(j) == track) {
                  trackMask[i] = true;  // accept track
                }
              } // end of loop over tracks at vertex
            }
          } // end of loop over all tracks
        }
      }
    } // end of loop over vertices
    
    // Execute the thinning service based on the vtxMask.
    if (m_and) {
      if (m_thinningSvc->filter(*vertexContainer, vtxMask, IThinningSvc::Operator::And).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service for vertices failed! ");
        return StatusCode::FAILURE;
      }
    }
    if (!m_and) {
      if (m_thinningSvc->filter(*vertexContainer, vtxMask, IThinningSvc::Operator::Or).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service for vertices failed! ");
        return StatusCode::FAILURE;
      }
    }
    
  } // end of loop over vertex containers
  
  // Count up the trackMask contents
  for (unsigned int i=0; i<nTracks; ++i) {
    if (trackMask[i]) ++m_npass;
  }
  
  if(m_thinTracks || m_acceptanceR > 0.) {
    // Execute the thinning service based on the trackMask. Finish.
    if (m_trackAnd) {
      if (m_thinningSvc->filter(*importedTrackParticles, trackMask, IThinningSvc::Operator::And).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service for tracks failed! ");
        return StatusCode::FAILURE;
      }
    }
    if (!m_trackAnd) {
      if (m_thinningSvc->filter(*importedTrackParticles, trackMask, IThinningSvc::Operator::Or).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service for tracks failed! ");
        return StatusCode::FAILURE;
      }
    }
  }

  return StatusCode::SUCCESS;
}

