// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IBTaggingEfficiencyTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
/**
@class BTaggingEigenVectorRecompositionTool
Tool to help retrieving(from CDI) and providing coefficents values which
could be used for expressing eigen vector NPs by linear combination of
original uncertainty NPs in workspace level of the physics analysis.
Then eigenvector uncertainties are replaced by original uncertainties.
Replacement could help us correlate uncertainties between analyses
which are using different tagger algorighthms.
@author Y. Ke, Q. Buat
@contact yake@cern.ch, qbuat@cern.ch
**/
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
  /**
   * Return a vector which contains a list of original vector uncertainties names.
   * vector list is for the chosen flavour label. The order of the names is the same
   * as the coefficient values given by getCoefficients()  
   **/
  virtual std::vector<std::string> getListOfOriginalNuisanceParameters(const std::string& label) const = 0;
    /**
   * Produce a coefficient map contains only eigenvectors that is showing in
   * eigenIdxList and return it to user. If given empty evIdxList, the function
   * returns a full map. Produced map is for the chosen flavour label.
   **/
  virtual std::map<std::string, std::map<std::string, float>> getCoefficientMap(const std::string& label, const std::vector<unsigned int> eigenIdxList = std::vector<unsigned int>()) const = 0;
  /**
   * Returns a vector contains the coefficients value of the chosen label
   * and the chosen eigenvector. The order of the value is the same as
   * the order of original uncertainty names given by
   * getListOfOriginalNuisanceParameters()
   **/
  virtual std::vector<float> getCoefficients(const std::string & label, const unsigned int evIdx) const = 0;
  // Return number of eigenvectors used for the chosen label.
  virtual int getNumEigenVectors(const std::string & label) const = 0;

};

#endif // CPIBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H
