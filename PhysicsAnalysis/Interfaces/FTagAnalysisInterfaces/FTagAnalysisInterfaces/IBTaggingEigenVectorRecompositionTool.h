// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include <map>

class IBTaggingEigenVectorRecompositionTool : virtual public CP::ISystematicsTool {

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( IBTagEigenVectorRecompositionTool )

  public:

  virtual CP::CorrectionCode printListOfOriginalNuisanceParameters(const std::string & label) const = 0;

  virtual CP::CorrectionCode printListOfCoefficients(const std::string & label,const int& evIdx) const = 0;

  virtual std::vector<std::string> getListOfOriginalNuisanceParameters(const std::string& label) const = 0;
  
  virtual std::map<std::string, std::map<std::string, double>> getCoefficientMap(const std::string& label, const std::vector<int> eigenIdxList = std::vector<int>()) const = 0;

  virtual std::vector<double> getCoefficients(const std::string & label, const int& evIdx) const = 0;

};

#endif // CPIBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H
