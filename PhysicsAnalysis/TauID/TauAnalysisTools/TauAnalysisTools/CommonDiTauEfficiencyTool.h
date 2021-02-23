/**
 * @file CommonDiTauEfficiencyTool.h
 * @author David Kirchmeier
 * @author Guillermo Hamity (ghamity@cern.ch)
 * @brief 
 * @date 2021-02-18
 * 
 * @copyright Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 * 
 */


#ifndef TAUANALYSISTOOLS_COMMONDITAUEFFICIENCYTOOL_H
#define TAUANALYSISTOOLS_COMMONDITAUEFFICIENCYTOOL_H


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
/** return the truth vis pT of the leading pT matched particle.*/
double TruthLeadPt(const xAOD::DiTauJet& xDiTau);
/** return the truth vis pT of the subleading pT matched particle.*/
double TruthSubleadPt(const xAOD::DiTauJet& xDiTau);
/** return the dR of between the leading and subleading pT matched particle.*/
double TruthDeltaR(const xAOD::DiTauJet& xDiTau);

class CommonDiTauEfficiencyTool
  : public CommonEfficiencyTool
  , public virtual IDiTauEfficiencyCorrectionsTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( CommonDiTauEfficiencyTool, TauAnalysisTools::IDiTauEfficiencyCorrectionsTool )

public:

  CommonDiTauEfficiencyTool(const std::string& sName);

  ~CommonDiTauEfficiencyTool();

  virtual StatusCode initialize() override;

  // next two lines are needed to achieve overloading of those methods 
  using CommonEfficiencyTool::getEfficiencyScaleFactor;
  using CommonEfficiencyTool::applyEfficiencyScaleFactor;

  /**
   * @brief Get the Efficiency Scale Factor of ditau jet
   * 
   * @param xDiTau : reco DiTauJet
   * @param dEfficiencyScaleFactor : reference to output variable where efficiency is returned
   * @param iRunNumber : run number
   * @param iMu : number of interactions
   * @return CP::CorrectionCode 
   */
  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::DiTauJet& xDiTau, double& dEfficiencyScaleFactor, 
    unsigned int iRunNumber = 0, unsigned int iMu = 0 ) override;

  /**
   * @brief Get the Efficiency Scale Factor of ditau jet
   * 
   * @param xDiTau 
   * @param iRunNumber 
   * @param iMu 
   * @return CP::CorrectionCode 
   */
  virtual CP::CorrectionCode applyEfficiencyScaleFactor(const xAOD::DiTauJet& xDiTau,
    unsigned int iRunNumber = 0, unsigned int iMu = 0 ) override;

  /** scale factor bin x (e.g. lead match pT)*/
  double (*m_fX)(const xAOD::DiTauJet& xDiTau);
  /** scale factor bin y (e.g. sublead match pT)*/
  double (*m_fY)(const xAOD::DiTauJet& xDiTau);
  /** scale factor bin z (e.g. dR match particles)*/
  double (*m_fZ)(const xAOD::DiTauJet& xDiTau);

  void ReadInputs(std::unique_ptr<TFile> &fFile);

  using CommonEfficiencyTool::getValue;
  /**
   * @brief Get the scale factor from a particular recommendations histogram.
   * 
   * @param sHistName 
   * @param xDiTau 
   * @param dEfficiencyScaleFactor 
   * @return CP::CorrectionCode 
   */
  CP::CorrectionCode getValue(const std::string& sHistName,
                              const xAOD::DiTauJet& xDiTau,
                              double& dEfficiencyScaleFactor) const;

  /** generate a set of relevant systematic variations to be applied*/
  void generateSystematicSets();
  /** true if scale factor name is already decorated*/ 
  bool m_bSFIsAvailable;
  /** true if cale factor name is already decorated has already been checked*/
  bool m_bSFIsAvailableChecked;

};
} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_COMMONDITAUEFFICIENCYTOOL_H
