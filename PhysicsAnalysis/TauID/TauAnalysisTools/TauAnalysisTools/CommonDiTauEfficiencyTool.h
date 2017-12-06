/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_COMMONDITAUEFFICIENCYTOOL_H
#define TAUANALYSISTOOLS_COMMONDITAUEFFICIENCYTOOL_H

/*
  author: David Kirchmeier
  mail: david.kirchmeier@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
        or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Framework include(s):
#include "AsgTools/AsgTool.h"

// EDM include(s):
#include "xAODTau/DiTauJet.h"

// Local include(s):
#include "TauAnalysisTools/Enums.h"
// #include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/IDiTauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/CommonEfficiencyTool.h"
#include "TauAnalysisTools/HelperFunctions.h"

namespace TauAnalysisTools
{

double TruthLeadPt(const xAOD::DiTauJet& xDiTau);
double TruthSubleadPt(const xAOD::DiTauJet& xDiTau);
double TruthDeltaR(const xAOD::DiTauJet& xDiTau);

class CommonDiTauEfficiencyTool
  : public CommonEfficiencyTool
  , public virtual IDiTauEfficiencyCorrectionsTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( CommonDiTauEfficiencyTool, TauAnalysisTools::IDiTauEfficiencyCorrectionsTool )

public:

  CommonDiTauEfficiencyTool(std::string sName);

  ~CommonDiTauEfficiencyTool();

  virtual StatusCode initialize() override;

  // next two lines are needed to achieve overloading of those methods 
  using CommonEfficiencyTool::getEfficiencyScaleFactor;
  using CommonEfficiencyTool::applyEfficiencyScaleFactor;
  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::DiTauJet& xDiTau, double& dEfficiencyScaleFactor) override;
  virtual CP::CorrectionCode applyEfficiencyScaleFactor(const xAOD::DiTauJet& xDiTau) override;

  double (*m_fX)(const xAOD::DiTauJet& xDiTau);
  double (*m_fY)(const xAOD::DiTauJet& xDiTau);
  double (*m_fZ)(const xAOD::DiTauJet& xDiTau);

  void ReadInputs(std::unique_ptr<TFile> &fFile);

  using CommonEfficiencyTool::getValue;
  CP::CorrectionCode getValue(const std::string& sHistName,
                              const xAOD::DiTauJet& xDiTau,
                              double& dEfficiencyScaleFactor) const;

  e_TruthMatchedParticleType checkTruthMatch(const xAOD::DiTauJet& xDiTau) const;
  void generateSystematicSets();

  bool m_bSFIsAvailable;
  bool m_bSFIsAvailableChecked;

};
} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_COMMONDITAUEFFICIENCYTOOL_H
