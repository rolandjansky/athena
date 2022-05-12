/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_CHAMBERGEOMETRY_H
#define DCMATH_CHAMBERGEOMETRY_H

#include <vector>

#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/Line.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace TrkDriftCircleMath {

    class ChamberGeometry {
    public:
        /// Retrieves the vector of drift circles traveresed by the line
        virtual DCVec tubesPassedByLine(const Line& line) const = 0;

        virtual bool validGeometry() const = 0;

        virtual void print(MsgStream& msg) const = 0;

        virtual double stationTheta() const = 0;

        virtual unsigned int nlay() const = 0;

        virtual const Identifier& stationId() const = 0;

        virtual LocVec2D tubePosition(unsigned int ml, unsigned int lay, unsigned int tube) const = 0;

        virtual double tubeRadius() const = 0;
    };

}  // namespace TrkDriftCircleMath

#endif
