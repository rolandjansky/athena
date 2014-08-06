/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************

 NAME:     T2CaloSwCluster.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma

 AUTHOR:   A. Phan
 
 PURPOSE:  Based very closely on T2CaloEgamma.cxx . Given an inputTE
           containing a TrigEmCluster, calls a set of IAlgToolCalos
           to perform Feature EXtraction.
 
 *****************************************************************************/

#ifndef TRIGT2CALOEGAMMA_T2CALOSWCLUSTER_H
#define TRIGT2CALOEGAMMA_T2CALOSWCLUSTER_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

//#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "TrigT2CaloCommon/T2CaloBase.h"
#include "TrigT2CaloCalibration/IEgammaCalibration.h"
#include "egammaInterfaces/Iegammaqweta2c.h"

class IAlgToolCalo;
class IEgammaCalibration;
class MsgStream;
//class TrigEMCluster;

class T2CaloSwCluster : public T2CaloBase {

  public:
  
    /** Constructor*/
    T2CaloSwCluster(const std::string & name, ISvcLocator* pSvcLocator);
    
    /** Destructor */
    virtual ~T2CaloSwCluster();
    
    /** Steering functions */
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute( const HLT::TriggerElement* inputTE, 
                               HLT::TriggerElement* outputTE );

  protected:
  
    /** Cached MsgStream */
    MsgStream* m_log;

    /** Properties */
    std::string m_trigEmClusterKey;

    /** Calibration tools */
    ToolHandleArray<IEgammaCalibration> m_calibs;        

    /** Monitored cluster */
    const xAOD::TrigEMCluster* m_monitoredCluster;

    /** Monitored variables */
    float m_rCore;
    float m_eRatio;
    float m_l1eta;
    float m_l1phi;
    unsigned int m_conversionError;
    unsigned int m_algorithmError;
    float m_MonEta, m_MonPhi;
    /** Correction tool for the cluster width */
    ToolHandle<Iegammaqweta2c> m_egammaqweta2c;
    
};

#endif
