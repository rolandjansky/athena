/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARHVCORRMAKER_H
#define LARHVCORRMAKER_H

// Include files
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArElecCalib/ILArHVCorrTool.h"

//#include "LArIdentifier/LArOnlineID.h"

class LArOnlineID;

//-----------------------------------------------------------------------
class LArHVCorrMaker : public AthAlgorithm
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
  const LArOnlineID*        m_lar_on_id;
  ToolHandle<ILArHVCorrTool> m_hvCorrTool;
};

#endif
