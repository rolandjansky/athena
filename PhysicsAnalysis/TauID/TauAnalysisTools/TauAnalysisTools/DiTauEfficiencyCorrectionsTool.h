// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_DITAUEFFICIENCYTOOL_H
#define TAUANALYSISTOOLS_DITAUEFFICIENCYTOOL_H

/*
  author: David Kirchmeier
  mail: davd.kirchmeier@cern.ch
  documentation in: 
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/doc/README-DiTauEfficiencyCorrectionsTool.rst
        or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-DiTauEfficiencyCorrectionsTool.rst
*/

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AnaToolHandle.h"

// Local include(s):
#include "TauAnalysisTools/IDiTauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/CommonDiTauEfficiencyTool.h"

namespace TauAnalysisTools
{

class DiTauEfficiencyCorrectionsTool
  : public virtual IDiTauEfficiencyCorrectionsTool
  , public asg::AsgMetadataTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( DiTauEfficiencyCorrectionsTool, TauAnalysisTools::IDiTauEfficiencyCorrectionsTool )

public:
  /// Create a constructor for standalone usage
  DiTauEfficiencyCorrectionsTool( const std::string& sName );

  ~DiTauEfficiencyCorrectionsTool();

  /// Function initialising the tool
  virtual StatusCode initialize();

  /// Print tool configuration
  virtual void printConfig(bool bAlways = true);

  /// Get the "tau efficiency" as a return value
  virtual CP::CorrectionCode getEfficiencyScaleFactor( const xAOD::DiTauJet& xDiTau,
      double& eff, unsigned int iRunNumber = 0, unsigned int iMu = 0);
  /// Decorate the tau with its efficiency
  virtual CP::CorrectionCode applyEfficiencyScaleFactor( const xAOD::DiTauJet& xDiTau,
      unsigned int iRunNumber = 0, unsigned int iMu = 0);

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

  StatusCode initializeTools_2017_moriond();

private:

  std::vector<int> m_vEfficiencyCorrectionTypes;
  std::vector< asg::AnaToolHandle<IDiTauEfficiencyCorrectionsTool>* > m_vCommonEfficiencyTools;
  std::string m_sInputFilePathRecoHadTau;
  std::string m_sInputFilePathJetIDHadTau;
  std::string m_sVarNameBase;
  std::string m_sVarNameRecoHadTau;
  std::string m_sVarNameJetIDHadTau;
  std::string m_sRecommendationTag;
  bool m_bSkipTruthMatchCheck;
  bool m_bIsData;
  bool m_bIsConfigured;
  int m_iIDLevel;

  // unsigned int m_iRunNumber;

  std::string m_sEventInfoName;
}; // class DiTauEfficiencyCorrectionsTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_DITAUEFFICIENCYTOOL_H
