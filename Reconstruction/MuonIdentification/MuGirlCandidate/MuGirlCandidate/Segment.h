/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_SEGMENT_H
#define MUGIRL_SEGMENT_H

//#include "TrkEventPrimitives/GlobalPosition.h"

#include "MuGirlCandidate/MuGirlNS.h"

namespace Muon
{
    class MuonSegment;
}

namespace Trk
{
    class FitQuality;
}

namespace MuGirlNS
{

    class Station;

    class Segment
    {
    public:
		Segment(SegmentType eType,
                const Muon::MuonSegment* pMuonSegment,
                TechnologyType eTech,
                DistanceType eDist,
                RegionType eReg,
                Station* pStation);

        SegmentType type() const;
        const Muon::MuonSegment* muonSegment() const;
        TechnologyType technologyType() const;
        DistanceType distanceType() const;
        RegionType regionType() const;
        Station* station() const;
        int index() const;
        void setIndex(int nIndex);
        Amg::Vector3D position() const;
        Amg::Vector3D direction() const;
        const Trk::FitQuality* fitQuality() const;
        const HitList& hits() const;
        void addHit(Hit* pHit);
        double fitProbability() const;
        Intersection* intersection() const;
        void setIntersection(Intersection* pIsect);
        static double fitProbability(const Muon::MuonSegment* pMuonSegment);

    private:
        SegmentType    m_eType;
        const Muon::MuonSegment* m_pMuonSegment;
        TechnologyType m_eTech;
        DistanceType   m_eDist;
        RegionType     m_eReg;
        Station*       m_pStation;
        int            m_nIndex;
        HitList        m_hits;
        double         m_dProb;
        Intersection*  m_pIsect;
    };

    inline SegmentType Segment::type() const
    {
        return m_eType;
    }

    inline const Muon::MuonSegment* Segment::muonSegment() const
    {
        return m_pMuonSegment;
    }

    inline TechnologyType Segment::technologyType() const
    {
        return m_eTech;
    }

    inline DistanceType Segment::distanceType() const
    {
        return m_eDist;
    }

    inline RegionType Segment::regionType() const
    {
        return m_eReg;
    }

    inline Station* Segment::station() const
    {
        return m_pStation;
    }

    inline int Segment::index() const
    {
        return m_nIndex;
    }

    inline void Segment::setIndex(int nIndex)
    {
        m_nIndex = nIndex;
    }

    inline const HitList& Segment::hits() const
    {
        return m_hits;
    }

    inline void Segment::addHit(Hit* pHit)
    {
        m_hits.push_back(pHit);
    }

    inline double Segment::fitProbability() const
    {
        return m_dProb;
    }

    inline Intersection* Segment::intersection() const
    {
        return m_pIsect;
    }

}

#endif //MUGIRL_SEGMENT_H
