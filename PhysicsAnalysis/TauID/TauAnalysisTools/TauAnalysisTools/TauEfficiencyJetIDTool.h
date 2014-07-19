/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUEFFICIENCYJETIDTOOL_H 
#define TAUEFFICIENCYJETIDTOOL_H

// Local include(s):
#include "AsgTools/StatusCode.h"
#include "TauAnalysisTools/Enums.h"
#include "CommonEfficiencyTool.h"

namespace TauAnalysisTools {
    
  class TauEfficiencyJetIDTool : public CommonEfficiencyTool
  {
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS( TauEfficiencyJetIDTool, TauAnalysisTools::ITauEfficiencyCorrectionsTool )

    public:
    
    TauEfficiencyJetIDTool(std::string sName);
    
    virtual ~TauEfficiencyJetIDTool()
    {
    }

    StatusCode configure();
    
    StatusCode configure(PropertyMgr* pPropertyMgr);

    TauAnalysisTools::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& xTau, double& efficiencyScaleFactor);
    TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactor(xAOD::TauJet& xTau);

    TauAnalysisTools::CorrectionCode getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau, double& efficiencyScaleFactor);
    TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactorStatUnc(xAOD::TauJet& xTau);
    TauAnalysisTools::CorrectionCode getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau, double& efficiencyScaleFactor);
    TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactorSysUnc(xAOD::TauJet& xTau);

    // virtual bool AlreadyApplied(const xAOD::TauJet& xTau) const = 0;

    
  private:
    
    std::string ConvertEtaToString(const float& fEta);

    TauAnalysisTools::CorrectionCode GetIDSF(double& val,
					     double eta,
					     int prongness,
					     double pT=30);
    
    TauAnalysisTools::CorrectionCode GetIDSysUnc(double& val,
						 double eta,
						 int prongness,
						 double pT=30);
    
    TauAnalysisTools::CorrectionCode GetIDStatUnc(double& val,
						  double eta,
						  int prongness,
						  double pT=30);
    
    TauAnalysisTools::CorrectionCode GetExclIDSF(double& val,
						 double eta,
						 int prongness,
						 double pT=30);
    
    TauAnalysisTools::CorrectionCode GetExclIDSysUnc(double& val,
						     double eta,
						     int prongness,
						     double pT=30);
    
    TauAnalysisTools::CorrectionCode GetExclIDStatUnc(double& val,
						      double eta,
						      int prongness,
						      double pT=30);
    
    std::string ConvertIDToString(const int& iLevel);
    
    TauAnalysisTools::CorrectionCode GetIDValue(double& val,
						const std::string& sWorkingPoint,
						const float& fPt);
    
    std::string getWorkingPoint(const xAOD::TauJet& xTau);

    void SwitchOnPtBinnedIDSF(bool bSwitch);
    
    // bool configs
    bool m_bUseIDExclusiveSF;
    bool m_bUseInclusiveEta;
    bool m_bUsePtBinnedSF;
    bool m_bUseHighPtUncert;

    // string configs
    std::string m_sIDLevel;
    std::string m_sIDpt;

    // enum configs, need to be int to work within athena
    int m_iIDLevel;

    // declaration of the working point
    std::string m_sWorkingPoint;

  };
} // namespace TauAnalysisTools

#endif // TAUEFFICIENCYJETIDTOOL_H
