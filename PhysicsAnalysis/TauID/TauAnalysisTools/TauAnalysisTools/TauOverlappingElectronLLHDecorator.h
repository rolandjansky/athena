// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUOVERLAPPINGELECTRONLLHDECORATOR_H
#define TAUOVERLAPPINGELECTRONLLHDECORATOR_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  report any issues on JIRA: https://its.cern.ch/jira/browse/TAUAT/?selectedTab=com.atlassian.jira.jira-projects-plugin:issues-panel
*/

// Framework include(s):
#include "AsgTools/AsgTool.h"
// #include "PathResolver/PathResolver.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

// EDM include(s):
#ifndef __MAKECINT__
#include "xAODEgamma/ElectronContainer.h"
#endif // __MAKECINT__

// Local include(s):
#include "TauAnalysisTools/ITauOverlappingElectronLLHDecorator.h"

namespace TauAnalysisTools
{

class TauOverlappingElectronLLHDecorator
  : public virtual ITauOverlappingElectronLLHDecorator
  , public asg::AsgTool
{

  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauOverlappingElectronLLHDecorator,
		  TauAnalysisTools::ITauOverlappingElectronLLHDecorator)

public:
  /// Create a constructor for standalone usage
  TauOverlappingElectronLLHDecorator( const std::string& name );

  ~TauOverlappingElectronLLHDecorator( );

  /// Function initialising the tool
  virtual StatusCode initialize();

  virtual StatusCode initializeEvent();

  virtual StatusCode decorate(const xAOD::TauJet& xTau);

private:
  AsgElectronLikelihoodTool* m_tEMLHTool;
  const xAOD::ElectronContainer* m_xElectronContainer;
  std::string m_sElectronContainerName;
  bool m_bElectonsAvailable;

}; // class TauOverlappingElectronLLHDecorator

} // namespace TauAnalysisTools

#endif // TAUOVERLAPPINGELECTRONLLHDECORATOR_H
