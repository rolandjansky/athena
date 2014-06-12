/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOREC_CaloCellRescaler_H
#define CALOREC_CaloCellRescaler_H


#include "CaloUtils/CaloCellCorrection.h"
#include "CaloIdentifier/CaloCell_ID.h"

class CaloCell;

//Tool to re-scale cell energies. Factors to be given by jobOptions

//To invoke this tool:
//CaloCellMaker.CaloCellMakerToolNames+=["CaloCellContainerCorrectorTool"]
//ToolSvc.CaloCellContainerCorrectorTool.CellCorrectionToolNames+=["CaloCellRescaler"]
//ToolSvc.CaloCellContainerCorrectorTool.CaloNums=[0,3] #LAr EM and Tile 
//ToolSvc.CaloCellRescaler.FactorToCellsEMB0=1.5
//ToolSvc.CaloCellRescaler.FactorToCellsEMB1=1.5
//ToolSvc.CaloCellRescaler.FactorToCellsEMB2=1.5
//ToolSvc.CaloCellRescaler.FactorToCellsEMB3=1.5
//ToolSvc.CaloCellRescaler.FactorToCellsTILEB0=1.5
//ToolSvc.CaloCellRescaler.FactorToCellsTILEB1=1.5
//ToolSvc.CaloCellRescaler.FactorToCellsTILEB2=1.5

class CaloCellRescaler : public CaloCellCorrection {

public:
  
  CaloCellRescaler (const std::string& type, const std::string& name, 
                   const IInterface* parent);

  ~CaloCellRescaler();
  virtual StatusCode initialize(); 

  void MakeCorrection(CaloCell* theCell);    

 private: 

  float m_factorToCells[CaloCell_ID::Unknown+1];

};

#endif     

