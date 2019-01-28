// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_ITAUONSELECTIONTOOL_H
#define TAUANALYSISTOOLS_ITAUONSELECTIONTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

// EDM include(s):
#include "xAODTau/TauJet.h"

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
  virtual StatusCode initializeEvent() __attribute__ ((deprecated("This function is deprecated. Please remove it from your code.\nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauSelectionTool.rst"))) = 0;

  /// Get an object describing the "selection steps" of the tool
  virtual const asg::AcceptInfo& getAcceptInfo() const = 0;

  /// Get the decision using a generic IParticle pointer
  virtual asg::AcceptData accept( const xAOD::IParticle* p ) const = 0;

  /// Get the decision for a specific TauJet object
  virtual asg::AcceptData accept( const xAOD::TauJet& tau ) const = 0;

  /// Set output file for histograms
  virtual void setOutFile( TFile* fOutFile ) = 0;

  /// Write control histograms to output file
  virtual void writeControlHistograms() = 0;

  /// Call this for each new event.
  virtual StatusCode beginEvent() __attribute__ ((deprecated("This function is deprecated and will be removed in the future without further notice. Please remove it from your code."))) = 0;

}; // class ITauSelectionTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_ITAUONSELECTIONTOOL_H
