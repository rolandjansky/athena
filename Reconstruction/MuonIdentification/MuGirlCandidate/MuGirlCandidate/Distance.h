/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_DISTANCE_H
#define MUGIRL_DISTANCE_H

#include "MuGirlCandidate/MuGirlNS.h"

namespace MuGirlNS
{

    class Distance
    {
    public:
        Distance(DistanceType eType,
                 double crudeR,
                 double crudeZ,
                 double minR,
                 double maxR,
                 double minZ,
                 double maxZ);

        DistanceType type() const;

        double       crudeR;
        double       crudeZ;
        double       minR;
        double       maxR;
        double       minZ;
        double       maxZ;

    private:
        DistanceType m_eType;
    };

    inline Distance::Distance(DistanceType eType,
                              double crudeR,
                              double crudeZ,
                              double minR,
                              double maxR,
                              double minZ,
                              double maxZ) :
    crudeR(crudeR),
    crudeZ(crudeZ),
    minR(minR),
    maxR(maxR),
    minZ(minZ),
    maxZ(maxZ),
    m_eType(eType)
    {
    }

    inline DistanceType Distance::type() const
    {
        return m_eType;
    }

}

#endif //MUGIRL_DISTANCE_H
