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
  , m_tCommonSmearingTool(0)
  , m_sInputFilePath("")
  , m_sRecommendationTag("mc15-moriond")
  , m_bSkipTruthMatchCheck(false)
  , m_bApplyFading(true)
  , m_bIsData(false)
{
  declareProperty( "IsData", m_bIsData = false );
  declareProperty( "InputFilePath", m_sInputFilePath = "" );
  declareProperty( "RecommendationTag", m_sRecommendationTag = "mc15-moriond" );
  declareProperty( "SkipTruthMatchCheck", m_bSkipTruthMatchCheck = false );
  declareProperty( "ApplyFading", m_bApplyFading = true);
}

TauSmearingTool::~TauSmearingTool()
{
}

StatusCode TauSmearingTool::initialize()
{
  // Greet the user:
  ATH_MSG_INFO( "Initializing TauSmearingTool" );

  if (m_bSkipTruthMatchCheck)
    ATH_MSG_WARNING("Truth match check will be skipped. This is ONLY FOR TESTING PURPOSE!");

  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/Smearing/";

  if (m_sRecommendationTag == "mc15-moriond")
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"TES_TrueHadTau_mc15_moriond.root";
    m_tCommonSmearingTool.reset(new CommonSmearingTool(name()+"_"+m_sRecommendationTag));
    if (m_tCommonSmearingTool->setProperty("InputFilePath", m_sInputFilePath).isFailure()) return StatusCode::FAILURE;
    if (m_tCommonSmearingTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck).isFailure()) return StatusCode::FAILURE;
    if (m_tCommonSmearingTool->setProperty("ApplyFading", m_bApplyFading).isFailure()) return StatusCode::FAILURE;

  }
  else if (m_sRecommendationTag == "mc15-pre-recommendations")
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"TES_TrueHadTau_mc15_prerec.root";
    m_tCommonSmearingTool.reset(new CommonSmearingTool(name()+"_"+m_sRecommendationTag));
    if (m_tCommonSmearingTool->setProperty("InputFilePath", m_sInputFilePath).isFailure()) return StatusCode::FAILURE;
    if (m_tCommonSmearingTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck).isFailure()) return StatusCode::FAILURE;
    if (m_tCommonSmearingTool->setProperty("ApplyFading", false).isFailure()) return StatusCode::FAILURE; // apply fading off by default
  }
  else if (m_sRecommendationTag == "mc12-final")
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"mc12_p1344_medium.root";
    m_tCommonSmearingTool.reset(new TauSmearingRun1Tool(name()+"_"+m_sRecommendationTag));
  }
  else if (m_sRecommendationTag == "mc11-final")
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"mc11.root";
    m_tCommonSmearingTool.reset(new TauSmearingRun1Tool(name()+"_"+m_sRecommendationTag));
  }

  if (!m_tCommonSmearingTool)
  {
    ATH_MSG_FATAL("unknown recommendation tag "<<m_sRecommendationTag);
    return StatusCode::FAILURE;
  }
  // only set vars if they differ from "", which means they have been configured by the user

  m_tCommonSmearingTool->msg().setLevel( this->msg().level() );
  m_tCommonSmearingTool->setParent(this);
  ATH_CHECK(m_tCommonSmearingTool->initialize());

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
