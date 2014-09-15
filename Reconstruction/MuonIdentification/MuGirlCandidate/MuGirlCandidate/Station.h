/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_STATION_H
#define MUGIRL_STATION_H

#include "IRegionSelector/RegSelEnums.h"
#include "MuGirlCandidate/MuGirlNS.h"

namespace MuGirlNS
{

    class Technology;
    class Distance;

    class Station
    {
    public:
        Station(int id,
                const std::string& name,
                Technology* pTech,
                Distance* pDist,
                RegionType eReg);

        const std::string name() const;
        Technology* technology() const;
        Distance* distance() const;
        TechnologyType technologyType() const;
        DistanceType distanceType() const;
        RegionType regionType() const;
        int id() const;
        TYPEID typeId() const;
        int numEta() const;
        void setNumEta(int numEta);
        int numPhi() const;
        void setNumPhi(int numPih);
        std::string toString() const;

    private:
        std::string  m_name;    /**< The name of the station */
        Technology*  m_pTech;   /**< The Technology */
        Distance*    m_pDist;   /**< The Distance */
        RegionType   m_eReg;    /**< The Region */
        int          m_id;      /**< The station ID */
        int          m_nEta;    /**< Number of modules in the eta direction */
        int          m_nPhi;    /**< Number of modules in the phi direction */
    };

    inline Station::Station(int id,
                            const std::string& name,
                            Technology* pTech,
                            Distance* pDist,
                            RegionType eReg) :
    m_name(name),
    m_pTech(pTech),
    m_pDist(pDist),
    m_eReg(eReg),
    m_id(id),
    m_nEta(0),
    m_nPhi(0)
    {
    }

    inline const std::string Station::name() const
    {
        return m_name;
    }

    inline Technology* Station::technology() const
    {
        return m_pTech;
    }

    inline Distance* Station::distance() const
    {
        return m_pDist;
    }

    inline RegionType Station::regionType() const
    {
        return m_eReg;
    }

    inline int Station::id() const
    {
        return m_id;
    }

    inline TYPEID Station::typeId() const
    {
        return (TYPEID)(m_id + 1);
    }

    inline int Station::numEta() const
    {
        return m_nEta;
    }

    inline void Station::setNumEta(int numEta)
    {
        m_nEta = numEta;
    }

    inline int Station::numPhi() const
    {
        return m_nPhi;
    }

    inline void Station::setNumPhi(int numPhi)
    {
        m_nPhi = numPhi;
    }

}

#endif //MUGIRL_STATION_H
