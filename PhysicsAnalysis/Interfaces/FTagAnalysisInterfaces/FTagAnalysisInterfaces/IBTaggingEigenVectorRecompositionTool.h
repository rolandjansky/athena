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

#include <vector>

class IBTaggingEigenVectorRecompositionTool : virtual public CP::ISystematicsTool {

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( IBTagEigenVectorRecompositionTool )

  public:

  virtual CP::CorrectionCode getListOfOriginalNuisanceParameters( std::vector<std::string> & np_list) const = 0;

  virtual CP::CorrectionCode getListOfCoefficients() const = 0;

};

#endif // CPIBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H
