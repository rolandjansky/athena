// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITAUONSELECTIONTOOL_H
#define ITAUONSELECTIONTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
  report any issues on JIRA: https://its.cern.ch/jira/browse/TAUAT/?selectedTab=com.atlassian.jira.jira-projects-plugin:issues-panel
*/

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

// EDM include(s):
#ifndef __MAKECINT__
#include "xAODTau/TauJet.h"
#endif // __MAKECINT__

// ROOT include(s):
#include "TFile.h"

namespace TauAnalysisTools
{

/// Interface for tau selector tool
///
class ITauSelectionTool : public virtual asg::IAsgTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( TauAnalysisTools::ITauSelectionTool )

public:
  /// Function initialising the tool
  virtual StatusCode initialize() = 0;

  /// Function initialising the tool
  virtual StatusCode initializeEvent() = 0;
  
  /// Get an object describing the "selection steps" of the tool
  virtual const Root::TAccept& getTAccept() const = 0;

  /// Get the decision using a generic IParticle pointer
  virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const = 0;

  /// Get the decision for a specific TauJet object
  virtual const Root::TAccept& accept( const xAOD::TauJet& tau ) const = 0;

  // Set default recommended properties
  virtual void setRecommendedProperties() __attribute__ ((deprecated("This function is deprecated. Recommended properties are set now by default.\nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauSelectionTool.rst"))) = 0;

  /// Set output file for histograms
  virtual void setOutFile( TFile* fOutFile ) = 0;

  /// Write control histograms to output file
  virtual void writeControlHistograms() = 0;

}; // class ITauSelectionTool

} // namespace TauAnalysisTools

#endif // ITAUONSELECTIONTOOL_H
