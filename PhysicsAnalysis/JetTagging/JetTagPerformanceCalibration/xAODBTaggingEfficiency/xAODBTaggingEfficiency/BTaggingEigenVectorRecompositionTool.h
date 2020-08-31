// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BTaggingEigenVectorRecomposition.h, (c) ATLAS Detector software
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
#ifndef CPBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H
#define CPBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H

#include "FTagAnalysisInterfaces/IBTaggingEigenVectorRecompositionTool.h"
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicCode.h"

#include <string>
#include <vector>
#include <map>

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AnaToolHandle.h"

class BTaggingEigenVectorRecompositionTool: public asg::AsgTool,
            public virtual IBTaggingEigenVectorRecompositionTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS2( BTaggingEigenVectorRecompositionTool, IBTaggingEigenVectorRecompositionTool, ISystematicsTool)

  public:


  /// Create a constructor for standalone usage
  BTaggingEigenVectorRecompositionTool( const std::string& name );

  /// Create a constructor for standalone usage
  virtual ~BTaggingEigenVectorRecompositionTool();

  /**
   * Return a vector which contains a list of original vector uncertainties names.
   * vector list is for the chosen flavour label. The order of the names is the same
   * as the coefficient values given by getCoefficients()  
   **/
  std::vector<std::string> getListOfOriginalNuisanceParameters(const std::string& label) const;
  
  /**
   * Produce a coefficient map contains only eigenvectors that is showing in
   * eigenIdxList and return it to user. If given empty evIdxList, the function
   * returns a full map. Produced map is for the chosen flavour label.
   **/
   std::map<std::string, std::map<std::string, float>> getCoefficientMap(const std::string & label,
									 const std::vector<unsigned int> eigenIdxList = 
									 std::vector<unsigned int>()) const;

  /**
   * Returns a vector contains the coefficients value of the chosen label
   * and the chosen eigenvector. The order of the value is the same as
   * the order of original uncertainty names given by
   * getListOfOriginalNuisanceParameters()
   **/
  std::vector<float> getCoefficients(const std::string & label, const unsigned int evIdx) const;

  // Return number of eigenvectors used for the chosen label.
  int getNumEigenVectors(const std::string & label)const;

  /**
   * Initialize BtaggingEfficiencyTool handle and retrieve coefficient map for
   * all flavours. Also initialize vectors which contains all original sources
   * uncertainties' names. One vector for each flavour.
   **/  
  StatusCode initialize();

  // this returns a list of systematics supported by the btaggingEfficiency tool handle
  CP::SystematicSet affectingSystematics() const;

  /** it indicates which systematic shifts are to be applied for all future calls
   * no systematics for now, proxy for later
   **/
  CP::SystematicCode applySystematicVariation( const CP::SystematicSet & systConfig);

  /**
   * subset of systematics that are recommended by the
   * btaggingEfficiency tool handle
   **/
  CP::SystematicSet recommendedSystematics() const;

  /**
   * returns true if the argument systematic is supported by the
   * btaggingEfficiency tool handle
   **/
  bool isAffectedBySystematic( const CP::SystematicVariation & systematic ) const;


 private:

  // memeber variable maps which has format:
  // map<"Eigen_[flavour]_[index]", map<"[original uncertainty name]", coefficent value>
  // One map for each flavour.
  std::map<std::string, std::map<std::string, float>> m_coefficientMapB;
  std::map<std::string, std::map<std::string, float>> m_coefficientMapC;
  std::map<std::string, std::map<std::string, float>> m_coefficientMapT;
  std::map<std::string, std::map<std::string, float>> m_coefficientMapLight;

  // contains list of original uncertainty names. The aim for having this
  // is to keep the order of uncertainty names.
  // One list for each flavour.
  std::vector<std::string> m_NPnameListB;
  std::vector<std::string> m_NPnameListC;
  std::vector<std::string> m_NPnameListT;
  std::vector<std::string> m_NPnameListLight;

  // BtaggingEfficiencyTool handle.
  ToolHandle<IBTaggingEfficiencyTool> m_btageffTool;

};

#endif // CPBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H
