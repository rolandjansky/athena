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

  std::vector<std::string> getListOfOriginalNuisanceParameters() const;
  std::vector<float> getCoefficients(const std::string & ev_name) const;


  StatusCode initialize();

  CP::SystematicSet affectingSystematics() const;
  CP::SystematicCode applySystematicVariation( const CP::SystematicSet & systConfig);
  CP::SystematicSet recommendedSystematics() const;
  bool isAffectedBySystematic( const CP::SystematicVariation & systematic ) const;




 private:

  ToolHandle<IBTaggingEfficiencyTool> m_btageffTool;

};

#endif // CPBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H
