/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LArCellHVCorr_H
#define LARCELLREC_LArCellHVCorr_H


#include "CaloUtils/CaloCellCorrection.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArHVCorrTool.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "StoreGate/DataHandle.h"  

class CaloCell;

/** @class LArCellHVCorr
    @brief Tool to re-scale cell energies using HVCorr tool

To invoke this tool:
CaloCellMaker.CaloCellMakerToolNames+=["CaloCellContainerCorrectorTool"]
ToolSvc.CaloCellContainerCorrectorTool.CellCorrectionToolNames+=["LArCellHVCorr"]
ToolSvc.CaloCellContainerCorrectorTool.CaloNums=[0] #LAr EM only

*/

class LArCellHVCorr : public CaloCellCorrection {

public:
  
  LArCellHVCorr (const std::string& type, const std::string& name, 
		   const IInterface* parent);

  ~LArCellHVCorr();
  virtual StatusCode initialize(); 

  void MakeCorrection(CaloCell* theCell);    

 private: 

 ToolHandle<ILArHVCorrTool> m_hvCorrTool;
 std::string m_keyHVScaleCorr;
 bool m_undoHVonline;

 const DataHandle<ILArHVScaleCorr> m_dd_HVScaleCorr;

};

#endif     
