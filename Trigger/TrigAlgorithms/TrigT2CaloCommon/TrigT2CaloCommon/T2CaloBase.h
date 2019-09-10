/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     T2Calo.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
 
 AUTHOR:   M.P. Casado
 
 PURPOSE: This is the common base for T2CaloEgamma and T2CaloTau
	  algorithm. It interfaces with LVL1 to fetch Region of
	  Interest position, builds the LVL2 output cell cluster
	  and calls the execute method of the IAlgToolCalos which
	  will build the cluster properties (feature extraction).
 *******************************************************************/

#ifndef TRIGT2CALOCOMMON_T2CALOBASE_H 
#define TRIGT2CALOCOMMON_T2CALOBASE_H

#include <string>
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "TrigT2CaloCommon/T2CaloConfig.h"
#include "GaudiKernel/ToolHandle.h"

class ITrigDataAccess;

/**T2CaloBase class definition. This is an HLT::FexAlgo */
class T2CaloBase: public HLT::FexAlgo {
  enum { MaxNumberTools = 10 };      
  
  public:
    /** Constructor of this HLT Algorithm */
    T2CaloBase(const std::string & name, ISvcLocator* pSvcLocator);
    /** Destructor */
    virtual ~T2CaloBase();

    /** Initialize method. Tools are loaded for FEX. */
    StatusCode initialize();
    /** Finalize method. */
    StatusCode finalize();     

    /** Prepare ROB requests */
    using HLT::FexAlgo::prepareRobRequests;
    HLT::ErrorCode prepareRobRequests(const HLT::TriggerElement* inputTE );
 
    /*
    int EtaPhiRange(const int nCaloType, const int nCaloSamp, const int nStripEta, const int nStripPhi, const double energyEta, const double energyPhi, double *CellEtaMin, double *CellEtaMax, double *CellPhiMin, double *CellPhiMax);
    */

  protected:

    /** Eta width of Region of Interest. Should be 0.2 for EGamma. */
    double 		      m_etaWidth;
    /** Phi width of Region of Interest. Should be 0.2 for EGamma. */
    double 		      m_phiWidth;
    /** Eta width of Region of Interest for ID. Should be 0.1 for EGamma. */
    double 		      m_etaWidthForID;
    /** Phi width of Region of Interest for ID. Should be 0.1 for EGamma. */
    double 		      m_phiWidthForID;

    /** Name of the Total Algorithm timer item. */
    std::string               m_timerString;
    std::string               m_paramSet;
    std::string               m_triggerType;

    /** Array of IAlgToolCalo Handles. These ones build the cluster
	properties. */
    ToolHandleArray<IAlgToolCalo>   m_emAlgTools;
    ToolHandle<ITrigDataAccess>     m_data;

    /** Time Service Handle */
    ServiceHandle<ITrigTimerSvc> m_timersvc;
    /** Timer Items for the main algorithms. */
    TrigTimer* m_timer[2];
 
    /** Some variables for RegionSelector specification */
    double m_roiEtaLimit;
    double m_fcalEtaMin;
    double m_fcalEtaMax;

    /** vector to keep track of ROBs to be requested */
    std::vector<uint32_t> m_vec_robs;

    /** whether to use standard, or composite roi **/
    bool m_useCompositeRoi; 
    
};

#endif
