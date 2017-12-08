/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArAutoCorrToolToDB_H
#define LArAutoCorrToolToDB_H
/********************************************************************
 
 NAME:     LArAutoCorrToolToDB.h
                      
********************************************************************/

// Include files
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArElecCalib/ILArAutoCorrTotalTool.h"

//-----------------------------------------------------------------------
class LArAutoCorrToolToDB : public AthAlgorithm
//-----------------------------------------------------------------------
{
 public:

  // Constructor
  LArAutoCorrToolToDB(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  ~LArAutoCorrToolToDB();

  // Algorithm initialization   
  StatusCode initialize(); 

  // Algorithm execution
  StatusCode execute();

  // Algorithm stop
  StatusCode stop();
 
  // Algorithm finalization
  StatusCode finalize();
  
 private:
  const LArOnlineID * m_onlineHelper;

  ToolHandle<ILArAutoCorrTotalTool> m_autocorrTool;

  // Grouping type
  std::string m_groupingType;

  // key
  std::string m_acContName;
  
  
};

#endif

