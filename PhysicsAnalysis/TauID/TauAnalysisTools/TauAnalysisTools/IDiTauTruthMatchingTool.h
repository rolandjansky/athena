// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_IDITAUTRUTHMATCHINGTOOL_H
#define TAUANALYSISTOOLS_IDITAUTRUTHMATCHINGTOOL_H

/*
  author: David Kirchmeier
  mail: david.kirchmeier@cern.ch
  implementation close to ITauTruthMatchingTool.h
  documentation in: 
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/doc/README-DiTauTruthMatchingTool.rst
        or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-DiTauTruthMatchingTool.rst

*/

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
  // initialize the tool
  virtual StatusCode initialize() = 0;

  // apply match to a single tau
  virtual void applyTruthMatch(const xAOD::DiTauJet& xDiTau) = 0;

  // apply match to all taus in a vector
  virtual void applyTruthMatch(const std::vector<const xAOD::DiTauJet*>& vDiTaus) = 0;

  // get pointer to truth tau, if no truth tau was found a null pointer is returned
  virtual void getTruth(const xAOD::DiTauJet& xDiTau) = 0;

}; // class IDiTauTruthMatchingTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_IDITAUTRUTHMATCHINGTOOL_H
