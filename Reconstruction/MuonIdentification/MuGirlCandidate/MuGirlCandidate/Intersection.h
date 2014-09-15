/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_INTERSECTION_H
#define MUGIRL_INTERSECTION_H

#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "MuGirlCandidate/MuGirlNS.h"
#include <sstream>

namespace MuGirlNS
{

    class Chamber;
    class Segment;

    class Intersection
    {
    public:
        Intersection(IntersectionType eType,
                     const Trk::TrackSurfaceIntersection* pTrkIsect,
                     TechnologyType eOrigTech,
                     DistanceType m_eOrigDist,
                     TechnologyType eTargTech,
                     DistanceType m_eTargDist,
                     RegionType eReg,
                     Chamber* pChamber);
        ~Intersection();

        IntersectionType type() const;
        const Trk::TrackSurfaceIntersection* trkIntersection() const;
        TechnologyType originTechnologyType() const;
        DistanceType originDistanceType()const;
        TechnologyType targetTechnologyType() const;
        DistanceType targetDistanceType()const;
        RegionType regionType() const;
        Chamber* chamber() const;
        int index() const;
        void setIndex(int nIndex);
        const Amg::Vector3D& position() const;
        const Amg::Vector3D& direction() const;
        Segment* segment() const;
        void setSegment(Segment* pSeg);
        std::string toString() const;

    private:
        const Trk::TrackSurfaceIntersection* m_pTrkIsect;
        IntersectionType m_eType;
        TechnologyType   m_eOrigTech;
        DistanceType     m_eOrigDist;
        TechnologyType   m_eTargTech;
        DistanceType     m_eTargDist;
        RegionType       m_eReg;
        Chamber*         m_pChamber;
        int              m_nIndex;
        Segment*         m_pSegment;

        static std::set<const Trk::TrackSurfaceIntersection*> m_TrkIsects;
    };

    inline Intersection::Intersection(IntersectionType eType,
                                      const Trk::TrackSurfaceIntersection* pTrkIsect,
                                      TechnologyType eOrigTech,
                                      DistanceType eOrigDist,
                                      TechnologyType eTargTech,
                                      DistanceType eTargDist,
                                      RegionType eReg,
                                      Chamber* pChamber) :
    m_pTrkIsect(pTrkIsect),
    m_eType(eType),
    m_eOrigTech(eOrigTech),
    m_eOrigDist(eOrigDist),
    m_eTargTech(eTargTech),
    m_eTargDist(eTargDist),
    m_eReg(eReg),
    m_pChamber(pChamber),
    m_nIndex(-1),
    m_pSegment(NULL)
    {
        m_TrkIsects.insert(pTrkIsect);
    }

    inline Intersection::~Intersection()
    {
        if (m_pTrkIsect != NULL)
        {
            std::set<const Trk::TrackSurfaceIntersection*>::iterator it = m_TrkIsects.find(m_pTrkIsect);
            if (it != m_TrkIsects.end())
            {
                delete m_pTrkIsect;
                m_TrkIsects.erase(it);
            }
        }
    }

    inline IntersectionType Intersection::type() const
    {
        return m_eType;
    }

    inline const Trk::TrackSurfaceIntersection* Intersection::trkIntersection() const
    {
        return m_pTrkIsect;
    }

    inline TechnologyType Intersection::originTechnologyType() const
    {
        return m_eOrigTech;
    }

    inline DistanceType Intersection::originDistanceType()const
    {
        return m_eOrigDist;
    }

    inline TechnologyType Intersection::targetTechnologyType() const
    {
        return m_eTargTech;
    }

    inline DistanceType Intersection::targetDistanceType()const
    {
        return m_eTargDist;
    }

    inline RegionType Intersection::regionType() const
    {
        return m_eReg;
    }

    inline Chamber* Intersection::chamber() const
    {
        return m_pChamber;
    }

    inline int Intersection::index() const
    {
        return m_nIndex;
    }

    inline void Intersection::setIndex(int nIndex)
    {
        m_nIndex = nIndex;
    }

    inline const Amg::Vector3D& Intersection::position() const
    {
        return m_pTrkIsect->position();
    }

    inline const Amg::Vector3D& Intersection::direction() const
    {
        return m_pTrkIsect->direction();
    }

    inline Segment* Intersection::segment() const
    {
        return m_pSegment;
    }

    inline void Intersection::setSegment(Segment* pSeg)
    {
        m_pSegment = pSeg;
    }

    inline std::string Intersection::toString() const
    {
        std::ostringstream oss;
        oss << IntersectionTypeName(m_eType)
        << " " << TechnologyTypeName(m_eOrigTech)
        << " " << DistanceTypeName(m_eOrigDist)
        << " " << TechnologyTypeName(m_eTargTech)
        << " " << DistanceTypeName(m_eTargDist)
        << " " << RegionTypeName(m_eReg)
        << " " << m_pTrkIsect;
        return(oss.str());
    }

}

#endif //MUGIRL_INTERSECTION_H
