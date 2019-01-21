/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "InDetGeoModelUtils/TubeVolData.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>
#include <string>
#include <iostream>

namespace InDetDD {
  std::string
  TubeVolData::material() const {
    return m_record->getString("MATERIAL");
  }

  double
  TubeVolData::maxRadius() const {
    return std::max(m_rmax1, m_rmax2);
  }

  TubeVolData::TubeVolData(const IRDBRecord* record)
    : m_record(record),
    m_bothZ(false),
    m_nRepeat(0),
    m_radialDiv(0),
    m_phiStart(0.),
    m_phiDelta(0.),
    m_phiStep(0.),
    m_rmin1(0.),
    m_rmin2(0.),
    m_rmax1(0.),
    m_rmax2(0.),
    m_length(0.),
    m_zMid(0.) {
    // add an 2*epsilon gap between phi sectors.
    const double phiepsilon = 0.001 * Gaudi::Units::degree;

    bool fullPhiSector = false;


    // Get the parameters which we need to do some preprocessing with.
    // The rest are obtained directly from RDB.

    if (m_record) {
      m_phiStart = m_record->getDouble("PHISTART") * Gaudi::Units::degree;
      m_phiDelta = m_record->getDouble("PHIDELTA") * Gaudi::Units::degree;
      m_phiStep = m_record->getDouble("PHISTEP") * Gaudi::Units::degree;
      m_nRepeat = m_record->getInt("NREPEAT");
      m_rmin1 = m_record->getDouble("RMIN") * Gaudi::Units::mm;
      m_rmax1 = m_record->getDouble("RMAX") * Gaudi::Units::mm;
      m_rmin2 = m_record->getDouble("RMIN2") * Gaudi::Units::mm;
      m_rmax2 = m_record->getDouble("RMAX2") * Gaudi::Units::mm;
      m_radialDiv = 0;
      if (!m_record->isFieldNull("RADIAL")) {
        m_radialDiv = m_record->getInt("RADIAL");
      }
      m_bothZ = 0;
      if (!m_record->isFieldNull("ZSYMM")) {
        m_bothZ = m_record->getInt("ZSYMM");
      }

      double zmin = m_record->getDouble("ZMIN") * Gaudi::Units::mm;
      double zmax = m_record->getDouble("ZMAX") * Gaudi::Units::mm;
      m_length = std::abs(zmax - zmin);
      m_zMid = 0.5 * (zmin + zmax);

      if (m_phiDelta == 0 || m_phiDelta >= 359.9 * Gaudi::Units::degree) {
        m_phiDelta = 360 * Gaudi::Units::degree;
        fullPhiSector = true;
      } else {
        m_phiDelta -= 2 * phiepsilon;
        m_phiStart += phiepsilon;
      }

      // Force nRepeat to be >= 1;
      if (m_nRepeat <= 0) m_nRepeat = 1;
      // if PHISTEP==0 then set it to be equi-distant steps filling up phi.
      if (m_phiStep == 0) {
        m_phiStep = 360 * Gaudi::Units::degree / m_nRepeat;
      }

      if (m_rmin2 <= 0) m_rmin2 = m_rmin1;
      if (m_rmax2 <= 0) m_rmax2 = m_rmax1;

      if (m_radialDiv > 0) {
        m_shape = TubeVolData::RADIAL;
      } else if (m_rmin1 == m_rmin2 && m_rmax1 == m_rmax2) {
        if (fullPhiSector) {
          m_shape = TubeVolData::TUBE;
        } else {
          m_shape = TubeVolData::TUBS;
        }
      } else {
        m_shape = TubeVolData::CONS;
      }
    } else std::cout << "Unexpected ERROR in ExtraMaterial!" << std::endl;
  }
} // end namespace
