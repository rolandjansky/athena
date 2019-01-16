/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// ATHENA
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

// PACKAGE
#include "ActsGeometry/ActsLayerBuilder.h"
#include "ActsInterop/IdentityHelper.h"

// ACTS
#include "Acts/Material/SurfaceMaterialProxy.hpp"
#include "Acts/Surfaces/CylinderSurface.hpp"
#include "Acts/Surfaces/DiscSurface.hpp"
#include "Acts/Layers/GenericApproachDescriptor.hpp"
#include "Acts/Utilities/ApproachDescriptor.hpp"
#include "Acts/Layers/ProtoLayer.hpp"
#include "Acts/Tools/LayerCreator.hpp"
#include "Acts/Utilities/Definitions.hpp"

using Acts::Surface;
using Acts::Transform3D;
using Acts::Translation3D;

const Acts::LayerVector
ActsLayerBuilder::negativeLayers() const
{
  ACTS_VERBOSE("Building negative layers");
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<ActsLayerBuilder*>(this);
  Acts::LayerVector nVector;
  mutableThis->buildLayers(nVector, -1);
  return nVector;
}

const Acts::LayerVector
ActsLayerBuilder::centralLayers() const
{
  ACTS_VERBOSE("Building central layers");
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<ActsLayerBuilder*>(this);
  Acts::LayerVector cVector;
  mutableThis->buildLayers(cVector, 0);
  return cVector;
}

const Acts::LayerVector
ActsLayerBuilder::positiveLayers() const
{
  ACTS_VERBOSE("Building positive layers");
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<ActsLayerBuilder*>(this);
  Acts::LayerVector pVector;
  mutableThis->buildLayers(pVector, 1);
  return pVector;

}

std::vector<std::shared_ptr<const ActsDetectorElement>>
ActsLayerBuilder::getDetectorElements() const
{
  auto siDetMng = static_cast<const InDetDD::SiDetectorManager*>(m_cfg.mng);
  
  std::vector<std::shared_ptr<const ActsDetectorElement>> elements;

  InDetDD::SiDetectorElementCollection::const_iterator iter;
  for (iter = siDetMng->getDetectorElementBegin();
      iter != siDetMng->getDetectorElementEnd();
      ++iter) {
    const InDetDD::SiDetectorElement* siDetElement = *iter;
    //elements.emplace_back(siDetElement, m_cfg.trackingGeometrySvc);
    elements.push_back(
        std::make_shared<const ActsDetectorElement>(
          siDetElement, m_cfg.trackingGeometrySvc));
  }

  return elements;
}

