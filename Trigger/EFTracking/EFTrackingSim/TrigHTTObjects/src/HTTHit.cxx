/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigHTTObjects/HTTHit.h"

#include <stdexcept>

ClassImp(HTTHit)



bool HTTHit::isMapped() const
{
    switch (m_hitType)
    {
    case HitType::unmapped:
    case HitType::undefined:
        return false;
    default:
        return true;
    }
}

bool HTTHit::isClustered() const
{
    switch (m_hitType)
    {
    case HitType::clustered: // TODO do wildcard and guessed hits have cluster info too?
    case HitType::spacepoint:
        return true;
    default:
        return false;
    }
}

bool HTTHit::isReal() const
{
    switch (m_hitType)
    {
    case HitType::unmapped:
    case HitType::mapped:
    case HitType::clustered:
    case HitType::spacepoint:
        return true;
    default:
        return false;
    }
}

// Sets using the physical layer index as defined by HTTPlaneMap
void HTTHit::setPhysLayer(unsigned v)
{
    if (m_detType == SiliconTech::strip)
    {
        m_layer_disk = v / 2;
        m_side = v % 2;
    }
    else
    {
        m_layer_disk = v;
    }
}

// Returns the physical layer index as defined by HTTPlaneMap
unsigned HTTHit::getPhysLayer() const
{
    if (m_detType == SiliconTech::strip)
        return 2 * m_layer_disk + m_side;
    return m_layer_disk;
}


unsigned HTTHit::getLayer() const
{
    if (isMapped()) return m_layer;
    throw std::domain_error("HTTHit::getLayer() called on a hit with invalid type: " + to_string(m_hitType));
}

unsigned HTTHit::getSection() const
{
    if (isMapped()) return m_section;
    throw std::domain_error("HTTHit::getSection() called on a hit with invalid type");
}


// TODO TODO TODO this is a temporary fix for release 21.9. Here, each strip chip
// (the entire physical 10cm x 10cm chip) is considered a single module, and has a
// single hashID. In rel22 a unique hashID will be assigned to each row of strips
// (4 or 2 per chip). This change fixes the 21.9 version to match the 22 version,
// by adding the row index (eta coord) to the hashID.
unsigned HTTHit::getHTTIdentifierHash() const
{
    if (m_detType == SiliconTech::strip)
        return (m_identifierHash << 2) + m_etaIndex;
    return m_identifierHash;
}

// TODO TODO TODO as above
int HTTHit::getHTTEtaModule() const
{
    if (m_detType == SiliconTech::strip)
    {
        if (m_detectorZone == DetectorZone::barrel)
        {
            // First two SCT ITK layers have 4 rows per module
            if (m_layer_disk < 2) return (4 * m_etaModule) + m_etaIndex;

            // Last two SCT ITK layers have 2 rows per module
            return (2 * m_etaModule) + m_etaIndex;
        }
        // endcap row: number of rows indexed by etaModule is [4,4,2,4,2,2]
        else
        {
            static const unsigned cumulative_rows[6] = { 0,4,8,10,14,16 };
            return cumulative_rows[m_etaModule] + m_etaIndex;
        }
    }

    return m_etaModule;
}


std::ostream& operator<<(std::ostream& out, const HTTHit& hit)
{
    out << "type=" << hit.m_hitType
        << " tech=" << hit.m_detType
        << " zone=" << hit.m_detectorZone
        << " etamod=" << hit.getHTTEtaModule()
        << " phimod=" << hit.getPhiModule()
        << " physLayer=" << hit.getPhysLayer()
        << " eta=" << hit.getEtaCoord()
        << " phi=" << hit.getPhiCoord();

    return out;
}


std::string to_string(HitType t)
{
    switch (t)
    {
    case HitType::unmapped:   return "unmapped";
    case HitType::mapped:     return "mapped";
    case HitType::clustered:  return "clustered";
    case HitType::wildcard:   return "wildcard";
    case HitType::guessed:    return "guessed";
    case HitType::extrapolated:  return "extrapolated";
    case HitType::spacepoint: return "spacepoint";
    default:                  return "undefined";
    }
}


std::ostream& operator<<(std::ostream& os, HitType t)
{
    return (os << to_string(t));
}


