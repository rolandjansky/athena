/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LayerSort.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/CylinderBounds.h"
#include <cmath>

namespace GXF{
    using Trk::Layer;
    using Trk::CylinderSurface;
    using Trk::DiscSurface;
    using Trk::CylinderBounds;
    using Trk::DiscBounds;
    
    bool 
    LayerSort::operator() (const LayerPtrPair & one,const LayerPtrPair & two) const {
      auto castToCylinder = [](const LayerPtrPair & pair)->const Trk::CylinderSurface* {
        if (pair.first) return (const CylinderSurface *) &pair.first->surfaceRepresentation();
        else return nullptr;
      };
      auto castToDisc = [](const LayerPtrPair & pair)->const Trk::DiscSurface* {
        if (pair.second) return (const DiscSurface *) &pair.second->surfaceRepresentation();
        else return nullptr;
      };
      const CylinderSurface *cyl1 = castToCylinder(one);
      const DiscSurface *disc1  = castToDisc(one);
      const CylinderSurface *cyl2 = castToCylinder(two);
      const DiscSurface *disc2 = castToDisc(two);

      if (cyl1 and cyl2) {
        if (std::abs(cyl1->center().z() - cyl2->center().z()) > 1.) {
          return (std::abs(cyl1->center().z()) < std::abs(cyl2->center().z()));
        }
        return (cyl1->bounds().r() < cyl2->bounds().r());
      }
      if (disc1 and disc2) {
        const DiscBounds *discbounds1 = (const DiscBounds *) &disc1->bounds();
        const DiscBounds *discbounds2 = (const DiscBounds *) &disc2->bounds();
        if (discbounds1->rMax() < discbounds2->rMin() + 1) {
          return true;
        }
        if (discbounds1->rMin() > discbounds2->rMax() - 1) {
          return false;
        }
        return (std::abs(disc1->center().z()) < std::abs(disc2->center().z()));
      }
      
      if ((cyl1 != nullptr) && (disc2 != nullptr)) {
        const DiscBounds *discbounds = (const DiscBounds *) &disc2->bounds();
        if (cyl1->bounds().r() > discbounds->rMax() - 1) {
          return false;
        }
        if (cyl1->bounds().r() < discbounds->rMin() + 1) {
          return true;
        }
        return (std::abs(cyl1->center().z()) < std::abs(disc2->center().z()));
      }
      
      if ((disc1 == nullptr) || (cyl2 == nullptr)) {
        throw std::logic_error("Unhandled surface combination.");
      }
      
      const DiscBounds *discbounds = (const DiscBounds *) &disc1->bounds();
      
      if (cyl2->bounds().r() > discbounds->rMax() - 1) {
        return true;
      }
      
      if (cyl2->bounds().r() < discbounds->rMin() + 1) {
        return false;
      }

      return (std::abs(cyl2->center().z()) > std::abs(disc1->center().z()));
    }
    
    
    bool 
    LayerSort2::operator() (const Layer * one, const Layer * two) const {
      auto castToCylinder = [](const Layer * pLayer) -> const CylinderSurface * {
        if (pLayer->surfaceRepresentation().type() == Trk::SurfaceType::Cylinder)
        return static_cast<const CylinderSurface *>(&pLayer->surfaceRepresentation());
        return nullptr;
      };
      auto castToDisc = [](const Layer * pLayer) -> const DiscSurface *{
        if (pLayer->surfaceRepresentation().type() == Trk::SurfaceType::Disc)
        return static_cast<const DiscSurface *>(&pLayer->surfaceRepresentation());
        return nullptr;
      };
      //
      const CylinderSurface *cyl1 = castToCylinder(one);
      const CylinderSurface *cyl2 = castToCylinder(two);
      if (cyl1 and cyl2) {
        return (cyl1->bounds().r() < cyl2->bounds().r());
      }
      const DiscSurface *disc1 = castToDisc(one);
      const DiscSurface *disc2 = castToDisc(two);
      if (disc1 and disc2) {
        return (std::abs(disc1->center().z()) < std::abs(disc2->center().z()));
      }
      
      return false;
    }

  }
