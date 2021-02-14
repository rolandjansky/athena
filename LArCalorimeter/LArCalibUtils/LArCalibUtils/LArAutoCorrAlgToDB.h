/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArAutoCorrAlgToDB_H
#define LArAutoCorrAlgToDB_H
/********************************************************************
 
 NAME:     LArAutoCorrAlgToDB.h
                      
********************************************************************/

// Include files
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArRawConditions/LArAutoCorrTotal.h"
#include "StoreGate/ReadCondHandleKey.h"

//-----------------------------------------------------------------------
class LArAutoCorrAlgToDB : public AthAlgorithm
//-----------------------------------------------------------------------
{
 public:

  // Constructor
  LArAutoCorrAlgToDB(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  ~LArAutoCorrAlgToDB();

  // Algorithm initialization   
  StatusCode initialize(); 

  StatusCode execute(){return StatusCode::SUCCESS;}; 

  // Algorithm stop
  StatusCode stop();
 
 private:
  const LArOnlineID_Base* m_onlineHelper;
  
  bool m_isSC;

  SG::ReadCondHandleKey<LArAutoCorrTotal> m_autocorrKey{this,"LArAutoCorrTotal","LArPhysAutoCorr","Key for total autocorr on Conditions store"};

  // Grouping type
  std::string m_groupingType;

  // key
  std::string m_acContName;
  
  // number of gains
  unsigned int m_nGains; 
  
};

#endif

