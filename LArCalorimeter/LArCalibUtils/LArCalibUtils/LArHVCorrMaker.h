/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARHVCORRMAKER_H
#define LARHVCORRMAKER_H

// Include files
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "GaudiKernel/ToolHandle.h"


class LArOnlineID;

//-----------------------------------------------------------------------
class LArHVCorrMaker : public AthAlgorithm
//-----------------------------------------------------------------------
{
 public:

  // Constructor
  LArHVCorrMaker(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  virtual ~LArHVCorrMaker();

  // Algorithm initialization   
  virtual StatusCode initialize() override;

  // Algorithm execution
  virtual StatusCode execute() override;

  // Algorithm finalization
  virtual StatusCode stop() override;
  virtual StatusCode finalize() override {return StatusCode::SUCCESS;}
  
 private:
  const LArOnlineID*        m_lar_on_id;
  std::string m_keyOutput;
  std::string m_folderName;
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_scaleCorrKey
  { this, "LArHVScaleCorr", "LArHVScaleCorrRecomputed", "" };
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_onlineScaleCorrKey
  { this, "OnlineLArHVScaleCorr", "LArHVScaleCorr", "" };
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
  { this, "CablingKey", "LArOnOffIdMap", "SG Key of LArOnOffIdMapping object" };
  
};

#endif
