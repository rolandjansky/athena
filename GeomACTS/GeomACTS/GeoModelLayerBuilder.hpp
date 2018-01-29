// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// GeoModelLayerBuilder.h, ACTS project, TGeoDetector plugin
///////////////////////////////////////////////////////////////////

#ifndef ACTS_GEOMODELPLUGIN_GEOMODELLAYERBUILDER_H
#define ACTS_GEOMODELPLUGIN_GEOMODELLAYERBUILDER_H

// @todo: re-check this


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

// ACTS
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Layers/ProtoLayer.hpp"
#include "ACTS/Tools/ILayerBuilder.hpp"
#include "ACTS/Tools/LayerCreator.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

#include "GeomACTS/GeoModelDetectorElement.hpp"

#define DUMP(v) #v << "=" << v
#define PRV(v)                                                                 \
  #v << "(x="                                                                  \
     << v.x() << ", y="                                                        \
              << v.y() << ", z="                                               \
                       << v.z() << " r="                                       \
                                << v.perp() << ", phi=" << v.phi() << ")"

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
  template <typename ConfigT>
  GeoModelLayerBuilder(const ConfigT&                cfg,
                       std::unique_ptr<const Logger> logger
                       = getDefaultLogger("LayerArrayCreator", Logging::INFO))
    : m_logger(std::move(logger))
  {
    // std::cout << "GMLB construct" << std::endl;
    m_cfg = cfg;
  }

  /// Destructor
  //~GeoModelLayerBuilder();

  ///// LayerBuilder interface method - returning the layers at negative side
  // const LayerVector
  // negativeLayers() const final;

  ///// LayerBuilder interface method - returning the central layers
  // const LayerVector
  // centralLayers() const final;

  ///// LayerBuilder interface method - returning the layers at negative side
  // const LayerVector
  // positiveLayers() const final;

  const LayerVector
  negativeLayers() const
  {
    // @todo Remove this hack once the m_elementStore mess is sorted out
    auto        mutableThis = const_cast<GeoModelLayerBuilder*>(this);
    LayerVector nVector;
    mutableThis->buildLayers(nVector, -1);
    return nVector;
  }

  const LayerVector
  centralLayers() const
  {
    // @todo Remove this hack once the m_elementStore mess is sorted out
    auto        mutableThis = const_cast<GeoModelLayerBuilder*>(this);
    LayerVector cVector;
    mutableThis->buildLayers(cVector, 0);
    return cVector;
  }

  const LayerVector
  positiveLayers() const
  {
    // @todo Remove this hack once the m_elementStore mess is sorted out
    auto        mutableThis = const_cast<GeoModelLayerBuilder*>(this);
    LayerVector pVector;
    mutableThis->buildLayers(pVector, 1);
    return pVector;
  }

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

  /// logging instance
  std::unique_ptr<const Logger> m_logger;

  /// @todo make clear where the GeoModelDetectorElement lives
  std::vector<std::shared_ptr<const GeoModelDetectorElement>> m_elementStore;

  /// Private helper function to parse the geometry tree
  // void
  // collectSensitive(std::vector<const Surface*>& layerSurfaces,
  // TGeoVolume*                  tgVolume,
  // TGeoNode*                    tgNode,
  // const TGeoMatrix&            ctGlobal,
  // const LayerConfig&           layerConfig,
  // int                          type,
  // bool                         correctVolume = false,
  // const std::string&           offset        = "");

  // Private helper method : build layers
  // @param layers is goint to be filled
  // @param type is the indication which ones to build -1 | 0 | 1
  void
  buildLayers(LayerVector& layersOutput, int type = 0)
  {

    InDetDD::SiDetectorElementCollection::const_iterator iter;

    // collect elements for this layer type
    // also determine layer count
    //size_t nLayers = 0;

    std::map<std::pair<int, int>, std::vector<const Surface*>> layers;

    for (iter = m_cfg.mng->getDetectorElementBegin();
         iter != m_cfg.mng->getDetectorElementEnd();
         ++iter) {
      const InDetDD::SiDetectorElement* siDetElement = *iter;

      auto element
          = std::make_shared<const GeoModelDetectorElement>(siDetElement);

      // std::cout << "ELEMENT: ";
      // std::cout << element->bec() << " ";
      // std::cout << element->layer_disk() << " ";;
      // std::cout << element->eta_module() << " ";
      // std::cout << element->phi_module() << " ";
      // std::cout << "Z = " << element->surface().center().z() << std::endl;

      // wrong subdetector
      if (m_cfg.subdetector != element->det()) continue;
      // want barrel but not barrel
      if (type == 0 && element->bec() != 0) continue;

      if (type != 0 && element->bec() == 0) continue;
      // want endcap, but wrong side
      if (type != 0 && type * element->bec() < 0) continue;

      m_cfg.elementStore->push_back(element);

      // const PixelID* idhlp = dynamic_cast<const
      // PixelID*>(siDetElement->getIdHelper());
      // int layer_disk = idhlp->layer_disk(siDetElement->identify());
      // int eta_module = idhlp->eta_module(siDetElement->identify());

      // std::cout << "SELECTED: ";
      // std::cout << element->bec() << " ";
      // std::cout << "(" << element->layer_disk() << " " << layer_disk << ") ";
      // std::cout << "(" << element->eta_module() << " " << eta_module << ") ";
      // std::cout << element->phi_module() << " ";
      // std::cout << "Z = " << element->surface().center().z() << std::endl;

      int elementLayer;
      elementLayer = element->layer_disk();

      std::pair<int, int> layerKey(elementLayer, element->bec());

      if (layers.count(layerKey) == 0) {
        // layer not set yet
        layers.insert(std::make_pair(layerKey, std::vector<const Surface*>()));
        // layers.at(elementLayer) = std::vector<const Surface*>();
      }

      // push into correct layer
      layers.at(layerKey).push_back(&element->surface());

      // layer_disk_max appears to be broken, determine empirically
      // int elementLayer = static_cast<inz>(element->layer_disk());
      // nLayers             = std::max(nLayers, elementLayer + 1);

      // sort surface into corresponding layer vector
      // resize layer surface vector if necessary
      // if (layers.size() < elementLayer + 1) {
      // layers.resize(elementLayer + 1);
      //}

      // std::vector<const Surface*>& layerElements
      //= layers.at(elementLayer);
      // layerElements.push_back(&element->surface());
    }

    for (const auto& layerPair : layers) {

      std::vector<const Surface*> layerSurfaces = layerPair.second;

      if (type == 0) {  // BARREL
        // layers and extent are determined, build actual layer
        ProtoLayer pl(layerSurfaces);
        pl.envR    = {2, 2};
        pl.envZ    = {20, 20};
        auto layer = m_cfg.layerCreator->cylinderLayer(
            layerSurfaces, equidistant, equidistant, pl);

        layersOutput.push_back(layer);
        // std::cout << (*layer->surfaceArray()) << std::endl;
      } else {  // ENDCAP
        // for(const auto &srf : layerSurfaces) {
        // Vector3D ctr = srf->center();
        // std::cout << "SRF: " << ctr.z() << " " << ctr.perp() << " " <<
        // ctr.phi() << std::endl;
        //}
        ProtoLayer pl(layerSurfaces);
        pl.envR    = {2, 2};
        pl.envZ    = {5, 5};
        auto layer = m_cfg.layerCreator->discLayer(
            layerSurfaces, equidistant, equidistant, pl);
        layersOutput.push_back(layer);
        // std::cout << (*layer->surfaceArray()) << std::endl;
      }
    }
  }
};
}

#endif
