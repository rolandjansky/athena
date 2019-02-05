/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "PATInterfaces/SystematicRegistry.h"

// Local include(s):
#include "TauAnalysisTools/DiTauSmearingTool.h"
#include "TauAnalysisTools/SharedFilesVersion.h"

namespace TauAnalysisTools
{

DiTauSmearingTool::DiTauSmearingTool( const std::string& sName )
  : asg::AsgTool( sName )
  , m_tCommonDiTauSmearingTool(sName+"_CommonDiTauSmearingTool", this)
  , m_sInputFilePath("")
{
  declareProperty( "RecommendationTag",   m_sRecommendationTag = "2019-winter" );
  declareProperty( "SkipTruthMatchCheck", m_bSkipTruthMatchCheck = false );
}

DiTauSmearingTool::~DiTauSmearingTool()
{
}

StatusCode DiTauSmearingTool::initialize()
{
  // Greet the user:
  ATH_MSG_INFO( "Initializing DiTauSmearingTool" );

  if (m_bSkipTruthMatchCheck)
    ATH_MSG_WARNING("Truth match check will be skipped. This is ONLY FOR TESTING PURPOSE!");

  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/Smearing/";

  if (m_sRecommendationTag == "2019-winter")
  {
    if (m_sInputFilePath.empty())
      m_sInputFilePath = sDirectory+"TES_TrueHadDiTau_2019-winter.root";
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tCommonDiTauSmearingTool, TauAnalysisTools::CommonDiTauSmearingTool));
    ATH_CHECK(m_tCommonDiTauSmearingTool.setProperty("InputFilePath", m_sInputFilePath));
    ATH_CHECK(m_tCommonDiTauSmearingTool.setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    ATH_CHECK(m_tCommonDiTauSmearingTool.setProperty("OutputLevel", this->msg().level()));
  }
  else
  {
    ATH_MSG_FATAL("unknown recommendation tag "<<m_sRecommendationTag);
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_tCommonDiTauSmearingTool.initialize());

  // Add the affecting systematics to the global registry
  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  if (!registry.registerSystematics(*this))
  {
    ATH_MSG_ERROR ("Unable to register the systematics");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

CP::CorrectionCode DiTauSmearingTool::applyCorrection( xAOD::DiTauJet& xDiTau )
{
  return m_tCommonDiTauSmearingTool->applyCorrection(xDiTau);
}

CP::CorrectionCode DiTauSmearingTool::correctedCopy( const xAOD::DiTauJet& input,
    xAOD::DiTauJet*& output )
{
  return m_tCommonDiTauSmearingTool->correctedCopy(input, output);
}

/// returns: whether this tool is affected by the given systematis
bool DiTauSmearingTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  return m_tCommonDiTauSmearingTool->isAffectedBySystematic( systematic );
}

/// returns: the list of all systematics this tool can be affected by
CP::SystematicSet DiTauSmearingTool::affectingSystematics() const
{
  return m_tCommonDiTauSmearingTool->affectingSystematics();
}

/// returns: the list of all systematics this tool recommends to use
CP::SystematicSet DiTauSmearingTool::recommendedSystematics() const
{
  return m_tCommonDiTauSmearingTool->recommendedSystematics();
}

CP::SystematicCode DiTauSmearingTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  return m_tCommonDiTauSmearingTool->applySystematicVariation( sSystematicSet );
}


} // namespace TauAnalysisTools
