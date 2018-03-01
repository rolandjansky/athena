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
class GeoModelLayerBuilder : public ILayerBuilder
{
public:
  using ElementVector
      = std::vector<std::shared_ptr<const GeoModelDetectorElement>>;

  /// @struct Config
  /// nested configuration struct for steering of the layer builder

  struct Config
  {
    /// string based identification
    std::string                          configurationName = "undefined";
    GeoModelDetectorElement::Subdetector subdetector
        = GeoModelDetectorElement::Subdetector::Pixel;
    const InDetDD::SiDetectorManager*   mng;
    std::shared_ptr<const LayerCreator> layerCreator = nullptr;
    /// the binning type of the contained surfaces in phi
    /// (equidistant/arbitrary)
    BinningType bTypePhi = equidistant;
    /// the binning type of the contained surfaces in r
    /// (equidistant/arbitrary)
    BinningType bTypeR = equidistant;
    /// the binning type of the contained surfaces in z
    /// (equidistant/arbitrary)
    BinningType                    bTypeZ = equidistant;
    std::shared_ptr<ElementVector> elementStore;
  };

  /// Constructor
  /// @param cfg is the configuration struct
  /// @param logger the local logging instance
  GeoModelLayerBuilder(const Config&                cfg,
                       std::unique_ptr<const Logger> logger
                       = getDefaultLogger("GMLayBldr", Logging::INFO))
    : m_logger(std::move(logger))
  {
    // std::cout << "GMLB construct" << std::endl;
    m_cfg = cfg;
  }

  /// Destructor
  ~GeoModelLayerBuilder() {}

  const LayerVector
  negativeLayers() const override;

  const LayerVector
  centralLayers() const override;

  const LayerVector
  positiveLayers() const override;

  /// Name identification
  // const std::string&
  // identification() const final;

  /// set the configuration object
  /// @param cfg is the configuration struct
  void
  setConfiguration(const Config& cfg);

  /// get the configuration object
  // Config
  // getConfiguration() const;

  GeoModelLayerBuilder::Config
  getConfiguration() const
  {
    return m_cfg;
  }

  const std::string&
  identification() const
  {
    return m_cfg.configurationName;
  }

  /// set logging instance
  void
  setLogger(std::unique_ptr<const Logger> logger);

private:
  /// configruation object
  Config m_cfg;

  /// Private access to the logger
  const Logger&
  logger() const
  {
    return *m_logger;
  }

  std::vector<std::shared_ptr<const GeoModelDetectorElement>>
  getDetectorElements() const;

  /// logging instance
  std::unique_ptr<const Logger> m_logger;

  ///// @todo make clear where the GeoModelDetectorElement lives
  //std::vector<std::shared_ptr<const GeoModelDetectorElement>> m_elementStore;


  // Private helper method : build layers
  // @param layers is goint to be filled
  // @param type is the indication which ones to build -1 | 0 | 1
  void
  buildLayers(LayerVector& layersOutput, int type = 0);
};
}

#endif
