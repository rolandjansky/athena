/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_PIXELFASTDIGITIZATION_H
#define PIXELDIGITIZATION_PIXELFASTDIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IPixelFastDigitizationTool;

/** Top algorithm class for Pixel digitization */
class PixelFastDigitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  PixelFastDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Basic algorithm methods */
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
   ToolHandle<IPixelFastDigitizationTool> m_digTool;
};

#endif // PixelDIGITIZATION_PIXELFASTDIGITIZATION_H