void
ActsLayerBuilder::buildLayers(Acts::LayerVector& layersOutput, int type)
{

  std::vector<std::shared_ptr<const ActsDetectorElement>> elements = getDetectorElements();


  std::map<std::pair<int, int>, std::vector<std::shared_ptr<const Surface>>> layers;

  for (const auto &element : elements) {

    IdentityHelper id = element->identityHelper();

    // wrong subdetector
    // want barrel but not barrel
    if (type == 0 && id.bec() != 0) continue;
    // want endcap but is barrel
    if (type != 0 && id.bec() == 0) continue;
    // want endcap, but wrong side
    if (type != 0 && type * id.bec() < 0) continue;

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
    elementLayer = id.layer_disk();

    std::pair<int, int> layerKey(elementLayer, id.bec());

    if (layers.count(layerKey) == 0) {
      // layer not set yet
      layers.insert(std::make_pair(layerKey, std::vector<std::shared_ptr<const Surface>>()));
    }

    // push into correct layer
    layers.at(layerKey).push_back(element->surface().getSharedPtr());

  }

  // @TODO: Maybe exclude BLM (BCM?)

  ACTS_VERBOSE("Found " << layers.size() << " layers");
  // pre loop for more concise output. only do if we actually print it
  if(logger().doPrint(Acts::Logging::VERBOSE)) {
    size_t n = 1;
    for (const auto& layerPair : layers) {
      const std::vector<std::shared_ptr<const Surface>>& layerSurfaces = layerPair.second;
      auto key = layerPair.first;
      Acts::ProtoLayer pl(layerSurfaces);
      ACTS_VERBOSE("Layer #" << n << " with layerKey: ("
          << key.first << ", " << key.second << ")");
      if (type == 0) {  // BARREL
        ACTS_VERBOSE(" -> at rMin / rMax: " << pl.minR << " / " << pl.maxR);
      }
      else {
        ACTS_VERBOSE(" -> at zMin / zMax: " << pl.minZ << " / " << pl.maxZ);
      }

      n++;
    }
  }

  for (const auto& layerPair : layers) {

    std::unique_ptr<Acts::ApproachDescriptor> approachDescriptor = nullptr;
    std::shared_ptr<const Acts::SurfaceMaterialProxy> materialProxy = nullptr;

    // use ref here, copy later
    const std::vector<std::shared_ptr<const Surface>>& layerSurfaces = layerPair.second;

    if (type == 0) {  // BARREL
      // layers and extent are determined, build actual layer
      Acts::ProtoLayer pl(layerSurfaces);
      pl.envR    = {0, 0};
      pl.envZ    = {20, 20};
        
      double binPosZ   = 0.5 * (pl.minZ + pl.maxZ);
      double envZShift = 0.5 * (-pl.envZ.first + pl.envZ.second);
      double layerZ    = binPosZ + envZShift;
      double layerHalfZ
        = std::abs(pl.maxZ + pl.envZ.second - layerZ);

      auto transform
        = std::make_shared<const Transform3D>(Translation3D(0., 0., -layerZ));
      // set up approach descriptor

      std::shared_ptr<Acts::CylinderSurface> innerBoundary 
        = Acts::Surface::makeShared<Acts::CylinderSurface>(transform, pl.minR, layerHalfZ);
      
      std::shared_ptr<Acts::CylinderSurface> outerBoundary 
        = Acts::Surface::makeShared<Acts::CylinderSurface>(transform, pl.maxR, layerHalfZ);
      
      std::shared_ptr<Acts::CylinderSurface> centralSurface 
        = Acts::Surface::makeShared<Acts::CylinderSurface>(transform, (pl.minR + pl.maxR)/2., layerHalfZ);
      
      size_t binsPhi = m_cfg.barrelMaterialBins.first;
      size_t binsZ = m_cfg.barrelMaterialBins.second;

      Acts::BinUtility materialBinUtil(
          binsPhi, -M_PI, M_PI, Acts::closed, Acts::binPhi);
      materialBinUtil += Acts::BinUtility(
            binsZ, -layerHalfZ, layerHalfZ, Acts::open, Acts::binZ, transform);

      materialProxy
        = std::make_shared<const Acts::SurfaceMaterialProxy>(materialBinUtil);

      ACTS_VERBOSE("[L] Layer is marked to carry support material on Surface ( "
          "inner=0 / center=1 / outer=2 ) : " << "inner");
      ACTS_VERBOSE("with binning: [" << binsPhi << ", " << binsZ << "]");

      ACTS_VERBOSE("Created ApproachSurfaces for cylinder layer at:");
      ACTS_VERBOSE(" - inner:   R=" << pl.minR);
      ACTS_VERBOSE(" - central: R=" << (pl.minR + pl.maxR)/2.);
      ACTS_VERBOSE(" - outer:   R=" << pl.maxR);

      // set material on inner
      // @TODO: make this configurable somehow
      innerBoundary->setAssociatedMaterial(materialProxy);

      std::vector<std::shared_ptr<const Acts::Surface>> aSurfaces;
      aSurfaces.push_back(std::move(innerBoundary));
      aSurfaces.push_back(std::move(centralSurface));
      aSurfaces.push_back(std::move(outerBoundary));

      approachDescriptor 
        = std::make_unique<Acts::GenericApproachDescriptor>(std::move(aSurfaces));

      auto layer = m_cfg.layerCreator->cylinderLayer(layerSurfaces,
                                                     Acts::equidistant, 
                                                     Acts::equidistant, 
                                                     pl, 
                                                     transform,
                                                     std::move(approachDescriptor));

      layersOutput.push_back(layer);
    } else {  // ENDCAP
      Acts::ProtoLayer pl(layerSurfaces);
      pl.envR    = {0, 0};
      pl.envZ    = {10, 10};

      // copied from layercreator
      double layerZ
        = 0.5 * (pl.minZ - pl.envZ.first + pl.maxZ
            + pl.envZ.second);
      double layerThickness = (pl.maxZ - pl.minZ)
        + pl.envZ.first + pl.envZ.second;

      double layerZInner = layerZ - layerThickness/2.;
      double layerZOuter = layerZ + layerThickness/2.;

      if (std::abs(layerZInner) > std::abs(layerZOuter)) std::swap(layerZInner, layerZOuter);

      auto transformNominal
        = std::make_shared<const Transform3D>(Translation3D(0., 0., layerZ));
      
      auto transformInner
        = std::make_shared<const Transform3D>(Translation3D(0., 0., layerZInner));
      
      auto transformOuter
        = std::make_shared<const Transform3D>(Translation3D(0., 0., layerZOuter));

      std::shared_ptr<Acts::DiscSurface> innerBoundary 
        = Acts::Surface::makeShared<Acts::DiscSurface>(transformInner, pl.minR, pl.maxR);
      
      std::shared_ptr<Acts::DiscSurface> nominalSurface 
        = Acts::Surface::makeShared<Acts::DiscSurface>(transformNominal, pl.minR, pl.maxR);
      
      std::shared_ptr<Acts::DiscSurface> outerBoundary 
        = Acts::Surface::makeShared<Acts::DiscSurface>(transformOuter, pl.minR, pl.maxR);

      size_t matBinsPhi = m_cfg.endcapMaterialBins.first;
      size_t matBinsR = m_cfg.endcapMaterialBins.second;

      Acts::BinUtility materialBinUtil(
          matBinsPhi, -M_PI, M_PI, Acts::closed, Acts::binPhi);
      materialBinUtil += Acts::BinUtility(
          matBinsR, pl.minR, pl.maxR, Acts::open, Acts::binR, transformNominal);
      
      materialProxy
        = std::make_shared<const Acts::SurfaceMaterialProxy>(materialBinUtil);

      ACTS_VERBOSE("[L] Layer is marked to carry support material on Surface ( "
          "inner=0 / center=1 / outer=2 ) : " << "inner");
      ACTS_VERBOSE("with binning: [" << matBinsPhi << ", " << matBinsR << "]");

      ACTS_VERBOSE("Created ApproachSurfaces for disc layer at:");
      ACTS_VERBOSE(" - inner:   Z=" << layerZInner);
      ACTS_VERBOSE(" - central: Z=" << layerZ);
      ACTS_VERBOSE(" - outer:   Z=" << layerZOuter);
      

      // set material on inner
      // @TODO: make this configurable somehow
      innerBoundary->setAssociatedMaterial(materialProxy);

      int nModPhi = std::numeric_limits<int>::max();
      int nModR = 0;

      // want to figure out bins in phi
      for (const auto& srf : layerSurfaces) {
        auto elm = dynamic_cast<const ActsDetectorElement*>(srf->associatedDetectorElement());
        if (elm){
          auto id = elm->identityHelper();
          int phi_mod_max = id.phi_module_max();
          int eta_mod_max = id.eta_module_max();
          nModPhi = std::min(nModPhi, phi_mod_max+1);
          nModR = eta_mod_max+1;
        }
      }

      ACTS_VERBOSE("Identifier reports: " << nModPhi << " is lowest for " << nModR << " r-rings");

      // the modules in the innermost r-rings are exactly shifted by one half module width
      // since it's the same number of modules, this gives binning trouble. Reduce bins
      // by half: about 2 module pairs should be in each bin. This should be fine.
      // @TODO: evaluate
      size_t nBinsPhi = nModPhi/2.;
      size_t nBinsR = nModR;

      ACTS_VERBOSE("Creating r x phi binned layer with " << nBinsR << " x " << nBinsPhi << " bins");

      std::vector<std::shared_ptr<const Acts::Surface>> aSurfaces;
      aSurfaces.push_back(std::move(innerBoundary));
      aSurfaces.push_back(std::move(nominalSurface));
      aSurfaces.push_back(std::move(outerBoundary));

      approachDescriptor 
        = std::make_unique<Acts::GenericApproachDescriptor>(aSurfaces);

      auto layer = m_cfg.layerCreator->discLayer(layerSurfaces, 
                                                 nBinsR, 
                                                 nBinsPhi, 
                                                 pl,
                                                 transformNominal,
                                                 std::move(approachDescriptor));

      layersOutput.push_back(layer);
    }
  }
}
