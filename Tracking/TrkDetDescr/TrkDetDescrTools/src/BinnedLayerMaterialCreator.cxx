/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedLayerMaterialCreator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkDetDescrTools/BinnedLayerMaterialCreator.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/LayerMaterialRecord.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
    

// constructor
Trk::BinnedLayerMaterialCreator::BinnedLayerMaterialCreator(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_compressedMaterialThickness(1.)
{
    declareInterface<Trk::ILayerMaterialCreator>(this);
    
    // give the map a name
    declareProperty("LayerMaterialName"                 , m_layerMaterialName);
    declareProperty("LayerMaterialDirectory"            , m_layerMaterialDirectory);
    // setup for compressed layer creation
    declareProperty("MaterialThickness"                 , m_compressedMaterialThickness);
    
}

// destructor
Trk::BinnedLayerMaterialCreator::~BinnedLayerMaterialCreator()
{}


// initialize
StatusCode Trk::BinnedLayerMaterialCreator::initialize() 
{
    return StatusCode::SUCCESS;
}
    
// finalize
StatusCode Trk::BinnedLayerMaterialCreator::finalize() 
{
    return StatusCode::SUCCESS;
}

const Trk::LayerMaterialProperties* Trk::BinnedLayerMaterialCreator::createLayerMaterial(const Trk::Layer&, const Trk::LayerMaterialRecord& lmr) const
{
  // get the material matrix
  const Trk::MaterialPropertiesMatrix& mpm = lmr.associatedLayerMaterial();
  // this method creates a compressed representation of the BinnedLayerMaterial
  const Trk::BinUtility* cbinutil = lmr.binUtility();
  // create the compressed material
  return new Trk::BinnedLayerMaterial(*cbinutil,mpm);
}    
    