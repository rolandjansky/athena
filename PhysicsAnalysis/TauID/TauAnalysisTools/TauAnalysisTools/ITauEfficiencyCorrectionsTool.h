// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITAUEFFICIENCYTOOL_H
#define ITAUEFFICIENCYTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTau/TauJet.h"

// Local include(s):
#include "TauAnalysisTools/CorrectionCode.h"

namespace TauAnalysisTools {

  class ITauEfficiencyCorrectionsTool : public virtual asg::IAsgTool {

    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( TauAnalysisTools::ITauEfficiencyCorrectionsTool )

    public:
    /// Get the "tau efficiency" as a return value
    virtual CorrectionCode getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
						     double& eff ) = 0;
    /// Decorate the tau with its efficiency
    virtual CorrectionCode applyEfficiencyScaleFactor( xAOD::TauJet& xTau ) = 0;

    /// Get the "tau efficiency stat uncertainty" as a return value
    virtual CorrectionCode getEfficiencyScaleFactorStatUnc( const xAOD::TauJet& xTau,
    							    double& eff ) = 0;
    /// Decorate the tau with its efficiency stat uncertainty
    virtual CorrectionCode applyEfficiencyScaleFactorStatUnc( xAOD::TauJet& xTau ) = 0;

    /// Get the "tau efficiency sys uncertainty" as a return value
    virtual CorrectionCode getEfficiencyScaleFactorSysUnc( const xAOD::TauJet& xTau,
    							   double& eff ) = 0;
    /// Decorate the tau with its efficiency sys uncertainty
    virtual CorrectionCode applyEfficiencyScaleFactorSysUnc( xAOD::TauJet& xTau ) = 0;

  }; // class ITauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // ITAUEFFICIENCYTOOL_H
