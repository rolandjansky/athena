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
#include "TrkDetDescrUtils/BinUtility.h"
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


Trk::LayerMaterialProperties* Trk::BinnedLayerMaterialCreator::createLayerMaterial(const Trk::LayerMaterialRecord& lmr) const
{
  // get the material matrix and copy 
  const Trk::MaterialPropertiesMatrix& mpm = lmr.associatedLayerMaterial();
  // this method creates a compressed representation of the BinnedLayerMaterial
  const Trk::BinUtility* cbinutil = lmr.binUtility();
  // create the compressed material
  return new Trk::BinnedLayerMaterial(*cbinutil,mpm);
}    

Trk::LayerMaterialProperties* Trk::BinnedLayerMaterialCreator::convertLayerMaterial(const Trk::LayerMaterialProperties& lmProperties) const
{
    // the return object
    Trk::LayerMaterialProperties* bLayerMaterial = nullptr;
    // get the binUtility of the LayerMaterialProperties
    const Trk::BinUtility* bUtility = lmProperties.binUtility();
    // we have a bin utility, get the matrix and fill it
    if (bUtility){
        // prepare the matrix
        // nF x nS
        size_t nBins0  = bUtility->max(0)+1;
        size_t nBins1  = bUtility->max(1)+1;
        // create the MaterialMatrix
        Trk::MaterialPropertiesMatrix materialMatrix;
        materialMatrix.reserve(nBins1);
        // fill the matrix
        for (size_t ibin1 = 0; ibin1 < nBins1; ++ibin1) {
            // create the vector first
            Trk::MaterialPropertiesVector materialVector;
            materialVector.reserve(nBins0);
            // loop over local 1 bins
            for (size_t ibin0 = 0; ibin0 < nBins0; ++ibin0) {
                // get the material from the properties and push them as clones for the BinnedLayerMaterial 
                const Trk::MaterialProperties* mProperties = lmProperties.material(ibin0,ibin1) ? lmProperties.material(ibin0,ibin1)->clone() : nullptr;
                materialVector.push_back(mProperties);
            }
            // now pus the vector into the matrix
            materialMatrix.push_back(materialVector);
        }
                
        
    } else {
        // must be homogenous material, can be transformed into a 0-bin material, would be silly though
        bLayerMaterial = lmProperties.clone();
    }
    // 
    return bLayerMaterial;
}

    
