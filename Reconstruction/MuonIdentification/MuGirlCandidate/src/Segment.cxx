/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegment/MuonSegment.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

#include "MuGirlCandidate/Segment.h"
#include "MuGirlCandidate/Intersection.h"

namespace MuGirlNS
{

    Segment::Segment(SegmentType eType,
                     const Muon::MuonSegment* pMuonSegment,
                     TechnologyType eTech,
                     DistanceType eDist,
                     RegionType eReg,
                     Station* pStation) :
    m_eType(eType),
    m_pMuonSegment(pMuonSegment),
    m_eTech(eTech),
    m_eDist(eDist),
    m_eReg(eReg),
    m_pStation(pStation),
    m_nIndex(-1),
    m_dProb(Segment::fitProbability(pMuonSegment)),
    m_pIsect(NULL)
    {
    }

    Amg::Vector3D Segment::position() const
    {
        return m_pMuonSegment == NULL ? Amg::Vector3D() : m_pMuonSegment->globalPosition();
    }

    Amg::Vector3D Segment::direction() const
    {
        return m_pMuonSegment == NULL ? Amg::Vector3D() : m_pMuonSegment->globalDirection();
    }

    const Trk::FitQuality* Segment::fitQuality() const
    {
        return m_pMuonSegment == NULL ? NULL : m_pMuonSegment->fitQuality();
    }

    void Segment::setIntersection(Intersection* pIsect)
    {
        m_pIsect = pIsect;
        pIsect->setSegment(this);
    }

    double Segment::fitProbability(const Muon::MuonSegment* pMuonSegment)
    {
        double dProb = 0.0;
        const Trk::FitQuality* pFitQuality = pMuonSegment->fitQuality();
        if (pFitQuality != NULL)
        {
            if (pFitQuality->numberDoF() == 1 || pFitQuality->numberDoF() == 2)
                dProb = 1.0;
            else if (pFitQuality->numberDoF() > 2 && pFitQuality->chiSquared() > 0.0)
            {
                Genfun::CumulativeChiSquare ccs(pFitQuality->numberDoF());
                dProb = 1.0 - ccs(pFitQuality->chiSquared());
            }
        }
        return dProb;
    }

}
