/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ZDC_DIGITIZATION_H
#define ZDC_DIGITIZATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

class ZDC_DigiTop: public AthAlgorithm
{

 public:

  ZDC_DigiTop(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~ZDC_DigiTop() = default;

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }

 private:

  ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "ZDC_PileUpTool", ""};
};

#endif
