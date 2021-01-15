/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    throw std::domain_error("HTTHit::getLayer() called on a hit with invalid type");
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
unsigned HTTHit::getIdentifierHash() const
{
    if (m_detType == SiliconTech::strip)
        return (m_identifierHash << 2) + m_etaIndex;
    return m_identifierHash;
}


std::ostream & operator<<(std::ostream& out, const HTTHit& hit)
{
    out << "type=" << hit.m_hitType
        << " tech=" << hit.m_detType
        << " zone=" << hit.m_detectorZone
        << " physLayer=" << hit.getPhysLayer()
        << " eta=" << hit.getEtaCoord()
        << " phi=" << hit.getPhiCoord();
    return out;
}

std::vector<std::vector<const HTTHit*>> sortByLayer(std::vector<const HTTHit*> const & hits)
{
    std::vector<std::vector<const HTTHit*>> out;
    for (HTTHit const * hit : hits)
    {
        size_t layer = hit->getLayer();
        if (layer >= out.size()) out.resize(layer + 1);
        out[layer].push_back(hit);
    }
    return out;
}


std::ostream & operator<<(std::ostream & os, HitType t)
{
    switch (t)
    {
        case HitType::unmapped:  return (os << "unmapped");
        case HitType::mapped:    return (os << "mapped");
        case HitType::clustered: return (os << "clustered");
        case HitType::wildcard:  return (os << "wildcard");
        case HitType::guessed:   return (os << "guessed");
        default:                 return (os << "undefined");
    }
}
