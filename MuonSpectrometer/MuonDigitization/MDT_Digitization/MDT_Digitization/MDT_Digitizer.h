/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DIGITIZATION_MDT_DIGITIZER_H
#define MDT_DIGITIZATION_MDT_DIGITIZER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

class MDT_Digitizer : public AthAlgorithm {
 public:

  /** Constructor with parameters */
  MDT_Digitizer(const std::string& name, ISvcLocator* pSvcLocator);

  /** Destructor */
  virtual ~MDT_Digitizer() = default;

  /** Basic algorithm methods */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

 private:
   ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "MdtDigitizationTool", "MdtDigitizationTool name"};
};

#endif // MDT_DIGITIZATION_MDT_DIGITIZER_H
