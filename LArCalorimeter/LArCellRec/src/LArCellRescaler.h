/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCELLREC_LARCELLRESCALER_H
#define LARCELLREC_LARCELLRESCALER_H


#include "CaloUtils/CaloCellCorrection.h"
#include "CaloConditions/CaloCellFactor.h"
#include "StoreGate/DataHandle.h"  
#include "AthenaKernel/IOVSvcDefs.h"
#include "CxxUtils/checker_macros.h"
#include <string>

class CaloCell;

/** @class LArCellRescaler
    @brief Tool to re-scale cell energies. Weights would be typically 
obtained by Z->ee intercalibration.

To invoke this tool:
CaloCellMaker.CaloCellMakerToolNames+=["CaloCellContainerCorrectorTool"]
ToolSvc.CaloCellContainerCorrectorTool.CellCorrectionToolNames+=["LArCellRescaler"]
ToolSvc.CaloCellContainerCorrectorTool.CaloNums=[0] #LAr EM only
ToolSvc.LArCellRescaler.CorrectionKey="myCorrectionKey"

*/

//Not safe due to callbacks
class ATLAS_NOT_THREAD_SAFE LArCellRescaler : public CaloCellCorrection
{

public:
  
  LArCellRescaler (const std::string& type, const std::string& name, 
		   const IInterface* parent);

  virtual ~LArCellRescaler();
  virtual StatusCode initialize() override;

  virtual void MakeCorrection (CaloCell* theCell,
                               const EventContext& ctx) const override;

 private: 
  const DataHandle<CaloRec::CaloCellFactor> m_factors;
  std::string m_key;
  
  StatusCode checkConstants(IOVSVC_CALLBACK_ARGS);
};

#endif     
