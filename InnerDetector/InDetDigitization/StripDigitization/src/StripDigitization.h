// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** @file StripDigitization.h  Header file for StripDigitization class.
 */

// Multiple inclusion protection
#ifndef STRIPDIGITIZATION_STRIPDIGITIZATION_H
#define STRIPDIGITIZATION_STRIPDIGITIZATION_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
// Gaudi
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

namespace ITk
{

/** Top algorithm class for Strip digitization */
class StripDigitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  StripDigitization(const std::string& name, ISvcLocator* pSvcLocator);

  /** Destructor */
  virtual ~StripDigitization() = default;

  /** Basic algorithm methods */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }

 private:

  ToolHandle<IPileUpTool> m_stripDigitizationTool{this, "DigitizationTool", "StripDigitizationTool", "StripDigitizationTool name"};

};

} // namespace ITk

#endif // STRIPDIGITIZATION_STRIPDIGITIZATION_H
