// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


#ifndef ACTS_GEOMODELPLUGIN_GEOMODELSTRAWLAYERBUILDER_H
#define ACTS_GEOMODELPLUGIN_GEOMODELSTRAWLAYERBUILDER_H


#ifndef ACTS_PARAMETER_DEFINITIONS_PLUGIN
#define ACTS_PARAMETER_DEFINITIONS_PLUGIN "ACTS/Utilities/detail/DefaultParameterDefinitions.hpp"
#endif
// ATHENA INCLUDES
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

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
