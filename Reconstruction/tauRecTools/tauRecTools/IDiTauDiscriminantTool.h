// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_DITAUDISCRIMINANTTOOL_H
#define TAURECTOOLS_DITAUDISCRIMINANTTOOL_H

/**
 * @brief Implementation of boosted di-tau ID.
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

class IDiTauDiscriminantTool :
  public virtual asg::IAsgTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( tauRecTools::IDiTauDiscriminantTool )

public:
  // initialize the tool
  virtual StatusCode initialize() = 0;

  // set pointer to event
  virtual StatusCode initializeEvent() = 0;

  // calculate ID variables
  virtual double getJetBDTScore(const xAOD::DiTauJet& xDiTau) = 0;

}; // class IDiTauDiscriminantTool

} // namespace tauRecTools

#endif // TAURECTOOLS_DITAUDISCRIMINANTTOOL_H


