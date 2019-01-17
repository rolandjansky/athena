/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ATHENA
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"
#include "InDetIdentifier/TRT_ID.h"
#include "Identifier/Identifier.h"

// PACKAGE
#include "ActsGeometry/ActsStrawLayerBuilder.h"
#include "ActsGeometry/ActsDetectorElement.h"

// ACTS
#include "Acts/Tools/ILayerBuilder.hpp"
#include "Acts/Surfaces/CylinderSurface.hpp"
#include "Acts/Surfaces/StrawSurface.hpp"
#include "Acts/Surfaces/DiscSurface.hpp"
#include "Acts/Tools/LayerCreator.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Layers/ProtoLayer.hpp"

// STL
#include <iostream>
#include <fstream>

using Acts::Transform3D;
using Acts::Vector3D;

const Acts::LayerVector
ActsStrawLayerBuilder::negativeLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<ActsStrawLayerBuilder*>(this);
  return mutableThis->endcapLayers(-1);
}

const Acts::LayerVector
ActsStrawLayerBuilder::centralLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<ActsStrawLayerBuilder*>(this);
  return mutableThis->centralLayers();
}

const Acts::LayerVector
ActsStrawLayerBuilder::positiveLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<ActsStrawLayerBuilder*>(this);
  return mutableThis->endcapLayers(1);

}


const Acts::LayerVector
ActsStrawLayerBuilder::centralLayers()
{
  ACTS_VERBOSE("Building central Straw layers")

  const InDetDD::TRT_Numerology* trtNums = m_cfg.mng->getNumerology();
  size_t    nBarrelRings  = trtNums->getNBarrelRings();
  size_t    nBarrelPhiSectors = trtNums->getNBarrelPhi();


  ACTS_VERBOSE("- Numerology reports: - " << nBarrelRings << " barrel rings");
  ACTS_VERBOSE("                      - " << nBarrelPhiSectors << " barrel phi sectors");

  Acts::LayerVector layers;

  std::vector<Acts::ProtoLayer> protoLayers;

  for(size_t iring=0; iring < nBarrelRings;iring++) {
    ACTS_VERBOSE("- Collecting elements for ring " << iring);
  
    // were calculating min/max radius while were at it.
    Acts::ProtoLayer pl;
    pl.minR = std::numeric_limits<double>::max();
    pl.maxR = std::numeric_limits<double>::lowest();
    pl.minZ = std::numeric_limits<double>::max();
    pl.maxZ = std::numeric_limits<double>::lowest();
    pl.minPhi = -M_PI;
    pl.maxPhi = M_PI;

    pl.envZ = {1, 1};
    pl.envR = {0, 0};

    double fudge = 0;
    // RING in TRT speak is translated to Layer in ACTS speak
    std::vector<std::shared_ptr<const Acts::Surface>> layerSurfaces;

    size_t nBarrelLayers = trtNums->getNBarrelLayers(iring);
    ACTS_VERBOSE("  - Numerology reports: " << nBarrelLayers << " layers in ring " << iring);
    for (size_t ilayer=0;ilayer < nBarrelLayers;ilayer++) {
      for (size_t iphisec=0;iphisec < nBarrelPhiSectors;iphisec++) {
        // what is this?
        for (size_t iposneg=0;iposneg<2;iposneg++) {

          const InDetDD::TRT_BarrelElement* brlElem 
            = m_cfg.mng->getBarrelElement(iposneg, iring, iphisec, ilayer);

          unsigned int nStraws = brlElem->nStraws();

          for(unsigned int istraw=0;istraw<nStraws;istraw++) {

            auto trf = std::make_shared<const Transform3D>(brlElem->strawTransform(istraw));
            auto code = brlElem->getCode(); 
            Identifier straw_id = m_cfg.idHelper->straw_id(code.isPosZ() == 1 ? 1 : -1, 
                                                    code.getPhiIndex(), 
                                                    code.getModuleIndex(),
                                                    code.getStrawLayerIndex(), 
                                                    istraw);

            auto elem = std::make_shared<const ActsDetectorElement>(
                trf, brlElem, straw_id, m_cfg.trackingGeometrySvc);

            m_cfg.elementStore->push_back(elem);

            auto straw = dynamic_cast<const Acts::StrawSurface*>(&elem->surface());
            if (not straw) continue;
            auto strawBounds = dynamic_cast<const Acts::LineBounds*>(&straw->bounds());
            if (not strawBounds) continue;
            double radius = strawBounds->r();
            double length = strawBounds->halflengthZ();
            fudge = radius / 4.;

            // calculate min/max R and Z
            Vector3D ctr = straw->center();
            pl.maxR = std::max(pl.maxR, ctr.perp() + radius);
            pl.minR = std::min(pl.minR, ctr.perp() - radius);
            pl.maxZ = std::max(pl.maxZ, ctr.z() + length);
            pl.minZ = std::min(pl.minZ, ctr.z() - length);

            layerSurfaces.push_back(straw->getSharedPtr());
          }
        }
      }
    }

    ACTS_VERBOSE("  - Collected " << layerSurfaces.size() << " straws");

    if(iring > 0) {
      // match outer radius of previous ring
      const Acts::ProtoLayer &prev = protoLayers.at(iring-1);
      pl.minR = prev.maxR + prev.envR.second + pl.envR.first + fudge;
    }

    std::shared_ptr<Acts::Layer> layer 
      = m_cfg.layerCreator->cylinderLayer(std::move(layerSurfaces), 100, 1, pl);
    layers.push_back(layer);

    protoLayers.push_back(pl);

  }

  return layers;
}

