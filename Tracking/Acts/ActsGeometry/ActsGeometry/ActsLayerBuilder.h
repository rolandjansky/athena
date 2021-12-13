/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSLAYERBUILDER_H
#define ACTSGEOMETRY_ACTSLAYERBUILDER_H

// PACKAGE
#include "ActsGeometry/ActsDetectorElement.h"

// ATHENA

// ACTS
#include "Acts/Geometry/ILayerBuilder.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/BinningType.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Definitions/Units.hpp"

class ActsTrackingGeomtrySvc;

namespace InDetDD {
  class SiDetectorManager;
}

namespace Acts {
class Surface;
class LayerCreator;
}

/// @class ActsLayerBuilder
class ActsLayerBuilder : public Acts::ILayerBuilder
{
public:
  enum class Mode {
    Undefined, Pixel, SCT, TRT, ITkPixelInner, ITkPixelOuter, ITkStrip,
  };

  using ElementVector
      = std::vector<std::shared_ptr<const ActsDetectorElement>>;

  /// @struct Config
  /// nested configuration struct for steering of the layer builder

  struct Config
  {
    /// string based identification
    std::string                          configurationName = "undefined";
    Mode mode = Mode::Undefined;
    const InDetDD::SiDetectorManager*   mng;
    std::shared_ptr<const Acts::LayerCreator> layerCreator = nullptr;
    /// the binning type of the contained surfaces in phi
    /// (equidistant/arbitrary)
    Acts::BinningType bTypePhi = Acts::equidistant;
    /// the binning type of the contained surfaces in r
    /// (equidistant/arbitrary)
    Acts::BinningType bTypeR = Acts::equidistant;
    /// the binning type of the contained surfaces in z
    /// (equidistant/arbitrary)
    Acts::BinningType                    bTypeZ = Acts::equidistant;
    std::shared_ptr<ElementVector> elementStore;

    std::pair<double, double> endcapEnvelopeR = {2 * Acts::UnitConstants::mm,
                                                 2 * Acts::UnitConstants::mm};
    std::pair<double, double> endcapEnvelopeZ = {2 * Acts::UnitConstants::mm,
                                                 2 * Acts::UnitConstants::mm};

    std::pair<double, double> barrelEnvelopeR = {2 * Acts::UnitConstants::mm,
                                                 2 * Acts::UnitConstants::mm};
    std::pair<double, double> barrelEnvelopeZ = {2 * Acts::UnitConstants::mm,
                                                 2 * Acts::UnitConstants::mm};

    std::pair<size_t, size_t> endcapMaterialBins = {20, 5};
    std::pair<size_t, size_t> barrelMaterialBins = {10, 10};

    std::function<bool(const Acts::GeometryContext &, Acts::BinningValue,
                       const Acts::Surface *, const Acts::Surface *)>
        surfaceMatcher;

    bool doEndcapLayerMerging = true;

    bool objDebugOutput = false;
  };

  /// Constructor
  /// @param cfg is the configuration struct
  /// @param logger the local logging instance
  ActsLayerBuilder(const Config&                cfg,
                       std::unique_ptr<const Acts::Logger> logger
                       = Acts::getDefaultLogger("GMLayBldr", Acts::Logging::INFO));

  /// Destructor
  ~ActsLayerBuilder() {}

  const Acts::LayerVector
  negativeLayers(const Acts::GeometryContext& gctx) const override;

  const Acts::LayerVector
  centralLayers(const Acts::GeometryContext& gctx) const override;

  const Acts::LayerVector
  positiveLayers(const Acts::GeometryContext& gctx) const override;


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

  ActsLayerBuilder::Config
  getConfiguration() const
  {
    return m_cfg;
  }

  virtual
  const std::string&
  identification() const override
  {
    return m_cfg.configurationName;
  }

  /// set logging instance
  void
  setLogger(std::unique_ptr<const Acts::Logger> logger);

private:
  /// configruation object
  Config m_cfg;

  /// Private access to the logger
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }

  std::vector<std::shared_ptr<const ActsDetectorElement>>
  getDetectorElements() const;

  /// logging instance
  std::unique_ptr<const Acts::Logger> m_logger;

  ///// @todo make clear where the ActsDetectorElement lives
  //std::vector<std::shared_ptr<const ActsDetectorElement>> m_elementStore;


  // Private helper method : build layers
  // @param layers is goint to be filled
  // @param type is the indication which ones to build -1 | 0 | 1
  void
  buildBarrel(const Acts::GeometryContext& gctx, Acts::LayerVector& layersOutput);

  void
  buildEndcap(const Acts::GeometryContext& gctx, Acts::LayerVector& layersOutput, int type = 0);

};

std::ostream& operator<<(std::ostream& os, const ActsLayerBuilder::Mode& mode);

#endif
