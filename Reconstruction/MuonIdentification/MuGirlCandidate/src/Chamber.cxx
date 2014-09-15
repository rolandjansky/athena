/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/Surface.h"
#include "TrkRoad/TrackRoad.h"

#include "MuGirlCandidate/Chamber.h"
#include "MuGirlCandidate/Station.h"
//#include "MuGirlCandidate/Road.h"
#include "MuGirlCandidate/Technology.h"
#include "MuGirlCandidate/Utilities.h"
#include <sstream>

using namespace MuGirlNS;

Chamber::Chamber(Identifier id,
                 IdentifierHash hashId,
                 Station* pStation,
                 const MuonGM::MuonReadoutElement* pReadoutElement) :
        m_id(id),
        m_hashId(hashId),
        m_pStation(pStation),
        m_pReadoutElement(pReadoutElement),
        m_nIndex(-1),
        m_pBaseIsect(NULL),
        m_pBaseRoad(NULL)
{
    for (unsigned iTech = 0; iTech < ALL_TECHNOLOGIES; iTech++)
    {
        for (unsigned iOrig = 0; iOrig < ORIGINS; iOrig++)
        {
            m_Isects[iTech][iOrig] = NULL;
        }
    }
    if (m_pReadoutElement != NULL)
        createBbox();
}

Chamber::~Chamber()
{

    for (RIOList::iterator itRIO = m_RIOs.begin(); itRIO != m_RIOs.end(); itRIO++)
    {
        delete *itRIO;
    }
/*
    for (HitSet::iterator itHit = m_hits.begin(); itHit != m_hits.end(); itHit++)
    {
        Hit* pHit = *itHit;
        if (!pHit->isInSegment())
            delete pHit->RIO();
    }
*/
    delete m_pBaseRoad;
}

TechnologyType Chamber::technologyType() const
{
    return m_pStation->technologyType();
}

DistanceType Chamber::distanceType() const
{
    return m_pStation->distanceType();
}

RegionType Chamber::regionType() const
{
    return m_pStation->regionType();
}

const Amg::Vector3D& Chamber::normal() const
{
    return m_pReadoutElement->normal();
}

const Amg::Vector3D& Chamber::center() const
{
    return m_pReadoutElement->center();
}

bool Chamber::inChamber(const Road& road) const
{
    return m_bbox.on(road);
}

unsigned Chamber::fillPrepData()
{
    m_pStation->technology()->prepData(this, m_prepDataArray);
    if (m_pReadoutElement == NULL && !m_prepDataArray.empty())
    {
        Identifier id = m_prepDataArray.front()->identify();
        m_pReadoutElement = m_pStation->technology()->readoutElement(id);
        if (m_pReadoutElement != NULL)
            createBbox();
    }
    return m_prepDataArray.size();
}

std::string Chamber::toString() const
{
    std::ostringstream oss;
    oss << std::hex << m_id << std::dec << " (" << m_pStation->toString() << ")";
    return oss.str();
}

void Chamber::createBbox()
{
    DetElement2Bbox(m_pReadoutElement, m_bbox);
}

void Chamber::addRIO(const Trk::RIO_OnTrack* pRIO, Hit* pHit)
{
    m_RIOs.push_back(pRIO);
    m_rioHitMap.insert(RIOHitMap::value_type(pRIO->prepRawData(), pHit));
}

Hit* Chamber::findHitByRIO(const Trk::RIO_OnTrack* pRIO) const
{
    RIOHitMap::const_iterator itHit = m_rioHitMap.find(pRIO->prepRawData());
    return itHit == m_rioHitMap.end() ? NULL : itHit->second;
}
