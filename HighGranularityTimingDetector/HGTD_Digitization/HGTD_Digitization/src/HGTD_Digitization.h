/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef HGTDDIGITIZATION_HGTDDIGITIZATION_H
#define HGTDDIGITIZATION_HGTDDIGITIZATION_H

#include <AthenaBaseComps/AthAlgorithm.h>
#include <GaudiKernel/ToolHandle.h>
#include <PileUpTools/IPileUpTool.h>


class HGTD_Digitization : public AthAlgorithm
{
public:
  HGTD_Digitization(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~HGTD_Digitization() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }

private:
  ToolHandle<IPileUpTool> m_HGTD_DigitizationTool {
    this, "DigitizationTool", "HGTD_DigitizationTool", "HGTD_DigitizationTool name"
  };
};

#endif // HGTDDIGITIZATION_HGTDDIGITIZATION_H
