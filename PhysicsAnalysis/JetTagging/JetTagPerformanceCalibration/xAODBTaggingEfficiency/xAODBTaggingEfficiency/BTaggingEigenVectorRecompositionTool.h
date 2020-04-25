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

#include <string>
#include <vector>

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

class BTaggingEigenVectorRecompositionTool: public asg::AsgTool,
            public virtual IBTaggingEigenVectorRecompositionTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS2( BTaggingEigenVectorRecompositionTool, IBTaggingEigenVectorRecompositionTool, ISystematicsTool )

  public:


  /// Create a constructor for standalone usage
  BTaggingEigenVectorRecompositionTool( const std::string& name );

  /// Create a constructor for standalone usage
  virtual ~BTaggingEigenVectorRecompositionTool();

  /* Get list of original nuisance parameters that will linearly combined with the list of 
     coefficients returns below to recompose each eigen vector
 */
  CP::CorrectionCode getListOfOriginalNuisanceParameters( std::vector<std::string> & np_list) const;

  /* Get list of coefficients to be used to build the linear combination 
     FOR NOW THIS IS JUST GOING TO BE PRINTED: need to define how to retrieve in the most convenient way
   */
  CP::CorrectionCode getListOfCoefficients() const;

  StatusCode initialize();


 private:

  ToolHandle<IBTaggingEfficiencyTool> m_btageffTool;

};

#endif // CPBTAGGINGEIGENVECTORRECOMPOSITIONTOOL_H
