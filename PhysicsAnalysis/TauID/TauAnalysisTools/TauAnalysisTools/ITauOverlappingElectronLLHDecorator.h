// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_ITAUOVERLAPPINGELECTRONLLHDECORATOR_H
#define TAUANALYSISTOOLS_ITAUOVERLAPPINGELECTRONLLHDECORATOR_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauOverlappingElectronLLHDecorator.rst
                    
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

  virtual StatusCode decorate(const xAOD::TauJet& xTau) const = 0;
  virtual StatusCode setEleOlrPassDecorationName(const std::string& name) = 0;
  virtual StatusCode setEleOlrLhScoreDecorationName(const std::string& name) = 0;

}; // class ITauOverlappingElectronLLHDecorator

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_ITAUOVERLAPPINGELECTRONLLHDECORATOR_H
