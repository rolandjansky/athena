/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArCondDataTest.h
 *
 * @brief This file contains an algorithm for testing lar conditions
 * data access
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 * @author Hong Ma      <hma@bnl.gov>
 *
 * $Id: LArCondDataTest.h,v 1.1 2006-05-21 16:44:40 hma Exp $ */
#ifndef LARIOV_LARCONDDATATEST_H
#define LARIOV_LARCONDDATATEST_H

/**
 * @class LArCondDataTest
 *
 * @brief Athena algorithm used for testing LAr conditions data access
 *
**/

#include "AthenaBaseComps/AthAlgorithm.h" 
#include "GaudiKernel/IIncidentListener.h" 
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include "LArRecConditions/LArFebRodMapping.h"

class StoreGateSvc;
class LArOnlineID; 
class LArEM_ID; 

class LArCondDataTest : public AthAlgorithm 
{

public:


    LArCondDataTest(const std::string& name, ISvcLocator* pSvcLocator);
    ~LArCondDataTest();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:
    SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
    SG::ReadCondHandleKey<LArCalibLineMapping>  m_CLKey{this, "CalibLineKey", "LArCalibLineMap", "SG calib line key"};
    SG::ReadCondHandleKey<LArFebRodMapping>  m_RodKey{this, "FebRodKey", "LArFebRodMap", "SG ROD mapping key"};
    const LArOnlineID* m_onlineID; 
    const LArOnlineID* m_emid;
    int m_tbin;
    bool m_fixShape;
};
#endif 

