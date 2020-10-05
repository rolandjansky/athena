/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ATHENA
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "TRT_ReadoutGeometry/TRT_Numerology.h"
#include "InDetIdentifier/TRT_ID.h"
#include "Identifier/Identifier.h"

// PACKAGE
#include "ActsGeometry/ActsStrawLayerBuilder.h"
#include "ActsGeometry/ActsDetectorElement.h"

// ACTS
#include "Acts/Geometry/ILayerBuilder.hpp"
#include "Acts/Surfaces/CylinderSurface.hpp"
#include "Acts/Surfaces/StrawSurface.hpp"
#include "Acts/Surfaces/DiscSurface.hpp"
#include "Acts/Geometry/LayerCreator.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Geometry/ProtoLayer.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Acts/Utilities/BinningType.hpp"

// STL
#include <iostream>
#include <fstream>

using namespace Acts::UnitLiterals;

const Acts::LayerVector
ActsStrawLayerBuilder::negativeLayers(const Acts::GeometryContext& gctx) const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<ActsStrawLayerBuilder*>(this);
  return mutableThis->endcapLayers(gctx, -1);
}

const Acts::LayerVector
ActsStrawLayerBuilder::centralLayers(const Acts::GeometryContext& gctx) const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<ActsStrawLayerBuilder*>(this);
  return mutableThis->centralLayers(gctx);
}

const Acts::LayerVector
ActsStrawLayerBuilder::positiveLayers(const Acts::GeometryContext& gctx) const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<ActsStrawLayerBuilder*>(this);
  return mutableThis->endcapLayers(gctx, 1);

}


const Acts::LayerVector
ActsStrawLayerBuilder::centralLayers(const Acts::GeometryContext& gctx)
{
  ACTS_VERBOSE("Building central Straw layers");

  using LBBV = Acts::LineBounds::BoundValues;

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
    auto& ext = pl.extent;
    ext.min(Acts::binR) = std::numeric_limits<double>::max();
    ext.max(Acts::binR) = std::numeric_limits<double>::lowest();
    ext.min(Acts::binZ) = std::numeric_limits<double>::max();
    ext.max(Acts::binZ) = std::numeric_limits<double>::lowest();
    ext.min(Acts::binPhi) = -M_PI;
    ext.max(Acts::binPhi) = M_PI;

    pl.envelope[Acts::binZ] = std::make_pair(1_mm, 1_mm);
    pl.envelope[Acts::binR] = std::make_pair(0_mm, 0_mm);

    double fudge = 0_mm;
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

            Acts::Transform3D trf = brlElem->strawTransform(istraw);
            // need to convert translation to length unit
            trf.translation() *= 1_mm;
            auto code = brlElem->getCode();
            Identifier straw_id = m_cfg.idHelper->straw_id(code.isPosZ() == 1 ? 1 : -1,
                                                    code.getPhiIndex(),
                                                    code.getModuleIndex(),
                                                    code.getStrawLayerIndex(),
                                                    istraw);

            auto elem = std::make_shared<const ActsDetectorElement>(
                trf, brlElem, straw_id);

            m_cfg.elementStore->push_back(elem);

            auto straw = dynamic_cast<const Acts::StrawSurface*>(&elem->surface());
            if (not straw) continue;
            auto strawBounds = dynamic_cast<const Acts::LineBounds*>(&straw->bounds());
            if (not strawBounds) continue;
            // units should be fine since they're already converted in det elem construction
            double radius = strawBounds->get(LBBV::eR);
            double length = strawBounds->get(LBBV::eHalfLengthZ);
            fudge = radius / 4.;

            // calculate min/max R and Z
            Acts::Vector3D ctr = straw->center(gctx);
            ext.max(Acts::binR) = std::max(ext.max(Acts::binR), ctr.perp() + radius);
            ext.min(Acts::binR) = std::min(ext.min(Acts::binR), ctr.perp() - radius);
            ext.max(Acts::binZ) = std::max(ext.max(Acts::binZ), ctr.z() + length);
            ext.min(Acts::binZ) = std::min(ext.min(Acts::binZ), ctr.z() - length);

            layerSurfaces.push_back(straw->getSharedPtr());
          }
        }
      }
    }

    ACTS_VERBOSE("  - Collected " << layerSurfaces.size() << " straws");

    if(iring > 0) {
      // match outer radius of previous ring
      const Acts::ProtoLayer &prev = protoLayers.at(iring-1);
      ext.min(Acts::binR) = prev.extent.max(Acts::binR) + prev.envelope[Acts::binR].second + pl.envelope[Acts::binR].first + fudge;
    }

    std::shared_ptr<Acts::Layer> layer
      = m_cfg.layerCreator->cylinderLayer(gctx, std::move(layerSurfaces), 100, 1, pl);
    layers.push_back(layer);

    protoLayers.push_back(pl);

  }

  return layers;
}

