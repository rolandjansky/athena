/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_COMMONDITAUSMEARINGTOOL_H
#define TAUANALYSISTOOLS_COMMONDITAUSMEARINGTOOL_H

/*
  author: David Kirchmeier
  mail: david.kirchmeier@cern.ch
  documentation in: ../doc/README-DiTauSmearingTool.rst
*/

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AnaToolHandle.h"

// EDM include(s):
#include "xAODTau/DiTauJet.h"
#include "PATInterfaces/CorrectionCode.h"

// Local include(s):
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/IDiTauSmearingTool.h"
#include "TauAnalysisTools/CommonSmearingTool.h"
#include "TauAnalysisTools/CommonDiTauEfficiencyTool.h"
#include "TauAnalysisTools/HelperFunctions.h"

namespace TauAnalysisTools
{

class CommonDiTauSmearingTool
  : public CommonSmearingTool
  , public virtual IDiTauSmearingTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( CommonDiTauSmearingTool, TauAnalysisTools::IDiTauSmearingTool )

public:

  CommonDiTauSmearingTool(std::string sName);

  ~CommonDiTauSmearingTool();

  virtual StatusCode initialize() override;

  /// Apply the correction on a modifiable object
  using CommonSmearingTool::applyCorrection;
  virtual CP::CorrectionCode applyCorrection( xAOD::DiTauJet& xDiTau ) override;
  /// Create a corrected copy from a constant ditau
  using CommonSmearingTool::correctedCopy;
  virtual CP::CorrectionCode correctedCopy( const xAOD::DiTauJet& xDiTau,
      xAOD::DiTauJet*& xDiTauCopy) override;


protected:

  double (*m_fX)(const xAOD::DiTauJet& xDiTau);
  double (*m_fY)(const xAOD::DiTauJet& xDiTau);
  double (*m_fZ)(const xAOD::DiTauJet& xDiTau);

  template<class T>
  void ReadInputs(TFile& fFile, std::map<std::string, T>* mMap);
  using CommonSmearingTool::getValue;
  virtual CP::CorrectionCode getValue(const std::string& sHistName,
                                      const xAOD::DiTauJet& xDiTau,
                                      double& dCorrectionFactor) const;
  e_TruthMatchedParticleType checkTruthMatch(const xAOD::DiTauJet& xDiTau) const;
  void generateSystematicSets();

};
} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_COMMONEFFICIENCYTOOL_H
