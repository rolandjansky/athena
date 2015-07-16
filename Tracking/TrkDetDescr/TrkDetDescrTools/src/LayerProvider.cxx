/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerProvider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkDetDescrTools/LayerProvider.h"
#include "TrkDetDescrInterfaces/ILayerBuilder.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"

// constructor
Trk::LayerProvider::LayerProvider(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p)
{
    declareInterface<Trk::ILayerProvider>(this);
    
    // Name specification from outside
    declareProperty("LayerBuilder", m_layerBuilder);
    
}

// destructor
Trk::LayerProvider::~LayerProvider()
{}

// initialize
StatusCode Trk::LayerProvider::initialize()
{
    if (m_layerBuilder.retrieve().isFailure()){
        ATH_MSG_WARNING("Could not retrieve layer builder");
    }
    return StatusCode::SUCCESS;
}

/** LayerBuilder interface method - returning the layers at negative side */
const std::vector< const Trk::Layer* > Trk::LayerProvider::negativeLayers() const
{
    // if any cache is there it has to be returned
    if (m_layerCache.size()) return m_layerCache;
    // this will fill the cache with positive layers
    return discLayers(-1);
}    

/** LayerBuilder interface method - returning the central layers */
const std::vector< const Trk::Layer* > Trk::LayerProvider::centralLayers() const
{
    // central layers
    std::vector< const Trk::Layer* >            cLayers;
    // retrieving the cylinder layers from the layer builder
    const std::vector< const Trk::CylinderLayer* >*   cylinderLayers = m_layerBuilder->cylindricalLayers();
    // loop over it and push into the return vector;
    if (cylinderLayers){
        for (auto& cL : (*cylinderLayers))
            cLayers.push_back(cL);
    }
    // memory cleanup
    delete cylinderLayers;
    // and return
    return cLayers;
} 

/** LayerBuilder interface method - returning the layers at negative side */
const std::vector< const Trk::Layer* > Trk::LayerProvider::positiveLayers() const
{
    // if any cache is there it has to be returned
    if (m_layerCache.size()) return m_layerCache;
    // this will fill the cache with negative layers
    return discLayers(1);
}

/** LayerBuilder interface method - returning the layers at negative side */
const std::vector< const Trk::Layer* > Trk::LayerProvider::discLayers(int posneg) const
{
    // get the disc layers
    std::vector < const Trk::Layer* >   dLayers;
    // retrieving the cylinder layers from the layer builder
    const std::vector< const Trk::DiscLayer* >* discLayers = m_layerBuilder->discLayers();
    // loop and fill either cache or dLayers
    if (discLayers){
        // loop over and push into the return/cache vector 
        for (auto& dL : (*discLayers) ){
            // get the center posituion 
            double zpos = dL->surfaceRepresentation().center().z();
            if (posneg > 0){
                // configured to provide positive and cache negative
                if (zpos > 0.) dLayers.push_back(dL);
                else m_layerCache.push_back(dL);
            } else {
                // configured to provide negative and cache positive
                if (zpos < 0.) dLayers.push_back(dL);
                else m_layerCache.push_back(dL);
            }
        }
    }
    // memory cleanup
    delete discLayers;
    // and return
    return dLayers;   
}


// finalize
StatusCode Trk::LayerProvider::finalize()
{
    return StatusCode::SUCCESS;
}

const std::string& Trk::LayerProvider::identification() const
{
    return m_layerBuilder->identification();
}
