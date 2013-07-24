/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  LARHVCORRMAKER_H
#define LARHVCORRMAKER_H

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "LArElecCalib/ILArHVCorrTool.h"

#include "LArRawConditions/LArHVScaleCorrComplete.h"
#include "LArIdentifier/LArOnlineID.h"

//-----------------------------------------------------------------------
class LArHVCorrMaker : public Algorithm
//-----------------------------------------------------------------------
{
 public:

  // Constructor
  LArHVCorrMaker(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  ~LArHVCorrMaker();

  // Algorithm initialization   
  StatusCode initialize(); 

  // Algorithm execution
  StatusCode execute();

  // Algorithm finalization
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  
 private:

  // StoreGate service
  StoreGateSvc * m_detStore;

  const LArOnlineID*        m_lar_on_id;

  ToolHandle<ILArHVCorrTool> m_hvCorrTool;
};

#endif
