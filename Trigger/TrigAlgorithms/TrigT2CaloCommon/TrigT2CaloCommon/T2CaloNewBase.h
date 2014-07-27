// emacs: -*- c++ -*-

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

#ifndef TRIGT2CALOCOMMON_T2CALONEWBASE_H 
#define TRIGT2CALOCOMMON_T2CALONEWBASE_H

#include <string>
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "TrigT2CaloCommon/T2CaloConfig.h"


class IAlgToolCalo;

/**T2CaloBase class definition. This is an HLT::FexAlgo */
class T2CaloNewBase: public HLT::FexAlgo {
  enum { MaxNumberTools = 10 };      
  
public:
  /** Constructor of this HLT Algorithm */
  T2CaloNewBase(const std::string & name, ISvcLocator* pSvcLocator);
  /** Destructor */
  virtual ~T2CaloNewBase();
  
  /** Initialize method. Tools are loaded for FEX. */
  StatusCode initialize();
  /** Finalize method. */
  StatusCode finalize();     
  
  
protected:
  
  /** Eta width of Region of Interest. Should be 0.2 for EGamma. */
  double 		      m_etaWidth;
  /** Phi width of Region of Interest. Should be 0.2 for EGamma. */
  double 		      m_phiWidth;
  /** Name of the tools. Not using Configurables here. */
  std::vector<std::string>  m_emToolName; 
  /** Name of the Total Algorithm timer item. */
  std::string               m_timerString;
  std::string               m_paramSet;
  std::string               m_triggerType;
  
  /** Vector of IAlgToolCalo pointers. These ones build the cluster
      properties. iNot using configurables here. */
  std::vector<IAlgToolCalo*>   m_emAlgTools;
  
  /** Time Service Handle */
  ITrigTimerSvc* m_timersvc;
  /** Timer Items for the main algorithms. */
  TrigTimer* m_timer[2];
  
};

#endif
