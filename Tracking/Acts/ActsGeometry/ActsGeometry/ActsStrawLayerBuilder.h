/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSSTRAWLAYERBUILDER_H
#define ACTSGEOMETRY_ACTSSTRAWLAYERBUILDER_H


// ATHENA

// PACKAGE

// ACTS
#include "Acts/Geometry/ILayerBuilder.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Geometry/GeometryContext.hpp"

class TRT_ID;
class TRT_DetectorManager;
class ActsDetectorElement;

namespace Acts {
class Surface;
class LayerCreator;
}

/// @class ActsStrawLayerBuilder
class ActsStrawLayerBuilder : public Acts::ILayerBuilder
{
public:
  using ElementVector
      = std::vector<std::shared_ptr<const ActsDetectorElement>>;

  struct Config
  {
    /// string based identification
    std::string                          configurationName = "undefined";
    const InDetDD::TRT_DetectorManager*   mng;
    std::shared_ptr<const Acts::LayerCreator> layerCreator = nullptr;
    std::shared_ptr<ElementVector> elementStore;
    const TRT_ID* idHelper = nullptr;
  };

  /// Constructor
  /// @param cfg is the configuration struct
  /// @param logger the local logging instance
  ActsStrawLayerBuilder(const Config&           cfg,
                       std::unique_ptr<const Acts::Logger> logger
                       = Acts::getDefaultLogger("GMSLayBldr", Acts::Logging::INFO))
    : m_logger(std::move(logger))
  {
    m_cfg = cfg;
  }

  ~ActsStrawLayerBuilder() {}

  const Acts::LayerVector
  negativeLayers(const Acts::GeometryContext& gctx) const override;

  const Acts::LayerVector
  centralLayers(const Acts::GeometryContext& gctx) const override;

  const Acts::LayerVector
  centralLayers(const Acts::GeometryContext& gctx);

  const Acts::LayerVector
  positiveLayers(const Acts::GeometryContext& gctx) const override;

  const std::string&
  identification() const override
  {
    return m_cfg.configurationName;
  }

  const Acts::LayerVector
  endcapLayers(const Acts::GeometryContext& gctx, int side);

private:
  /// configruation object
  Config m_cfg;

  /// Private access to the logger
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }

  /// logging instance
  std::unique_ptr<const Acts::Logger> m_logger;


};

#endif
