// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


#ifndef ACTS_GEOMODELPLUGIN_GEOMODELLAYERBUILDER_H
#define ACTS_GEOMODELPLUGIN_GEOMODELLAYERBUILDER_H


// ATHENA INCLUDES
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

#include "GeomACTS/GeoModelDetectorElement.hpp"

// ACTS
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Layers/ProtoLayer.hpp"
#include "ACTS/Tools/ILayerBuilder.hpp"
#include "ACTS/Tools/LayerCreator.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Logger.hpp"


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
