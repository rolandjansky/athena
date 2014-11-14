// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUEFFICIENCYJETIDTOOL_H
#define TAUEFFICIENCYJETIDTOOL_H

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

class TauEfficiencyJetIDTool : public CommonEfficiencyTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauEfficiencyJetIDTool, TauAnalysisTools::ITauEfficiencyCorrectionsTool )

public:

  TauEfficiencyJetIDTool(std::string sName);

  virtual ~TauEfficiencyJetIDTool();

  virtual StatusCode initialize();

  CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
      double& efficiencyScaleFactor);
  CP::CorrectionCode applyEfficiencyScaleFactor(const xAOD::TauJet& xTau);

  CP::CorrectionCode getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau,
      double& efficiencyScaleFactor);
  CP::CorrectionCode applyEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau);
  CP::CorrectionCode getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau,
      double& efficiencyScaleFactor);
  CP::CorrectionCode applyEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau);

  bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;
  CP::SystematicSet affectingSystematics() const;
  CP::SystematicSet recommendedSystematics() const;
  CP::SystematicCode applySystematicVariation ( const CP::SystematicSet& sSystematicSet);

private:

  std::string ConvertEtaToString(const float& fEta);

  CP::CorrectionCode getTotalSF(const xAOD::TauJet& xTau,
                                double& dEfficiencyScaleFactor);
  CP::CorrectionCode getExclTotalSF(const xAOD::TauJet& xTau,
                                    double& dEfficiencyScaleFactor);

  CP::CorrectionCode GetIDSF(double& val,
                             double eta,
                             int prongness,
                             double pT=30);

  CP::CorrectionCode GetIDSysUnc(double& val,
                                 double eta,
                                 int prongness,
                                 double pT=30);

  CP::CorrectionCode GetIDStatUnc(double& val,
                                  double eta,
                                  int prongness,
                                  double pT=30);

  CP::CorrectionCode GetExclIDSF(double& val,
                                 double eta,
                                 int prongness,
                                 double pT=30);

  CP::CorrectionCode GetExclIDSysUnc(double& val,
                                     double eta,
                                     int prongness,
                                     double pT=30);

  CP::CorrectionCode GetExclIDStatUnc(double& val,
                                      double eta,
                                      int prongness,
                                      double pT=30);

  std::string ConvertIDToString(const int& iLevel);

  CP::CorrectionCode GetIDValue(double& val,
                                const std::string& sWorkingPoint,
                                const float& fPt);

  std::string getWorkingPoint(const xAOD::TauJet& xTau);

  void SwitchOnPtBinnedIDSF(bool bSwitch);

  // string configs
  std::string m_sIDLevel;
  std::string m_sIDpt;

  // declaration of the working point
  std::string m_sWorkingPoint;

};
} // namespace TauAnalysisTools

#endif // TAUEFFICIENCYJETIDTOOL_H
