/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_CHAMBER_H
#define MUGIRL_CHAMBER_H

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "MuGirlCandidate/MuGirlNS.h"
#include "MuGirlCandidate/Hit.h"
#include "MuGirlCandidate/BBox.h"

namespace MuonGM
{
    class MuonReadoutElement;
}
namespace Trk
{
    class TrackRoad;
}

namespace MuGirlNS
{
    class Station;

    class Chamber
    {
    public:
        enum Origin { SAME_DISTANCE, OTHER_DISTANCE, ORIGINS };

        Chamber(Identifier id,
                IdentifierHash hashId,
                Station* pStation,
                const MuonGM::MuonReadoutElement* pReadoutElement);
        ~Chamber();

        const Identifier& id() const;
        const IdentifierHash& hashId() const;
        Station* station() const;
        const MuonGM::MuonReadoutElement* readoutElement() const;
        int index() const;
        void setIndex(int nIndex);
        const Amg::Vector3D& normal() const;
        const Amg::Vector3D& center() const;
        TechnologyType technologyType() const;
        DistanceType distanceType() const;
        RegionType regionType() const;
        bool inChamber(const Road& road) const;
        unsigned fillPrepData();
        const PrepDataList& prepData() const;
        Intersection* intersection(TechnologyType eTech, Origin eOrig) const;
        void setIntersection(TechnologyType eTech, Origin eOrig, Intersection* pIsect);
        std::string toString() const;
        void addRIO(const Trk::RIO_OnTrack* pRIO, Hit* pHit);
        const RIOList& RIOs() const;
        bool addHit(Hit* pHit);
        HitSet& hits();
        Hit* findHitByRIO(const Trk::RIO_OnTrack* pRIO) const;
        const BBox& bbox() const;
        Intersection* baseIntersection() const;
        void setBaseIntersection(Intersection* pIsect);
        Trk::TrackRoad* baseRoad() const;
        void setBaseRoad(Trk::TrackRoad* pRoad);

    private:
        typedef std::map<const Trk::PrepRawData*, Hit*> RIOHitMap;
        void createBbox();

        Identifier      m_id;
        IdentifierHash  m_hashId;
        Station*        m_pStation;         /**< The containing Station */
        const MuonGM::MuonReadoutElement* m_pReadoutElement;
        int             m_nIndex;
        PrepDataList    m_prepDataArray;    /**< Array of original hits */
        RIOList         m_RIOs;             /**< The list of RIO_OnTrack objects */
        Intersection*   m_Isects[ALL_TECHNOLOGIES][ORIGINS];
        Intersection*   m_pBaseIsect;       /**< The intersection used in selecting hits */
        Trk::TrackRoad* m_pBaseRoad;        /**< The road used in selecting hits */
        HitSet          m_hits;             /**< The set of unique hits */
        BBox            m_bbox;
        RIOHitMap       m_rioHitMap;
    };

    inline const Identifier& Chamber::id() const
    {
        return m_id;
    }

    inline const IdentifierHash& Chamber::hashId() const
    {
        return m_hashId;
    }

    inline Station* Chamber::station() const
    {
        return m_pStation;
    }

    inline const MuonGM::MuonReadoutElement* Chamber::readoutElement() const
    {
        return m_pReadoutElement;
    }

    inline int Chamber::index() const
    {
        return m_nIndex;
    }

    inline void Chamber::setIndex(int nIndex)
    {
        m_nIndex = nIndex;
    }

    inline const PrepDataList& Chamber::prepData() const
    {
        return m_prepDataArray;
    }

    inline Intersection* Chamber::intersection(TechnologyType eTech, Origin eOrig) const
    {
        return m_Isects[eTech][eOrig];
    }

    inline void Chamber::setIntersection(TechnologyType eTech, Origin eOrig, Intersection* pIsect)
    {
        m_Isects[eTech][eOrig] = pIsect;
    }

    inline const RIOList& Chamber::RIOs() const
    {
        return m_RIOs;
    }

    inline bool Chamber::addHit(Hit* pHit)
    {
        std::pair<HitSet::iterator, bool> p = m_hits.insert(pHit);
        return p.second;
    }

    inline HitSet& Chamber::hits()
    {
        return m_hits;
    }

    inline const BBox& Chamber::bbox() const
    {
        return m_bbox;
    }

    inline Intersection* Chamber::baseIntersection() const
    {
        return m_pBaseIsect;
    }

    inline void Chamber::setBaseIntersection(Intersection* pIsect)
    {
        m_pBaseIsect = pIsect;
    }

    inline Trk::TrackRoad* Chamber::baseRoad() const
    {
        return m_pBaseRoad;
    }

    inline void Chamber::setBaseRoad(Trk::TrackRoad* pRoad)
    {
        m_pBaseRoad = pRoad;
    }

}

#endif //MUGIRL_CHAMBER_H
