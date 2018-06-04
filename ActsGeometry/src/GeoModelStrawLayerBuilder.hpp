#ifndef ACTS_GEOMODELPLUGIN_GEOMODELLAYERBUILDER_H
#define ACTS_GEOMODELPLUGIN_GEOMODELLAYERBUILDER_H

// PACKAGE
#include "ActsGeometry/GeoModelDetectorElement.hpp"

// ACTS
#include "Acts/Layers/Layer.hpp"
#include "Acts/Layers/ProtoLayer.hpp"
#include "Acts/Tools/ILayerBuilder.hpp"
#include "Acts/Tools/LayerCreator.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Logger.hpp"

// ATHENA
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "TrkSurfaces/TrapezoidBounds.h"



namespace Acts {

class Surface;

/// @class GeoModelLayerBuilder
class GeoModelStrawLayerBuilder : public GeoModelLayerBuilder
{
public:

  const LayerVector
  negativeLayers() const override;

  const LayerVector
  centralLayers() const override;

  const LayerVector
  positiveLayers() const override;

};
}

#endif
