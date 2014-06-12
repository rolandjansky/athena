/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CaloCellMBAverageCorr_H
#define CALOREC_CaloCellMBAverageCorr_H

#include "CaloUtils/CaloCellCorrection.h"
#include "CaloInterface/ICaloMBAverageTool.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ToolHandle.h"

class CaloCell;

class CaloCellMBAverageCorr : public CaloCellCorrection

{

public:

  CaloCellMBAverageCorr(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~CaloCellMBAverageCorr() {};

  virtual StatusCode initialize();

  void MakeCorrection(CaloCell* theCell);

private:

 ToolHandle<ICaloMBAverageTool> m_caloMBAverageTool;

};

#endif
