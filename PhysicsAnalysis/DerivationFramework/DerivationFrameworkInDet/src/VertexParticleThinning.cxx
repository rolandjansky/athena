/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// VertexParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkInDet/VertexParticleThinning.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::VertexParticleThinning::VertexParticleThinning(const std::string& t,
                                                                        const std::string& n,
                                                                        const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
  m_ntot(0),
  m_npass(0),
  m_nVtxTot(0),
  m_nVtxPass(0),
  m_vertexSGKey(""),
  m_inDetSGKey("InDetTrackParticles"),
  m_selectionString(""),
  m_trkSelTool("",this),
  m_minGoodTracks(0),
  m_and(false),
  m_parser(0)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("VertexKey", m_vertexSGKey);
    declareProperty("InDetTrackParticlesKey", m_inDetSGKey);
    declareProperty("SelectionString", m_selectionString);
    declareProperty("TrackSelectionTool", m_trkSelTool);
    declareProperty("MinGoodTracks",m_minGoodTracks);
    declareProperty("ApplyAnd", m_and);
}

// Destructor
DerivationFramework::VertexParticleThinning::~VertexParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::VertexParticleThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_inDetSGKey=="") {
        ATH_MSG_FATAL("No inner detector track collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");}
    if (m_vertexSGKey=="") {
        ATH_MSG_FATAL("No vertex collection provided for thinning.");
        return StatusCode::FAILURE;
    } else { ATH_MSG_INFO("Inner detector track particles associated with objects in " << m_vertexSGKey << " will be retained in this format with the rest being thinned away");}
    
    // Set up the text-parsing machinery for selectiong the vertex directly according to user cuts
    if (m_selectionString!="") {
	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
	    m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
	    m_parser->loadExpression(m_selectionString);
    }

    // Track selection tool
    if( !m_trkSelTool.empty() ){
      ATH_CHECK(m_trkSelTool.retrieve());
    }

    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::VertexParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    ATH_MSG_INFO("Processed "<< m_nVtxTot <<" vertices "<< m_nVtxPass<< " were retained ");
    if (m_selectionString!="") {
        delete m_parser;
        m_parser = 0;
    }
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::VertexParticleThinning::doThinning() const
{
    
    // Retrieve main TrackParticle collection
    const xAOD::TrackParticleContainer* importedTrackParticles;
    if (evtStore()->retrieve(importedTrackParticles,m_inDetSGKey).isFailure()) {
        ATH_MSG_ERROR("No TrackParticle collection with name " << m_inDetSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    const xAOD::VertexContainer* primaryVertices { nullptr };
    ATH_CHECK( evtStore()->retrieve( primaryVertices, "PrimaryVertices" ) );

    const xAOD::Vertex* priVtx { nullptr };
    for( const auto* vtx : *primaryVertices ) {
      if( vtx->vertexType() == xAOD::VxType::PriVtx ) {
        priVtx = vtx;
        break;
      }
    }
    
    // Check the event contains tracks
    unsigned int nTracks = importedTrackParticles->size();
    if (nTracks==0) return StatusCode::SUCCESS;
    
    // Set up a mask with the same entries as the full TrackParticle collection
    std::vector<bool> mask;
    mask.assign(nTracks,false); // default: don't keep any tracks
    m_ntot += nTracks;
    
    // Retrieve containers
    // ... vertexs
    const xAOD::VertexContainer* importedVertices(0);
    if (evtStore()->retrieve(importedVertices,m_vertexSGKey).isFailure()) {
        ATH_MSG_ERROR("No vertex collection with name " << m_vertexSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    unsigned int nVertices(importedVertices->size());
    std::vector<const xAOD::Vertex*> vertexToCheck; vertexToCheck.clear();
    std::vector<bool> vtxMask;
    vtxMask.assign(nVertices,false); // default: don't keep any vertices 
    m_nVtxTot += nVertices;
    
    // Execute the text parser if requested
    if (m_selectionString!="") {
        std::vector<int> entries =  m_parser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nVertices != nEntries ) {
        	ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used vertices??");
            return StatusCode::FAILURE;
        } else {
        	// identify which jets to keep for the thinning check
        	for (unsigned int i=0; i<nVertices; ++i) if (entries[i]==1) vertexToCheck.push_back((*importedVertices)[i]);
        }
    }
    
    // Set elements in the mask to true if there is a corresponding ElementLink from a reconstructed object
    // ... vertexs
    if (m_selectionString=="") { // check all vertices as user didn't provide a selection string
        for (xAOD::VertexContainer::const_iterator vtxIt=importedVertices->begin(); vtxIt!=importedVertices->end(); ++vtxIt) {
       
          // Check whether vertex has good tracks
          if( !m_trkSelTool.empty() ){
            unsigned int nGoodTracks=0;
            for (size_t i = 0; i<(*vtxIt)->nTrackParticles(); i++) {
              const xAOD::TrackParticle *trk = (*vtxIt)->trackParticle(i);
              if( !priVtx ) continue;
              if( m_trkSelTool->accept(*trk,priVtx) ) nGoodTracks++;
            }
            // User requests only a minimum number of tracks to pass track selection
            if( m_minGoodTracks>0 && nGoodTracks<m_minGoodTracks                     ) continue; 
            // Default, all tracks from vertex pass track selection
            else if ( m_minGoodTracks==0 && nGoodTracks!=(*vtxIt)->nTrackParticles() ) continue; 
 
            // Keep vertice passing this selection
            int vtxIdx = (*vtxIt)->index();
            vtxMask[vtxIdx] = true;

          }

	  for (size_t i = 0; i<(*vtxIt)->nTrackParticles(); i++) {
	    const xAOD::TrackParticle *trk = (*vtxIt)->trackParticle(i);
	    int index = trk->index();
	    mask[index] = true;
	  }
	 
	}
    }   

    for( bool bit : mask ) {
      if( bit ) {
        ++m_npass;
      }
    }
    for( bool bit : vtxMask ) {
      if( bit ) {
        ++m_nVtxPass;
      }
    }
    

    // Execute the thinning service based on the mask. Finish.
    if (m_and) {
        if (m_thinningSvc->filter(*importedTrackParticles, mask, IThinningSvc::Operator::And).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }

        if( !m_trkSelTool.empty() ){
          if (m_thinningSvc->filter(*importedVertices, vtxMask, IThinningSvc::Operator::And).isFailure()) {
                  ATH_MSG_ERROR("Application of thinning service failed! ");
                  return StatusCode::FAILURE;
          }
        }

    }
    if (!m_and) {
        if (m_thinningSvc->filter(*importedTrackParticles, mask, IThinningSvc::Operator::Or).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }

        if( !m_trkSelTool.empty() ){
          if (m_thinningSvc->filter(*importedVertices, vtxMask, IThinningSvc::Operator::Or).isFailure()) {
                  ATH_MSG_ERROR("Application of thinning service failed! ");
                  return StatusCode::FAILURE;
          }
        }


    }

    return StatusCode::SUCCESS;
}

