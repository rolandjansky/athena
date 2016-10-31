// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_IDITAUIDVARCALCULATOR_H
#define TAURECTOOLS_IDITAUIDVARCALCULATOR_H

/**
 * @brief Tool to calculate and decorate ID variables for boosted di-tau candidates.
 * 
 * @author David Kirchmeier (david.kirchmeier@cern.ch)
 *                                                                              
 */
 
// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTau/DiTauJet.h"

namespace tauRecTools
{

class IDiTauIDVarCalculator :
  public virtual asg::IAsgTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( tauRecTools::IDiTauIDVarCalculator )

public:
  // initialize the tool
  virtual StatusCode initialize() = 0;

  // set pointer to event
  virtual StatusCode initializeEvent() = 0;

  // calculate ID variables
  virtual StatusCode calculateIDVariables(const xAOD::DiTauJet& xDiTau) = 0;



}; // class IDiTauIDVarCalculator

} // namespace DiTauID

#endif // TAURECTOOLS_IDITAUIDVARCALCULATOR_H


