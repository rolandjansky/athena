/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_STATIONMANAGER_H
#define MUGIRL_STATIONMANAGER_H

#include "MuGirlCandidate/MuGirlNS.h"
#include <map>

namespace MuGirlNS
{
    class CandidateTool;
    
    class StationManger
    {
    public:

        StationManger(CandidateTool* pMuGirl);
        ~StationManger();

        const StationList& stations(TechnologyType eTech,
                                    DistanceType eDist,
                                    RegionType eReg) const;
        Station* station(int stationId, TechnologyType eTech) const;

    private:
        typedef std::map<int, Station*> MapType;

        void makeStation(CandidateTool* pMuGirl,
                         const std::string& name,
                         TechnologyType eTech,
                         DistanceType eDist,
                         RegionType eReg,
                         int nEta = 0,
                         int nPhi = 0);

        StationList m_stations;
        StationList m_stationGroups[TECHNOLOGIES][DISTANCES][REGIONS];
        MapType m_stationMap;
    };

    inline const StationList& StationManger::stations(TechnologyType eTech,
                                                      DistanceType eDist,
                                                      RegionType eReg) const
    {
        return m_stationGroups[eTech][eDist][eReg];
    }

}

#endif //MUGIRL_STATIONMANAGER_H

