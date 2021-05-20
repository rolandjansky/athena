/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialConverter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkDetDescrTools/LayerMaterialConverter.h"

// constructor
Trk::LayerMaterialConverter::LayerMaterialConverter(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p)
{
    declareInterface<Trk::ILayerMaterialManipulator>(this);

    // Name specification from outside
    declareProperty("LayerMaterialCreator", m_layerMaterialCreator);
}

// destructor
Trk::LayerMaterialConverter::~LayerMaterialConverter()
{}


// the AthAlgTool interface methods
StatusCode Trk::LayerMaterialConverter::initialize()
{
    if (m_layerMaterialCreator.retrieve().isFailure()){
        ATH_MSG_FATAL("Could not retrieve material creator - the converter is useless. Abort.");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}



/** process the layer material - after material creation and before loading */
const Trk::LayerMaterialProperties* Trk::LayerMaterialConverter::processLayerMaterial(const LayerIndex&, const LayerMaterialProperties& lmp) const 
{
    return m_layerMaterialCreator->convertLayerMaterial(lmp);
}
