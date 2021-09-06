/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXForwardJetsInfo - Class to store information about trigger towers
//                                    in the fcal and associated jet information.
//                              -------------------
//     begin                : 07 07 2021
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************
#ifndef jFEXForwardJetsInfo_H
#define jFEXForwardJetsInfo_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1 {
class jFEXForwardJetsInfo {

    public:
        void setCentreTTID(int TT_ID);
        int getCentreTTID();

        void setCentreTTEta(float centreTTEta);
        float getCentreTTEta();

        void setCentreTTPhi(float centreTTPhi);
        float getCentreTTPhi();

        void  setCentreLocalTTEta(float centreLocalTTEta);
        float getCentreLocalTTEta();
        void  setCentreLocalTTPhi(float centreLocalTTPhi);
        float getCentreLocalTTPhi();

        void  includeTTinSeed(int TT_ID);
        std::vector<int> getTTinSeed();

        void addToSeedET(int seed_ET);
        int getSeedET();

        void addToFirstEnergyRingET(int firstER_ET);
        int getFirstEnergyRingET();

        void includeTTIDinFirstER(int firstER_TTID);
        std::vector<int> getTTIDinFirstER();

        void addToSecondEnergyRingET(int secondER_ET);
        int getSecondEnergyRingET();

        void includeTTIDinSecondER(int secondER_TTID);
        std::vector<int> getTTIDinSecondER();

        void includeTTinSearchWindow(int TT_ID);
        std::vector<int> getTTinSearchWindow();
        void addToSearchWindowET(int seed_ET);
        int getSearchWindowET();

    private:
        int m_centre_TTID;
        float m_centreTTPhi;
        float m_centreTTEta;

        int m_centreLocalTTPhi;
        int m_centreLocalTTEta;

        int m_seed_ET =0;
        int m_SearchWindow_ET =0;
        int m_firstEnergyRing_ET =0;
        int m_secondEnergyRing_ET =0;
        std::vector<int> m_TTsInSeed = {};
        std::vector<int> m_TTsInSearchWindow = {};
        std::vector<int> m_firstEnergyRing_TTIDs = {};
        std::vector<int> m_secondEnergyRing_TTIDs = {};

};



}//end of namespace
#endif
