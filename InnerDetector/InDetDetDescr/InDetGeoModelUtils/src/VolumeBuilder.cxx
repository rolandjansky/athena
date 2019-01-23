/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/ServiceVolume.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace InDetDD {
  VolumeBuilder::VolumeBuilder(const Zone& zone, const std::vector<const ServiceVolume* >& services)
    : m_msg("InDetDDVolumeBuilder"),
    m_region("None"), // Empty refers to a valid region. Set some default so we can check it is actually set.
    m_zcenter(0),
    m_services(0),
    m_servEnvelope(0),
    m_servChild(0),
    m_matManager(0) {
    m_splitter.splitAll(zone, services);
  }

  VolumeBuilder::VolumeBuilder(const std::vector<const ServiceVolume* >& services)
    : m_msg("InDetDDVolumeBuilder"),
    m_region("None"), // Empty refers to a valid region. Set some default so we can check it is actually set.
    m_zcenter(0),
    m_services(&services),
    m_servEnvelope(0),
    m_servChild(0),
    m_matManager(0)
  {}

  VolumeBuilder::VolumeBuilder(const Zone& zone, const std::vector<const ServiceVolume* >& services,
                               const std::vector<const ServiceVolume* >& servEnvelope,
                               const std::vector<const ServiceVolume* >& servChild)
    : m_msg("InDetDDVolumeBuilder"),
    m_region("None"), // Empty refers to a valid region. Set some default so we can check it is actually set.
    m_zcenter(0),
    m_services(0),
    m_servEnvelope(&servEnvelope),
    m_servChild(&servChild),
    m_matManager(0) {
    m_splitter.splitAll(zone, services);
  }

  void
  VolumeBuilder::addServices(const Zone& zone, const std::vector<const ServiceVolume* >& services) {
    m_splitter.splitAll(zone, services);
  }

  void
  VolumeBuilder::setRegion(const std::string& region, double zcenter) {
    m_region = region;
    m_zcenter = zcenter;
  }

  const std::vector<const ServiceVolume* >&
  VolumeBuilder::services() {
    // Return volumes defined in VolumeSplitter
    if (m_services) return *m_services;
    return m_splitter.getVolumes();
  }

  const std::vector<const ServiceVolume* >&
  VolumeBuilder::servicesEnv() {
    return *m_servEnvelope;
  }

  const std::vector<const ServiceVolume* >&
  VolumeBuilder::servicesChild() {
    return *m_servChild;
  }

  void
  VolumeBuilder::buildAndPlace(const std::string& region, GeoPhysVol* parent, double zcenter) {
    // Get volumes defined by Volume splitter and add them on top GeoPhysVol
    setRegion(region, zcenter);
    for (unsigned int iElement = 0; iElement < services().size(); ++iElement)
      if (!isEnvelopeOrChild(iElement)) {
        GeoVPhysVol* physVol = build(iElement);
        if (physVol) {
          physVol->ref();
          for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) {
            parent->add(getPlacement(iElement, iCopy));
            parent->add(physVol);
          }
          physVol->unref();//should delete if never added
        }
      }
  }

  void
  VolumeBuilder::buildAndPlace(const std::string& region, GeoFullPhysVol* parent, double zcenter) {
    // Get volumes defined by Volume splitter and add them on top GeoPhysVol
    setRegion(region, zcenter);
    for (unsigned int iElement = 0; iElement < services().size(); ++iElement) {
      if (!isEnvelopeOrChild(iElement)) {
        //    GeoVPhysVol* physVol = build(iElement);
        GeoVPhysVol* physVol = build(iElement);
        if (physVol) {
          physVol->ref();
          for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) {
            parent->add(getPlacement(iElement, iCopy));
            parent->add(physVol);
          }
          physVol->unref();//should delete if never used
        }
      }
    }
    // if region is not Pixel -> stop here
    if (region.compare("Pixel") != 0) return;

    for (unsigned int iElement = 0; iElement < services().size(); ++iElement) {
      if (getEnvelopeNum(iElement) > 0 && services()[iElement]->envelopeParent() == 0) {
        buildAndPlaceEnvelope(region, parent, -1, iElement, zcenter);
      }
    }
  }

  void
  VolumeBuilder::buildAndPlaceEnvelope(const std::string& region, GeoFullPhysVol* parent, int iParent, int iElement,
                                       double zcenter) {
    GeoPhysVol* physVol = dynamic_cast<GeoPhysVol*>(build(iElement));
    if (physVol) {
      physVol->ref();
      for (unsigned int iChild = 0; iChild < services().size(); ++iChild) {
        if (isChildService(iElement, iChild) && services()[iChild]->envelopeNum() > 0) {
          // if volume is a child volume : build and place it
          buildAndPlaceEnvelope(region, physVol, iElement, iChild, zcenter);
        }
      }
      for (unsigned int iChild = 0; iChild < services().size(); ++iChild) {
        if (isChildService(iElement, iChild) && services()[iChild]->envelopeNum() == 0) {
          // if volume is not a child volume
          GeoVPhysVol* physVol_child = build(iChild);
          if (physVol_child) {
            physVol_child->ref();
            for (int iCopy2 = 0; iCopy2 < numCopies(iChild); ++iCopy2) {
              physVol->add(getPlacementEnvelope(iChild, iCopy2, iElement));
              physVol->add(physVol_child);
            }
            physVol_child->unref();//should delete if was never added
          }
        }
      }
      for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) {
        // add all the copies
        if (iParent < 0) parent->add(getPlacement(iElement, iCopy));
        else parent->add(getPlacementEnvelope(iElement, iCopy, iParent));
        parent->add(physVol);
      }
      physVol->unref(); ///should delete even if it was never added
    }
  }

  void
  VolumeBuilder::buildAndPlaceEnvelope(const std::string& region, GeoPhysVol* parent, int iParent, int iElement,
                                       double zcenter) {
    GeoPhysVol* physVol = dynamic_cast<GeoPhysVol*>(build(iElement));
    if (physVol) {
      physVol->ref();
      for (unsigned int iChild = 0; iChild < services().size(); ++iChild) {
        if (isChildService(iElement, iChild) && services()[iChild]->envelopeNum() > 0) {
          // if volume is a child volume : build and place it
          buildAndPlaceEnvelope(region, physVol, iElement, iChild, zcenter);
        }
      }
      for (unsigned int iChild = 0; iChild < services().size(); ++iChild) {
        if (isChildService(iElement, iChild) && services()[iChild]->envelopeNum() == 0) {
          // if volume is not a child volume
          GeoVPhysVol* physVol_child = build(iChild);
          if (physVol_child) {
            physVol_child->ref();
            for (int iCopy2 = 0; iCopy2 < numCopies(iChild); ++iCopy2) {
              physVol->add(getPlacementEnvelope(iChild, iCopy2, iElement));
              physVol->add(physVol_child);
            }
            physVol_child->unref(); //will delete physVol_child if never used
          }
        }
      }
      for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) {
        // add all the copies
        if (iParent < 0) parent->add(getPlacement(iElement, iCopy));
        else parent->add(getPlacementEnvelope(iElement, iCopy, iParent));
        parent->add(physVol);
      }
      physVol->unref();//will delete physvol if it was never used
    }
  }

  GeoVPhysVol*
  VolumeBuilder::build(int iElement) {
    if (m_region == "None") {
      msg(MSG::ERROR) << "No region set. Cannot build services" << endmsg;
      return nullptr;
    }
    const ServiceVolume& param = *(services()[iElement]);
    // If the subelement does not belong to the current region return 0.
    if (param.region() != m_region) return 0;

    const GeoShape* serviceShape = param.getShape();
    double volume = param.origVolume();
    std::string logName = param.fullLabel();
    const GeoMaterial* serviceMat = param.material();
    std::string materialName;
    if (!serviceMat) {
      materialName = param.materialName();
      if (m_matManager) {
        //serviceMat = m_matManager->getMaterialForVolume(materialName,volume/param.fractionInRegion());
        // FIXME
        serviceMat = m_matManager->getMaterialForVolume(materialName, volume);
      } else {
        msg(MSG::ERROR) << "Material manager not available. Cannot build material." << endmsg;
        return nullptr;
      }
    } else {
      materialName = serviceMat->getName();
    }
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Volume/material: " << logName << "/" << materialName << endmsg;
      if (!param.shapeType().empty()) msg(MSG::DEBUG) << " shape: " << param.shapeType() << endmsg;
      msg(MSG::DEBUG) << " volume (cm3): " << volume / Gaudi::Units::cm3 << endmsg;
      msg(MSG::DEBUG) << " rmin,rmax,zmin,zmax: "
                      << param.rmin() << ", "
                      << param.rmax() << ", "
                      << param.zmin() << ", "
                      << param.zmax() << endmsg;
    }
    // Or use volume of original volume in param.
    //const GeoMaterial* serviceMat = mat_mgr->getMaterialForVolume(param.material(),param.origVolume());
    GeoLogVol* serviceLog = new GeoLogVol(logName, serviceShape, serviceMat);
    GeoPhysVol* servicePhys = new GeoPhysVol(serviceLog);
    return servicePhys;
  }

  bool
  VolumeBuilder::isEnvelopeOrChild(int iElement) {
    const ServiceVolume& param = *(services()[iElement]);
    if (param.envelopeNum() == 0 && param.envelopeParent() == 0) return false;
    return true;
  }

  int
  VolumeBuilder::getEnvelopeNum(int iElement) {
    const ServiceVolume& param = *(services()[iElement]);
    return param.envelopeNum();
  }

  int
  VolumeBuilder::getParentNum(int iElement) {
    const ServiceVolume& param = *(services()[iElement]);
    return param.envelopeParent();
  }

  double
  VolumeBuilder::getZcenter(int iElement) {
    const ServiceVolume& param = *(services()[iElement]);
    return (param.zmin() + param.zmax()) * 0.5;
  }

  bool
  VolumeBuilder::isChildService(int iElt, int iChld) {
    const ServiceVolume& param1 = *(services()[iElt]);
    const ServiceVolume& param2 = *(services()[iChld]);
    if (iElt == iChld || param1.envelopeNum() != param2.envelopeParent()) return false;
    if (param1.zsymm() == 1) {
      double zmin = (param1.zmin() * param2.zmin());
      double zmax = (param1.zmax() * param2.zmax());
      if (zmin > 0 && zmax > 0) return true;
      return false;
    }
    return true;
  }

  int
  VolumeBuilder::numCopies(int iElement) {
    return services()[iElement]->nCopies();
  }

  GeoTransform*
  VolumeBuilder::getPlacement(int iElement, int iCopy) {
    const ServiceVolume& param = *(services()[iElement]);
    // NB. Corrected for placement in endcaps
    double zpos = param.zposition() - m_zcenter;
    // Shift along Z axis ( IBL shift )
    double zshift = param.zShift();
    zpos += zshift;
    // Check if we need to rotate around Y axis.
    bool rotateAroundY = false;
    if (param.needsRotation()) { // zpos will always be negative in this case
      zpos = -zpos;
      rotateAroundY = true;
    }
    // Most are just translated in z
    GeoTrf::Transform3D xform = GeoTrf::TranslateZ3D(zpos);
    double phiStart = 0;
    // BOX, ROD and TRAP need special treatment.
    const std::string& shapeType = param.shapeType();
    if (shapeType == "TRAP" || shapeType == "TRAP2") {
      // Need to rotate by -90 deg.
      xform = GeoTrf::RotateZ3D(-90. * Gaudi::Units::deg) * xform;
    }
    if (shapeType == "TRAP2") {
      xform = GeoTrf::RotateZ3D(-90. * Gaudi::Units::deg) * xform;  // * GeoTrf::RotateX3D(-90.*Gaudi::Units::deg);
    }
    if (shapeType == "BOX" || shapeType == "TRAP" || shapeType == "TRAP2") {
      double radius = 0.5 * (param.rmin() + param.rmax());
      xform = GeoTrf::TranslateX3D(radius) * xform;
      phiStart = param.phiLoc();
    } else if (shapeType == "ROD" || shapeType == "ROD2") {
      double radius = param.rmin();
      xform = GeoTrf::TranslateX3D(radius) * xform;
      phiStart = param.phiLoc();
    }
    // For volumes that are placed more than once.
    double deltaPhi = 0;
    if (iCopy > 0) {
      deltaPhi = 2. * M_PI / param.nCopies();
    }
    double phi = phiStart + deltaPhi * iCopy;
    if (phi) {
      xform = GeoTrf::RotateZ3D(phi) * xform;
    }
    // For shapes that are not symmetric about a rotation around Y axis. We need to rotate.
    if (rotateAroundY) {
      xform = GeoTrf::RotateY3D(180. * Gaudi::Units::degree) * xform;
    }
    return new GeoTransform(xform);
  }

  GeoTransform*
  VolumeBuilder::getPlacementEnvelope(int iElement, int iCopy, int iEnvElement) {
    const ServiceVolume& param = *(services()[iElement]);
    const ServiceVolume& paramEnv = *(services()[iEnvElement]);
    double zCenter = (paramEnv.zmin() + paramEnv.zmax()) * 0.5;
    double rCenter = 0.;
    bool bMoveToCenter = false;
    if (paramEnv.shapeType() == "BOX") bMoveToCenter = true;
    if (paramEnv.shapeType() == "TUBE" && paramEnv.zsymm() == 1 && fabs(paramEnv.zmin()) > 0.01) bMoveToCenter = true;
    if (bMoveToCenter) rCenter = (paramEnv.rmin() + paramEnv.rmax()) * 0.5;
    // NB. Corrected for placement in endcaps
    double zpos = param.zposition() - zCenter;
    // Check if we need to rotate around Y axis.
    bool rotateAroundY = false;
    if (param.needsRotation()) { // zpos will always be negative in this case
      zpos = -zpos;
      rotateAroundY = true;
    }
    // Most are just translated in z
    GeoTrf::Transform3D xform = GeoTrf::TranslateZ3D(zpos);
    const std::string& shapeType = param.shapeType();
    double phiStart = 0;
    // BOX, ROD and TRAP need special treatment.
    if (shapeType == "TRAP") {
      // Need to rotate by -90 deg.
      xform = GeoTrf::RotateZ3D(-90. * Gaudi::Units::deg) * xform;
    }
    if (shapeType == "TRAP2") {
      // Need to rotate by -90 deg.
      xform = GeoTrf::RotateX3D(-90. * Gaudi::Units::deg) * xform;
    }
    if (shapeType == "BOX" || shapeType == "TRAP" || shapeType == "TRAP2") {
      double radius = 0.5 * (param.rmin() + param.rmax()) - rCenter;
      xform = GeoTrf::TranslateX3D(radius) * xform;
      phiStart = param.phiLoc();
    } else if (shapeType == "ROD" || shapeType == "ROD2") {
      double radius = param.rmin();
      xform = GeoTrf::TranslateX3D(radius) * xform;
      phiStart = param.phiLoc();
    }
    // For volumes that are placed more than once.
    double deltaPhi = 0;
    if (iCopy > 0) {
      deltaPhi = 2. * M_PI / param.nCopies();
    }
    double phi = phiStart + deltaPhi * iCopy;
    if (phi) {
      xform = GeoTrf::RotateZ3D(phi) * xform;
    }
    // For shapes that are not symmetric about a rotation around Y axis. We need to rotate.
    if (rotateAroundY) {
      xform = GeoTrf::RotateY3D(180. * Gaudi::Units::degree) * xform;
    }
    return new GeoTransform(xform);
  }
} // end namespace
