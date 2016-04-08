/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_CHAMBERGEOMETRY_H
#define DCMATH_CHAMBERGEOMETRY_H

#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/MdtStationId.h"
#include <vector>

namespace TrkDriftCircleMath {

  class ChamberGeometry {
  public:
    
    virtual const std::vector<LocPos>& allTubes() const = 0;
    
    virtual const DCVec& tubesPassedByLine( const Line& line ) const = 0;

    virtual bool validGeometry() const = 0;

    virtual void print() const = 0;

    virtual double stationTheta() const = 0;

    virtual unsigned int nlay() const = 0;
    
    virtual const MdtStationId& stationId() const = 0; 

    virtual const LocPos& tubePosition(unsigned int ml, unsigned int lay, unsigned int tube) const = 0;
 
    virtual double tubeRadius() const = 0;
 };

}


#endif
