/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InputLayerMaterialProvider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkDetDescrTools/InputLayerMaterialProvider.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/LayerMaterialMap.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"

// constructor
Trk::InputLayerMaterialProvider::InputLayerMaterialProvider(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
   m_constantMaterialToAllLayers(true),
   m_constantThicknessInX0(0.02),       
   m_constantThicknessInL0(0.06),       
   m_constantAverageA(14.),            
   m_constantAverageZ(7.),            
   m_constantAverageRho(0.00233)          
  
{
    declareInterface<Trk::IGeometryProcessor>(this);
    
    declareProperty("AssignConstantMaterial",   m_constantMaterialToAllLayers);
    declareProperty("ConstantMaterialInX0",     m_constantThicknessInX0);      
    declareProperty("ConstantMaterialInL0",     m_constantThicknessInL0);      
    declareProperty("ConstantMaterialA",        m_constantAverageA);            
    declareProperty("ConstantMaterialZ",        m_constantAverageZ);            
    declareProperty("ConstantMaterialRho",      m_constantAverageRho);        
    
}

// destructor
Trk::InputLayerMaterialProvider::~InputLayerMaterialProvider()
{}

StatusCode Trk::InputLayerMaterialProvider::initialize() {

  m_constantMaterialProperties = Trk::MaterialProperties(1.,
							 1./m_constantThicknessInX0,      
							 1./m_constantThicknessInL0,      
							 m_constantAverageA,           
							 m_constantAverageZ,           
							 m_constantAverageRho);
                                                                   
    return StatusCode::SUCCESS;
}

// Processor Action to work on TrackingGeometry 
StatusCode Trk::InputLayerMaterialProvider::process(const Trk::TrackingGeometry& tgeo) const {
  
  ATH_MSG_VERBOSE("Start processing the TrackingGeometry recursively");
  // retrieve the highest tracking volume
  const Trk::TrackingVolume* worldVolume = tgeo.highestTrackingVolume();  
  if (worldVolume){
      ATH_MSG_VERBOSE("TrackingVolume '" << worldVolume->volumeName() << "' retrieved as highest level node.");
      return process(*worldVolume, 0);
  }
  // abort job
  ATH_MSG_FATAL("No highest level TrackingVolume found. Stopping recursive parsing, abort job.");
  return StatusCode::FAILURE;
}

// Processor Action to work on TrackingVolumes
StatusCode Trk::InputLayerMaterialProvider::process(const Trk::TrackingVolume& tvol, size_t level) const {
  
  std::stringstream displayBuffer;
  for (size_t il = 0; il < level; ++il) displayBuffer << " ";
  // formatted screen output     
  ATH_MSG_VERBOSE(displayBuffer.str() << "TrackingVolume '" << tvol.volumeName() << "'");
  
  // Process the contained layers if they exist
  const Trk::LayerArray* layerArray = tvol.confinedLayers();
  if (layerArray) {
      // display output
      const std::vector<const Trk::Layer*>& layers = layerArray->arrayObjects();
      std::vector<const Trk::Layer*>::const_iterator layIter  = layers.begin();
      std::vector<const Trk::Layer*>::const_iterator layIterE = layers.end();    
      ATH_MSG_VERBOSE(displayBuffer.str() << "--> has " << layers.size() << " confined layers." ); 
      for ( ; layIter != layIterE; ++layIter){
          if (!(*layIter))
             ATH_MSG_WARNING("Zero-pointer found in LayerArray - indicates problem !");
          if ((*layIter) && process(**layIter, level).isFailure()){
             ATH_MSG_FATAL("Failed to call process(const Layer&) on confined layers. Aborting.");
             return StatusCode::FAILURE;
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
StatusCode Trk::InputLayerMaterialProvider::process(const Trk::Layer& lay, size_t level) const {
    
    // skip Layers w/o material
    if (!lay.layerMaterialProperties()) 
        return StatusCode::SUCCESS;
    
    // get the layer index for assignment
    Trk::LayerIndex lIndex = lay.layerIndex();
    
    // display 
    std::stringstream displayBuffer;
    for (size_t il = 0; il < level; ++il) displayBuffer << " ";

    ATH_MSG_DEBUG(displayBuffer.str() << "   assigning material for Layer with Index: " << lIndex.value());
    ATH_MSG_VERBOSE(displayBuffer.str() << "                 Layer memory adress is : " << &lay);
    
    Trk::HomogeneousLayerMaterial hLayerMaterial = Trk::HomogeneousLayerMaterial(m_constantMaterialProperties, 1.);
    // assign it to the layer
    lay.assignMaterialProperties(hLayerMaterial);
    
    return StatusCode::SUCCESS;
}

// Processor Action to work on Surfaces 
StatusCode Trk::InputLayerMaterialProvider::process(const Trk::Surface&, size_t) const {
    return StatusCode::SUCCESS;
}


