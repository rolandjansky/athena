// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTRoad.cxx
 * @author Riley Xu - rixu@cern.ch
 * @date Janurary 13th, 2020
 * @brief Defines a struct for roads.
 */


#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTUtils/HTTFunctions.h"

using namespace std;


ostream& operator<<(ostream& os, const HTTRoad & road)
{
    os << "road " << road.m_roadID << ": PID " << road.m_pid << ", sector " << road.m_sector
        << " hitLayers " << formatHex(road.m_hit_layers) << " wcLayers " << formatHex(road.m_wildcard_layers)
        << ", nHits " << road.getNHits();
    /*
    for (size_t l=0; l < road.m_hits.size(); l++)
    {
        for (HTTHit const * hit : road.m_hits[l])
            os << "\n\t" << *hit;
    }
    */

    return os;
}

