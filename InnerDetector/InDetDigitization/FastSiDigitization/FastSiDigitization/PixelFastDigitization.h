/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTSIDIGITIZATION_PIXELFASTDIGITIZATION_H
#define FASTSIDIGITIZATION_PIXELFASTDIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IPileUpTool;

/** Top algorithm class for Pixel digitization */
class PixelFastDigitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  PixelFastDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Destructor */
  virtual ~PixelFastDigitization() = default;

  /** Basic algorithm methods */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

 private:
  ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "PixelFastDigitizationTool", "AthAlgTool which performs the Pixel digitization"};
};

#endif // FASTSIDIGITIZATION_PIXELFASTDIGITIZATION_H