const Acts::LayerVector
ActsStrawLayerBuilder::endcapLayers(const Acts::GeometryContext& gctx, int side)
{
  ACTS_VERBOSE("Building endcap Straw layers");
  using LBBV = Acts::LineBounds::BoundValues;

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
      auto& ext = pl.extent;;
      ext.min(Acts::binR) = std::numeric_limits<double>::max();
      ext.max(Acts::binR) = std::numeric_limits<double>::lowest();
      ext.min(Acts::binZ) = std::numeric_limits<double>::max();
      ext.max(Acts::binZ) = std::numeric_limits<double>::lowest();
      ext.min(Acts::binPhi) = -M_PI;
      ext.max(Acts::binPhi) = M_PI;
      pl.envelope[Acts::binR] = std::make_pair(0_mm, 0_mm);

      for (unsigned int iphisec=0; iphisec<nEndcapPhiSectors; ++iphisec) {

        size_t iposneg = side < 0 ? 0 : 1;
        const InDetDD::TRT_EndcapElement* ecElem = m_cfg.mng->getEndcapElement(iposneg, iwheel, ilayer, iphisec);
        unsigned int nStraws = ecElem->nStraws();

        for(unsigned int istraw=0;istraw<nStraws;istraw++) {

          Acts::Transform3D trf = (ecElem->strawTransform(istraw));
          // need to convert translation to length unit
          trf.translation() *= 1_mm;

          auto code = ecElem->getCode();
          Identifier straw_id = m_cfg.idHelper->straw_id(code.isPosZ() == 1 ? 2 : -2,
                                                  code.getPhiIndex(),
                                                  code.getWheelIndex(),
                                                  code.getStrawLayerIndex(),
                                                  istraw);


          auto elem = std::make_shared<const ActsDetectorElement>(
              trf, ecElem, straw_id);

          m_cfg.elementStore->push_back(elem);

          auto straw = dynamic_cast<const Acts::StrawSurface*>(&elem->surface());
          if (straw){
            auto strawBounds = dynamic_cast<const Acts::LineBounds*>(&straw->bounds());
            if (strawBounds){
              double radius = strawBounds->get(LBBV::eR);
              double length = strawBounds->get(LBBV::eHalfLengthZ);

              Acts::Vector3D ctr = straw->center(gctx);
              ext.max(Acts::binZ) = std::max(ext.max(Acts::binZ), ctr.z() + radius);
              ext.min(Acts::binZ) = std::min(ext.min(Acts::binZ), ctr.z() - radius);
              ext.max(Acts::binR) = std::max(ext.max(Acts::binR), ctr.perp() + length);
              ext.min(Acts::binR) = std::min(ext.min(Acts::binR), ctr.perp() - length);
	      pl.envelope[Acts::binZ] = std::make_pair(radius/2., radius/2.);

              wheelSurfaces.push_back(straw->getSharedPtr());
            }
          }
        }
      }

      std::shared_ptr<Acts::Layer> layer
        = m_cfg.layerCreator->discLayer(gctx, std::move(wheelSurfaces), 1, 100, pl);
      layers.push_back(layer);
      ACTS_VERBOSE("  - Collected " << wheelSurfaces.size() << " straws");
    }

  }

  ACTS_VERBOSE(" - Built " << layers.size() << " straw endcap layers");
  return layers;
}
