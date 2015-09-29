// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITAUOVERLAPPINGELECTRONLLHDECORATOR_H
#define ITAUOVERLAPPINGELECTRONLLHDECORATOR_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  report any issues on JIRA: https://its.cern.ch/jira/browse/TAUAT/?selectedTab=com.atlassian.jira.jira-projects-plugin:issues-panel
*/

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTau/TauJet.h"
#include "PATInterfaces/CorrectionCode.h"

namespace TauAnalysisTools
{

class ITauOverlappingElectronLLHDecorator : public virtual asg::IAsgTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( TauAnalysisTools::ITauOverlappingElectronLLHDecorator )

public:

  /// Function initialising the tool
  virtual StatusCode initialize() = 0;

  virtual StatusCode initializeEvent() = 0;

  virtual StatusCode decorate(const xAOD::TauJet& xTau) = 0;

}; // class ITauOverlappingElectronLLHDecorator

} // namespace TauAnalysisTools

#endif // ITAUOVERLAPPINGELECTRONLLHDECORATOR_H
