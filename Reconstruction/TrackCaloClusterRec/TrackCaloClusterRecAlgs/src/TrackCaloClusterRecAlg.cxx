// TrackCaloClusterRecAlgs includes
#include "TrackCaloClusterRecAlgs/TrackCaloClusterRecAlg.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "TrackCaloClusterRecInterfaces/IParticleExtrapolationTool.h"

TrackCaloClusterRecAlg::TrackCaloClusterRecAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_particleToCaloExtrapolator("TrackCaloClusterRecTools/ParticleToCaloExtrapolationTool") {
    declareProperty("ParticleCaloExtrapolationTool",  m_particleToCaloExtrapolator );
    declareProperty("TrackParticleContainerName"   ,  m_trkParticleName = "InDetTrackParticles");
  }
  
TrackCaloClusterRecAlg::~TrackCaloClusterRecAlg() {}
  
StatusCode TrackCaloClusterRecAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");    
  ATH_CHECK(m_particleToCaloExtrapolator.retrieve());    
  return StatusCode::SUCCESS;
}
  
StatusCode TrackCaloClusterRecAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");  
  return StatusCode::SUCCESS;
}
  
StatusCode TrackCaloClusterRecAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
    
  // retrieve trackParticle container
  const auto orig_track_collectionl = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);
  if (not orig_track_collectionl) {
    ATH_MSG_DEBUG ("TrackParticleContainer not found... returning.");
    return StatusCode::SUCCESS;
  }
  
  for (auto thisTrack: *orig_track_collectionl) {
    ATH_MSG_DEBUG ("Getting trackParticle");
      
    // get calo extension
    const Trk::CaloExtension* caloExtension = 0;
    m_particleToCaloExtrapolator->particleToCaloExtrapolate( *thisTrack, caloExtension );    
    
    if( !caloExtension ) {
      ATH_MSG_WARNING("Failed to get CaloExtension... returning.");
      delete caloExtension;
      return StatusCode::SUCCESS;
    }
    
    ATH_MSG_DEBUG("Found caloEntryLayerIntersection() --> " << *caloExtension->caloEntryLayerIntersection());
    ATH_MSG_DEBUG("------------------");
    for (const auto intersection: caloExtension->caloLayerIntersections()) {
      ATH_MSG_DEBUG("Found intersection --> " << *intersection);
      Trk::TrackParametersIdHelper  parsIdHelper;
      TrackParametersIdentifier id =  intersection->cIdentifier();
      CaloSampling::CaloSample sample = parsIdHelper.caloSample(id);
      ATH_MSG_DEBUG("   Corresponding sample --> " << sample);
    }
    ATH_MSG_DEBUG("------------------");
    
    
    // Roland --> here you start playing with the caloExtension object :-)
    
    delete caloExtension;
  }
  
  return StatusCode::SUCCESS;
}
