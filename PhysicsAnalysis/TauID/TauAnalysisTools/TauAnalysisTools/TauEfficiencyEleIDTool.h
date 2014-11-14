// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUEFFICIENCYELEIDTOOL_H
#define TAUEFFICIENCYELEIDTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Local include(s):
#include "CommonEfficiencyTool.h"

namespace TauAnalysisTools
{

class TauEfficiencyEleIDTool : public CommonEfficiencyTool
{
  ASG_TOOL_CLASS( TauEfficiencyEleIDTool, TauAnalysisTools::ITauEfficiencyCorrectionsTool )

public:

  TauEfficiencyEleIDTool(std::string sName);

  virtual ~TauEfficiencyEleIDTool();

  virtual StatusCode initialize();

  CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
      double& dEfficiencyScaleFactor);
  CP::CorrectionCode applyEfficiencyScaleFactor(const xAOD::TauJet& xTau);

  CP::CorrectionCode getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau,
      double& dEfficiencyScaleFactorStatUnc);
  CP::CorrectionCode applyEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau);
  CP::CorrectionCode getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau,
      double& dEfficiencyScaleFactorSysUnc);
  CP::CorrectionCode applyEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau);

  bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;
  CP::SystematicSet affectingSystematics() const;
  CP::SystematicSet recommendedSystematics() const;
  CP::SystematicCode applySystematicVariation ( const CP::SystematicSet& sSystematicSet);

private:

  CP::CorrectionCode GetEVetoSF(double& dVal,
                                float fTrackEta,
                                int iNTrack);

  CP::CorrectionCode GetEVetoSFUnc(double& dVal,
                                   float fTrackEta,
                                   int iNTrack);
  CP::CorrectionCode GetIDValue(double& dVal,
                                const std::string& sWorkingPoint,
                                const float& fPt);

  void setupWorkingPointSubstrings();
  std::string ConvertIDToString(int iLevel);
  std::string ConvertEVetoToString(int iLevel);
  std::string ConvertOLRToString(int iLevel);

  float checkTrackEtaValidity(float& fTrackEta);
  double getLeadTrackEta(const xAOD::TauJet* xTau);

  float GetIDValue(const std::string& sWorkingPoint, const float& fPt);


  // string configs
  std::string m_sIDLevel;
  std::string m_sEVLevel;
  std::string m_sOLRLevel;

  // string configs for 3p eveto
  std::string m_sIDLevel_eveto3p;
  std::string m_sEVLevel_eveto3p;
  std::string m_sOLRLevel_eveto3p;

  // declaration of the working point
  std::string m_sWorkingPoint;

  // up/down direction
  int m_iSysDirection;

};
} // namespace TauAnalysisTools

#endif // TAUEFFICIENCYELEIDTOOL_H
