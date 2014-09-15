/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_HIT_H
#define MUGIRL_HIT_H

#include "MuGirlCandidate/MuGirlNS.h"
#include "MuGirlCandidate/Utilities.h"
#include <sstream>

namespace MuGirlNS
{

    class Chamber;

    class Hit
    {
    public:
        Hit(const Amg::Vector3D& pos,
            double dEta,
            double dPhi,
            bool isEta,
            Chamber* pChamber);

        const Amg::Vector3D& position() const;
        double deltaEta() const;
        double deltaPhi() const;
        bool isEta() const;
        const Trk::RIO_OnTrack* RIO() const;
        void setRIO(const Trk::RIO_OnTrack* pRIO);
        Chamber* chamber() const;
        bool isInSegment() const;
        void setInSegment(bool bInSegment = true);
        std::string toString() const;
        int index() const;
        void setIndex(int nIndex);

    private:
        Amg::Vector3D m_position;      /**< The hit position */
        double        m_dDeltaEta;     /**< The eta distance to the chamber fine intersection */
        double        m_dDeltaPhi;     /**< The phi distance to the chamber fine intersection */
        bool          m_bIsEta;        /**< Indicates that the hit measures eta */
        const Trk::RIO_OnTrack* m_pRIO;
        Chamber*      m_pChamber;      /**< The containing chamber */
        bool          m_bInSegment;    /**< Indicates that the hit participates in a reconstructed segment */
        int           m_nIndex;
    };

    inline Hit::Hit(const Amg::Vector3D& pos,
                    double dEta,
                    double dPhi,
                    bool isEta,
                    Chamber* pChamber) :
    m_position(pos),
    m_dDeltaEta(dEta),
    m_dDeltaPhi(dPhi),
    m_bIsEta(isEta),
    m_pRIO(NULL),
    m_pChamber(pChamber),
    m_bInSegment(false),
    m_nIndex(-1)
    {
    }

    inline const Amg::Vector3D& Hit::position() const
    {
        return m_position;
    }

    inline double Hit::deltaEta() const
    {
        return m_dDeltaEta;
    }

    inline double Hit::deltaPhi() const
    {
        return m_dDeltaPhi;
    }

    inline bool Hit::isEta() const
    {
        return m_bIsEta;
    }

    inline const Trk::RIO_OnTrack* Hit::RIO() const
    {
        return m_pRIO;
    }

    inline void Hit::setRIO(const Trk::RIO_OnTrack* pRIO)
    {
        m_pRIO = pRIO;
    }

    inline Chamber* Hit::chamber() const
    {
        return m_pChamber;
    }

    inline bool Hit::isInSegment() const
    {
        return m_bInSegment;
    }

    inline void Hit::setInSegment(bool bInSegment)
    {
        m_bInSegment = bInSegment;
    }

    inline std::string Hit::toString() const
    {
        std::ostringstream oss;
        oss << (m_bIsEta ? "Eta" : "Phi")
        << " pos=" << m_position
        << " deltaEta=" << m_dDeltaEta
        << " deltaPhi=" << m_dDeltaPhi;
        return oss.str();
    }

    inline int Hit::index() const
    {
        return m_nIndex;
    }

    inline void Hit::setIndex(int nIndex)
    {
        m_nIndex = nIndex;
    }

    struct lessHit
    {
        /**
         * The comparison operator
         * \param h1 The first hit
         * \param h2 The second hit
         */
        bool operator()(const Hit* h1, const Hit* h2)
        {
            if (!h1->isEta() &&  h2->isEta()) return true;
            if ( h1->isEta() && !h2->isEta()) return false;

            const Amg::Vector3D& p1 = h1->position();
            const Amg::Vector3D& p2 = h2->position();
            if (p1.x() < p2.x()) return true;
            if (p1.x() > p2.x()) return false;
            if (p1.y() < p2.y()) return true;
            if (p1.y() > p2.y()) return false;
            if (p1.z() < p2.z()) return true;
            return false;
        }
    };

    typedef std::set<Hit*, lessHit> HitSet;     /**< A set of unique hits */

}

#endif //MUGIRL_HIT_H
