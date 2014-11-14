// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUEFFICIENCYTOOL_H
#define TAUEFFICIENCYTOOL_H

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
#include "TauAnalysisTools/TauSelectionTool.h"

namespace TauAnalysisTools
{

class TauEfficiencyCorrectionsTool
  : public virtual ITauEfficiencyCorrectionsTool
  , public virtual CP::ISystematicsTool
  , public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauEfficiencyCorrectionsTool, TauAnalysisTools::ITauEfficiencyCorrectionsTool )

  // declaration of classes as friends to access private member variables
  friend class CommonEfficiencyTool;
  friend class TauEfficiencyJetIDTool;
  friend class TauEfficiencyContJetIDTool;
  friend class TauEfficiencyEleIDTool;

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
  virtual void printConfig();

  /// Get the "tau efficiency" as a return value
  virtual CP::CorrectionCode getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
      double& eff );
  /// Decorate the tau with its efficiency
  virtual CP::CorrectionCode applyEfficiencyScaleFactor( const xAOD::TauJet& xTau );

  /// Get the "tau efficiency stat uncertainty" as a return value
  virtual CP::CorrectionCode getEfficiencyScaleFactorStatUnc( const xAOD::TauJet& xTau,
      double& eff );
  /// Decorate the tau with its efficiency stat uncertainty
  virtual CP::CorrectionCode applyEfficiencyScaleFactorStatUnc( const xAOD::TauJet& xTau );

  /// Get the "tau efficiency sys uncertainty" as a return value
  virtual CP::CorrectionCode getEfficiencyScaleFactorSysUnc( const xAOD::TauJet& xTau,
      double& eff );
  /// Decorate the tau with its efficiency sys uncertainty
  virtual CP::CorrectionCode applyEfficiencyScaleFactorSysUnc( const xAOD::TauJet& xTau );

  /// returns: whether this tool is affected by the given systematis
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;

  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const;

  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const;

  virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& systConfig );

private:
  /// Some dummy property
  int m_iEfficiencyCorrectionType;
  std::map<std::string,CommonEfficiencyTool*> m_mCommonEfficiencyTool;
  CommonEfficiencyTool* m_tCommonEfficiencyTool;
  std::string m_sInputFilePath;
  std::string m_sVarNameBase;
  bool m_bUseIDExclusiveSF;
  bool m_bUseInclusiveEta;
  bool m_bUsePtBinnedSF;
  bool m_bUseHighPtUncert;
  int m_iSysDirection;
  int m_iIDLevel;
  int m_iEVLevel;
  int m_iOLRLevel;
  int m_iContSysType;

}; // class TauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // CPTOOLTESTS_TAUEFFICIENCYTOOL_H
