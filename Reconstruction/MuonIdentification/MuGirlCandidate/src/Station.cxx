/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlCandidate/Station.h"
#include "MuGirlCandidate/Technology.h"
#include "MuGirlCandidate/Distance.h"
#include <sstream>

namespace MuGirlNS
{

    TechnologyType Station::technologyType() const
    {
        return m_pTech->type();
    }

    DistanceType Station::distanceType() const
    {
        return m_pDist->type();
    }

    std::string Station::toString() const
    {
        std::ostringstream oss;

        oss << m_name << " " << m_id << " "
        << TechnologyTypeName(m_pTech->type()) << " "
        << DistanceTypeName(m_pDist->type()) << " "
        << RegionTypeName(m_eReg);

        return oss.str();
    }

}
