/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerProviderCond.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkDetDescrTools/LayerProviderCond.h"
#include "TrkDetDescrInterfaces/ILayerBuilderCond.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"

// constructor
Trk::LayerProviderCond::LayerProviderCond(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p)
{
    declareInterface<Trk::ILayerProviderCond>(this);
    
    // Name specification from outside
    declareProperty("LayerBuilder", m_layerBuilder);
    
}

// destructor
Trk::LayerProviderCond::~LayerProviderCond()
{}

// initialize
StatusCode Trk::LayerProviderCond::initialize()
{
    if (m_layerBuilder.retrieve().isFailure()){
        ATH_MSG_WARNING("Could not retrieve layer builder");
    }
    return StatusCode::SUCCESS;
}

/** LayerBuilderCond interface method - returning the layers at negative side */
std::pair<EventIDRange, const std::vector< const Trk::Layer*>  > Trk::LayerProviderCond::negativeLayers(const EventContext& ctx) const
{
    // this will fill the cache with positive layers
    return discLayers(ctx, -1);
}    

/** LayerBuilderCond interface method - returning the central layers */
std::pair<EventIDRange, const std::vector< const Trk::Layer* > > Trk::LayerProviderCond::centralLayers(const EventContext& ctx) const
{
    // central layers
    std::vector< const Trk::Layer* >            cLayers;
    // retrieving the cylinder layers from the layer builder
    std::pair<EventIDRange, const std::vector< const Trk::CylinderLayer* >* >  cylinderLayersPair = m_layerBuilder->cylindricalLayers(ctx);
    auto cylinderLayers = cylinderLayersPair.second;
    // loop over it and push into the return vector;
    if (cylinderLayers){
        for (auto& cL : (*cylinderLayers))
            cLayers.push_back(cL);
    }
    // memory cleanup
    delete cylinderLayers;
    // and return
    return std::make_pair(cylinderLayersPair.first, cLayers);
} 

/** LayerBuilderCond interface method - returning the layers at negative side */
std::pair<EventIDRange,const std::vector< const Trk::Layer*> > Trk::LayerProviderCond::positiveLayers(const EventContext& ctx) const
{
    // this will fill the cache with negative layers
    return discLayers(ctx, 1);
}

/** LayerBuilderCond interface method - returning the layers at negative side */
std::pair<EventIDRange, const std::vector< const Trk::Layer* > > Trk::LayerProviderCond::discLayers(const EventContext& ctx, int posneg) const
{
    // get the disc layers
    std::vector < const Trk::Layer* >   dLayers;
    // retrieving the cylinder layers from the layer builder
    std::pair<EventIDRange, const std::vector<const Trk::DiscLayer*>*>  discLayersPair = m_layerBuilder->discLayers(ctx);
    auto discLayers = discLayersPair.second;
    // loop and fill either cache or dLayers
    if (discLayers){
        // loop over and push into the return/cache vector 
        for (auto& dL : (*discLayers) ){
            // get the center posituion 
            double zpos = dL->surfaceRepresentation().center().z();
            if (posneg > 0){
                // configured to provide positive and cache negative
                if (zpos > 0.) dLayers.push_back(dL);
            } else {
                // configured to provide negative and cache positive
                if (zpos < 0.) dLayers.push_back(dL);
            }
        }
    }
    // memory cleanup
    delete discLayers;
    // and return
    return std::make_pair(discLayersPair.first, dLayers);   
}


// finalize
StatusCode Trk::LayerProviderCond::finalize()
{
    return StatusCode::SUCCESS;
}

const std::string& Trk::LayerProviderCond::identification() const
{
    return m_layerBuilder->identification();
}
