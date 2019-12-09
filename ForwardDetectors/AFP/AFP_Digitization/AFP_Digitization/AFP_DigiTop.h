/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_DIGITIZATION_H
#define AFP_DIGITIZATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

class AFP_DigiTop: public AthAlgorithm
{

 public:

  AFP_DigiTop(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~AFP_DigiTop() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }

 private:

  ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "AFP_PileUpTool", ""};
};

#endif
