/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "PATInterfaces/SystematicRegistry.h"
#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "TauAnalysisTools/DiTauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/SharedFilesVersion.h"


namespace TauAnalysisTools
{

//______________________________________________________________________________
DiTauEfficiencyCorrectionsTool::DiTauEfficiencyCorrectionsTool( const std::string& sName )
  : asg::AsgMetadataTool( sName )
  , m_vCommonEfficiencyTools()
  , m_bIsData(false)
  , m_bIsConfigured(false)
  , m_iRunNumber(0)
{
  declareProperty( "EfficiencyCorrectionTypes",    m_vEfficiencyCorrectionTypes    = {} );
  declareProperty( "InputFilePathRecoHadTau",      m_sInputFilePathRecoHadTau      = "" );
  declareProperty( "InputFilePathJetIDHadTau",     m_sInputFilePathJetIDHadTau     = "" );
  declareProperty( "VarNameRecoHadTau",            m_sVarNameRecoHadTau            = "" );
  declareProperty( "VarNameJetIDHadTau",           m_sVarNameJetIDHadTau           = "" );
  declareProperty( "RecommendationTag",            m_sRecommendationTag            = "2017-moriond" );
  declareProperty( "IDLevel",                      m_iIDLevel                      = (int)JETIDBDTTIGHT );
  declareProperty( "EventInfoName",                m_sEventInfoName                = "EventInfo" );
  declareProperty( "SkipTruthMatchCheck",          m_bSkipTruthMatchCheck          = false );
}


//______________________________________________________________________________
DiTauEfficiencyCorrectionsTool::~DiTauEfficiencyCorrectionsTool()
{
  for (auto tTool : m_vCommonEfficiencyTools)
    delete tTool;
}


//______________________________________________________________________________
StatusCode DiTauEfficiencyCorrectionsTool::initialize()
{
  // Greet the user:
  ATH_MSG_INFO( "Initializing DiTauEfficiencyCorrectionsTool" );

  if (m_bSkipTruthMatchCheck)
    ATH_MSG_WARNING("Truth match check will be skipped. This is ONLY FOR TESTING PURPOSE!");

  // configure default set of variations if not set by the constructor using TauSelectionTool or the user
  if ((m_sRecommendationTag== "2017-moriond") and m_vEfficiencyCorrectionTypes.size() == 0)
    m_vEfficiencyCorrectionTypes = {SFJetIDHadTau
                                   };

  if (m_sRecommendationTag == "2017-moriond")
    ATH_CHECK(initializeTools_2017_moriond());
  else
  {
    ATH_MSG_FATAL("Unknown RecommendationTag: "<<m_sRecommendationTag);
    return StatusCode::FAILURE;
  }

  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
  {
    ATH_CHECK((**it).setProperty("OutputLevel", this->msg().level()));
    ATH_CHECK((**it).initialize());
  }

  // Add the affecting systematics to the global registry
  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  if (!registry.registerSystematics(*this))
  {
    ATH_MSG_ERROR ("Unable to register the systematics");
    return StatusCode::FAILURE;
  }

  printConfig(/* bAlways = */ false);

  return StatusCode::SUCCESS;
}


//______________________________________________________________________________
StatusCode DiTauEfficiencyCorrectionsTool::beginEvent()
{
  if (!m_bIsConfigured)
  {
    const xAOD::EventInfo* xEventInfo = nullptr;
    ATH_CHECK(evtStore()->retrieve(xEventInfo,"EventInfo"));
    m_bIsData = !(xEventInfo->eventType( xAOD::EventInfo::IS_SIMULATION));
    m_bIsConfigured=true;
  }

  if (m_bIsData)
    return StatusCode::SUCCESS;

  return StatusCode::SUCCESS;
}


//______________________________________________________________________________
void DiTauEfficiencyCorrectionsTool::printConfig(bool bAlways)
{
  if (bAlways)
  {
    ATH_MSG_DEBUG( "DiTauEfficiencyCorrectionsTool with name " << name() << " is configured as follows:" );
    for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
      ATH_MSG_ALWAYS( "  EfficiencyCorrectionTypes " << iEfficiencyCorrectionType );
    ATH_MSG_ALWAYS( "  InputFilePathRecoHadTau " << m_sInputFilePathRecoHadTau );
    ATH_MSG_ALWAYS( "  InputFilePathJetIDHadTau " << m_sInputFilePathJetIDHadTau );
    ATH_MSG_ALWAYS( "  VarNameRecoHadTau " << m_sVarNameRecoHadTau );
    ATH_MSG_ALWAYS( "  VarNameJetIDHadTau " << m_sVarNameJetIDHadTau );
    ATH_MSG_ALWAYS( "  RecommendationTag " << m_sRecommendationTag );
  }
  else
  {
    ATH_MSG_DEBUG( "DiTauEfficiencyCorrectionsTool with name " << name() << " is configured as follows:" );
    for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
      ATH_MSG_DEBUG( "  EfficiencyCorrectionTypes " << iEfficiencyCorrectionType );
    ATH_MSG_DEBUG( "  InputFilePathRecoHadTau " << m_sInputFilePathRecoHadTau );
    ATH_MSG_DEBUG( "  VarNameRecoHadTau " << m_sVarNameRecoHadTau );
    ATH_MSG_DEBUG( "  VarNameJetIDHadTau " << m_sVarNameJetIDHadTau );
    ATH_MSG_DEBUG( "  RecommendationTag " << m_sRecommendationTag );
  }
}

//______________________________________________________________________________
CP::CorrectionCode DiTauEfficiencyCorrectionsTool::getEfficiencyScaleFactor( const xAOD::DiTauJet& xDiTau,
    double& eff )
{
  eff = 1.;

  if (m_bIsData)
    return CP::CorrectionCode::Ok;

  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
  {
    double dToolEff = 1.;
    CP::CorrectionCode tmpCorrectionCode = (**it)->getEfficiencyScaleFactor(xDiTau, dToolEff);
    if (tmpCorrectionCode != CP::CorrectionCode::Ok)
      return tmpCorrectionCode;
    eff *= dToolEff;
  }
  return CP::CorrectionCode::Ok;
}

//______________________________________________________________________________
CP::CorrectionCode DiTauEfficiencyCorrectionsTool::applyEfficiencyScaleFactor( const xAOD::DiTauJet& xDiTau )
{
  if (m_bIsData)
    return CP::CorrectionCode::Ok;

  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
  {
    CP::CorrectionCode tmpCorrectionCode = (**it)->applyEfficiencyScaleFactor(xDiTau);
    if (tmpCorrectionCode != CP::CorrectionCode::Ok)
    {
      return tmpCorrectionCode;
    }
  }
  return CP::CorrectionCode::Ok;
}

/// returns: whether this tool is affected by the given systematis
//______________________________________________________________________________
bool DiTauEfficiencyCorrectionsTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
    if ((**it)->isAffectedBySystematic(systematic))
      return true;
  return false;
}

