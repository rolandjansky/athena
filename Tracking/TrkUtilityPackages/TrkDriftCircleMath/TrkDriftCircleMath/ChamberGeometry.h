/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_CHAMBERGEOMETRY_H
#define DCMATH_CHAMBERGEOMETRY_H

#include <vector>

#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/MdtStationId.h"

namespace TrkDriftCircleMath {

    class ChamberGeometry {
    public:
        /// Retrieves a vector of all tubes in the geometry object
        virtual const std::vector<LocVec2D>& allTubes() const = 0;
        /// Retrieves the vector of drift circles traveresed by the line
        virtual DCVec tubesPassedByLine(const Line& line) const = 0;

        virtual bool validGeometry() const = 0;

        virtual void print() const = 0;

        virtual double stationTheta() const = 0;

        virtual unsigned int nlay() const = 0;

        virtual const MdtStationId& stationId() const = 0;

        virtual LocVec2D tubePosition(unsigned int ml, unsigned int lay, unsigned int tube) const = 0;

        virtual double tubeRadius() const = 0;
    };

}  // namespace TrkDriftCircleMath

#endif
