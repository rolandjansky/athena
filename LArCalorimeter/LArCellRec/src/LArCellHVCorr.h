/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCELLREC_LARCELLHVCORR_H
#define LARCELLREC_LARCELLHVCORR_H


#include "CaloUtils/CaloCellCorrection.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArElecCalib/ILArCellHVCorrTool.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

class CaloCell;

/** @class LArCellHVCorr
    @brief Tool to re-scale cell energies using HVCorr tool

To invoke this tool:
CaloCellMaker.CaloCellMakerToolNames+=["CaloCellContainerCorrectorTool"]
ToolSvc.CaloCellContainerCorrectorTool.CellCorrectionToolNames+=["LArCellHVCorr"]
ToolSvc.CaloCellContainerCorrectorTool.CaloNums=[0] #LAr EM only

*/

class LArCellHVCorr : public CaloCellCorrection, virtual public ILArCellHVCorrTool {

public:
  
  LArCellHVCorr (const std::string& type, const std::string& name, 
		   const IInterface* parent);

  ~LArCellHVCorr();
  virtual StatusCode initialize() override;


  //Implements the CaloCellCorrection interface    
  virtual void MakeCorrection (CaloCell* theCell,
                               const EventContext& ctx) const override;

  //Implements the ILArCellHVCorrTool interface
  virtual float getCorrection(const Identifier id) override;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) override
  { return StatusCode::SUCCESS; }

  virtual bool updateOnLastCallback() override {return true;}

private: 
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

 float getCorrection(const Identifier id) const;

 SG::ReadCondHandleKey<ILArHVScaleCorr> m_scaleCorrKey
 { this, "LArHVScaleCorr", "LArHVScaleCorrRecomputed", "" };

};

#endif     
