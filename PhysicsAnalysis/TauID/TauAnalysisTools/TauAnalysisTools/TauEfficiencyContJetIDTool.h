/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ATHENADEV
#ifdef DEVEL
#ifndef TAUEFFICIENCYCONTJETIDTOOL_H
#define TAUEFFICIENCYCONTJETIDTOOL_H

#include "CommonEfficiencyTool.h"

namespace TauAnalysisTools {

  class TauEfficiencyContJetIDTool : public CommonEfficiencyTool
  {
    ASG_TOOL_CLASS( TauEfficiencyContJetIDTool, TauAnalysisTools::TauEfficiencyContJetIDTool )
    
    public:
    
    TauEfficiencyContJetIDTool(std::string sName, PropertyMgr* pPropertyMgr);

    ~TauEfficiencyContJetIDTool(){}

    void configure(PropertyMgr* pPropertyMgr);

    TauAnalysisTools::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
							      double& dEfficiencyScaleFactor);
    TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactor(xAOD::TauJet& xTau);
    
    TauAnalysisTools::CorrectionCode getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau,
								     double& dEfficiencyScaleFactorStatUnc);
    TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactorStatUnc(xAOD::TauJet& xTau);
    TauAnalysisTools::CorrectionCode getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau,
								    double& dEfficiencyScaleFactorSysUnc);
    TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactorSysUnc(xAOD::TauJet& xTau);


  private:
    bool m_bUseInclusiveEta;
    std::string ConvertEtaToString(const float& fEta);

    TauAnalysisTools::CorrectionCode getBDTbinnedSF(double& val,
					 int prongness,
					 const double& BDT);
    TauAnalysisTools::CorrectionCode getBDTbinnedSF(double& val,
					 int prongness,
					 const double& eta,
					 const double& BDT);
    
    TauAnalysisTools::CorrectionCode getBDTbinnedSFSysUnc(double& val,
							  int prongness,
							  const double& BDT,
							  CONTIDSYS sys);
    TauAnalysisTools::CorrectionCode getBDTbinnedSFSysUnc(double& val,
							  int prongness,
							  const double& eta,
							  const double& BDT,
							  CONTIDSYS sys);
    TauAnalysisTools::CorrectionCode getBDTbinnedSFStatUnc(double& val,
							   int prongness,
							   const double& BDT);
    TauAnalysisTools::CorrectionCode getBDTbinnedSFStatUnc(double& val,
							   int prongness,
							   const double& eta,
							   const double& BDT);
    
    TauAnalysisTools::CorrectionCode GetIDValue(double& val,
				     const std::string& sWorkingPoint,
				     const float& fBDT);
    
    std::string ConvertSystematicToString(CONTIDSYS sys);
    std::string ConvertDirectionToString(const int direction);
  };
} // namespace TauCorrUncert

#endif // TAUEFFICIENCYCONTJETIDTOOL_H
#endif
#endif