/// returns: the list of all systematics this tool can be affected by
//______________________________________________________________________________
CP::SystematicSet DiTauEfficiencyCorrectionsTool::affectingSystematics() const
{
  CP::SystematicSet sAffectingSystematics;
  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
    sAffectingSystematics.insert((**it)->affectingSystematics());
  return sAffectingSystematics;
}

/// returns: the list of all systematics this tool recommends to use
//______________________________________________________________________________
CP::SystematicSet DiTauEfficiencyCorrectionsTool::recommendedSystematics() const
{
  CP::SystematicSet sRecommendedSystematics;
  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
  {
    sRecommendedSystematics.insert((**it)->recommendedSystematics());
  }
  return sRecommendedSystematics;
}

//______________________________________________________________________________
CP::SystematicCode DiTauEfficiencyCorrectionsTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
    if ((**it)->applySystematicVariation(sSystematicSet) == CP::SystematicCode::Unsupported)
    {
      return CP::SystematicCode::Unsupported;
    }
  return CP::SystematicCode::Ok;
}

//=================================PRIVATE-PART=================================

//______________________________________________________________________________
StatusCode DiTauEfficiencyCorrectionsTool::initializeTools_2017_moriond()
{
  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/EfficiencyCorrections/";
  for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
  {
    if (iEfficiencyCorrectionType == SFJetIDHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathJetIDHadTau.empty()) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadDiTau_2017-fall.root";
      if (m_sVarNameJetIDHadTau.length() == 0) m_sVarNameJetIDHadTau = "DiTauScaleFactorJetIDHadTau";

      asg::AnaToolHandle<IDiTauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<IDiTauEfficiencyCorrectionsTool>("JetIDHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonDiTauEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathJetIDHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameJetIDHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertJetIDToString(m_iIDLevel)));
    }
    else
    {
      ATH_MSG_WARNING("unsupported EfficiencyCorrectionsType with enum "<<iEfficiencyCorrectionType);
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
std::string DiTauEfficiencyCorrectionsTool::ConvertJetIDToString(const int& iLevel)
{
  switch(iLevel)
  {
  case JETIDNONE:
    return "ditaureconstruction";
    break;
  case JETIDBDTVERYLOOSE:
    return "jetbdtsigveryloose";
    break;
  case JETIDBDTLOOSE:
    return "jetbdtsigloose";
    break;
  case JETIDBDTMEDIUM:
    return "jetbdtsigmedium";
    break;
  case JETIDBDTTIGHT:
    return "jetbdtsigtight";
    break;
  default:
    assert(false && "No valid ID level passed. Breaking up ...");
    break;
  }
  return "";
}


} // namespace TauAnalysisTools
