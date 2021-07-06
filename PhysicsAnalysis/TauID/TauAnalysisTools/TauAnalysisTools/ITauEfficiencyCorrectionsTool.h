/**
 * @file ITauEfficiencyCorrectionsTool.h
 * @author Guillermo Hamity (ghamity@cern.ch)
 * @author Dirk Duschinger
 * @brief 
 * @date 2021-02-19
 * 
 * @copyright Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 * 
 */

#ifndef TAUANALYSISTOOLS_ITAUEFFICIENCYTOOL_H
#define TAUANALYSISTOOLS_ITAUEFFICIENCYTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTau/TauJet.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

// Local include(s):

namespace TauAnalysisTools
{

class ITauEfficiencyCorrectionsTool
  : public virtual asg::IAsgTool
  , public virtual CP::ISystematicsTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( TauAnalysisTools::ITauEfficiencyCorrectionsTool )

public:
  /** Get the "tau efficiency" as a return value*/
  virtual CP::CorrectionCode getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
      double& eff, unsigned int iRunNumber = 0, unsigned int iMu = 0 ) = 0;

  /** Decorate the tau with its efficiency*/
  virtual CP::CorrectionCode applyEfficiencyScaleFactor( const xAOD::TauJet& xTau,
      unsigned int iRunNumber = 0, unsigned int iMu = 0 ) = 0;
  /** check if run number is supperted in recommendations */
  virtual bool isSupportedRunNumber(int iRunNumber) = 0;

}; // class ITauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_ITAUEFFICIENCYTOOL_H
