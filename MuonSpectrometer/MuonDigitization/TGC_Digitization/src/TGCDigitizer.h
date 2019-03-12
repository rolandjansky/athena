/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGC_DIGITIZATION_TGC_DIGITIZER_H
#define TGC_DIGITIZATION_TGC_DIGITIZER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

class TGCDigitizer : public AthAlgorithm {
 public:

  /** Constructor with parameters */
  TGCDigitizer(const std::string& name, ISvcLocator* pSvcLocator);

  /** Destructor */
  virtual ~TGCDigitizer() = default;

  /** Basic algorithm methods */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

 private:
   ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "TgcDigitizationTool", "TgcDigitizationTool name"};
};

#endif // TGC_DIGITIZATION_TGC_DIGITIZER_H
