/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerProviderCond.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkDetDescrInterfaces/ILayerBuilderCond.h"
#include "TrkDetDescrTools/LayerProviderCond.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/Layer.h"

// constructor
Trk::LayerProviderCond::LayerProviderCond(const std::string& t,
                                          const std::string& n,
                                          const IInterface* p)
  : AthAlgTool(t, n, p)
{
  declareInterface<Trk::ILayerProviderCond>(this);

  // Name specification from outside
  declareProperty("LayerBuilder", m_layerBuilder);
}

// destructor
Trk::LayerProviderCond::~LayerProviderCond() = default;

// initialize
StatusCode
Trk::LayerProviderCond::initialize()
{
  if (m_layerBuilder.retrieve().isFailure()) {
    ATH_MSG_WARNING("Could not retrieve layer builder");
  }
  return StatusCode::SUCCESS;
}

/** LayerBuilderCond interface method - returning the central layers */
const std::vector<Trk::Layer*>
Trk::LayerProviderCond::centralLayers(const EventContext& ctx,
                                      SG::WriteCondHandle<TrackingGeometry>& whandle) const
{
  // central layers
  std::vector<Trk::Layer*> cLayers;
  // retrieving the cylinder layers from the layer builder
  std::unique_ptr<const std::vector<Trk::CylinderLayer*> >
    cylinderLayers = m_layerBuilder->cylindricalLayers(ctx, whandle);
  // loop over it and push into the return vector;
  if (cylinderLayers) {
    for (Trk::CylinderLayer* cL : (*cylinderLayers))
      cLayers.push_back(cL);
  }
  // and return
  return cLayers;
}

/** LayerBuilderCond interface method - returning the layers at negative side */
std::pair<const std::vector<Trk::Layer*>, const std::vector<Trk::Layer*> >
Trk::LayerProviderCond::endcapLayer(const EventContext& ctx,
                                    SG::WriteCondHandle<TrackingGeometry>& whandle) const
{
  // get the disc layers
  std::vector<Trk::Layer*> dLayers_pos;
  std::vector<Trk::Layer*> dLayers_neg;
  // retrieving the cylinder layers from the layer builder
  std::unique_ptr<const std::vector<Trk::DiscLayer*> > discLayers =
    m_layerBuilder->discLayers(ctx, whandle);

  // loop and fill either cache or dLayers
  if (discLayers) {
    // loop over and push into the return/cache vector
    for (Trk::DiscLayer* dL : (*discLayers)) {
      // get the center posituion
      double zpos = dL->surfaceRepresentation().center().z();
      if (zpos > 0.)
          dLayers_pos.push_back(dL);
      else
          dLayers_neg.push_back(dL);
    }
  }
  // and return
  return std::make_pair(dLayers_pos, dLayers_neg);
}

const std::string&
Trk::LayerProviderCond::identification() const
{
  return m_layerBuilder->identification();
}
