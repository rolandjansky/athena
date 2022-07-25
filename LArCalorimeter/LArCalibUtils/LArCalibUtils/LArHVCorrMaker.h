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


class LArOnlineID;

//-----------------------------------------------------------------------
class LArHVCorrMaker : public AthAlgorithm
//------`-----------------------------------------------------------------
{
 public:

  //Delgate constructor
  using AthAlgorithm::AthAlgorithm;


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
  const LArOnlineID*        m_lar_on_id = nullptr;

  Gaudi::Property<std::string> m_folderName{this,"folderName", "/LAR/ElecCalibFlat/HVScaleCorr",
      "Folder to store the CondAttrListCollection containing the HVScale correction"};

  SG::ReadCondHandleKey<ILArHVScaleCorr> m_scaleCorrKey
  { this, "LArHVScaleCorr", "LArHVScaleCorrRecomputed", "" };
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
  { this, "CablingKey", "LArOnOffIdMap", "SG Key of LArOnOffIdMapping object" };
  
};

#endif
