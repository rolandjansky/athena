/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCM_DIGITIZATION_BCM_DIGITIZATION_H
#define BCM_DIGITIZATION_BCM_DIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IPileUpTool;

/** Top algorithm class for BCM digitization */
class BCM_Digitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  BCM_Digitization(const std::string &name,ISvcLocator *pSvcLocator);

  virtual ~BCM_Digitization() = default;

  /** Basic algorithm methods */
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual bool isClonable() const override final { return true; }

 private:
  ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "BCM_DigitizationTool", ""};
};

#endif // BCM_DIGITIZATION_BCM_DIGITIZATION_H
