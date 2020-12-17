// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_ITAUONSELECTIONTOOL_H
#define TAUANALYSISTOOLS_ITAUONSELECTIONTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauSelectionTool.rst
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
}; // class ITauSelectionTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_ITAUONSELECTIONTOOL_H