const Acts::LayerVector
ActsStrawLayerBuilder::endcapLayers(int side)
{
  ACTS_VERBOSE("Building endcap Straw layers")

  const InDetDD::TRT_Numerology* trtNums = m_cfg.mng->getNumerology();
  size_t    nEndcapWheels  = trtNums->getNEndcapWheels();
  size_t    nEndcapPhiSectors = trtNums->getNEndcapPhi();

  ACTS_VERBOSE("- Numerology reports: - " << nEndcapWheels<< " endcap wheels");
  ACTS_VERBOSE("                      - " << nEndcapPhiSectors << " endcap phi sectors");

  Acts::LayerVector layers;

  for(size_t iwheel=0;iwheel<nEndcapWheels;++iwheel) {
    ACTS_VERBOSE("- Collecting elements for wheel " << iwheel);


    size_t nEndcapLayers = trtNums->getNEndcapLayers(iwheel);
    ACTS_VERBOSE("  - Numerology reports: " << nEndcapLayers << " layers in wheel " << iwheel);
    for(size_t ilayer=0;ilayer<nEndcapLayers;++ilayer) {
      std::vector<std::shared_ptr<const Acts::Surface>> wheelSurfaces;

            
      Acts::ProtoLayer pl;
      pl.minR = std::numeric_limits<double>::max();
      pl.maxR = std::numeric_limits<double>::lowest();
      pl.minZ = std::numeric_limits<double>::max();
      pl.maxZ = std::numeric_limits<double>::lowest();
      pl.minPhi = -M_PI;
      pl.maxPhi = M_PI;
      pl.envR = {0, 0};

      for (unsigned int iphisec=0; iphisec<nEndcapPhiSectors; ++iphisec) {
      
        size_t iposneg = side < 0 ? 0 : 1;
        const InDetDD::TRT_EndcapElement* ecElem = m_cfg.mng->getEndcapElement(iposneg, iwheel, ilayer, iphisec);
        unsigned int nStraws = ecElem->nStraws();

        for(unsigned int istraw=0;istraw<nStraws;istraw++) {

          auto trf = std::make_shared<const Transform3D>(ecElem->strawTransform(istraw));

          auto code = ecElem->getCode(); 
          Identifier straw_id = m_cfg.idHelper->straw_id(code.isPosZ() == 1 ? 2 : -2, 
                                                  code.getPhiIndex(), 
                                                  code.getWheelIndex(),
                                                  code.getStrawLayerIndex(), 
                                                  istraw);


          auto elem = std::make_shared<const ActsDetectorElement>(
              trf, ecElem, straw_id, m_cfg.trackingGeometrySvc);

          m_cfg.elementStore->push_back(elem);

          auto straw = dynamic_cast<const Acts::StrawSurface*>(&elem->surface());
          if (straw){
            auto strawBounds = dynamic_cast<const Acts::LineBounds*>(&straw->bounds());
            if (strawBounds){
              double radius = strawBounds->r();
              double length = strawBounds->halflengthZ();

              Vector3D ctr = straw->center();
              pl.maxZ = std::max(pl.maxZ, ctr.z() + radius);
              pl.minZ = std::min(pl.minZ, ctr.z() - radius);
              pl.maxR = std::max(pl.maxR, ctr.perp() + length);
              pl.minR = std::min(pl.minR, ctr.perp() - length);
              pl.envZ = {radius/2., radius/2.};

              wheelSurfaces.push_back(straw->getSharedPtr());
            }
          }
        }
      }

      std::shared_ptr<Acts::Layer> layer 
        = m_cfg.layerCreator->discLayer(std::move(wheelSurfaces), 1, 100, pl);
      layers.push_back(layer);
      ACTS_VERBOSE("  - Collected " << wheelSurfaces.size() << " straws");
    }
    
  }
  
  ACTS_VERBOSE(" - Built " << layers.size() << " straw endcap layers");
  return layers;
}

