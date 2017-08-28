// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUEFFICIENCYCONTJETIDTOOL_H
#define TAUANALYSISTOOLS_TAUEFFICIENCYCONTJETIDTOOL_H

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
#include "TauAnalysisTools/CommonEfficiencyTool.h"

namespace TauAnalysisTools
{

class TauEfficiencyContJetIDTool : public CommonEfficiencyTool
{
  ASG_TOOL_CLASS( TauEfficiencyContJetIDTool,
                  TauAnalysisTools::ITauEfficiencyCorrectionsTool )

public:

  TauEfficiencyContJetIDTool(std::string sName);

  ~TauEfficiencyContJetIDTool();

  StatusCode initialize();

  CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
      double& dEfficiencyScaleFactor);
  CP::CorrectionCode applyEfficiencyScaleFactor(const xAOD::TauJet& xTau);

  bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;
  CP::SystematicSet affectingSystematics() const;
  CP::SystematicSet recommendedSystematics() const;
  CP::SystematicCode applySystematicVariation ( const CP::SystematicSet& sSystematicSet);


private:
  std::string ConvertEtaToString(const float& fEta);

  CP::CorrectionCode getTotalSF(const xAOD::TauJet& xTau,
                                double& dEfficiencyScaleFactor);
  CP::CorrectionCode getTotalSFInclusiveEta(const xAOD::TauJet& xTau,
      double& dEfficiencyScaleFactor);

  CP::CorrectionCode getBDTbinnedSF(double& val,
                                    int prongness,
                                    const double& BDT);
  CP::CorrectionCode getBDTbinnedSF(double& val,
                                    int prongness,
                                    const double& eta,
                                    const double& BDT);

  CP::CorrectionCode getBDTbinnedSFSysUnc(double& val,
                                          int prongness,
                                          const double& BDT);
  CP::CorrectionCode getBDTbinnedSFSysUnc(double& val,
                                          int prongness,
                                          const double& eta,
                                          const double& BDT);
  CP::CorrectionCode getBDTbinnedSFStatUnc(double& val,
      int prongness,
      const double& BDT);
  CP::CorrectionCode getBDTbinnedSFStatUnc(double& val,
      int prongness,
      const double& eta,
      const double& BDT);

  CP::CorrectionCode GetIDValue(double& val,
                                const std::string& sWorkingPoint,
                                const float& fBDT);

  std::string ConvertSystematicToString(int iContSysType);
  std::string ConvertDirectionToString(const int iSysDirection);

  // up/down direction
  int m_iSysDirection;

};
} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUEFFICIENCYCONTJETIDTOOL_H
