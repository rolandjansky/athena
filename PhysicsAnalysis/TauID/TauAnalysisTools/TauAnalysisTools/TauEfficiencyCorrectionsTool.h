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
#include "AsgTools/AsgMetadataTool.h"

// Local include(s):
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/CommonEfficiencyTool.h"
#include "TauAnalysisTools/TauEfficiencyJetIDTool.h"
#include "TauAnalysisTools/TauEfficiencyContJetIDTool.h"
#include "TauAnalysisTools/TauEfficiencyEleIDTool.h"
#include "TauAnalysisTools/TauEfficiencyTriggerTool.h"

#if __has_include("PileupReweighting/IPileupReweightingTool.h")
// Tool include(s)
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"
#define TAUANALYSISTOOLS_PRWTOOL_AVAILABLE
#endif

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

  /// Create a constructor for standalone usage with preconfiguration for scale
  /// factors types depending on applied cuts from TauSelection Tool
  TauEfficiencyCorrectionsTool( const std::string& sName, TauSelectionTool* tTauSelectionTool )  __attribute__ ((deprecated("This function is deprecated. Please pass the TauSelectionTool as a ToolHandle via the property \"TauSelectionTool\". The function will be removed in the future without further notice.\nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauEfficiencyCorrectionsTool.rst")));

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

  virtual bool isSupportedRunNumber( int iRunNumber )
  {
    (void) iRunNumber;
    return true;
  };

private:
  StatusCode beginEvent();

  std::string ConvertJetIDToString(const int& iLevel);
  std::string ConvertEleOLRToString(const int& iLevel);
  std::string ConvertTriggerIDToString(const int& iLevel);
  std::string GetTriggerSFMeasrementString();

  StatusCode initializeWithTauSelectionTool();

  StatusCode initializeTools_2017_moriond();
  StatusCode initializeTools_2016_fall();
  StatusCode initializeTools_2016_ichep();
  StatusCode initializeTools_mc15_moriond();
  StatusCode initializeTools_mc15_pre_recommendations();
  StatusCode initializeTools_mc12_final();

private:

  std::vector<int> m_vEfficiencyCorrectionTypes;
  std::vector< asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* > m_vCommonEfficiencyTools;
  std::vector< asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* > m_vTriggerEfficiencyTools;
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
  std::string m_sTriggerYear;
  std::string m_sTriggerSFMeasurement;
  bool m_bSkipTruthMatchCheck;
  //bool m_bNoMultiprong;
  bool m_bUseIDExclusiveSF;
  bool m_bUseInclusiveEta;
  bool m_bUseTriggerInclusiveEta;
  bool m_bUsePtBinnedSF;
  bool m_bUseHighPtUncert;
  bool m_bIsData;
  bool m_bIsConfigured;
  int m_iIDLevel;
  int m_iEVLevel;
  int m_iOLRLevel;
  int m_iContSysType;
  int m_iTriggerPeriodBinning;

  unsigned int m_iRunNumber;

  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_tTauSelectionToolHandle;
#ifdef TAUANALYSISTOOLS_PRWTOOL_AVAILABLE
  ToolHandle<CP::IPileupReweightingTool> m_tPRWTool;
#endif // TAUANALYSISTOOLS_PRWTOOL_AVAILABLE
  TauSelectionTool* m_tTauSelectionTool;

  std::string m_sEventInfoName;
}; // class TauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUEFFICIENCYTOOL_H
