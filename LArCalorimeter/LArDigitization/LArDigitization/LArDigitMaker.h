/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  ~LArDigitMaker();

  virtual StatusCode initialize();

  virtual StatusCode execute();

  virtual StatusCode finalize();

 private:

  ToolHandle<ILArPileUpTool> m_LArPileUpTool;

};

#endif
