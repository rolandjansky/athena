/**
 * @file IDiTauEfficiencyCorrectionsTool.h
 * @author David Kirchmeier
 * @author Guillermo Hamity (ghamity@cern.ch)
 * @brief recommended ditau efficiency correction scale factors and uncertainties
 * @date 2021-02-18
 * 
 * @copyright Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 * 
 */

// Dear emacs, this is -*- c++ -*-

#ifndef TAUANALYSISTOOLS_IDITAUEFFICIENCYTOOL_H
#define TAUANALYSISTOOLS_IDITAUEFFICIENCYTOOL_H


// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTau/DiTauJet.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

// Local include(s):

namespace TauAnalysisTools
{

class IDiTauEfficiencyCorrectionsTool
  : public virtual asg::IAsgTool
  , public virtual CP::ISystematicsTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( TauAnalysisTools::IDiTauEfficiencyCorrectionsTool )

public:
  /**
   * @brief Get the ditau Efficiency Scale Factor as a return value
   * 
   * @param xDiTau reco DiTauJet
   * @param eff reference to output variable where efficiency is returned
   * @param iRunNumber run number 
   * @param iMu number of interactions
   * @return CP::CorrectionCode 
   */
  virtual CP::CorrectionCode getEfficiencyScaleFactor( const xAOD::DiTauJet& xDiTau,
      double& eff, unsigned int iRunNumber = 0, unsigned int iMu = 0 ) = 0;

  /**
   * @brief Decorate the ditau Efficiency Scale Factor
   * 
   * @param xDiTau reco DiTauJet
   * @param iRunNumber run number 
   * @param iMu number of interactions
   * @return CP::CorrectionCode 
   */

  virtual CP::CorrectionCode applyEfficiencyScaleFactor( const xAOD::DiTauJet& xDiTau,
    unsigned int iRunNumber = 0, unsigned int iMu = 0) = 0;

}; // class IDiTauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_IDITAUEFFICIENCYTOOL_H
