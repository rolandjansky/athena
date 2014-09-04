/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LArCellRecalibration_H
#define LARCELLREC_LArCellRecalibration_H


#include "CaloUtils/CaloCellCorrection.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArADC2MeVTool.h"

class CaloCell;

/** @class LArCellRecalibration
    @brief Tool to re-scale cell energies undoing online calibration and applying new MeV/ADC factors

To invoke this tool:
CaloCellMaker.CaloCellMakerToolNames+=["CaloCellContainerCorrectorTool"]
ToolSvc.CaloCellContainerCorrectorTool.CellCorrectionToolNames+=["LArCellRecalibration"]

*/

class LArCellRecalibration : public CaloCellCorrection {

public:
  
  LArCellRecalibration (const std::string& type, const std::string& name, 
		   const IInterface* parent);

  ~LArCellRecalibration();
  virtual StatusCode initialize(); 

  void MakeCorrection(CaloCell* theCell);    

 private: 

 ToolHandle<ILArADC2MeVTool> m_adc2mevTool;
 ToolHandle<ILArADC2MeVTool> m_adc2mevToolOnline;
 
 bool m_useInterceptOld;
 bool m_useInterceptNew;

};

#endif     
