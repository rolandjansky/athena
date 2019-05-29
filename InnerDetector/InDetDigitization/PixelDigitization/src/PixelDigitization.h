/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_PIXELDIGITIZATION_H
#define PIXELDIGITIZATION_PIXELDIGITIZATION_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
// Data members
#include "GaudiKernel/ToolHandle.h"
#include "PileUpTools/IPileUpTool.h"

class PixelDigitization : public AthAlgorithm {

public:

  /** Constructor with parameters */
  PixelDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Destructor */
  ~PixelDigitization();

  /** Algorithm::initialize() */
  virtual StatusCode initialize() override final;

  /** Algorithm::execute() */
  virtual StatusCode execute() override final;

private:
  ToolHandle<IPileUpTool> m_pixelDigitizationTool{this, "DigitizationTool", "PixelDigitizationTool", "PixelDigitizationTool name"};
};

#endif // PIXELDIGITIZATION_PIXELDIGITIZATION_H
