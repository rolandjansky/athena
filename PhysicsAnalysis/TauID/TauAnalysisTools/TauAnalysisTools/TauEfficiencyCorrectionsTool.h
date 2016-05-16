// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUEFFICIENCYTOOL_H
#define TAUANALYSISTOOLS_TAUEFFICIENCYTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Local include(s):
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/CommonEfficiencyTool.h"
#include "TauAnalysisTools/TauEfficiencyJetIDTool.h"
#include "TauAnalysisTools/TauEfficiencyContJetIDTool.h"
#include "TauAnalysisTools/TauEfficiencyEleIDTool.h"
#include "TauAnalysisTools/TauEfficiencyTriggerTool.h"
#include "TauAnalysisTools/TauSelectionTool.h"

namespace TauAnalysisTools
{

class TauEfficiencyCorrectionsTool
  : public virtual ITauEfficiencyCorrectionsTool
  , public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauEfficiencyCorrectionsTool, TauAnalysisTools::ITauEfficiencyCorrectionsTool )

  // declaration of classes as friends to access private member variables
  friend class CommonEfficiencyTool;
  friend class TauEfficiencyJetIDTool;
  friend class TauEfficiencyJetIDRun2Tool;
  friend class TauEfficiencyContJetIDTool;
  friend class TauEfficiencyEleIDTool;
  friend class TauEfficiencyTriggerTool;

public:
  /// Create a constructor for standalone usage
  TauEfficiencyCorrectionsTool( const std::string& sName );

  /// Create a constructor for standalone usage with preconfiguration for scale
  /// factors types depending on applied cuts from TauSelection Tool
  TauEfficiencyCorrectionsTool( const std::string& sName, TauSelectionTool* tTauSelectionTool );

  ~TauEfficiencyCorrectionsTool();

  /// Function initialising the tool
  virtual StatusCode initialize();

  /// Print tool configuration
  virtual void printConfig(bool bAlways = true);

  /// Get the "tau efficiency" as a return value
  virtual CP::CorrectionCode getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
      double& eff );
  /// Decorate the tau with its efficiency
  virtual CP::CorrectionCode applyEfficiencyScaleFactor( const xAOD::TauJet& xTau );

  /// returns: whether this tool is affected by the given systematis
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;

  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const;

  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const;

  virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& systConfig );

  virtual StatusCode setRunNumber(int iRunNumber);

private:
  std::string ConvertJetIDToString(const int& iLevel);
  std::string ConvertEleOLRToString(const int& iLevel);
  std::string ConvertTriggerIDToString(const int& iLevel);

  StatusCode initializeWithTauSelectionTool();

private:
  /// Some dummy property
  int m_iEfficiencyCorrectionType;
  std::vector<int> m_vEfficiencyCorrectionTypes;
  std::vector< CommonEfficiencyTool* > m_vCommonEfficiencyTools;
  std::string m_sInputFilePath;
  std::string m_sInputFilePathRecoHadTau;
  std::string m_sInputFilePathEleOLRHadTau;
  std::string m_sInputFilePathEleOLRElectron;
  std::string m_sInputFilePathJetIDHadTau;
  std::string m_sInputFilePathContJetIDHadTau;
  std::string m_sInputFilePathEleIDHadTau;
  std::string m_sInputFilePathTriggerHadTau;
  std::string m_sVarNameBase;
  std::string m_sVarNameRecoHadTau;
  std::string m_sVarNameEleOLRHadTau;
  std::string m_sVarNameEleOLRElectron;
  std::string m_sVarNameJetIDHadTau;
  std::string m_sVarNameContJetIDHadTau;
  std::string m_sVarNameEleIDHadTau;
  std::string m_sVarNameTriggerHadTau;
  std::string m_sRecommendationTag;
  std::string m_sTriggerName;
  bool m_bSkipTruthMatchCheck;
  bool m_bNoMultiprong;
  bool m_bUseIDExclusiveSF;
  bool m_bUseInclusiveEta;
  bool m_bUseTriggerInclusiveEta;
  bool m_bUsePtBinnedSF;
  bool m_bUseHighPtUncert;
  int m_iIDLevel;
  int m_iEVLevel;
  int m_iOLRLevel;
  int m_iContSysType;
  int m_iTriggerPeriodBinning;

  TauSelectionTool* m_tTauSelectionTool;

}; // class TauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUEFFICIENCYTOOL_H
