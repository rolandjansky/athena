/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// Thin_vtxDuplicates.cxx
///////////////////////////////////////////////////////////////////
// Matteo Bedognetti (matteo.bedognetti@cern.ch)
//Based on Thin_vtxTrk.cxx, by



#include "DerivationFrameworkBPhys/Thin_vtxDuplicates.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODBPhys/BPhysHypoHelper.h"
#include <vector>
#include <string>
#include <algorithm>  // for the sort function
#include <iomanip>

// Constructor
DerivationFramework::Thin_vtxDuplicates::Thin_vtxDuplicates(const std::string& t, const std::string& n, const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
 // m_acceptanceR(-1.),
  m_noFlags(false),
  m_nVtxTot(0),
  m_nVtxPass(0)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  
  declareProperty("ThinningService", m_thinningSvc);
  
  declareProperty("TrackParticleContainerName", m_trackParticleContainerName = "InDetTrackParticles");
  declareProperty("VertexContainerNames"      , m_vertexContainerNames);
  declareProperty("PassFlags"                 , m_passFlags);
  //declareProperty("AcceptanceRadius"          , m_acceptanceR);
  declareProperty("ApplyAnd"                  , m_and = true);  //This will be applied depending on the order in which the thinning tools are added to the kernel
  declareProperty("IgnoreFlags"               , m_noFlags);
  //declareProperty("ApplyAndForTracks"         , m_trackAnd = false);
  //declareProperty("ThinTracks"                , m_thinTracks = true);
}

// Destructor
DerivationFramework::Thin_vtxDuplicates::~Thin_vtxDuplicates() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::Thin_vtxDuplicates::initialize()
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
  
  if (m_passFlags.empty()) {
    ATH_MSG_FATAL("No pass flags provided for thinning.");
    return StatusCode::FAILURE;
  } else {
    for(std::vector<std::string>::iterator itr = m_passFlags.begin(); itr!=m_passFlags.end(); ++itr) {
      ATH_MSG_INFO("Vertices must pass the \"" << *itr << "\" selection");
    }
  }
  


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
  // Retrieve main TrackParticle collection
  const xAOD::TrackParticleContainer* importedTrackParticles;
  CHECK( evtStore()->retrieve(importedTrackParticles,m_trackParticleContainerName) );

  // Retrieve vertex containers
  for(std::vector<std::string>::const_iterator itr = m_vertexContainerNames.begin(); itr!=m_vertexContainerNames.end(); ++itr) {
    // retieve vertex 
    xAOD::VertexContainer* vertexContainer;
    CHECK( evtStore()->retrieve(vertexContainer, *itr) );
    std::vector<bool> vtxMask;
    vtxMask.assign(vertexContainer->size(), true); // default: keep all vertices
    
    // loop over vertices
    int k = 0;
    for(xAOD::VertexContainer::iterator vtxItr = vertexContainer->begin(); vtxItr!=vertexContainer->end(); ++vtxItr, ++k) {
      xAOD::Vertex* vtx = *vtxItr;


      // check if the vertex passed the required selections criteria (is run when the vertex is already excluded, because the counter needs the info)
      bool passed = false;
      if(m_noFlags){passed = true; m_nVtxTot++;}
      else{
        for(std::vector<std::string>::const_iterator flagItr = m_passFlags.begin(); flagItr!=m_passFlags.end(); ++flagItr) {
          SG::AuxElement::Accessor<Char_t> flagAcc(*flagItr);
          if(flagAcc.isAvailable(*vtx) && flagAcc(*vtx) != 0) {
            passed = true;
	    m_nVtxTot++;  //Have to count the ones which are accepted to start with
            break;
          }
        } // end of loop over flags
      }

      // Skip if it has already been identified as duplicate
      if(vtxMask[k] == false)continue;  //After the flag-check to have the total-passed work correctly

      if(!passed)vtxMask[k]= false;

      if(passed) {
          // vertex passed the selection
    	  m_nVtxPass++;

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
		  for(xAOD::VertexContainer::iterator vtxLoopItr = vtxItr+1; vtxLoopItr!=vertexContainer->end(); vtxLoopItr++, loop_k++){

			  xAOD::Vertex* loop_vtx = *vtxLoopItr;

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
  

  

  return StatusCode::SUCCESS;
}

