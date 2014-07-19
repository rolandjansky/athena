// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUEFFICIENCYTOOL_H
#define TAUEFFICIENCYTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Local include(s):
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/CommonEfficiencyTool.h"
#include "TauAnalysisTools/TauEfficiencyJetIDTool.h"
#include "TauAnalysisTools/TauEfficiencyContJetIDTool.h"
#include "TauAnalysisTools/TauEfficiencyEleIDTool.h"

namespace TauAnalysisTools {

  class TauEfficiencyCorrectionsTool : public virtual ITauEfficiencyCorrectionsTool,
				       public asg::AsgTool {

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS( TauEfficiencyCorrectionsTool, TauAnalysisTools::ITauEfficiencyCorrectionsTool )

    public:
    /// Create a constructor for standalone usage
    TauEfficiencyCorrectionsTool( const std::string& name );

    /// Function initialising the tool
    virtual StatusCode initialize();

    /// Get the "tau efficiency" as a return value
    virtual CorrectionCode getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
						     double& eff );
    /// Decorate the tau with its efficiency
    virtual CorrectionCode applyEfficiencyScaleFactor( xAOD::TauJet& xTau );

    /// Get the "tau efficiency stat uncertainty" as a return value
    virtual CorrectionCode getEfficiencyScaleFactorStatUnc( const xAOD::TauJet& xTau,
							    double& eff );
    /// Decorate the tau with its efficiency stat uncertainty
    virtual CorrectionCode applyEfficiencyScaleFactorStatUnc( xAOD::TauJet& xTau );

    /// Get the "tau efficiency sys uncertainty" as a return value
    virtual CorrectionCode getEfficiencyScaleFactorSysUnc( const xAOD::TauJet& xTau,
							   double& eff );
    /// Decorate the tau with its efficiency sys uncertainty
    virtual CorrectionCode applyEfficiencyScaleFactorSysUnc( xAOD::TauJet& xTau );

  private:
    /// Some dummy property
    int m_iEfficiencyCorrectionType;
    CommonEfficiencyTool* m_tCommonEfficiencyTool;
    std::string m_sSharepath;
    std::string m_sFileName;
    bool m_bUseIDExclusiveSF;
    bool m_bUseInclusiveEta;
    bool m_bUsePtBinnedSF;
    bool m_bUseHighPtUncert;
    int m_iSysDirection;
    int m_iIDLevel;
  
  }; // class TauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // CPTOOLTESTS_TAUEFFICIENCYTOOL_H
