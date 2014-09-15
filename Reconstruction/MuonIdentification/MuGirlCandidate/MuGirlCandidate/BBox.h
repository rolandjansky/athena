/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_BBOX_H
#define MUGIRL_BBOX_H

#include "MuGirlCandidate/MuGirlNS.h"
#include "MuGirlCandidate/Utilities.h"
#include <float.h>

namespace MuGirlNS
{

    /**
     * A rectangle in Eta/Phi.
     *
     * Before dimensions are set, a box is \e null.
     */
    class BBox
    {
    public:
        /**
         * Default constructor.
         *
         * Establishes a null box.
         */
        BBox();

        /**
         * Constructor
         * \param etaMin Lowest Eta
         * \param etaMax Highest Eta
         * \param phiMin Lowest Phi
         * \param phiMax Highest Phi
         */
        BBox(double etaMin, double etaMax, double phiMin, double phiMax);

        /**
         * Reset the box to null
         */
        void clear();

        /**
         * Set the box from the given parameters
         * \param etaMin Lowest Eta
         * \param etaMax Highest Eta
         * \param phiMin Lowest Phi
         * \param phiMax Highest Phi
         */
        void set(double etaMin, double etaMax, double phiMin, double phiMax);

        /**
         * Set the box from the given parameters
         * \param eta Lowest and highest Eta
         * \param phi Lowest and highest Phi
         */
        void set(double eta, double phi);

        /**
         * Get the lowest eta
         */
        double etaMin() const;

        /**
         * Get the highest eta
         */
        double etaMax() const;

        /**
         * Get the lowest phi
         */
        double phiMin() const;

        /**
         * Get the highest phi
         */
        double phiMax() const;

        /**
         * Determine if the box is null
         */
        bool isNull() const;

        /**
         * Detemine if a point is in the box
         * \param eta The eta coordinate of the test point
         * \param phi The phi coordinate of the test point
         * \return \c true if the point is in the box
         */
        bool inside(double eta, double phi) const;

        /**
         * Detemine if a point is in the box
         * \param pt The coordinate of the test point
         * \return \c true if the point is in the box
         */
        bool inside(const Amg::Vector3D& pt) const;

        /**
         * Determine if another box overlaps this box
         * \param bbox The test box
         * \return \c true if the other box overlaps this box
         */
        bool on(const BBox& bbox) const;

        /**
         * Update (expand) the box to accommodate a new point
         * \param eta The new eta
         * \param phi The new phi
         */
        void update(double eta, double phi);

        /**
         * Update (expand) the box to accommodate a new point
         * \param pt The new point
         */
        void update(const Amg::Vector3D& pt);

        /**
         * Expand the box by a fixed amount in each direction
         * \param deltaEta Half the additional eta amount
         * \param deltaPhi Half the additional phi amount
         */
        void expandBy(double deltaEta, double deltaPhi);

        /**
         * Get a string representation of the box
         */
        std::string toString() const;

    protected:
        void canonicPhis();
        bool inPhiRange(double phi) const;

        double m_etaMin;    /**< Lowest Eta */
        double m_etaMax;    /**< Highest Eta */
        double m_phiMin;    /**< Lowest Phi */
        double m_phiMax;    /**< Highest Phi */
        double m_deletaPhi; /**< Canonic Phi range */
    };

    inline BBox::BBox() :
    m_etaMin(DBL_MAX),
    m_etaMax(-DBL_MAX),
    m_phiMin(DBL_MAX),
    m_phiMax(-DBL_MAX),
    m_deletaPhi(DBL_MAX)
    {
    }

    inline BBox::BBox(double etaMin, double etaMax, double phiMin, double phiMax) :
    m_etaMin(etaMin),
    m_etaMax(etaMax),
    m_phiMin(phiMin),
    m_phiMax(phiMax)
    {
        canonicPhis();
    }

    inline void BBox::clear()
    {
        set(DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX);
        m_deletaPhi = DBL_MAX;
    }

    inline void BBox::set(double etaMin, double etaMax, double phiMin, double phiMax)
    {
        m_etaMin = etaMin;
        m_etaMax = etaMax;
        m_phiMin = phiMin;
        m_phiMax = phiMax;
        canonicPhis();
    }

    inline void BBox::set(double eta, double phi)
    {
        m_etaMin = m_etaMax = eta;
        m_phiMin = m_phiMax = phi;
        m_deletaPhi = 0.0;
    }

    inline double BBox::etaMin() const
    {
        return m_etaMin;
    }

    inline double BBox::etaMax() const
    {
        return m_etaMax;
    }

    inline double BBox::phiMin() const
    {
        return m_phiMin;
    }

    inline double BBox::phiMax() const
    {
        return m_phiMax;
    }

    inline bool BBox::isNull() const
    {
        return m_etaMin == DBL_MAX;
    }

    inline bool BBox::inside(double eta, double phi) const
    {
        if (isNull())
            return true;
        return eta >= m_etaMin && eta <= m_etaMax && inPhiRange(phi);
    }

    inline bool BBox::inside(const Amg::Vector3D& pt) const
    {
        return BBox::inside(pt.eta(), pt.phi());
    }

    inline bool BBox::on(const BBox& bbox) const
    {
        return isNull() || bbox.isNull() ||
        (m_etaMin <= bbox.m_etaMax && m_etaMax >= bbox.m_etaMin &&
         (inPhiRange(bbox.m_phiMin) || inPhiRange(bbox.m_phiMax) ||
          bbox.inPhiRange(m_phiMin) || bbox.inPhiRange(m_phiMax)));
    }

    inline void BBox::update(const Amg::Vector3D& pt)
    {
        update(pt.eta(), pt.phi());
    }

    inline void BBox::expandBy(double deltaEta, double deltaPhi)
    {
        if (!isNull())
        {
            m_etaMin -= deltaEta;
            m_etaMax += deltaEta;
            m_phiMin -= deltaPhi;
            m_phiMax += deltaPhi;
            canonicPhis();
        }
    }

    inline void BBox::canonicPhis()
    {
        if (isNull())
            m_deletaPhi = DBL_MAX;
        else
        {
            m_deletaPhi = canonicPhi(m_phiMax - m_phiMin);
            if (m_deletaPhi < 0.0)
            {
                std::swap(m_phiMin, m_phiMax);
                m_deletaPhi = canonicPhi(m_phiMax - m_phiMin);
            }
        }
    }

    inline bool BBox::inPhiRange(double phi) const
    {
        double dPhi = canonicPhi(phi - m_phiMin);
        return isNull() || (dPhi >= 0 && dPhi <= m_deletaPhi);
    }

}

#endif //MUGIRL_BBOX_H

