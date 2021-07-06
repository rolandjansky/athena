/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// Thin_vtxDuplicates.cxx
///////////////////////////////////////////////////////////////////
// Matteo Bedognetti (matteo.bedognetti@cern.ch)
//Based on Thin_vtxTrk.cxx, by



#include "DerivationFrameworkBPhys/Thin_vtxDuplicates.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBPhys/BPhysHypoHelper.h"
#include <vector>
#include <string>
#include <algorithm>  // for the sort function
#include <iomanip>
#include "StoreGate/ThinningHandle.h"
// Constructor
DerivationFramework::Thin_vtxDuplicates::Thin_vtxDuplicates(const std::string& t, const std::string& n, const IInterface* p ) :
  AthAlgTool(t,n,p),
 // m_acceptanceR(-1.),
  m_noFlags(false),
  m_nVtxTot(0),
  m_nVtxPass(0)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  
  declareProperty("VertexContainerName"      , m_vertexContainerNames);
  declareProperty("PassFlags"                 , m_passFlags);
  //declareProperty("AcceptanceRadius"          , m_acceptanceR);
  declareProperty("ApplyAnd"                  , m_and = true);  //This will be applied depending on the order in which the thinning tools are added to the kernel
  declareProperty("IgnoreFlags"               , m_noFlags);
  //declareProperty("ApplyAndForTracks"         , m_trackAnd = false);
  //declareProperty("ThinTracks"                , m_thinTracks = true);
}

// Destructor
DerivationFramework::Thin_vtxDuplicates::~Thin_vtxDuplicates() = default;

// Athena initialize and finalize
StatusCode DerivationFramework::Thin_vtxDuplicates::initialize()
{
  // Decide which collections need to be checked for ID TrackParticles
  ATH_MSG_VERBOSE("initialize() ...");
  ATH_CHECK(m_vertexContainerNames.initialize(m_streamName));

  
  if (m_passFlags.empty()) {
    ATH_MSG_FATAL("No pass flags provided for thinning.");
    return StatusCode::FAILURE;
  } else {
    for(auto itr = m_passFlags.cbegin(); itr!=m_passFlags.cend(); ++itr) {
      ATH_MSG_INFO("Vertices must pass the \"" << itr->key() << "\" selection");
    }
  }

  for(auto &key : m_passFlags){
        key = m_vertexContainerNames.key() + '.' + key.key();
  }
  ATH_CHECK(m_passFlags.initialize());
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::Thin_vtxDuplicates::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed "<< m_nVtxTot <<" vertices, "<< m_nVtxPass<< " were retained ");
  
  return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::Thin_vtxDuplicates::doThinning() const
{
    // retieve vertex 
    SG::ThinningHandle< xAOD::VertexContainer > vertexContainer(m_vertexContainerNames);
    std::vector<bool> vtxMask(vertexContainer->size(), true); // default: keep all vertices
    int vtxTot = 0;
    int nVtxPass = 0;
    // loop over vertices
    int k = 0;
    std::vector<SG::ReadDecorHandle<xAOD::VertexContainer, Char_t>> handles;
    handles.reserve(m_passFlags.size());
    for(const auto &key : m_passFlags){
        handles.emplace_back(key);
        if(!handles.back().isPresent()) return StatusCode::FAILURE;
    }
    for(auto vtxItr = vertexContainer->cbegin(); vtxItr!=vertexContainer->cend(); ++vtxItr, ++k) {
      const xAOD::Vertex* vtx = *vtxItr;
      // check if the vertex passed the required selections criteria (is run when the vertex is already excluded, because the counter needs the info)
      bool passed = false;
      if(m_noFlags){passed = true; vtxTot++; }
      else{
        for(auto &flagAcc : handles) {
          if(flagAcc(*vtx) != 0) {
            passed = true;
            vtxTot++;//Have to count the ones which are accepted to start with
            break;
          }
        } // end of loop over flags
      }

      // Skip if it has already been identified as duplicate
      if(vtxMask[k] == false)continue;  //After the flag-check to have the total-passed work correctly

      if(!passed)vtxMask[k]= false;

      if(passed) {
          // vertex passed the selection
    	  nVtxPass++;

          // determine the sum of the tracks at vertex as centre for the cone
    	  std::vector<const xAOD::TrackParticle*> presentVertex, compareVertex;

    	  //Fill in the present vertex, for later comparison against other vertices
	          presentVertex.clear();
		  for(uint j=0; j<vtx->nTrackParticles(); ++j) {
			presentVertex.push_back(vtx->trackParticle(j));
		  }
		  sort( presentVertex.begin(), presentVertex.end() );  //Sort the trackparticles BY POINTER ADDRESS

		  //Loop over the remaining vertices and remove them if needed
		  int loop_k = k+1;
		  for(auto vtxLoopItr = vtxItr+1; vtxLoopItr!=vertexContainer->cend(); vtxLoopItr++, loop_k++){

			  const xAOD::Vertex* loop_vtx = *vtxLoopItr;

		      //Vertices are distinct if have different size
		      if(vtx->nTrackParticles() !=  loop_vtx->nTrackParticles())continue;

			  //If the vertex is still active load and compare
			  if(vtxMask[loop_k]){

					compareVertex.clear();
				  for(uint j=0; j<loop_vtx->nTrackParticles(); ++j) {
					compareVertex.push_back(loop_vtx->trackParticle(j));
				  } 

				  std::sort( compareVertex.begin(), compareVertex.end());

				  vtxMask[loop_k] = false;
			
				  ATH_MSG_DEBUG("Compared tracks: ");
                  ATH_MSG_DEBUG(std::setw(14)<<compareVertex[0]<<std::setw(14) << compareVertex[1]<<std::setw(14)<<compareVertex[2]);
                  ATH_MSG_DEBUG(std::setw(14)<<presentVertex[0]<<std::setw(14) << presentVertex[1]<<std::setw(14)<<presentVertex[2]);

				  for(uint j=0; j<loop_vtx->nTrackParticles(); ++j) {
					if( compareVertex[j] != presentVertex[j] ){vtxMask[loop_k] = true;  break;}
				  }
				  ATH_MSG_DEBUG("Verdict:"<<(vtxMask[loop_k]? "keep": "erase") );
			  }

		  } // Endo of extra loop over remaining vertices

      } // if( passed )
    } // end of loop over vertices
    
    // Execute the thinning service based on the vtxMask.
    if (m_and) {
      vertexContainer.keep(vtxMask, SG::ThinningHandleBase::Op::And);
    }
    if (!m_and) {
      vertexContainer.keep(vtxMask, SG::ThinningHandleBase::Op::Or);
    }
    
    m_nVtxTot.fetch_add( vtxTot, std::memory_order_relaxed);
    m_nVtxPass.fetch_add( nVtxPass, std::memory_order_relaxed);

  

  return StatusCode::SUCCESS;
}

