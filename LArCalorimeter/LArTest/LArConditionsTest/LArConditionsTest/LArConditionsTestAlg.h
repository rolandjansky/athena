/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArConditionsTestAlg.h
 *
 * @brief This file contains an algorithm for testing lar conditions
 * data access
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 * @author Hong Ma      <hma@bnl.gov>
 *
 * $Id: LArConditionsTestAlg.h,v 1.7 2009-01-15 15:06:50 gunal Exp $ */
#ifndef LARIOV_LARCONDITIONSTESTALG_H
#define LARIOV_LARCONDITIONSTESTALG_H

/**
 * @class LArConditionsTestAlg
 *
 * @brief Athena algorithm used for testing LAr conditions data access
 *
**/

#include "AthenaBaseComps/AthAlgorithm.h" 
#include "GaudiKernel/IIncidentListener.h" 
#include "AthenaKernel/IOVSvcDefs.h"

//  #include "LArCondCnv/ExampleData.h" 
//  #include "LArCondCnv/ExampleDataRamp.h" 
#include "LArElecCalib/ILArRamp.h" 
#include "StoreGate/DataHandle.h"
//#include "AthenaKernel/IOVSvcDefs.h" 
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include "LArRecConditions/LArFebRodMapping.h"

#include "LArElecCalib/ILArShape.h" 
#include "LArElecCalib/ILArNoise.h" 
#include "LArElecCalib/ILArRamp.h" 
#include "LArElecCalib/ILArDAC2uA.h" 
#include "LArElecCalib/ILAruA2MeV.h" 
#include "LArElecCalib/ILArAutoCorr.h" 
#include "LArElecCalib/ILArPedestal.h" 
#include "LArElecCalib/ILArfSampl.h" 
#include "LArElecCalib/ILArMinBias.h" 

#include "LArConditionsTest/LArRampPTmp.h" 


class LArOnlineID; 
class GenericDbTable; 
class LArRampMC;
class LArCablingLegacyService;

class LArConditionsTestAlg : public AthAlgorithm, public IIncidentListener 
{

public:


    LArConditionsTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~LArConditionsTestAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

    StatusCode testCallBack1(IOVSVC_CALLBACK_ARGS ) ; 
    StatusCode testCallBack2(IOVSVC_CALLBACK_ARGS ) ; 

    virtual void handle(const Incident&) ;

private:

    StatusCode createCompareObjects();
    StatusCode testCondObjects();
    StatusCode testEachCondObject(const LArRampMC* ramps);
    StatusCode testChannelSet();
    StatusCode printCondObjects();
    StatusCode streamOutCondObjects();
    StatusCode registerCondObjects();

    StatusCode testFillIOVDb()  ; 
    StatusCode testDbObjectRead() ;
    StatusCode testDCS_Objects() ; 
    void printTable( const GenericDbTable* t) ;

    // DataHandle
    // const DataHandle<ExampleData> m_example; 

    // Handles to interfaces
    const DataHandle<ILArNoise>    m_noise; 
    const DataHandle<ILArRamp>     m_ramp; 
    const DataHandle<ILArDAC2uA>   m_DAC2uA; 
    const DataHandle<ILAruA2MeV>   m_uA2MeV; 
    const DataHandle<ILArAutoCorr> m_autoCorr; 
    const DataHandle<ILArShape>    m_shape; 
    const DataHandle<ILArPedestal> m_pedestal; 
    const DataHandle<ILArfSampl>   m_fSampl; 
    const DataHandle<ILArMinBias>  m_minBias; 

    // Cache of compare data
    std::vector<LArRampPTmp>          m_rampCache;
    std::vector<LArRampPTmp>          m_rampCorrections;

    const LArCablingLegacyService * m_cablingSvc;
    const LArOnlineID* m_onlineID; 
    BooleanProperty    m_testFill; 
    BooleanProperty    m_testCondObjs; 
    BooleanProperty    m_readCondObjs;
    BooleanProperty    m_writeCondObjs;
    BooleanProperty    m_writeCorrections;
    BooleanProperty    m_applyCorrections;
    BooleanProperty    m_testReadDB; 
    BooleanProperty    m_TB; 
    BooleanProperty    m_testMC; 
    int                m_tbin ; 

};
#endif // LARIOV_LARCONDITIONSTESTALG_H

