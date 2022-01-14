/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUEFFICIENCYCORRECTIONSTOOL_H
#define TAUANALYSISTOOLS_TAUEFFICIENCYCORRECTIONSTOOL_H

/*
  author: Dirk Duschinger
  maintainer: Guillermo Hamity
  mail: guillermo.nicolas.hamity@cern.ch
  documentation in: ../README.rst
                    https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/TauID/TauAnalysisTools/README.rst
*/

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"

// Local include(s):
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/CommonEfficiencyTool.h"
#include "TauAnalysisTools/TauEfficiencyTriggerTool.h"

// Tool include(s)
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

namespace TauAnalysisTools
{

class TauEfficiencyCorrectionsTool
  : public virtual ITauEfficiencyCorrectionsTool
  , public asg::AsgMetadataTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauEfficiencyCorrectionsTool, TauAnalysisTools::ITauEfficiencyCorrectionsTool )

public:
  /// Create a constructor for standalone usage
  TauEfficiencyCorrectionsTool( const std::string& sName );

  ~TauEfficiencyCorrectionsTool();

  /// Function initialising the tool
  virtual StatusCode initialize();

  virtual StatusCode beginInputFile();

  /// Print tool configuration
  virtual void printConfig() const;

  /// Get the tau efficiency scale factor
  virtual CP::CorrectionCode getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
      double& eff, unsigned int iRunNumber = 0, unsigned int iMu = 0);

  /// Decorate the tau with its efficiency scale factor
  virtual CP::CorrectionCode applyEfficiencyScaleFactor( const xAOD::TauJet& xTau,
      unsigned int iRunNumber = 0, unsigned int iMu = 0);

  /// returns: whether this tool is affected by the given systematics
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;

  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const;

  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const;

  virtual StatusCode applySystematicVariation( const CP::SystematicSet& systConfig );

  virtual bool isSupportedRunNumber( int /*iRunNumber*/ ) const
  {
    return true;
  };

private:
  StatusCode beginEvent();

  std::string ConvertJetIDToString(const int iLevel) const;

  std::string ConvertEleIDToString(const int iLevel) const;

  std::string ConvertTriggerIDToString(const int iLevel) const;

  std::string GetTriggerSFMeasurementString() const;

  StatusCode initializeWithTauSelectionTool(TauSelectionTool* tauSelectionTool);

  StatusCode initializeTools_2019_summer();

  StatusCode readRandomRunNumber();

private:

  std::vector<int> m_vEfficiencyCorrectionTypes;
  std::vector< asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* > m_vCommonEfficiencyTools;
  std::vector< asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* > m_vTriggerEfficiencyTools;
  std::string m_sInputFilePathRecoHadTau;
  std::string m_sInputFilePathJetIDHadTau;
  std::string m_sInputFilePathDecayModeHadTau;
  std::string m_sInputFilePathEleIDHadTau;
  std::string m_sInputFilePathEleIDElectron;
  std::string m_sInputFilePathTriggerHadTau;
  std::string m_sVarNameBase;
  std::string m_sVarNameRecoHadTau;
  std::string m_sVarNameEleIDHadTau;
  std::string m_sVarNameEleIDElectron;
  std::string m_sVarNameJetIDHadTau;
  std::string m_sVarNameDecayModeHadTau;
  std::string m_sVarNameTriggerHadTau;
  std::string m_sRecommendationTag;
  std::string m_sTriggerName;
  std::string m_sTriggerSFMeasurement;
  bool m_bSkipTruthMatchCheck;
  bool m_bUseTauSubstructure;
  bool m_bUseHighPtUncert;
  bool m_bIsData;
  bool m_bIsConfigured;
  bool m_bReadRandomRunNumber;
  int m_iJetIDLevel;
  int m_iEleIDLevel;
  int m_iTriggerPeriodBinning;
  std::string m_sMCCampaign;
  bool m_sAFII;

  unsigned int m_iRunNumber;
  unsigned int m_iMu;

  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_tTauSelectionToolHandle;
  ToolHandle<CP::IPileupReweightingTool> m_tPRWTool;

}; // class TauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUEFFICIENCYCORRECTIONSTOOL_H
