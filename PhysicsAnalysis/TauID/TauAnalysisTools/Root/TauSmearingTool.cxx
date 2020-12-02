/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "PATInterfaces/SystematicRegistry.h"
#include "xAODMetaData/FileMetaData.h"

// Local include(s):
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/SharedFilesVersion.h"
#include "TauAnalysisTools/TauSmearingRun1Tool.h"

#include <boost/algorithm/string.hpp>

namespace TauAnalysisTools
{

TauSmearingTool::TauSmearingTool( const std::string& sName )
  : asg::AsgMetadataTool( sName )
  , m_tCommonSmearingTool(sName+"_CommonSmearingTool", this)
  , m_sInputFilePath("")
{
  declareProperty( "RecommendationTag",   m_sRecommendationTag = "2019-summer" );
  declareProperty( "SkipTruthMatchCheck", m_bSkipTruthMatchCheck = false );
  declareProperty( "ApplyFading",         m_bApplyFading = true);
  declareProperty( "ApplyMVATES",         m_bApplyMVATES = true);
  declareProperty( "ApplyCombinedTES",    m_bApplyCombinedTES = false);
  declareProperty("ApplyMVATESQualityCheck", m_bApplyMVATESQualityCheck = true );
  declareProperty("ApplyInsituCorrection",   m_bApplyInsituCorrection   = true );
  declareProperty( "isAFII",	                   m_sAFII	                   = false );

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

  if (m_sRecommendationTag == "2019-summer")
  {
    if (m_sInputFilePath.empty())
    {
	    if (m_sAFII) m_sInputFilePath = sDirectory+"TES_TrueHadTau_2019-summer_AFII.root";
      else m_sInputFilePath = sDirectory+"TES_TrueHadTau_2019-summer.root";
    }
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

// auto detection of simulation flavour, used to cross check configuration of tool
//______________________________________________________________________________
StatusCode TauSmearingTool::beginInputFile()
{
  if (m_sRecommendationTag == "2019-summer")
  {
    std::string simType("");
    if (inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData"))
    {
      const xAOD::FileMetaData* fmd = 0;
      ATH_CHECK( inputMetaStore()->retrieve( fmd, "FileMetaData" ) );
      bool result = fmd->value( xAOD::FileMetaData::simFlavour , simType );
      // if no result -> no simFlavor metadata, so must be data
      if(result) boost::to_upper(simType);
    }
    if (simType.find("ATLFASTII")!=std::string::npos && !m_sAFII) ATH_MSG_WARNING("Input file is fast simulation but you are _not_ using AFII corrections and uncertainties, you should set \"isAFII\" to \"true\"");
    else if (simType.find("FULLG4")!=std::string::npos && m_sAFII) ATH_MSG_WARNING("Input file is full simulation but you are using AFII corrections and uncertainties, you should set \"isAFII\" to \"false\"");
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
  
StatusCode TauSmearingTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  return m_tCommonSmearingTool->applySystematicVariation( sSystematicSet );
}


} // namespace TauAnalysisTools
