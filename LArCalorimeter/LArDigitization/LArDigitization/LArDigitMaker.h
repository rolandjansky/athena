/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITIZATION_LARDIGITMAKER_H
#define LARDIGITIZATION_LARDIGITMAKER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "LArDigitization/ILArPileUpTool.h"

#include "GaudiKernel/ToolHandle.h"

class LArDigitMaker : public AthAlgorithm
{
 public:

  LArDigitMaker(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual bool isClonable() const override final { return true; }

 private:

  ToolHandle<ILArPileUpTool> m_LArPileUpTool;

};

#endif
