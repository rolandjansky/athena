/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCELLREC_LArCellHVCorr_H
#define LARCELLREC_LArCellHVCorr_H


#include "CaloUtils/CaloCellCorrection.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArHVCorrTool.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArElecCalib/ILArCellHVCorrTool.h"
#include "StoreGate/DataHandle.h"  

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
  virtual StatusCode initialize(); 

  void MakeCorrection(CaloCell* theCell); //Implements the CaloCellCorrection interface    

  float getCorrection(const Identifier id); //Implements the ILArCellHVCorrTool interface

  StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);

  bool updateOnLastCallback() {return m_updateOnLastCallback;}

 private: 

 ToolHandle<ILArHVCorrTool> m_hvCorrTool;
 std::string m_keyHVScaleCorr;
 bool m_undoHVonline;
 bool m_updateOnLastCallback;
 const DataHandle<ILArHVScaleCorr> m_dd_HVScaleCorr;

};

#endif     
