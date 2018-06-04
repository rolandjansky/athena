#ifndef ACTS_GEOMODELPLUGIN_GEOMODELLAYERBUILDER_H
#define ACTS_GEOMODELPLUGIN_GEOMODELLAYERBUILDER_H

// PACKAGE
#include "ActsGeometry/GeoModelDetectorElement.hpp"

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

// ACTS
#include "Acts/Layers/Layer.hpp"
#include "Acts/Layers/ProtoLayer.hpp"
#include "Acts/Tools/ILayerBuilder.hpp"
#include "Acts/Tools/LayerCreator.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Logger.hpp"

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

    std::pair<size_t, size_t> endcapMaterialBins = {20, 5};
    std::pair<size_t, size_t> barrelMaterialBins = {10, 10};
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
