/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "InDetGeoModelUtils/GenericTubeMaker.h"
#include "InDetGeoModelUtils/TubeVolData.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "RDBAccessSvc/IRDBRecord.h"

namespace InDetDD {
  GenericTubeMaker::GenericTubeMaker(const IRDBRecord* record)
    : m_record(record),
    m_volData(record)
  {}

  std::string
  GenericTubeMaker::materialName() const {
    return m_record->getString("MATERIAL");
  }

  std::string
  GenericTubeMaker::name() const {
    return m_record->getString("NAME");
  }

  const GeoShape*
  GenericTubeMaker::buildShape() {
    const GeoShape* shape = 0;

    switch (m_volData.shape()) {
    case TubeVolData::TUBE:
      shape = new GeoTube(m_volData.rmin(), m_volData.rmax(), 0.5 * m_volData.length());
      break;

    case TubeVolData::TUBS:
      shape = new GeoTubs(m_volData.rmin(), m_volData.rmax(), 0.5 * m_volData.length(),
                          m_volData.phiStart(), m_volData.phiDelta());
      break;

    case TubeVolData::CONS:
      shape = new GeoCons(m_volData.rmin(), m_volData.rmin2(), m_volData.rmax(), m_volData.rmax2(),
                          0.5 * m_volData.length(), m_volData.phiStart(), m_volData.phiDelta());
      break;

    case TubeVolData::RADIAL:
      // This simulates the radial decrease in density.
      double zstart = -0.5 * m_volData.length();
      GeoPcon* shapeTmp = new GeoPcon(m_volData.phiStart(), m_volData.phiDelta());
      shapeTmp->addPlane(zstart, m_volData.rmin(), m_volData.rmax());
      double radialDelta = (m_volData.rmax() - m_volData.rmin()) / m_volData.radialDivisions();
      for (int i = 0; i < m_volData.radialDivisions(); i++) {
        double rIntermediate = m_volData.rmax() - i * radialDelta;
        double reductionFactor = m_volData.rmin() / rIntermediate;
        shapeTmp->addPlane(zstart + reductionFactor * m_volData.length(), m_volData.rmin(), rIntermediate);
      }
      shapeTmp->addPlane(zstart + m_volData.length(), m_volData.rmin(), m_volData.rmin());
      shape = shapeTmp;
      break;
    }

    return shape;
  }

  void
  GenericTubeMaker::placeVolume(GeoPhysVol* parent, GeoVPhysVol* child, double zParent) {
    placeVolume(parent, 0, child, zParent);
  }

  void
  GenericTubeMaker::placeVolume(GeoFullPhysVol* fullparent, GeoVPhysVol* child, double zParent) {
    placeVolume(0, fullparent, child, zParent);
  }

  void
  GenericTubeMaker::placeVolTwoSide(GeoPhysVol* parentPos, GeoPhysVol* parentNeg, GeoVPhysVol* child, double zParent) {
    placeVolTwoSide(parentPos, parentNeg, 0, 0, child, zParent);
  }

  void
  GenericTubeMaker::placeVolTwoSide(GeoFullPhysVol* fullparentPos, GeoFullPhysVol* fullparentNeg, GeoVPhysVol* child,
                                    double zParent) {
    placeVolTwoSide(0, 0, fullparentPos, fullparentNeg, child, zParent);
  }

  void
  GenericTubeMaker::placeVolume(GeoPhysVol* parent, GeoFullPhysVol* fullparent, GeoVPhysVol* child, double zParent) {
    for (int iRepeat = 0; iRepeat < m_volData.nRepeat(); iRepeat++) {
      double phi = m_volData.phiStep() * iRepeat;

      GeoTransform* xform = 0;
      double zOffset = m_volData.zMid() - zParent;
      if (zOffset != 0 || iRepeat > 0) {
        xform = new GeoTransform(GeoTrf::TranslateZ3D(zOffset) * GeoTrf::RotateZ3D(phi));
      }

      if (parent) {
        if (xform) parent->add(xform);
        parent->add(child);
      } else {
        if (xform) fullparent->add(xform);
        fullparent->add(child);
      }

      // Place in negative z as well.
      if (m_volData.bothZ()) {
        GeoTransform* xformNeg = new GeoTransform(GeoTrf::RotateY3D(180 * Gaudi::Units::deg) * GeoTrf::TranslateZ3D(
                                                    zOffset) * GeoTrf::RotateZ3D(phi));
        if (parent) {
          parent->add(xformNeg);
          parent->add(child);
        } else {
          fullparent->add(xformNeg);
          fullparent->add(child);
        }
      }
    } // iRepeat loop
  }

  void
  GenericTubeMaker::placeVolTwoSide(GeoPhysVol* parentPos, GeoPhysVol* parentNeg,
                                    GeoFullPhysVol* fullparentPos, GeoFullPhysVol* fullparentNeg,
                                    GeoVPhysVol* child, double zParent) {
    for (int iRepeat = 0; iRepeat < m_volData.nRepeat(); iRepeat++) {
      double phi = m_volData.phiStep() * iRepeat;
      double zOffset = m_volData.zMid() - zParent;
      const bool newXform((zOffset != 0)or(iRepeat > 0));

      if (parentPos) {
        if (newXform) {
          parentPos->add(new GeoTransform(GeoTrf::TranslateZ3D(zOffset) * GeoTrf::RotateZ3D(phi)));
        }
        parentPos->add(child);
      } else if (fullparentPos) {
        if (newXform) {
          fullparentPos->add(new GeoTransform(GeoTrf::TranslateZ3D(zOffset) * GeoTrf::RotateZ3D(phi)));
        }
        fullparentPos->add(child);
      }

      // Place in negative z as well.
      if (m_volData.bothZ()) {
        GeoTransform* xformNeg = new GeoTransform(GeoTrf::RotateY3D(180 * Gaudi::Units::deg) * GeoTrf::TranslateZ3D(
                                                    zOffset) * GeoTrf::RotateZ3D(phi));
        if (parentNeg) {
          parentNeg->add(xformNeg);
          parentNeg->add(child);
        } else {
          fullparentNeg->add(xformNeg);
          fullparentNeg->add(child);
        }
      }
    } // iRepeat loop
  }
}// end namespace
