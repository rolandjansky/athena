/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlCandidate/StationManager.h"
#include "MuGirlCandidate/Station.h"
#include "MuGirlCandidate/CandidateTool.h"
#include "MuGirlCandidate/Technology.h"

namespace MuGirlNS
{

    StationManger::StationManger(CandidateTool* pMuGirl)
    {
        makeStation(pMuGirl, "BIL", MDT_TECH, INNER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "BIS", MDT_TECH, INNER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "BML", RPC_TECH, MIDDLE_DIST, BARREL_REGION);
        makeStation(pMuGirl, "BML", MDT_TECH, MIDDLE_DIST, BARREL_REGION);
        makeStation(pMuGirl, "BMS", RPC_TECH, MIDDLE_DIST, BARREL_REGION);
        makeStation(pMuGirl, "BMS", MDT_TECH, MIDDLE_DIST, BARREL_REGION);
        makeStation(pMuGirl, "BOL", RPC_TECH, OUTER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "BOL", MDT_TECH, OUTER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "BOS", RPC_TECH, OUTER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "BOS", MDT_TECH, OUTER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "BEE", MDT_TECH, EE_DIST,     ENDCAP_REGION);
        makeStation(pMuGirl, "BIR", MDT_TECH, INNER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "BMS", RPC_TECH, MIDDLE_DIST, BARREL_REGION);
        makeStation(pMuGirl, "BMS", MDT_TECH, MIDDLE_DIST, BARREL_REGION);
        makeStation(pMuGirl, "BOF", RPC_TECH, OUTER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "BOF", MDT_TECH, OUTER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "BOG", RPC_TECH, OUTER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "BOG", MDT_TECH, OUTER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "EIL", MDT_TECH, INNER_DIST,  ENDCAP_REGION);
        makeStation(pMuGirl, "EEL", MDT_TECH, EE_DIST,     BARREL_REGION);
        makeStation(pMuGirl, "EES", MDT_TECH, EE_DIST,     BARREL_REGION);
        makeStation(pMuGirl, "EML", MDT_TECH, MIDDLE_DIST, ENDCAP_REGION);
        makeStation(pMuGirl, "EMS", MDT_TECH, MIDDLE_DIST, ENDCAP_REGION);
        makeStation(pMuGirl, "EOL", MDT_TECH, OUTER_DIST,  ENDCAP_REGION);
        makeStation(pMuGirl, "EOS", MDT_TECH, OUTER_DIST,  ENDCAP_REGION);
        makeStation(pMuGirl, "T1F", TGC_TECH, MIDDLE_DIST, ENDCAP_REGION, 1, 24);
        makeStation(pMuGirl, "T1E", TGC_TECH, MIDDLE_DIST, ENDCAP_REGION, 4, 48);
        makeStation(pMuGirl, "T2F", TGC_TECH, MIDDLE_DIST, ENDCAP_REGION, 1, 24);
        makeStation(pMuGirl, "T2E", TGC_TECH, MIDDLE_DIST, ENDCAP_REGION, 5, 48);
        makeStation(pMuGirl, "T3F", TGC_TECH, MIDDLE_DIST, ENDCAP_REGION, 1, 24);
        makeStation(pMuGirl, "T3E", TGC_TECH, MIDDLE_DIST, ENDCAP_REGION, 5, 48);
        makeStation(pMuGirl, "T4F", TGC_TECH, INNER_DIST,  ENDCAP_REGION, 1, 24);
        makeStation(pMuGirl, "T4E", TGC_TECH, INNER_DIST,  ENDCAP_REGION, 1, 24);
        makeStation(pMuGirl, "EIS", MDT_TECH, INNER_DIST,  ENDCAP_REGION);
        makeStation(pMuGirl, "CSS", CSC_TECH, INNER_DIST,  ENDCAP_REGION);
        makeStation(pMuGirl, "CSL", CSC_TECH, INNER_DIST,  ENDCAP_REGION);
        makeStation(pMuGirl, "BIM", MDT_TECH, INNER_DIST,  BARREL_REGION);
        makeStation(pMuGirl, "BMF", RPC_TECH, MIDDLE_DIST, BARREL_REGION);
        makeStation(pMuGirl, "BMF", MDT_TECH, MIDDLE_DIST, BARREL_REGION);
        makeStation(pMuGirl, "MML", MM_TECH,  INNER_DIST,  ENDCAP_REGION);
        makeStation(pMuGirl, "MMS", MM_TECH,  INNER_DIST,  ENDCAP_REGION);
        makeStation(pMuGirl, "STL", STGC_TECH,INNER_DIST,  ENDCAP_REGION);
        makeStation(pMuGirl, "STS", STGC_TECH,INNER_DIST,  ENDCAP_REGION);
	makeStation(pMuGirl, "BME", MDT_TECH, MIDDLE_DIST, BARREL_REGION);
	makeStation(pMuGirl, "BME", RPC_TECH, MIDDLE_DIST, BARREL_REGION);
	makeStation(pMuGirl, "BOE", MDT_TECH, OUTER_DIST,  BARREL_REGION);
	makeStation(pMuGirl, "BOE", RPC_TECH, OUTER_DIST,  BARREL_REGION);
	makeStation(pMuGirl, "BMG", MDT_TECH, MIDDLE_DIST, BARREL_REGION);
    }

    StationManger::~StationManger()
    {
        for (StationList::iterator itSta = m_stations.begin();
            itSta != m_stations.end();
            itSta++)
        {
            delete *itSta;
        }
    }

    void StationManger::makeStation(CandidateTool* pMuGirl,
                                    const std::string& name,
                                    TechnologyType eTech,
                                    DistanceType eDist,
                                    RegionType eReg,
                                    int nEta,
                                    int nPhi)
    {
        Technology* pTech = pMuGirl->technology(eTech);
        int stationId = pTech->stationNameID(name);
        Station*  pStation = new Station(stationId,
                                         name,
                                         pTech,
                                         pMuGirl->distance(eDist),
                                         eReg);
        pStation->setNumEta(nEta);
        pStation->setNumPhi(nPhi);
        m_stations.push_back(pStation);
        m_stationGroups[eTech][eDist][eReg].push_back(pStation);
        int key = eTech * 100 + stationId;
        m_stationMap.insert(MapType::value_type(key, pStation));
    }

    Station* StationManger::station(int stationId, TechnologyType eTech) const
    {
        int key = eTech * 100 + stationId;
        MapType::const_iterator it = m_stationMap.find(key);
        return it == m_stationMap.end() ? NULL : it->second;
    }

}
