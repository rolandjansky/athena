/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "InDetGeoModelUtils/ServiceVolumeMaker.h"
#include "InDetGeoModelUtils/ServiceVolume.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

#include "GeoModelUtilities/DecodeVersionKey.h"

#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace InDetDD {
  ServiceVolumeSchema::ServiceVolumeSchema() {
    setSimpleSchema();
  }

  void
  ServiceVolumeSchema::setPixelSchema() {
    m_rmin = "RIN";
    m_rmax = "ROUT";
    m_rmin2 = "RIN2";
    m_rmax2 = "ROUT2";
    m_zmin = "ZIN";
    m_zmax = "ZOUT";
    m_zsymm = "ZSYMM";
    m_materialName = "MATERIALNAME";
    m_repeat = "REPEAT";
    m_phiStart = "PHI";
    m_phiDelta = "WIDTH";
    m_width = "WIDTH";
    m_shapeType = "SHAPE";
    m_volName = "VOLNAME";
    m_radialDiv = "";
    m_phiStep = "";
    m_volId = "FRAMENUM";
    m_shiftFlag = "SHIFT";
  }

  void
  ServiceVolumeSchema::setDefaultSchema() {
    m_rmin = "RMIN";
    m_rmax = "RMAX";
    m_rmin2 = "RMIN2";
    m_rmax2 = "RMAX2";
    m_zmin = "ZMIN";
    m_zmax = "ZMAX";
    m_zsymm = "ZSYMM";
    m_materialName = "MATERIAL";
    m_repeat = "NREPEAT";
    m_phiStart = "PHISTART";
    m_phiDelta = "PHIDELTA";
    m_width = "";
    m_shapeType = "";
    m_volName = "NAME";
    m_radialDiv = "RADIAL";
    m_phiStep = "PHISTEP";
    m_volId = "";
    m_shiftFlag = "SHIFT";
  }

  void
  ServiceVolumeSchema::setSimpleSchema() {
    m_rmin = "RMIN";
    m_rmax = "RMAX";
    m_rmin2 = "";
    m_rmax2 = "";
    m_zmin = "ZMIN";
    m_zmax = "ZMAX";
    m_zsymm = "ZSYMM";
    m_materialName = "MATERIAL";
    m_repeat = "";
    m_phiStart = "";
    m_phiDelta = "";
    m_width = "";
    m_shapeType = "";
    m_volName = "NAME";
    m_radialDiv = "";
    m_phiStep = "";
    m_volId = "";
    m_shiftFlag = "SHIFT";
  }

  ServiceVolumeMakerMgr::ServiceVolumeMakerMgr(IRDBRecordset_ptr table, const ServiceVolumeSchema& schema,
                                               const InDetDD::AthenaComps* athenaComps)
    : m_table(table),
    m_schema(schema),
    m_athenaComps(athenaComps)
  {}

  const IGeometryDBSvc*
  ServiceVolumeMakerMgr::db() const {
    return m_athenaComps->geomDB();
  }

  double
  ServiceVolumeMakerMgr::rmin(int index) const {
    return db()->getDouble(m_table, m_schema.rmin(), index) * Gaudi::Units::mm;
  }

  double
  ServiceVolumeMakerMgr::rmax(int index) const {
    return db()->getDouble(m_table, m_schema.rmax(), index) * Gaudi::Units::mm;
  }

  double
  ServiceVolumeMakerMgr::rmin2(int index) const {
    return db()->getDouble(m_table, m_schema.rmin2(), index) * Gaudi::Units::mm;
  }

  double
  ServiceVolumeMakerMgr::rmax2(int index) const {
    return db()->getDouble(m_table, m_schema.rmax2(), index) * Gaudi::Units::mm;
  }

  double
  ServiceVolumeMakerMgr::zmin(int index) const {
    return db()->getDouble(m_table, m_schema.zmin(), index) * Gaudi::Units::mm;
  }

  double
  ServiceVolumeMakerMgr::zmax(int index) const {
    return db()->getDouble(m_table, m_schema.zmax(), index) * Gaudi::Units::mm;
  }

  double
  ServiceVolumeMakerMgr::phiDelta(int index) const {
    return db()->getDouble(m_table, m_schema.phiDelta(), index) * Gaudi::Units::deg;
  }

  double
  ServiceVolumeMakerMgr::width(int index) const {
    if (m_schema.has_width()) {
      return db()->getDouble(m_table, m_schema.width(), index) * Gaudi::Units::mm;
    }
    return 0;
  }

  double
  ServiceVolumeMakerMgr::phiStart(int index) const {
    return db()->getDouble(m_table, m_schema.phiStart(), index) * Gaudi::Units::deg;
  }

  double
  ServiceVolumeMakerMgr::phiStep(int index) const {
    if (m_schema.has_phiStep()) {
      return db()->getDouble(m_table, m_schema.phiStep(), index) * Gaudi::Units::deg;
    }
    return 0;
  }

  bool
  ServiceVolumeMakerMgr::zsymm(int index) const {
    return db()->getInt(m_table, m_schema.zsymm(), index);
  }

  int
  ServiceVolumeMakerMgr::repeat(int index) const {
    return db()->getInt(m_table, m_schema.repeat(), index);
  }

  int
  ServiceVolumeMakerMgr::radialDiv(int index) const {
    if (m_schema.has_radial()) {
      return db()->getInt(m_table, m_schema.radialDiv(), index);
    } else {
      return 0;
    }
  }

  std::string
  ServiceVolumeMakerMgr::shapeType(int index) const {
    if (m_schema.has_shapeType()) {
      if (db()->testField(m_table, m_schema.shapeType(), index)) {
        return db()->getString(m_table, m_schema.shapeType(), index);
      } else {
        return "TUBE";
      }
    }
    return "UNKNOWN";
  }

  std::string
  ServiceVolumeMakerMgr::volName(int index) const {
    if (db()->testField(m_table, m_schema.volName(), index)) {
      return db()->getString(m_table, m_schema.volName(), index);
    }
    return "";
  }

  std::string
  ServiceVolumeMakerMgr::materialName(int index) const {
    return db()->getString(m_table, m_schema.materialName(), index);
  }

  unsigned int
  ServiceVolumeMakerMgr::numElements() const {
    return db()->getTableSize(m_table);
  }

  int
  ServiceVolumeMakerMgr::volId(int index) const {
    if (m_schema.has_volId()) {
      return db()->getInt(m_table, m_schema.volId(), index);
    }
    return 0;
  }

  int
  ServiceVolumeMakerMgr::shiftFlag(int index) const {
    if (m_schema.has_shiftFlag()) {
      if (db()->testField(m_table, m_schema.shiftFlag(), index)) return db()->getInt(m_table,
                                                                                     m_schema.shiftFlag(), index);
    }
    return 0;
  }

  std::vector<double>
  ServiceVolumeMakerMgr::readLayerShift() const {
    std::vector<double> layerShift;

    IRDBAccessSvc* rdbSvc = m_athenaComps->rdbAccessSvc();
    IGeoDbTagSvc* geoDbTag = m_athenaComps->geoDbTagSvc();

    DecodeVersionKey versionKey(geoDbTag, "Pixel");
    std::string detectorKey = versionKey.tag();
    std::string detectorNode = versionKey.node();

    IRDBRecordset_ptr PixelBarrelGeneral = rdbSvc->getRecordsetPtr("PixelBarrelGeneral", detectorKey, detectorNode);
    IRDBRecordset_ptr PixelLayer = rdbSvc->getRecordsetPtr("PixelLayer", detectorKey, detectorNode);

    int numLayers = db()->getInt(PixelBarrelGeneral, "NLAYER");
    for (int iLayer = 0; iLayer < numLayers; iLayer++) {
      double shift = 0;
      if (db()->testField(PixelLayer, "GBLSHIFT", iLayer)) shift = db()->getDouble(PixelLayer, "GBLSHIFT", iLayer);
      layerShift.push_back(shift);
    }

    return layerShift;
  }

  ServiceVolumeMaker::ServiceVolumeMaker(const std::string& label,
                                         IRDBRecordset_ptr table, const ServiceVolumeSchema& schema,
                                         const InDetDD::AthenaComps* athenaComps)
    : m_label(label) {
    m_mgr = new ServiceVolumeMakerMgr(table, schema, athenaComps);
    m_layerShift = m_mgr->readLayerShift();
    //  std::cout<<"LAYER SHIFT "<<m_layerShift[0]<<" "<<m_layerShift[1]<<" "<<m_layerShift[2]<<"
    // "<<m_layerShift[3]<<std::endl;
  }

  ServiceVolumeMaker::~ServiceVolumeMaker() {
    for (unsigned int i = 0; i < m_services.size(); i++) {
      delete m_services[i];
    }
    delete m_mgr;
  }

  const std::vector<const ServiceVolume*>&
  ServiceVolumeMaker::makeAll() {
    for (unsigned int ii = 0; ii < numElements(); ++ii) {
      m_services.push_back(make(ii));
    }
    return m_services;
  }

  unsigned int
  ServiceVolumeMaker::numElements() const {
    return m_mgr->numElements();
  }

  ServiceVolume*
  ServiceVolumeMaker::make(int ii) {
    //
    // Retrieve/calculate the parameters for the volume.
    //
    ServiceVolume* param = new ServiceVolume;

    param->setMaterial(m_mgr->materialName(ii));
    param->setRmin(m_mgr->rmin(ii));
    param->setRmax(m_mgr->rmax(ii));
    param->setZmin(m_mgr->zmin(ii));
    param->setZmax(m_mgr->zmax(ii));
    param->setZsymm(m_mgr->zsymm(ii));
    param->setVolName(m_mgr->volName(ii));

    double zShift = 0.;           // the famous IBL Z shift
    if (m_mgr->shiftFlag(ii) > 0) zShift = m_layerShift[m_mgr->shiftFlag(ii) - 100];
    param->setZShift(zShift);

    int volId = m_mgr->volId(ii);
    if (volId == 0) volId = ii + 1;

    bool needsRotation = false;

    // For TUBE there is no need to read the rest
    std::string shapeType = m_mgr->shapeType(ii);
    if (!m_mgr->schema().simple() && !shapeType.empty() && shapeType != "TUBE") {
      double rmin2 = m_mgr->rmin2(ii);
      double rmax2 = m_mgr->rmax2(ii);

      if (rmin2 <= 0) rmin2 = param->rmin();
      if (rmax2 <= 0) rmax2 = param->rmax();

      int radialDiv = m_mgr->radialDiv(ii);

      double phiDelta = m_mgr->phiDelta(ii);

      bool fullPhiSector = false;
      if (phiDelta == 0 || phiDelta >= 359.9 * Gaudi::Units::degree) {
        phiDelta = 360 * Gaudi::Units::degree;
        fullPhiSector = true;
      }
      //else {
      //phiDelta -= 2*phiepsilon;
      //phiStart += phiepsilon;
      // }

      if (shapeType == "UNKNOWN") {
        if (radialDiv > 0) {
          shapeType = "RADIAL";
        } else if (param->rmin() == rmin2 && param->rmax() == rmax2) {
          if (fullPhiSector) {
            shapeType = "TUBE";
          } else {
            shapeType = "TUBS";
          }
        } else {
          shapeType = "CONS";
        }
      }


      int repeat = m_mgr->repeat(ii);
      if (repeat == 0) repeat = 1;

      double phiStart = m_mgr->phiStart(ii);
      double phiWidth = phiDelta;

      if (shapeType == "CONS" || shapeType == "TUBS") {
        const double phiepsilon = 0.001 * Gaudi::Units::degree;
        phiWidth -= 2 * phiepsilon;
        phiStart += phiepsilon;
      }

      // Can be in degree or mm. Usually it is deg expect for BOX, TRAP and ROD shape
      // Geometry manager makes no assumptions about units. So we must interpret here.
      if (shapeType == "BOX" || shapeType == "ROD" || shapeType == "ROD2" || shapeType == "TRAP") {
        phiWidth = m_mgr->width(ii); // in mm
      }

      if (shapeType == "PGON" || shapeType == "PGON2" ||
          shapeType == "CONE" || shapeType == "CONS" ||
          shapeType == "PGON3" || shapeType == "PGON4") {
        if ((rmin2 != param->rmin()) || (rmax2 != param->rmax())) {
          needsRotation = true;
        }
      }

      int sides = 0;
      int nCopies = 1;
      if (shapeType == "PGON" || shapeType == "PGON2" ||
          shapeType == "PGON3" || shapeType == "PGON4") {
        sides = repeat;
      } else {
        nCopies = repeat;
      }

      // Force nCopies to 1 for TUBE and CONE
      if (shapeType.empty() || shapeType == "TUBE" || shapeType == "CONE") {
        nCopies = 1;
      }

      param->setShapeType(shapeType);
      param->setRmin2(rmin2);
      param->setRmax2(rmax2);
      param->setPhiLoc(phiStart);
      param->setPhiWidth(phiWidth);
      param->setSides(sides);
      param->setNCopies(nCopies);
      //param->setRadialDiv(radialDiv);
      //param->setPhiStep(phiStep);
    }

    param->setNeedsRotation(needsRotation);


    //
    // If zin is 0... (within 10^-5) this is a volume symmetric around
    // the origin
    //
    if (std::abs(param->zmin()) < 0.000001) {
      param->setZmin(-param->zmax());
      param->setZsymm(false);
    }

    param->setLabel(m_label, volId);

    return param;
  }
} // end namespace
