/**
 * @file IDiTauTruthMatchingTool.h
 * @author Guillermo Hamity (ghamity@cern.ch)
 * @author David Kirchmeier
 * @brief Tau, lepton and jet truth matching for ditau jets
 * @date 2021-02-18
 * 
 * @copyright Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 * 
 */
// Dear emacs, this is -*- c++ -*-


#ifndef TAUANALYSISTOOLS_IDITAUTRUTHMATCHINGTOOL_H
#define TAUANALYSISTOOLS_IDITAUTRUTHMATCHINGTOOL_H


// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTau/DiTauJet.h"

// // local include(s)
#include "IBuildTruthTaus.h"

// local include(s)
#include "TauAnalysisTools/Enums.h"

namespace TauAnalysisTools
{

class IDiTauTruthMatchingTool
  // The order matters, do not switch them !!!
  : public virtual TauAnalysisTools::IBuildTruthTaus
  , public virtual asg::IAsgTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( TauAnalysisTools::IDiTauTruthMatchingTool )

public:
  /** initialize the tool*/
  virtual StatusCode initialize() = 0;

  /** apply match to a single ditau jet*/
  virtual void applyTruthMatch(const xAOD::DiTauJet& xDiTau) = 0;

  /** apply match to all ditaus in a vector*/
  virtual void applyTruthMatch(const std::vector<const xAOD::DiTauJet*>& vDiTaus) = 0;

  /** get pointer to truth tau, if no truth tau was found a null pointer is returned*/
  virtual void getTruth(const xAOD::DiTauJet& xDiTau) = 0;

}; // class IDiTauTruthMatchingTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_IDITAUTRUTHMATCHINGTOOL_H
