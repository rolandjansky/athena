// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BTaggingEfficiencyTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

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

  /* Get list of original nuisance parameters that will linearly combined with the list of 
     coefficients returns below to recompose each eigen vector
 */
  CP::CorrectionCode printListOfOriginalNuisanceParameters() const;

  /* print list of coefficients to be used to build the linear combination 
   */
  CP::CorrectionCode printListOfCoefficients() const;

  std::vector<std::string> getListOfOriginalNuisanceParameters(const std::string& label) const;
  std::map<std::string, std::map<std::string, double>> getCoefficientMap(const std::string & label, const std::vector<int> eigenIdxList = std::vector<int>()) const;
  std::vector<double> getCoefficients(const std::string & label, const int& evIdx) const;


  StatusCode initialize();

  CP::SystematicSet affectingSystematics() const;
  CP::SystematicCode applySystematicVariation( const CP::SystematicSet & systConfig);
  CP::SystematicSet recommendedSystematics() const;
  bool isAffectedBySystematic( const CP::SystematicVariation & systematic ) const;


 private:

  bool m_initialised;

  std::map<std::string, std::map<std::string, double>> m_coefficientMapB;
  std::map<std::string, std::map<std::string, double>> m_coefficientMapC;
  std::map<std::string, std::map<std::string, double>> m_coefficientMapT;
  std::map<std::string, std::map<std::string, double>> m_coefficientMapLight;

  std::vector<std::string> m_NPnameListB;
  std::vector<std::string> m_NPnameListC;
  std::vector<std::string> m_NPnameListT;
  std::vector<std::string> m_NPnameListLight;

  ToolHandle<IBTaggingEfficiencyTool> m_btageffTool;

};

#endif // CPBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H
