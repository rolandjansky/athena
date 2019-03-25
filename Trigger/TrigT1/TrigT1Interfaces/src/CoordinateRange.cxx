/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         CoordinateRange.cxx  -  description
                            -------------------
   begin                : 26/02/2002
   email                : e.moyse@qmul.ac.uk
***************************************************************************/

#include "TrigT1Interfaces/CoordinateRange.h"

namespace LVL1 {

CoordinateRange::CoordinateRange(double phiMin, double phiMax, double etaMin,
                                 double etaMax)
    : m_phiRange(phiMin, phiMax), m_etaRange(etaMin, etaMax) {
  checkBounds();
  setCentre();
}

CoordinateRange::CoordinateRange()
    : m_phiRange(0.0, 0.0), m_etaRange(0.0, 0.0) {}

/** change coords of an existing CoordinateRange object*/
void CoordinateRange::setRanges(double phiMin, double phiMax, double etaMin,
                                double etaMax) {
  m_phiRange.setRange(phiMin, phiMax);
  m_etaRange.setRange(etaMin, etaMax);
  checkBounds();
  setCentre();
  return;
}

PhiRange CoordinateRange::phiRange() const { return m_phiRange; }

Range CoordinateRange::etaRange() const { return m_etaRange; }

/** returns true if the coordinate falls inside the coordinate range */
bool CoordinateRange::contains(Coordinate& coord) const {
  return ((phiRange().contains(coord.phi())) &&
          (etaRange().contains(coord.eta())));
}

/** calculates the centre of the ranges and sets the phi,eta coords to that */
void LVL1::CoordinateRange::setCentre() {
  double phi = m_phiRange.centre();
  double eta = m_etaRange.min() + ((m_etaRange.max() - m_etaRange.min()) / 2);

  Coordinate::setCoords(phi, eta);
  return;
}

}  // namespace LVL1
