// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         CoordinateRange.h  -  description
                            -------------------
   begin                : 28/02/2002
   email                : e.moyse@qmul.ac.uk
***************************************************************************/


#ifndef TRIGT1INTERFACES_COORDINATERANGE_H
#define TRIGT1INTERFACES_COORDINATERANGE_H

// Local include(s):
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/Range.h"
#include "TrigT1Interfaces/PhiRange.h"

namespace LVL1 {

/**
 * @short CoordinateRange class declaration
 *
 * Class defining a "window" in eta--phi space.
 *
 * $Revision: 734527 $
 * $Date: 2016-04-07 13:43:47 +0200 (Thu, 07 Apr 2016) $
 */
class CoordinateRange : public Coordinate {
 public:
  CoordinateRange(double phiMin, double phiMax, double etaMin, double etaMax);
  CoordinateRange();
  virtual ~CoordinateRange() = default;

  void setRanges(double phiMin, double phiMax, double etaMin, double etaMax);
  PhiRange phiRange() const;
  Range etaRange() const;
  /** returns true if the coordinate falls inside the coordinate range */
  bool contains(Coordinate& coord) const;

 private:
  /** calculates the centre of the ranges and sets the phi,eta coords to that */
  void setCentre();

  PhiRange m_phiRange;
  Range m_etaRange;

};  // class CoordinateRange

}  // namespace LVL1

#endif  // TRIGT1INTERFACES_COORDINATERANGE_H
