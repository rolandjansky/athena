// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_IDITAUEFFICIENCYTOOL_H
#define TAUANALYSISTOOLS_IDITAUEFFICIENCYTOOL_H

/*
  author: David Kirchmeier
  mail: david.kirchmeier@cern.ch
  documentation in: 
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/doc/README-DiTauEfficiencyCorrectionsTool.rst
        or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-DiTauEfficiencyCorrectionsTool.rst
*/

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
  /// Get the "tau efficiency" as a return value
  virtual CP::CorrectionCode getEfficiencyScaleFactor( const xAOD::DiTauJet& xDiTau,
      double& eff ) = 0;

  /// Decorate the tau with its efficiency
  virtual CP::CorrectionCode applyEfficiencyScaleFactor( const xAOD::DiTauJet& xDiTau ) = 0;

}; // class IDiTauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_IDITAUEFFICIENCYTOOL_H
