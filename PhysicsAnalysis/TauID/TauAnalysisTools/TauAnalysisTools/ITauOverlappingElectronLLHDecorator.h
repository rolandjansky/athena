// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_ITAUOVERLAPPINGELECTRONLLHDECORATOR_H
#define TAUANALYSISTOOLS_ITAUOVERLAPPINGELECTRONLLHDECORATOR_H

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

// EDM include(s):
#include "xAODTau/TauJet.h"
#include "PATInterfaces/CorrectionCode.h"

namespace TauAnalysisTools
{

class ITauOverlappingElectronLLHDecorator
  : public virtual asg::IAsgTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( TauAnalysisTools::ITauOverlappingElectronLLHDecorator )

public:

  virtual StatusCode initializeEvent() __attribute__ ((deprecated("This function is deprecated. Please remove it from your code.\nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauOverlappingElectronLLHDecorator.rst"))) = 0;

  virtual StatusCode decorate(const xAOD::TauJet& xTau) const = 0;
  virtual StatusCode setEleOlrPassDecorationName(const std::string& name) = 0;
  virtual StatusCode setEleOlrLhScoreDecorationName(const std::string& name) = 0;

}; // class ITauOverlappingElectronLLHDecorator

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_ITAUOVERLAPPINGELECTRONLLHDECORATOR_H
