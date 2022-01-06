/**
 * 
 * @copyright Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 * 
 * @file ITauEfficiencyCorrectionsTool.h
 * @author Guillermo Hamity (ghamity@cern.ch)
 * @author Dirk Duschinger
 * @brief 
 * @date 2021-02-19
 */

#ifndef TAUANALYSISTOOLS_ITAUEFFICIENCYCORRECTIONSTOOL_H
#define TAUANALYSISTOOLS_ITAUEFFICIENCYCORRECTIONSTOOL_H

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
  /** Get the tau efficiency scale factor */
  virtual CP::CorrectionCode getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
      double& eff, unsigned int iRunNumber = 0, unsigned int iMu = 0 ) = 0;

  /** Decorate the tau with its efficiency*/
  virtual CP::CorrectionCode applyEfficiencyScaleFactor( const xAOD::TauJet& xTau,
      unsigned int iRunNumber = 0, unsigned int iMu = 0 ) = 0;

  /** check if run number is supported in recommendations */
  virtual bool isSupportedRunNumber(int iRunNumber) const = 0;

}; // class ITauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_ITAUEFFICIENCYCORRECTIONSTOOL_H
