/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkInDet/VsiTrackThinningTool.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::VsiTrackThinningTool::VsiTrackThinningTool( const std::string& t,
                                                               const std::string& n,
                                                               const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
  m_ntot(0),
  m_npass(0),
  m_trackContainerName("InDetTrackParticles"),
  m_vertexContainerName("VrtSecInclusive_SecondaryVertices")
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningService", m_thinningSvc);
  declareProperty("TrackContainerName", m_trackContainerName);
  declareProperty("VertexContainerName", m_vertexContainerName);
}
  
// Destructor
DerivationFramework::VsiTrackThinningTool::~VsiTrackThinningTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::VsiTrackThinningTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::VsiTrackThinningTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
  return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::VsiTrackThinningTool::doThinning() const
{

  // Get the track container
  const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_trackContainerName );
  if(!tracks) {
    ATH_MSG_WARNING ( __FUNCTION__ << ": Couldn't retrieve TrackParticleContainer with key " << m_trackContainerName );
    return StatusCode::SUCCESS;
  }
  
  const xAOD::VertexContainer* vertices = evtStore()->retrieve< const xAOD::VertexContainer >( m_vertexContainerName );
  if(!vertices) {
    ATH_MSG_WARNING ( __FUNCTION__ << ": Couldn't retrieve TrackParticleContainer with key " << m_vertexContainerName );
    return StatusCode::SUCCESS;
  }
  
  m_ntot+=tracks->size();
  // Loop over tracks, see if they pass, set mask
  std::vector<bool> mask;
  for (size_t itrk=0; itrk< tracks->size(); ++itrk) {
    const auto* trk = tracks->at(itrk);
    
    bool flag { false };
    
    for( auto* vertex : *vertices ) {
      for( size_t jtrk=0; jtrk<vertex->nTrackParticles(); jtrk++) {
        if( vertex->trackParticle(jtrk) == trk ) {
          flag = true;
          break;
        }
      }
      if( flag ) break;
    }
    
    //const bool flag1 = trk->isAvailable<char>("is_svtrk_final")? ( trk->auxdataConst<char>("is_svtrk_final") == 1 ) : false;
    //const bool flag2 = trk->isAvailable<char>("is_associated") ? ( trk->auxdataConst<char>("is_associated")  == 1 ) : false;
    //const bool flag3 = ( trk->pt() > 1.e3 );
    
    mask.emplace_back( flag );
  }
  unsigned count { 0 };
  for( size_t i=0; i<mask.size(); i++) {
    if( mask[i] ) count++;
  }
  ATH_MSG_DEBUG( __FUNCTION__ << ": filtered " << count << " / " << tracks->size() );
  
  if (m_thinningSvc->filter(*tracks, mask, IThinningSvc::Operator::Or).isFailure()) {
    ATH_MSG_ERROR("Application of thinning service failed! ");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}  
