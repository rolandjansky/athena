/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialProvider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "ISF_FatrasDetDescrTools/LayerMaterialProvider.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/LayerMaterialMap.h"
#include "TrkVolumes/Volume.h"
#include "TrkSurfaces/Surface.h"

// constructor
iFatras::LayerMaterialProvider::LayerMaterialProvider(const std::string& t, const std::string& n, const IInterface* p)
: base_class(t,n,p),
  m_layerMaterialMap(0), 
  m_layerMaterialMapName("/GLOBAL/TrackingGeo/LayerMaterialV2")
{
    // Name specification from outside
    declareProperty("LayerMaterialMapName", m_layerMaterialMapName);
    
}

// destructor
iFatras::LayerMaterialProvider::~LayerMaterialProvider()
{}


// Processor Action to work on TrackingGeometry 
StatusCode iFatras::LayerMaterialProvider::process(const Trk::TrackingGeometry& tgeo) {
  
  ATH_MSG_VERBOSE("Start processing the TrackingGeometry recursively");
  // retrieve the highest tracking volume
  const Trk::TrackingVolume* worldVolume = tgeo.highestTrackingVolume();  
  // check for the world volume
  if (worldVolume){
      // TrackingVolume : confined layers
      ATH_MSG_VERBOSE("TrackingVolume '" << worldVolume->volumeName() << "' retrieved as highest level node.");
      if (process(*worldVolume, 0).isFailure() ) {
          ATH_MSG_FATAL("Could not load material maps for provided TrackingGeometry, abort job.");
          return StatusCode::FAILURE;
      }
      // Boundary layers
      if (tgeo.boundaryLayers().size()){
          ATH_MSG_VERBOSE("TrackingGeometry has " <<  tgeo.boundaryLayers().size() << " unique bounday layers, loading material.");
          auto bLayerIter = tgeo.boundaryLayers().begin();
          auto bLayerE    = tgeo.boundaryLayers().end();
          for ( ; bLayerIter != bLayerE; ++bLayerIter ){
              const Trk::Layer* lay = (*bLayerIter).first;
              int layCount = (*bLayerIter).second;
              int layIndex = lay->layerIndex().value();  
              // only move on if layer index is different from 0
              if (layIndex){
                  StatusCode sc( process(*lay, 0) );
		  // @TODO Currently recoverable errors are treated as failure. Is this the intended behaviour ? Elsewhere recoverable errors are treated as recoverable
                  if (sc.isSuccess())
                      ATH_MSG_DEBUG("---[B] Boundary layer with " << layCount << " references : successfully loaded material map for layer " << layIndex );
                  // else if (sc.isRecoverable())
                  //    ATH_MSG_WARNING("Failed to call process(const Layer&) on layers - but recoverable.");
                  else {
                      ATH_MSG_FATAL("Failed to call process(const Layer&) on layer. Aborting.");
                      return StatusCode::FAILURE;            
                  }
              }
           } // loop over layers
       }// we have boundary layers
       return StatusCode::SUCCESS;
  } // we have a world volume
  // abort job
  ATH_MSG_FATAL("No highest level TrackingVolume found. Stopping recursive parsing, abort job.");
  return StatusCode::FAILURE;
}

