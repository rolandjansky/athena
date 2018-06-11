// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


#ifndef ACTS_GEOMODELPLUGIN_GEOMODELSTRAWLAYERBUILDER_H
#define ACTS_GEOMODELPLUGIN_GEOMODELSTRAWLAYERBUILDER_H


// ATHENA INCLUDES
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "ActsGeometry/GeoModelDetectorElement.hpp"

// ACTS
#include "Acts/Layers/Layer.hpp"
#include "Acts/Layers/ProtoLayer.hpp"
#include "Acts/Tools/ILayerBuilder.hpp"
#include "Acts/Tools/LayerCreator.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Logger.hpp"


namespace Acts {

class Surface;
class TrackingGeomtrySvc;

/// @class GeoModelStrawLayerBuilder
class GeoModelStrawLayerBuilder : public ILayerBuilder
{
public:
  using ElementVector
      = std::vector<std::shared_ptr<const GeoModelDetectorElement>>;
  
  struct Config
  {
    /// string based identification
    std::string                          configurationName = "undefined";
    const InDetDD::TRT_DetectorManager*   mng;
    std::shared_ptr<const LayerCreator> layerCreator = nullptr;
    std::shared_ptr<ElementVector> elementStore;
    const Acts::TrackingGeometrySvc* trackingGeometrySvc = nullptr;
  };
  
  /// Constructor
  /// @param cfg is the configuration struct
  /// @param logger the local logging instance
  GeoModelStrawLayerBuilder(const Config&           cfg,
                       std::unique_ptr<const Logger> logger
                       = getDefaultLogger("GMSLayBldr", Logging::INFO))
    : m_logger(std::move(logger))
  {
    m_cfg = cfg;
  }
  
  ~GeoModelStrawLayerBuilder() {}

  const LayerVector
  negativeLayers() const override;

  const LayerVector
  centralLayers() const override;

  const LayerVector
  centralLayers();

  const LayerVector
  positiveLayers() const override;
  
  const std::string&
  identification() const override
  {
    return m_cfg.configurationName;
  }

  const LayerVector
  endcapLayers(int side);

private:
  /// configruation object
  Config m_cfg;

  /// Private access to the logger
  const Logger&
  logger() const
  {
    return *m_logger;
  }
  
  /// logging instance
  std::unique_ptr<const Logger> m_logger;


};
}

#endif
