// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITAUEFFICIENCYTOOL_H
#define ITAUEFFICIENCYTOOL_H

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
#include "PATInterfaces/ISystematicsTool.h"

// Local include(s):

namespace TauAnalysisTools
{

class ITauEfficiencyCorrectionsTool : public virtual asg::IAsgTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( TauAnalysisTools::ITauEfficiencyCorrectionsTool )

public:
  /// Function initialising the tool
  virtual StatusCode initialize() = 0;

  /// Get the "tau efficiency" as a return value
  virtual CP::CorrectionCode getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
      double& eff ) = 0;

  /// Decorate the tau with its efficiency
  virtual CP::CorrectionCode applyEfficiencyScaleFactor( const xAOD::TauJet& xTau ) = 0;

  /// Get the "tau efficiency stat uncertainty" as a return value
  virtual CP::CorrectionCode getEfficiencyScaleFactorStatUnc( const xAOD::TauJet& xTau,
      double& eff ) = 0;

  /// Decorate the tau with its efficiency stat uncertainty
  virtual CP::CorrectionCode applyEfficiencyScaleFactorStatUnc( const xAOD::TauJet& xTau ) = 0;

  /// Get the "tau efficiency sys uncertainty" as a return value
  virtual CP::CorrectionCode getEfficiencyScaleFactorSysUnc( const xAOD::TauJet& xTau,
      double& eff ) = 0;

  /// Decorate the tau with its efficiency sys uncertainty
  virtual CP::CorrectionCode applyEfficiencyScaleFactorSysUnc( const xAOD::TauJet& xTau ) = 0;

  /// returns: whether this tool is affected by the given systematis
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const = 0;

  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const = 0;

  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const = 0;

  virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& systConfig ) = 0;


}; // class ITauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // ITAUEFFICIENCYTOOL_H
