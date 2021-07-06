/**
 * @file DiTauEfficiencyCorrectionsTool.h
 * @author Guillermo Hamity (ghamity@cern.ch)
 * @author David Kirchmeier
 * @brief Efficiency scale factors and uncertainties for ditau jets
 * @date 2021-02-18
 * 
 * @copyright Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 * 
 */
// Dear emacs, this is -*- c++ -*-

#ifndef TAUANALYSISTOOLS_DITAUEFFICIENCYTOOL_H
#define TAUANALYSISTOOLS_DITAUEFFICIENCYTOOL_H


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
  /* Create a constructor for standalone usage*/
  DiTauEfficiencyCorrectionsTool( const std::string& sName );

  ~DiTauEfficiencyCorrectionsTool();

  /** Function initialising the tool*/
  virtual StatusCode initialize();

  /** Print tool configuration*/
  virtual void printConfig(bool bAlways = true);

  /**
   * @brief Get the ditau Efficiency Scale Factor as a return value
   * 
   * @param xDiTau : reco DiTauJet
   * @param eff : reference to output variable where efficiency is returned
   * @param iRunNumber : run number
   * @param iMu : number of interactions
   * @return CP::CorrectionCode 
   */
  virtual CP::CorrectionCode getEfficiencyScaleFactor( const xAOD::DiTauJet& xDiTau,
      double& eff, unsigned int iRunNumber = 0, unsigned int iMu = 0);


  /**
   * @brief decorate the ditau jet with eff scale factor
   * 
   * @param xDiTau : reco DiTauJet
   * @param iRunNumber : run number
   * @param iMu : number of interactions
   * @return CP::CorrectionCode 
   */
  virtual CP::CorrectionCode applyEfficiencyScaleFactor( const xAOD::DiTauJet& xDiTau,
      unsigned int iRunNumber = 0, unsigned int iMu = 0);


  /**
   * @brief returns whether this tool is affected by the given systematic
   * 
   * @param systematic 
   * @return true 
   * @return false 
   */
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;

  /** returns: the list of all systematics this tool can be affected by*/
  virtual CP::SystematicSet affectingSystematics() const;

  /** returns: the list of all systematics this tool recommends to use*/
  virtual CP::SystematicSet recommendedSystematics() const;

  /** apply systematic variations */
  virtual StatusCode applySystematicVariation( const CP::SystematicSet& systConfig );
  
  /**
   * @brief check if run number is supperted in recommendations
   * 
   * @param iRunNumber 
   * @return true if ditau recommendations support this run
   * @note currently no implementaion exists, alwasys returns true
   */
  virtual bool isSupportedRunNumber( int iRunNumber )
  {
    (void) iRunNumber;
    return true;
  };

private:
  StatusCode beginEvent();

  std::string ConvertJetIDToString(const int& iLevel);

  /** Initialize Moriond 2017 recommendations*/
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
