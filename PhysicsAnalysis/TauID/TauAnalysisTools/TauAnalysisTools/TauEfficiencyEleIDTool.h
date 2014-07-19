/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef DEVEL
#ifndef TAUEFFICIENCYELEIDTOOL_H
#define TAUEFFICIENCYELEIDTOOL_H

// Local include(s):
#include "CommonEfficiencyTool.h"

namespace TauAnalysisTools {
  
  class TauEfficiencyEleIDTool : public CommonEfficiencyTool
  {
    ASG_TOOL_CLASS( TauEfficiencyEleIDTool, TauAnalysisTools::TauEfficiencyEleIDTool )

    public:

    TauEfficiencyEleIDTool(EfficiencyCorrectionType eEfficiencyCorrectionType,
			   const std::string& sSharepath,
			   bool bDebug=false);

    ~TauEfficiencyEleIDTool()
    {
    }

    void configure(int iConfig1, int iConfig2 = 0, int iConfig3 = 0);

    TauAnalysisTools::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& xTau, double& efficiencyScaleFactor);
    TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactor(xAOD::TauJet& xTau);

    TauAnalysisTools::CorrectionCode getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau, double& efficiencyScaleFactorStatUnc, int iSysType = 0);
    TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactorStatUnc(xAOD::TauJet& xTau, int iSysType = 0);
    TauAnalysisTools::CorrectionCode getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau, double& efficiencyScaleFactorSysUnc, int iSysType = 0);
    TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactorSysUnc(xAOD::TauJet& xTau, int iSysType = 0);

  private:
    
    TauAnalysisTools::CorrectionCode GetEVetoSF(double& val,
				     float trackEta,
				     int nTrack);

    TauAnalysisTools::CorrectionCode GetEVetoSFUnc(double& val,
					float trackEta,
					int nTrack,
					int direction);
    TauAnalysisTools::CorrectionCode GetIDValue(double& val,
				     const std::string& sWorkingPoint,
				     const float& fPt);
    
    std::string ConvertIDToString(const JETID& level);
    std::string ConvertEVetoToString(const EVETO& level);
    std::string ConvertOLRToString(const OLR& level);

    float checkTrackEtaValidity(float& trackEta);
    double getLeadTrackEta(const xAOD::TauJet* xTau);

    float GetIDValue(const std::string& sWorkingPoint, const float& fPt);

    std::string getWorkingPoint(const xAOD::TauJet& tau);
    
    // enum configs
    JETID m_eIDLevel;
    EVETO m_eEVLevel;
    OLR m_eOLR;

    // string configs 
    std::string m_sIDLevel;
    std::string m_sEVLevel;
    std::string m_sOLR;

    // string configs for 3p eveto
    std::string m_sIDLevel_eveto3p;
    std::string m_sEVLevel_eveto3p;
    std::string m_sOLR_eveto3p;
    
    // declaration of the working point
    std::string m_sWorkingPoint;

  };
} // namespace TauAnalysisTools

#endif // TAUEFFICIENCYELEIDTOOL_H
#endif