// Processor Action to work on TrackingVolumes
StatusCode iFatras::LayerMaterialProvider::process(const Trk::TrackingVolume& tvol, size_t level) {
  
  // load the material map if not done yet
  if (!m_layerMaterialMap){ 
      if (loadMaterialMap().isFailure()){
            ATH_MSG_DEBUG( "Problems loading hte LayerMaterialMap - check name or call sequence." );
            return StatusCode::FAILURE;
      }
  }
  
  std::stringstream displayBuffer;
  for (size_t il = 0; il < level; ++il) displayBuffer << " ";
  // formatted screen output     
  ATH_MSG_VERBOSE(displayBuffer.str() << "TrackingVolume '" << tvol.volumeName() << "'");
  
  // @TODO add boundary surfaces
   
  // Process the contained layers 
  const Trk::LayerArray* layerArray = tvol.confinedLayers();
  if (layerArray) {
      // display output
      const std::vector<const Trk::Layer*>& layers = layerArray->arrayObjects(); 
      ATH_MSG_VERBOSE(displayBuffer.str() << "--> has " << layers.size() << " confined layers." ); 
      for ( auto& layIter : layers ){
          if (!layIter)
              ATH_MSG_WARNING("Zero-pointer found in LayerArray - indicates problem !");
          else {
	    // get the layer index and only process if it's an indexed layer
	    int layIndex = layIter->layerIndex().value();  
	    if (layIndex){
	      if (layIter->needsMaterialProperties()) {
	        ATH_MSG_DEBUG("Layer with index " << layIndex <<" needs MaterialProperties");
		StatusCode sc = process(*layIter, level);
		if (sc.isSuccess())
		  ATH_MSG_DEBUG(displayBuffer.str() << "---[M] Material layer: successfully loaded material map for layer " << layIndex );
		else if (sc.isRecoverable())
		  ATH_MSG_WARNING("Failed to call process(const Layer&) on layers - but recoverable.");
		else {
		  ATH_MSG_FATAL("Failed to call process(const Layer&) on layer. Aborting.");
		  return StatusCode::FAILURE;            
		}
	      } else ATH_MSG_DEBUG("Layer with index " << layIndex <<" doesn't need MaterialProperties");
	    } else 
	      ATH_MSG_DEBUG(displayBuffer.str() << "---[o] Navigation layer: skipping.");
	  }
      }
  }

   // Process the contained TrackingVolumes (recursively) if they exist
   const Trk::BinnedArray< Trk::TrackingVolume >* confinedVolumes = tvol.confinedVolumes();
   // register the next round
   if (confinedVolumes) {
       const std::vector<const Trk::TrackingVolume*>& volumes = confinedVolumes->arrayObjects();
       std::vector<const Trk::TrackingVolume*>::const_iterator volumesIter = volumes.begin();
       for (; volumesIter != volumes.end(); ++volumesIter){
           if (!(*volumesIter))
              ATH_MSG_WARNING("Zero-pointer found in VolumeArray - indicates problem !");
           if ((*volumesIter) && process(**volumesIter, ++level).isFailure() ){
               ATH_MSG_FATAL("Failed to call process(const TrackingVolume&) on confined volumes. Aborting.");
               return StatusCode::FAILURE;  
           }
       }
   }
   
   // return 
   return StatusCode::SUCCESS;    
    
}

// Processor Action to work on Layers 
StatusCode iFatras::LayerMaterialProvider::process(const Trk::Layer& lay, size_t level) {

    // load the material map if not done yet
    if (!m_layerMaterialMap){ 
        if (loadMaterialMap().isFailure()){
              ATH_MSG_WARNING( "Problems loading hte LayerMaterialMap - check name or call sequence." );
              return StatusCode::FAILURE;
        }
    }
    
    // skip Layers w/o material
    if (!lay.layerMaterialProperties()) 
        return StatusCode::SUCCESS;
    
    // get the layer index for assignment
    Trk::LayerIndex lIndex = lay.layerIndex();

    // display 
    std::stringstream displayBuffer;
    for (size_t il = 0; il < level; ++il) displayBuffer << " ";

    // find the layer and assign the material properties 
    auto lmIter= m_layerMaterialMap->find(lIndex);
    if ( lmIter != m_layerMaterialMap->end() ){
        ATH_MSG_VERBOSE(displayBuffer.str() << "---[+] found material for Layer with Index: " << lIndex.value());
        if ( lay.surfaceRepresentation().isFree() ) 
           ATH_MSG_VERBOSE(displayBuffer.str() << "---[!] the Layer is not owned by the TrackingGeometry, could indicate problem.");
	    else 
           ATH_MSG_VERBOSE(displayBuffer.str() << "---[+] the Layer is owned by the TrackingGeometry." ); 
        lay.assignMaterialProperties(*((*lmIter).second));
    } else {
        ATH_MSG_WARNING(displayBuffer.str() << "---[!] could not find material for Layer with Index: " << lIndex.value());
        return StatusCode::RECOVERABLE;
    }
    return StatusCode::SUCCESS;
}

// Processor Action to work on Surfaces 
StatusCode iFatras::LayerMaterialProvider::process(const Trk::Surface&, size_t) {
    return StatusCode::SUCCESS;
}

// load the material map from StoreGate
StatusCode iFatras::LayerMaterialProvider::loadMaterialMap() {
    
    // -------------------------------------------------------------------------------
    if (detStore()->retrieve(m_layerMaterialMap, m_layerMaterialMapName).isFailure()){
        ATH_MSG_FATAL( "Could not retrieve LayerMaterialMap wiht name '" << m_layerMaterialMapName << "'. Aborting.");
        return StatusCode::FAILURE;
    } else
        ATH_MSG_DEBUG( "Retrieved LayerMaterialMap wiht name '" << m_layerMaterialMapName 
            << "' for " <<  m_layerMaterialMap->size() << " layers.");
    
    // screen output as most VERBOSE debugging        
    if (msgLevel() == MSG::VERBOSE){
        ATH_MSG_VERBOSE("Listing the layer indeces found in the loaded LayerMaterialMap");
        for ( auto& lmIter : (*m_layerMaterialMap) ){
            ATH_MSG_VERBOSE("  -> Found map for layer with index " << lmIter.first);
        }
    }        
        
    return StatusCode::SUCCESS;       
}

