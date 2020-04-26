// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IBTaggingEfficiencyTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CPIBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H
#define CPIBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicSet.h"

#include <vector>

class IBTaggingEigenVectorRecompositionTool : virtual public CP::ISystematicsTool {

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( IBTagEigenVectorRecompositionTool )

  public:

  virtual CP::CorrectionCode printListOfOriginalNuisanceParameters() const = 0;

  virtual CP::CorrectionCode printListOfCoefficients() const = 0;

  virtual std::vector<std::string> getListOfOriginalNuisanceParameters() const = 0;
  virtual std::vector<float> getCoefficients(const std::string & ev_name) const = 0;

};

#endif // CPIBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H
