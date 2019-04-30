/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "PATInterfaces/SystematicRegistry.h"

// Local include(s):
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/SharedFilesVersion.h"
#include "TauAnalysisTools/TauSmearingRun1Tool.h"

namespace TauAnalysisTools
{

TauSmearingTool::TauSmearingTool( const std::string& sName )
  : asg::AsgTool( sName )
  , m_tCommonSmearingTool(sName+"_CommonSmearingTool", this)
  , m_sInputFilePath("")
{
  declareProperty( "RecommendationTag",   m_sRecommendationTag = "2018-summer" );
  declareProperty( "SkipTruthMatchCheck", m_bSkipTruthMatchCheck = false );
  declareProperty( "ApplyFading",         m_bApplyFading = true);
  declareProperty( "ApplyMVATES",         m_bApplyMVATES = true);
  declareProperty( "ApplyCombinedTES",    m_bApplyCombinedTES = false);
  declareProperty("ApplyMVATESQualityCheck", m_bApplyMVATESQualityCheck = true );
  declareProperty("ApplyInsituCorrection",   m_bApplyInsituCorrection   = true );

  // deprecated property
  declareProperty( "IsData",              m_bIsData = false );
}

TauSmearingTool::~TauSmearingTool()
{
}

StatusCode TauSmearingTool::initialize()
{
  // check if user has modified IsData property, if so, tell him that its deprecated
  if (m_bIsData)
  {
    ATH_MSG_ERROR("The property IsData is deprecated, please don't use it anymore. The check is now done automatically. The Property will be removed in the future without further notice");
    return StatusCode::FAILURE;
  }

  // Greet the user:
  ATH_MSG_INFO( "Initializing TauSmearingTool" );

  if (m_bSkipTruthMatchCheck)
    ATH_MSG_WARNING("Truth match check will be skipped. This is ONLY FOR TESTING PURPOSE!");

  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/Smearing/";

  // recommendations tags "mc16-prerec" and "2018-summer" have same recommendations in tau smearing
  // only adding "2018-summer" to be consistent with tags in efficiency corrections 
  if (m_sRecommendationTag == "mc16-prerec" || m_sRecommendationTag == "2018-summer" )
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"TES_TrueHadTau_mc16-prerec.root";
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tCommonSmearingTool, TauAnalysisTools::CommonSmearingTool));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("InputFilePath", m_sInputFilePath));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyFading", m_bApplyFading));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyMVATES", m_bApplyMVATES));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyMVATESQualityCheck", m_bApplyMVATESQualityCheck));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyInsituCorrection", m_bApplyInsituCorrection));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyCombinedTES", m_bApplyCombinedTES));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("OutputLevel", this->msg().level()));
  }
  else if (m_sRecommendationTag == "2017-moriond")
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"TES_TrueHadTau_2017-moriond.root";
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tCommonSmearingTool, TauAnalysisTools::CommonSmearingTool));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("InputFilePath", m_sInputFilePath));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyFading", m_bApplyFading));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyMVATES", m_bApplyMVATES));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyMVATESQualityCheck", m_bApplyMVATESQualityCheck));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyInsituCorrection", m_bApplyInsituCorrection));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyCombinedTES", m_bApplyCombinedTES));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("OutputLevel", this->msg().level()));
  }
  else if (m_sRecommendationTag == "2016-ichep")
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"TES_TrueHadTau_2016-ichep.root";
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tCommonSmearingTool, TauAnalysisTools::CommonSmearingTool));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("InputFilePath", m_sInputFilePath));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyFading", m_bApplyFading));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyMVATES", m_bApplyMVATES));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyCombinedTES", m_bApplyCombinedTES));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("OutputLevel", this->msg().level()));
  }
  else if (m_sRecommendationTag == "mc15-moriond")
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"TES_TrueHadTau_mc15_moriond.root";
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tCommonSmearingTool, TauAnalysisTools::CommonSmearingTool));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("InputFilePath", m_sInputFilePath));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyFading", m_bApplyFading));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyMVATES", m_bApplyMVATES));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyCombinedTES", m_bApplyCombinedTES));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("OutputLevel", this->msg().level()));
  }
  else if (m_sRecommendationTag == "mc15-pre-recommendations")
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"TES_TrueHadTau_mc15_prerec.root";
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tCommonSmearingTool, TauAnalysisTools::CommonSmearingTool));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("InputFilePath", m_sInputFilePath));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("ApplyFading", false)); // apply fading off by default
  }
  else if (m_sRecommendationTag == "mc12-final")
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"mc12_p1344_medium.root";
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tCommonSmearingTool, TauAnalysisTools::TauSmearingRun1Tool));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("InputFilePath", m_sInputFilePath));
  }
  else if (m_sRecommendationTag == "mc11-final")
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"mc11.root";
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tCommonSmearingTool, TauAnalysisTools::TauSmearingRun1Tool));
    ATH_CHECK(m_tCommonSmearingTool.setProperty("InputFilePath", m_sInputFilePath));
  }
  else
  {
    ATH_MSG_FATAL("unknown recommendation tag "<<m_sRecommendationTag);
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_tCommonSmearingTool.initialize());

  // Add the affecting systematics to the global registry
  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  if (!registry.registerSystematics(*this))
  {
    ATH_MSG_ERROR ("Unable to register the systematics");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

CP::CorrectionCode TauSmearingTool::applyCorrection( xAOD::TauJet& xTau )
{

  return m_tCommonSmearingTool->applyCorrection(xTau);
}

CP::CorrectionCode TauSmearingTool::correctedCopy( const xAOD::TauJet& input,
    xAOD::TauJet*& output )
{
  return m_tCommonSmearingTool->correctedCopy(input, output);
}

/// returns: whether this tool is affected by the given systematis
bool TauSmearingTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  return m_tCommonSmearingTool->isAffectedBySystematic( systematic );
}

/// returns: the list of all systematics this tool can be affected by
CP::SystematicSet TauSmearingTool::affectingSystematics() const
{
  return m_tCommonSmearingTool->affectingSystematics();
}

/// returns: the list of all systematics this tool recommends to use
CP::SystematicSet TauSmearingTool::recommendedSystematics() const
{
  return m_tCommonSmearingTool->recommendedSystematics();
}

CP::SystematicCode TauSmearingTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  return m_tCommonSmearingTool->applySystematicVariation( sSystematicSet );
}


} // namespace TauAnalysisTools
