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

#include "xAODBPhys/BPhysHypoHelper.h"
#include "StoreGate/ThinningHandle.h"
#include <vector>
#include <string>
// Constructor
DerivationFramework::Thin_vtxTrk::Thin_vtxTrk(const std::string& t, const std::string& n, const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_acceptanceR(-1.),  // Do not add tracks within a cone from the vertex by default
  m_nVtxTot(0),
  m_nVtxPass(0),
  m_noFlags(false)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  
  declareProperty("TrackParticleContainerName", m_trackParticleContainerName = "InDetTrackParticles");
  declareProperty("VertexContainerNames"      , m_vertexContainerName);
  declareProperty("PassFlags"                 , m_passFlags);
  declareProperty("AcceptanceRadius"          , m_acceptanceR);
  declareProperty("IgnoreFlags"                   , m_noFlags);
  declareProperty("ApplyAnd"                  , m_and = false);
  declareProperty("ApplyAndForTracks"         , m_trackAnd = false);
  declareProperty("ThinTracks"                , m_thinTracks = true);
}

// Destructor
DerivationFramework::Thin_vtxTrk::~Thin_vtxTrk() = default;

// Athena initialize and finalize
StatusCode DerivationFramework::Thin_vtxTrk::initialize()
{
  // Decide which collections need to be checked for ID TrackParticles
  ATH_MSG_VERBOSE("initialize() ...");
  ATH_CHECK(m_trackParticleContainerName.initialize(m_streamName));


  if( m_noFlags){
    ATH_MSG_INFO("IgnoreFlags is set, all vertices in the container will be kept");
  }

  if( ! m_noFlags){
    if (m_passFlags.empty()) {
      ATH_MSG_FATAL("No pass flags provided for thinning.");
      return StatusCode::FAILURE;
    } else {
      for(auto itr = m_passFlags.begin(); itr!=m_passFlags.end(); ++itr) {
        ATH_MSG_INFO("Vertices must pass the \"" << *itr << "\" selection");
      }
    }
  }
  
  if (m_acceptanceR > 0.) {
      ATH_MSG_INFO("Extra tracks must be within cone of "<<m_acceptanceR<<" from vertex candidate.");
  }

  for(auto &handle : m_vertexContainerName){
    ATH_CHECK(handle.initialize(m_streamName));
  }
  for(const auto &tracknames : m_vertexContainerName){
     for(const auto &str : m_passFlags){
        m_passArray.emplace_back(tracknames.key() + '.' + str);
     }
  }
  ATH_CHECK(m_passArray.initialize());
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
  SG::ThinningHandle<xAOD::TrackParticleContainer> importedTrackParticles(m_trackParticleContainerName);
  
  // Check the event contains tracks
  unsigned int nTracks = importedTrackParticles->size();
  if (nTracks==0) return StatusCode::SUCCESS;
  
  // Set up a trackMask with the same entries as the full TrackParticle collection
  std::vector<bool> trackMask(nTracks,false); // default: don't keep any tracks
  m_ntot += nTracks;
  int nVtxTot =0;
  int nVtxPass=0;

  std::unordered_map<std::string, SG::ReadDecorHandle<xAOD::VertexContainer, Char_t>> handles;
  handles.reserve(m_passArray.size());
  for(const auto &key : m_passArray){
        auto it = handles.emplace(key.key(), key);
        if(!(*it.first).second.isPresent()) return StatusCode::FAILURE;
  }

  // retieve vertex 
  for(const auto& name : m_vertexContainerName){
    SG::ThinningHandle<xAOD::VertexContainer> vertexContainer(name);
    std::vector<bool> vtxMask(vertexContainer->size(), false); // default: don't keep any vertices
    
    // loop over vertices
    int k = 0;
    for(auto vtxItr = vertexContainer->begin(); vtxItr!=vertexContainer->end(); ++vtxItr, ++k) {
      const xAOD::Vertex* vtx = *vtxItr;
      nVtxTot++;
      
      // check if the vertex passed the required selections criteria
      bool passed = false;
      for(std::vector<std::string>::const_iterator flagItr = m_passFlags.begin(); flagItr!=m_passFlags.end(); ++flagItr) {
        std::string lookupstr = name.key() + '.' + (*flagItr);
        const auto& handle = handles.at(lookupstr);
        if(handle(*vtx) != 0) {
          passed = true;
          break;
        }
      } // end of loop over flags
      
      if(passed || m_noFlags) {
        // vertex passed the selection
    	vtxMask[k] = true;
    	nVtxPass++;

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
      vertexContainer.keep(vtxMask, SG::ThinningHandleBase::Op::And);
    }
    if (!m_and) {
      vertexContainer.keep(vtxMask, SG::ThinningHandleBase::Op::Or);
    }
  }
  
  // Count up the trackMask contents
  m_npass += std::accumulate(trackMask.begin(), trackMask.end(), 0);
  m_nVtxTot += nVtxTot;
  m_nVtxPass+= nVtxPass;
  if(m_thinTracks || m_acceptanceR > 0.) {
    // Execute the thinning service based on the trackMask. Finish.
    if (m_trackAnd) {
      importedTrackParticles.keep(trackMask, SG::ThinningHandleBase::Op::And);
    }
    if (!m_trackAnd) {
      importedTrackParticles.keep(trackMask, SG::ThinningHandleBase::Op::Or);
    }
  }

  return StatusCode::SUCCESS;
}

