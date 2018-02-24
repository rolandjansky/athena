// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "GeomACTS/GeoModelLayerBuilder.hpp"
#include "GeomACTS/IdentityHelper.hpp"

#include "ACTS/Tools/ILayerBuilder.hpp"

const Acts::LayerVector
Acts::GeoModelLayerBuilder::negativeLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<GeoModelLayerBuilder*>(this);
  LayerVector nVector;
  mutableThis->buildLayers(nVector, -1);
  return nVector;
}

const Acts::LayerVector
Acts::GeoModelLayerBuilder::centralLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<GeoModelLayerBuilder*>(this);
  LayerVector cVector;
  mutableThis->buildLayers(cVector, 0);
  return cVector;
}

const Acts::LayerVector
Acts::GeoModelLayerBuilder::positiveLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<GeoModelLayerBuilder*>(this);
  LayerVector pVector;
  mutableThis->buildLayers(pVector, 1);
  return pVector;

}

std::vector<std::shared_ptr<const Acts::GeoModelDetectorElement>>
Acts::GeoModelLayerBuilder::getDetectorElements() const
{
  auto siDetMng = static_cast<const InDetDD::SiDetectorManager*>(m_cfg.mng);
  
  std::vector<std::shared_ptr<const GeoModelDetectorElement>> elements;

  //if (siDetMng) {
    InDetDD::SiDetectorElementCollection::const_iterator iter;
    for (iter = siDetMng->getDetectorElementBegin();
      iter != siDetMng->getDetectorElementEnd();
      ++iter) {
      const InDetDD::SiDetectorElement* siDetElement = *iter;
      elements.push_back(std::make_shared<const GeoModelDetectorElement>(siDetElement));
    }
  //}
  //else {
    //throw std::domain_error("GeoModelLayerBuilder does not know how "
                            //"to operator on this type of Detector Manager");
  //}

  return elements;
}

void
Acts::GeoModelLayerBuilder::buildLayers(LayerVector& layersOutput, int type)
{

  if (type != 0) return {};

  const InDetDD::SiNumerology* siNums = m_cfg.mng->getNumerology();

  //size_t numLayers = siNums->numLayers();
  //ACTS_VERBOSE("Numerology reports: " << numLayers << " layers");
  //ACTS_VERBOSE("                    ")

  std::vector<std::shared_ptr<const GeoModelDetectorElement>> elements = getDetectorElements();


  std::map<std::pair<int, int>, std::vector<const Surface*>> layers;

  for (const auto &element : elements) {
    //const InDetDD::SiDetectorElement* siDetElement = *iter;

    //auto element
      //= std::make_shared<const GeoModelDetectorElement>(siDetElement);
    
      //IdentityHelper id(element);
    IdentityHelper id = element->identityHelper();

    std::cout << "ELEMENT: ";
    std::cout << id->bec() << " ";
    std::cout << id->layer_disk() << " ";;
    std::cout << id->eta_module() << " ";
    std::cout << id->phi_module() << " ";
    std::cout << "Z = " << element->surface().center().z();
    std::cout << " PHI = " << element->surface().center().phi();
    std::cout << " R = " << element->surface().center().perp() << std::endl;


    // wrong subdetector
    //if (m_cfg.subdetector != element->det()) continue;
    // want barrel but not barrel
    if (type == 0 && id.bec() != 0) continue;

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
      layers.insert(std::make_pair(layerKey, std::vector<const Surface*>()));
      // layers.at(elementLayer) = std::vector<const Surface*>();
    }

    // push into correct layer
    layers.at(layerKey).push_back(&element->surface());
  }

  for (const auto& layerPair : layers) {

    std::vector<const Surface*> layerSurfaces = layerPair.second;

    if (type == 0) {  // BARREL
      // layers and extent are determined, build actual layer
      ProtoLayer pl(layerSurfaces);
      pl.envR    = {0, 0};
      pl.envZ    = {0, 0};
      auto layer = m_cfg.layerCreator->cylinderLayer(
          layerSurfaces, 16, 20, pl);

      layersOutput.push_back(layer);
      // std::cout << (*layer->surfaceArray()) << std::endl;
    } else {  // ENDCAP
      // for(const auto &srf : layerSurfaces) {
      // Vector3D ctr = srf->center();
      // std::cout << "SRF: " << ctr.z() << " " << ctr.perp() << " " <<
      // ctr.phi() << std::endl;
      //}
      ProtoLayer pl(layerSurfaces);
      pl.envR    = {0, 0};
      pl.envZ    = {0, 0};
      auto layer = m_cfg.layerCreator->discLayer(
          layerSurfaces, equidistant, equidistant, pl);
      layersOutput.push_back(layer);
    }
  }
}
