/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCELLCORRECTION_CALOCELLMBAVERAGECORR_H
#define CALOCELLCORRECTION_CALOCELLMBAVERAGECORR_H

#include "CaloUtils/CaloCellCorrection.h"
#include "CaloInterface/ICaloMBAverageTool.h"
#include "GaudiKernel/ToolHandle.h"

class CaloCell;

class CaloCellMBAverageCorr : public CaloCellCorrection

{

public:

  CaloCellMBAverageCorr(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~CaloCellMBAverageCorr() {};

  virtual StatusCode initialize() override;

  void MakeCorrection (CaloCell* theCell,
                       const EventContext& ctx) const override;

private:

 ToolHandle<ICaloMBAverageTool> m_caloMBAverageTool;

};

#endif
