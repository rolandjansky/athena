/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "PATInterfaces/SystematicRegistry.h"
#include "xAODMetaData/FileMetaData.h"

// Local include(s):
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/SharedFilesVersion.h"

#include <boost/algorithm/string.hpp>

namespace TauAnalysisTools
{

//______________________________________________________________________________
TauEfficiencyCorrectionsTool::TauEfficiencyCorrectionsTool( const std::string& sName )
  : asg::AsgMetadataTool( sName )
  , m_vCommonEfficiencyTools()
  , m_vTriggerEfficiencyTools()
  //, m_bNoMultiprong(false)
  , m_bIsData(false)
  , m_bIsConfigured(false)
  , m_iRunNumber(0)
  , m_iMu(0)
  , m_tTauSelectionToolHandle("")
  , m_tPRWTool("")
  , m_tTauSelectionTool(0)
{
  declareProperty( "EfficiencyCorrectionTypes",    m_vEfficiencyCorrectionTypes    = {} );
  declareProperty( "InputFilePathRecoHadTau",      m_sInputFilePathRecoHadTau      = "" );
  declareProperty( "InputFilePathEleOLRHadTau",    m_sInputFilePathEleOLRHadTau    = "" );
  declareProperty( "InputFilePathEleOLRElectron",  m_sInputFilePathEleOLRElectron  = "" );
  declareProperty( "InputFilePathEleBDTElectron",  m_sInputFilePathEleBDTElectron  = "" );
  declareProperty( "InputFilePathJetIDHadTau",     m_sInputFilePathJetIDHadTau     = "" );
  declareProperty( "InputFilePathContJetIDHadTau", m_sInputFilePathContJetIDHadTau = "" );
  declareProperty( "InputFilePathEleIDHadTau",     m_sInputFilePathEleIDHadTau     = "" );
  declareProperty( "InputFilePathTriggerHadTau",   m_sInputFilePathTriggerHadTau   = "" );
  declareProperty( "VarNameRecoHadTau",            m_sVarNameRecoHadTau            = "" );
  declareProperty( "VarNameEleOLRHadTau",          m_sVarNameEleOLRHadTau          = "" );
  declareProperty( "VarNameEleOLRElectron",        m_sVarNameEleOLRElectron        = "" );
  declareProperty( "VarNameJetIDHadTau",           m_sVarNameJetIDHadTau           = "" );
  declareProperty( "VarNameDecayModeHadTau",       m_sVarNameDecayModeHadTau       = "" );
  declareProperty( "VarNameContJetIDHadTau",       m_sVarNameContJetIDHadTau       = "" );
  declareProperty( "VarNameEleIDHadTau",           m_sVarNameEleIDHadTau           = "" );
  declareProperty( "VarNameTriggerHadTau",         m_sVarNameTriggerHadTau         = "" );
  declareProperty( "RecommendationTag",            m_sRecommendationTag            = "2019-summer" );
  declareProperty( "TriggerName",                  m_sTriggerName                  = "" );
  declareProperty( "TriggerYear",                  m_sTriggerYear                  = "2016" );
  declareProperty( "AutoTriggerYear",              m_bReadRandomRunNumber          = false );
  declareProperty( "TriggerSFMeasurement",         m_sTriggerSFMeasurement         = "combined" ); // "combined", "Ztauttau" or "ttbar"
  declareProperty( "UseTauSubstructure",           m_bUseTauSubstructure           = false );

  declareProperty( "UseIDExclusiveSF",             m_bUseIDExclusiveSF             = false );
  declareProperty( "UseInclusiveEta",              m_bUseInclusiveEta              = false );
  declareProperty( "UseTriggerInclusiveEta",       m_bUseTriggerInclusiveEta       = true );
  declareProperty( "UsePtBinnedSF",                m_bUsePtBinnedSF                = false );
  declareProperty( "UseHighPtUncert",              m_bUseHighPtUncert              = false );
  declareProperty( "IDLevel",                      m_iIDLevel                      = (int)JETIDBDTTIGHT );
  declareProperty( "OLRLevel",                     m_iOLRLevel                     = (int)OLRNONE );
  declareProperty( "ContSysType",                  m_iContSysType                  = (int)TOTAL );
  declareProperty( "TriggerPeriodBinning",         m_iTriggerPeriodBinning         = (int)PeriodBinningAll );
  declareProperty( "MCCampaign",                   m_sMCCampaign                   = "" ); // MC16a, MC16d or MC16e
  declareProperty( "isAFII",	                   m_sAFII	                   = false );

  declareProperty( "SkipTruthMatchCheck",          m_bSkipTruthMatchCheck          = false );

  declareProperty( "TauSelectionTool",             m_tTauSelectionToolHandle );
  declareProperty( "PileupReweightingTool",        m_tPRWTool);

  declareProperty( "EventInfoName",                m_sEventInfoName                = "EventInfo" );
}

//______________________________________________________________________________
TauEfficiencyCorrectionsTool::TauEfficiencyCorrectionsTool( const std::string& sName, TauSelectionTool* tTauSelectionTool )
  : TauEfficiencyCorrectionsTool( sName)
{
  ATH_MSG_WARNING("It is deprecated to pass the TauSelectionTool via the TauEfficiencyCorrectionsTool contructor. This functionality will be removed in the future without further notice. Please pass the TauSelectionTool as a ToolHandle via the property \"TauSelectionTool\".\nFor further information please refer to the README:\nhttps://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauEfficiencyCorrectionsTool.rst");
  m_tTauSelectionTool = tTauSelectionTool;
}

//______________________________________________________________________________
TauEfficiencyCorrectionsTool::~TauEfficiencyCorrectionsTool()
{
  for (auto tTool : m_vCommonEfficiencyTools)
    delete tTool;
  for (auto tTool : m_vTriggerEfficiencyTools)
    delete tTool;
}

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initializeWithTauSelectionTool()
{
  // Greet the user:
  ATH_MSG_DEBUG( "Initializing TauEfficiencyCorrectionsTool using TauSelectionTool" );

  // configure the tool depending on cuts used in the TauSelectionTool if
  // EfficiencyCorrectionTypes have not been set
  if (m_sRecommendationTag == "2019-summer" and m_vEfficiencyCorrectionTypes.size() == 0)
  {
    // add reco systematics in any case
    m_vEfficiencyCorrectionTypes.push_back(SFRecoHadTau);
    ATH_MSG_VERBOSE( "added SFRecoHadTau" );
    // use jet ID scale factors if TauSelectionTool applies jet ID cut and use
    // the same workingpoint
    if (m_tTauSelectionTool->m_iSelectionCuts & CutJetIDWP)
    {
      if ( m_tTauSelectionTool->m_iJetIDWP == JETIDNONE )
        ATH_MSG_VERBOSE( "CutJetIDWP is set but working point is JETIDNONE. No scale factors will be applied for the jet ID." );
      else
      {
        m_iIDLevel = m_tTauSelectionTool->m_iJetIDWP;
        m_vEfficiencyCorrectionTypes.push_back(SFJetIDHadTau);
        ATH_MSG_VERBOSE( "added SFJetIDHadTau" );
      }
    }

    if ((m_tTauSelectionTool->m_iSelectionCuts & CutEleBDTWP && (m_tTauSelectionTool->m_iEleBDTWP != ELEIDNONE))
        and !(m_tTauSelectionTool->m_iSelectionCuts & CutEleOLR && m_tTauSelectionTool->m_bEleOLR)) // re-tuned eleBDT working points
    {
      ATH_MSG_DEBUG("EleBDT");
      if ( m_tTauSelectionTool->m_iEleBDTWP == ELEIDBDTLOOSE)
        m_iOLRLevel = ELEBDTLOOSE;
      else if ( m_tTauSelectionTool->m_iEleBDTWP == ELEIDBDTMEDIUM)
        m_iOLRLevel = ELEBDTMEDIUM;
      else if ( m_tTauSelectionTool->m_iEleBDTWP == ELEIDBDTTIGHT)
        m_iOLRLevel = ELEBDTTIGHT;
      else
        ATH_MSG_ERROR("Recommendations for eleBDT working point with enum " << m_tTauSelectionTool->m_iEleBDTWP << 
          " are not supported in recommendations tag " << m_sRecommendationTag <<
          "\nFor further information please refer to the README:\nhttps://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauEfficiencyCorrectionsTool.rst");
    }
    else if (!(m_tTauSelectionTool->m_iSelectionCuts & CutEleBDTWP && !(m_tTauSelectionTool->m_iEleBDTWP == ELEIDNONE))
             and (m_tTauSelectionTool->m_iSelectionCuts & CutEleOLR && m_tTauSelectionTool->m_bEleOLR)) // old LLH based eVeto
    {
      ATH_MSG_DEBUG("EleVeto");
      m_iOLRLevel = TAUELEOLR;
    }
    else if ((m_tTauSelectionTool->m_iSelectionCuts & CutEleBDTWP && (m_tTauSelectionTool->m_iEleBDTWP != ELEIDNONE))
             and (m_tTauSelectionTool->m_iSelectionCuts & CutEleOLR && m_tTauSelectionTool->m_bEleOLR)) // old BDT+LLH based eVeto
    {
      ATH_MSG_DEBUG("EleBDTPLUSVeto");
      if ( m_tTauSelectionTool->m_iEleBDTWP == ELEIDBDTOLDLOOSE)
        m_iOLRLevel = ELEBDTLOOSEPLUSVETO;
      else if ( m_tTauSelectionTool->m_iEleBDTWP == ELEIDBDTOLDMEDIUM)
        m_iOLRLevel = ELEBDTMEDIUMPLUSVETO;
      else
        ATH_MSG_ERROR("Recommendations for eleBDT working point with enum " << m_tTauSelectionTool->m_iEleBDTWP << 
          " in combination with LLH eVeto are not supported in recommendations tag " << m_sRecommendationTag <<
          "\nFor further information please refer to the README:\nhttps://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauEfficiencyCorrectionsTool.rst"); 
    }

    // use electron OLR scale factors if TauSelectionTool applies electron veto
    if (m_iOLRLevel != OLRNONE)
    {
      m_vEfficiencyCorrectionTypes.push_back(SFEleOLRHadTau);
      ATH_MSG_VERBOSE( "added SFEleOLRHadTau" );
      m_vEfficiencyCorrectionTypes.push_back(SFEleOLRElectron);
      ATH_MSG_VERBOSE( "added SFEleOLRElectron" );
    }

    if (m_bUseTauSubstructure)
      m_vEfficiencyCorrectionTypes.push_back(SFDecayMode);

  }
  else if ((m_sRecommendationTag == "2018-summer" or
       m_sRecommendationTag == "mc16-prerec" or
       m_sRecommendationTag == "2017-moriond" or
       m_sRecommendationTag == "2016-fall" or
       m_sRecommendationTag == "2016-ichep" or
       m_sRecommendationTag == "mc15-moriond" or
       m_sRecommendationTag == "mc15-pre-recommendations") and m_vEfficiencyCorrectionTypes.size() == 0)
  {
    // add reco systematics in any case
    m_vEfficiencyCorrectionTypes.push_back(SFRecoHadTau);
    ATH_MSG_VERBOSE( "added SFRecoHadTau" );
    // use jet ID scale factors if TauSelectionTool applies jet ID cut and use
    // the same workingpoint
    if (m_tTauSelectionTool->m_iSelectionCuts & CutJetIDWP)
    {
      if ( m_tTauSelectionTool->m_iJetIDWP == JETIDNONE )
        ATH_MSG_VERBOSE( "CutJetIDWP is set but working point is JETIDNONE. No scale factors will be applied for the jet ID." );
      else
      {
        m_iIDLevel = m_tTauSelectionTool->m_iJetIDWP;
        m_vEfficiencyCorrectionTypes.push_back(SFJetIDHadTau);
        ATH_MSG_VERBOSE( "added SFJetIDHadTau" );
      }
    }

    
    if (m_tTauSelectionTool->m_iSelectionCuts & CutEleOLR
      and !(m_tTauSelectionTool->m_iSelectionCuts & CutEleBDTWP) )
    {
      ATH_MSG_DEBUG("TauEleOLR");
      m_iOLRLevel = TAUELEOLR;
    }
    else if (m_tTauSelectionTool->m_iSelectionCuts & CutEleOLR
      and m_tTauSelectionTool->m_iSelectionCuts & CutEleBDTWP )
    {
      ATH_MSG_DEBUG("TauBDTPLUSTauEleolr");
      if ( m_tTauSelectionTool->m_iEleBDTWP == ELEIDBDTOLDLOOSE)
        m_iOLRLevel = ELEBDTLOOSEPLUSVETO;
      else if ( m_tTauSelectionTool->m_iEleBDTWP == ELEIDBDTOLDMEDIUM)
        m_iOLRLevel = ELEBDTMEDIUMPLUSVETO;
      else
        ATH_MSG_ERROR("Recommendations for eleBDT working point with enum " << m_tTauSelectionTool->m_iEleBDTWP << 
          " in combination with LLH eVeto are not supported in recommendations tag " << m_sRecommendationTag <<
          "\nFor further information please refer to the README:\nhttps://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauEfficiencyCorrectionsTool.rst");
    }
    else if (!(m_tTauSelectionTool->m_iSelectionCuts & CutEleOLR)
      and m_tTauSelectionTool->m_iSelectionCuts & CutEleBDTWP )
    {
      ATH_MSG_DEBUG("TauBDT");
      if ( m_tTauSelectionTool->m_iEleBDTWP == ELEIDBDTOLDLOOSE)
        m_iOLRLevel = ELEBDTLOOSE;      
      else if ( m_tTauSelectionTool->m_iEleBDTWP == ELEIDBDTOLDMEDIUM)
        m_iOLRLevel = ELEBDTMEDIUM;
      else 
        ATH_MSG_ERROR("Recommendations for eleBDT working point with enum " << m_tTauSelectionTool->m_iEleBDTWP << 
          " are not supported in recommendations tag " << m_sRecommendationTag <<
          "\nFor further information please refer to the README:\nhttps://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauEfficiencyCorrectionsTool.rst");
    }

    // use electron OLR scale factors if TauSelectionTool applies electron veto
    if (m_iOLRLevel != OLRNONE)
    {
      m_vEfficiencyCorrectionTypes.push_back(SFEleOLRHadTau);
      ATH_MSG_VERBOSE( "added SFEleOLRHadTau" );
      m_vEfficiencyCorrectionTypes.push_back(SFEleOLRElectron);
      ATH_MSG_VERBOSE( "added SFEleOLRElectron" );
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initialize()
{
  // Greet the user:
  ATH_MSG_INFO( "Initializing TauEfficiencyCorrectionsTool" );

  if (m_bSkipTruthMatchCheck)
    ATH_MSG_WARNING("Truth match check will be skipped. This is ONLY FOR TESTING PURPOSE!");

  if (!m_tPRWTool.empty())
    ATH_CHECK(m_tPRWTool.retrieve());

  if (!m_tTauSelectionToolHandle.empty())
  {

    ATH_CHECK(m_tTauSelectionToolHandle.retrieve());
    m_tTauSelectionTool = dynamic_cast<TauAnalysisTools::TauSelectionTool*>(&*m_tTauSelectionToolHandle);
  }

  if (m_tTauSelectionTool)
    if (initializeWithTauSelectionTool().isFailure())
    {
      ATH_MSG_FATAL("problem in initializing with TauSelectionTool");
      return StatusCode::FAILURE;
    }

  // configure default set of variations if not set by the constructor using TauSelectionTool or the user
  if (m_sRecommendationTag== "2019-summer" and m_vEfficiencyCorrectionTypes.size() == 0)
  {
    m_vEfficiencyCorrectionTypes = {SFRecoHadTau,
                                    SFJetIDHadTau
                                   };
    if (m_iOLRLevel != OLRNONE)
    {
      if (!(m_iOLRLevel==TAUELEOLR or m_iOLRLevel==ELEBDTLOOSEPLUSVETO or
           m_iOLRLevel==ELEBDTMEDIUMPLUSVETO or m_iOLRLevel==ELEBDTLOOSE or
           m_iOLRLevel==ELEBDTMEDIUM or m_iOLRLevel==ELEBDTTIGHT))
      {
        ATH_MSG_ERROR("Recommendations for eleBDT working point with enum " << m_iOLRLevel <<
          " are not supported in recommendations tag " << m_sRecommendationTag <<
          "\nFor further information please refer to the README:\nhttps://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauEfficiencyCorrectionsTool.rst");
      }
      else
      {
        m_vEfficiencyCorrectionTypes.push_back(SFEleOLRHadTau);
        m_vEfficiencyCorrectionTypes.push_back(SFEleOLRElectron);
      }
    }

    if (m_bUseTauSubstructure)
      m_vEfficiencyCorrectionTypes.push_back(SFDecayMode);
  }
  else if ((m_sRecommendationTag== "2018-summer" or m_sRecommendationTag== "mc16-prerec" or m_sRecommendationTag== "2017-moriond" or m_sRecommendationTag == "2016-fall" or m_sRecommendationTag == "2016-ichep" or m_sRecommendationTag == "mc15-moriond") and m_vEfficiencyCorrectionTypes.size() == 0)
  {
    m_vEfficiencyCorrectionTypes = {SFRecoHadTau,
                                    SFJetIDHadTau,
                                    SFEleOLRHadTau,
                                    SFEleOLRElectron
                                   };
  }
  else if (m_sRecommendationTag == "mc15-pre-recommendations" and m_vEfficiencyCorrectionTypes.size() == 0)
    m_vEfficiencyCorrectionTypes = {SFRecoHadTau,
                                    SFJetIDHadTau,
                                    SFEleOLRHadTau
                                   };

  if (m_sRecommendationTag == "2019-summer")
    ATH_CHECK(initializeTools_2019_summer());
  else if (m_sRecommendationTag == "2018-summer")
    ATH_CHECK(initializeTools_2018_summer());
  else if (m_sRecommendationTag == "mc16-prerec")
    ATH_CHECK(initializeTools_mc16_prerec());
  else if (m_sRecommendationTag == "2017-moriond")
    ATH_CHECK(initializeTools_2017_moriond());
  else if (m_sRecommendationTag == "2016-fall")
    ATH_CHECK(initializeTools_2016_fall());
  else if (m_sRecommendationTag == "2016-ichep")
    ATH_CHECK(initializeTools_2016_ichep());
  else if (m_sRecommendationTag == "mc15-moriond")
    ATH_CHECK(initializeTools_mc15_moriond());
  else if (m_sRecommendationTag == "mc15-pre-recommendations")
    ATH_CHECK(initializeTools_mc15_pre_recommendations());
  else
  {
    ATH_MSG_FATAL("Unknown RecommendationTag: "<<m_sRecommendationTag);
    return StatusCode::FAILURE;
  }

  // for (auto tCommonEfficiencyTool : m_vCommonEfficiencyTools)
  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
  {
    ATH_CHECK((**it).setProperty("OutputLevel", this->msg().level()));
    ATH_CHECK((**it).initialize());
  }
  for (auto it = m_vTriggerEfficiencyTools.begin(); it != m_vTriggerEfficiencyTools.end(); it++)
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

StatusCode TauEfficiencyCorrectionsTool::beginEvent()
{
  if (!m_bIsConfigured)
  {
    const xAOD::EventInfo* xEventInfo = 0;
    ATH_CHECK(evtStore()->retrieve(xEventInfo,"EventInfo"));
    m_bIsData = !(xEventInfo->eventType( xAOD::EventInfo::IS_SIMULATION));
    m_bIsConfigured=true;
  }

  if (m_bIsData)
    return StatusCode::SUCCESS;

  const xAOD::EventInfo* xEventInfo = 0;
  ATH_CHECK(evtStore()->retrieve(xEventInfo, m_sEventInfoName ));
  m_iMu = xEventInfo->averageInteractionsPerCrossing();
  if (xEventInfo->runNumber()==284500)
  {
    unsigned int tmp_mu = xEventInfo->averageInteractionsPerCrossing();
    set_mu(tmp_mu);
  }
  else if (xEventInfo->runNumber()==300000 || xEventInfo->runNumber()==310000)
  {
    unsigned int tmp_mu = xEventInfo->actualInteractionsPerCrossing();
    set_mu(tmp_mu);
  }
  else
    ANA_MSG_WARNING( "Could not determine MC campaign from run number! The mu dependent systematic of the trigger scale factors should not be trusted." );
  
  if (m_bReadRandomRunNumber)
  {
    // Reset the number at the beginning of event
    m_iRunNumber = 0;
    return StatusCode::SUCCESS;
  }

  if (m_tPRWTool.empty())
    return StatusCode::SUCCESS;

  m_iRunNumber = m_tPRWTool->getRandomRunNumber(*xEventInfo);
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
void TauEfficiencyCorrectionsTool::printConfig(bool bAlways)
{
  if (bAlways)
  {
    ATH_MSG_DEBUG( "TauEfficiencyCorrectionsTool with name " << name() << " is configured as follows:" );
    for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
      ATH_MSG_ALWAYS( "  EfficiencyCorrectionTypes " << iEfficiencyCorrectionType );
    ATH_MSG_ALWAYS( "  InputFilePathRecoHadTau " << m_sInputFilePathRecoHadTau );
    ATH_MSG_ALWAYS( "  InputFilePathEleOLRHadTau " << m_sInputFilePathEleOLRHadTau );
    ATH_MSG_ALWAYS( "  InputFilePathJetIDHadTau " << m_sInputFilePathJetIDHadTau );
    ATH_MSG_ALWAYS( "  InputFilePathContJetIDHadTau " << m_sInputFilePathContJetIDHadTau );
    ATH_MSG_ALWAYS( "  InputFilePathEleIDHadTau " << m_sInputFilePathEleIDHadTau );
    ATH_MSG_ALWAYS( "  InputFilePathDecayModeHadTau " << m_sInputFilePathDecayModeHadTau );
    ATH_MSG_ALWAYS( "  InputFilePathTriggerHadTau " << m_sInputFilePathTriggerHadTau );
    ATH_MSG_ALWAYS( "  VarNameRecoHadTau " << m_sVarNameRecoHadTau );
    ATH_MSG_ALWAYS( "  VarNameEleOLRHadTau " << m_sVarNameEleOLRHadTau );
    ATH_MSG_ALWAYS( "  VarNameJetIDHadTau " << m_sVarNameJetIDHadTau );
    ATH_MSG_ALWAYS( "  VarNameDecayModeHadTau " << m_sVarNameDecayModeHadTau );
    ATH_MSG_ALWAYS( "  VarNameContJetIDHadTau " << m_sVarNameContJetIDHadTau );
    ATH_MSG_ALWAYS( "  VarNameEleIDHadTau " << m_sVarNameEleIDHadTau );
    ATH_MSG_ALWAYS( "  VarNameTriggerHadTau " << m_sVarNameTriggerHadTau );
    ATH_MSG_ALWAYS( "  RecommendationTag " << m_sRecommendationTag );
    ATH_MSG_ALWAYS( "  TriggerName " << m_sTriggerName );
    ATH_MSG_ALWAYS( "  TriggerYear " << m_sTriggerYear );

    ATH_MSG_ALWAYS( "  UseTauSubstructure " << m_bUseTauSubstructure );
    ATH_MSG_ALWAYS( "  UseIDExclusiveSF " << m_bUseIDExclusiveSF );
    ATH_MSG_ALWAYS( "  UseInclusiveEta " << m_bUseInclusiveEta );
    ATH_MSG_ALWAYS( "  UseTriggerInclusiveEta " << m_bUseTriggerInclusiveEta );
    ATH_MSG_ALWAYS( "  UsePtBinnedSF " << m_bUsePtBinnedSF );
    ATH_MSG_ALWAYS( "  UseHighPtUncert " << m_bUseHighPtUncert );
    ATH_MSG_ALWAYS( "  IDLevel " << m_iIDLevel );
    ATH_MSG_ALWAYS( "  OLRLevel " << m_iOLRLevel );
    ATH_MSG_ALWAYS( "  ContSysType " << m_iContSysType );
    ATH_MSG_ALWAYS( "  TriggerPeriodBinning " << m_iTriggerPeriodBinning );
    ATH_MSG_ALWAYS( "  MCCampaign " << m_sMCCampaign );
    ATH_MSG_ALWAYS( "  isAFII " << m_sAFII );
  }
  else
  {
    ATH_MSG_DEBUG( "TauEfficiencyCorrectionsTool with name " << name() << " is configured as follows:" );
    for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
      ATH_MSG_DEBUG( "  EfficiencyCorrectionTypes " << iEfficiencyCorrectionType );
    ATH_MSG_DEBUG( "  InputFilePathRecoHadTau " << m_sInputFilePathRecoHadTau );
    ATH_MSG_DEBUG( "  InputFilePathEleOLRHadTau " << m_sInputFilePathEleOLRHadTau );
    ATH_MSG_DEBUG( "  InputFilePathEleOLRElectron " << m_sInputFilePathEleOLRElectron );
    ATH_MSG_DEBUG( "  InputFilePathEleBDTElectron " << m_sInputFilePathEleBDTElectron );
    ATH_MSG_DEBUG( "  InputFilePathJetIDHadTau " << m_sInputFilePathJetIDHadTau );
    ATH_MSG_DEBUG( "  InputFilePathContJetIDHadTau " << m_sInputFilePathContJetIDHadTau );
    ATH_MSG_DEBUG( "  InputFilePathEleIDHadTau " << m_sInputFilePathEleIDHadTau );
    ATH_MSG_DEBUG( "  InputFilePathDecayModeHadTau " << m_sInputFilePathDecayModeHadTau );
    ATH_MSG_DEBUG( "  InputFilePathTriggerHadTau " << m_sInputFilePathTriggerHadTau );
    ATH_MSG_DEBUG( "  VarNameRecoHadTau " << m_sVarNameRecoHadTau );
    ATH_MSG_DEBUG( "  VarNameEleOLRHadTau " << m_sVarNameEleOLRHadTau );
    ATH_MSG_DEBUG( "  VarNameEleOLRElectron " << m_sVarNameEleOLRElectron );
    ATH_MSG_DEBUG( "  VarNameJetIDHadTau " << m_sVarNameJetIDHadTau );
    ATH_MSG_DEBUG( "  VarNameDecayModeHadTau " << m_sVarNameDecayModeHadTau );
    ATH_MSG_DEBUG( "  VarNameContJetIDHadTau " << m_sVarNameContJetIDHadTau );
    ATH_MSG_DEBUG( "  VarNameEleIDHadTau " << m_sVarNameEleIDHadTau );
    ATH_MSG_DEBUG( "  VarNameTriggerHadTau " << m_sVarNameTriggerHadTau );
    ATH_MSG_DEBUG( "  RecommendationTag " << m_sRecommendationTag );
    ATH_MSG_DEBUG( "  TriggerName " << m_sTriggerName );
    ATH_MSG_DEBUG( "  TriggerYear " << m_sTriggerYear );

    ATH_MSG_DEBUG( "  UseTauSubstructure " << m_bUseTauSubstructure );
    ATH_MSG_DEBUG( "  UseIDExclusiveSF " << m_bUseIDExclusiveSF );
    ATH_MSG_DEBUG( "  UseInclusiveEta " << m_bUseInclusiveEta );
    ATH_MSG_DEBUG( "  UseTriggerInclusiveEta " << m_bUseTriggerInclusiveEta );
    ATH_MSG_DEBUG( "  UsePtBinnedSF " << m_bUsePtBinnedSF );
    ATH_MSG_DEBUG( "  UseHighPtUncert " << m_bUseHighPtUncert );
    ATH_MSG_DEBUG( "  IDLevel " << m_iIDLevel );
    ATH_MSG_DEBUG( "  OLRLevel " << m_iOLRLevel );
    ATH_MSG_DEBUG( "  ContSysType " << m_iContSysType );
    ATH_MSG_DEBUG( "  TriggerPeriodBinning " << m_iTriggerPeriodBinning );
    ATH_MSG_DEBUG( "  MCCampaign " << m_sMCCampaign );
    ATH_MSG_DEBUG( "  isAFII " << m_sAFII );
  }
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyCorrectionsTool::getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
    double& eff, unsigned int /*iRunNumber*/, unsigned int /*iMu*/)
{
  eff = 1.;

  if (m_bIsData)
    return CP::CorrectionCode::Ok;

  ANA_CHECK_SET_TYPE (CP::CorrectionCode);
  ANA_CHECK(readRandomRunNumber());

  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
  {
    double dToolEff = 1.;
    CP::CorrectionCode tmpCorrectionCode = (**it)->getEfficiencyScaleFactor(xTau, dToolEff, m_iRunNumber, m_iMu);
    if (tmpCorrectionCode != CP::CorrectionCode::Ok)
      return tmpCorrectionCode;
    eff *= dToolEff;
  }
  for (auto it = m_vTriggerEfficiencyTools.begin(); it != m_vTriggerEfficiencyTools.end(); it++)
  {
    if ( !(**it)->isSupportedRunNumber(m_iRunNumber) )
      continue;
    double dToolEff = 1.;
    CP::CorrectionCode tmpCorrectionCode = (**it)->getEfficiencyScaleFactor(xTau, dToolEff);
    if (tmpCorrectionCode != CP::CorrectionCode::Ok)
      return tmpCorrectionCode;
    eff *= dToolEff;
  }
  return CP::CorrectionCode::Ok;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyCorrectionsTool::applyEfficiencyScaleFactor( const xAOD::TauJet& xTau, unsigned int /*iRunNumber*/, unsigned int /*iMu*/)
{
  if (m_bIsData)
    return CP::CorrectionCode::Ok;

  ANA_CHECK_SET_TYPE (CP::CorrectionCode);
  ANA_CHECK(readRandomRunNumber());

  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
  {
    CP::CorrectionCode tmpCorrectionCode = (**it)->applyEfficiencyScaleFactor(xTau, m_iRunNumber, m_iMu);
    if (tmpCorrectionCode != CP::CorrectionCode::Ok)
      return tmpCorrectionCode;
  }
  for (auto it = m_vTriggerEfficiencyTools.begin(); it != m_vTriggerEfficiencyTools.end(); it++)
  {
    if ( !(**it)->isSupportedRunNumber(m_iRunNumber) )
      continue;
    CP::CorrectionCode tmpCorrectionCode = (**it)->applyEfficiencyScaleFactor(xTau, m_iRunNumber, m_iMu);
    if (tmpCorrectionCode != CP::CorrectionCode::Ok)
      return tmpCorrectionCode;
  }
  return CP::CorrectionCode::Ok;
}

/// returns: whether this tool is affected by the given systematis
//______________________________________________________________________________
bool TauEfficiencyCorrectionsTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
    if ((**it)->isAffectedBySystematic(systematic))
      return true;
  for (auto it = m_vTriggerEfficiencyTools.begin(); it != m_vTriggerEfficiencyTools.end(); it++)
    if ((**it)->isAffectedBySystematic(systematic))
      return true;
  return false;
}

/// returns: the list of all systematics this tool can be affected by
//______________________________________________________________________________
CP::SystematicSet TauEfficiencyCorrectionsTool::affectingSystematics() const
{
  CP::SystematicSet sAffectingSystematics;
  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
    sAffectingSystematics.insert((**it)->affectingSystematics());
  for (auto it = m_vTriggerEfficiencyTools.begin(); it != m_vTriggerEfficiencyTools.end(); it++)
    sAffectingSystematics.insert((**it)->affectingSystematics());
  return sAffectingSystematics;
}

/// returns: the list of all systematics this tool recommends to use
//______________________________________________________________________________
CP::SystematicSet TauEfficiencyCorrectionsTool::recommendedSystematics() const
{
  CP::SystematicSet sRecommendedSystematics;
  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
    sRecommendedSystematics.insert((**it)->recommendedSystematics());
  for (auto it = m_vTriggerEfficiencyTools.begin(); it != m_vTriggerEfficiencyTools.end(); it++)
    sRecommendedSystematics.insert((**it)->recommendedSystematics());
  return sRecommendedSystematics;
}

//______________________________________________________________________________
CP::SystematicCode TauEfficiencyCorrectionsTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
    if ((**it)->applySystematicVariation(sSystematicSet) == CP::SystematicCode::Unsupported)
      return CP::SystematicCode::Unsupported;
  for (auto it = m_vTriggerEfficiencyTools.begin(); it != m_vTriggerEfficiencyTools.end(); it++)
    if ((**it)->applySystematicVariation(sSystematicSet) == CP::SystematicCode::Unsupported)
      return CP::SystematicCode::Unsupported;
  return CP::SystematicCode::Ok;
}

//=================================PRIVATE-PART=================================

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initializeTools_2019_summer()
{
  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/EfficiencyCorrections/";
  for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
  {
    if (iEfficiencyCorrectionType == SFJetIDHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathJetIDHadTau.empty())
      {
        if (m_iIDLevel == JETIDBDTLOOSE || m_iIDLevel == JETIDBDTMEDIUM || m_iIDLevel == JETIDBDTTIGHT)
        {
          if (m_bUseTauSubstructure)
            m_sInputFilePathJetIDHadTau = sDirectory+"JetIDSubstructure_TrueHadTau_2019-summer.root";
          else
            if (m_sAFII) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_2019-summer_AFII.root";
            else m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_2019-summer.root";
        }
        else if (m_iIDLevel == JETIDRNNLOOSE || m_iIDLevel == JETIDRNNMEDIUM || m_iIDLevel == JETIDRNNTIGHT)
        {
          if (m_sAFII) m_sInputFilePathJetIDHadTau = sDirectory+"RNNID_TrueHadTau_2019-summer_AFII.root";
          else m_sInputFilePathJetIDHadTau = sDirectory+"RNNID_TrueHadTau_2019-summer.root";
        }
        else 
        {
          ATH_MSG_WARNING("Unsupported ID working point with enum "<<m_iIDLevel);
          continue;
        }

      }

      if (m_sVarNameJetIDHadTau.length() == 0) m_sVarNameJetIDHadTau = "TauScaleFactorJetIDHadTau";

      std::string sJetIDWP = ConvertJetIDToString(m_iIDLevel);
      if (sJetIDWP.empty()) 
      {
        ATH_MSG_WARNING("Could not find valid ID working point. Skip ID efficiency corrections.");
        continue;
      }

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/JetIDHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathJetIDHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameJetIDHadTau));
      ATH_CHECK(tTool->setProperty("UseTauSubstructure", m_bUseTauSubstructure));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", sJetIDWP));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRHadTau.empty()) m_sInputFilePathEleOLRHadTau = sDirectory+"EleOLR_TrueHadTau_2016-ichep.root";
      if (m_sVarNameEleOLRHadTau.length() == 0) m_sVarNameEleOLRHadTau = "TauScaleFactorEleOLRHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/EleOLRHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRElectron)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRElectron.empty()) m_sInputFilePathEleOLRElectron = sDirectory+"EleOLR_TrueElectron_2019-summer.root";
      if (m_sInputFilePathEleBDTElectron.empty())
      {
        m_sInputFilePathEleBDTElectron = sDirectory+"EleBDT_TrueElectron_2019-summer.root";
	// sets input file for AFII recommendations if tool is configured to do so.
	// configuration is cross-checked in beginInputFile() using meta data
        if (m_sAFII) m_sInputFilePathEleBDTElectron = sDirectory+"EleBDT_TrueElectron_2019-summer_AFII.root";
      }
      if (m_sVarNameEleOLRElectron.length() == 0) m_sVarNameEleOLRElectron = "TauScaleFactorEleOLRElectron";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/EleOLRElectronTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRElectron));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertEleOLRToString(m_iOLRLevel)));

      if (m_iOLRLevel == ELEBDTLOOSE || m_iOLRLevel == ELEBDTMEDIUM || (m_iOLRLevel == ELEBDTTIGHT && !m_sAFII) )
      {
        ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleBDTElectron));
        ATH_CHECK(tTool->setProperty("UseTauSubstructure", true));
      }
      else
      {
        ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRElectron));
      }

    }
    else if (iEfficiencyCorrectionType == SFRecoHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathRecoHadTau.empty()) m_sInputFilePathRecoHadTau = sDirectory+"Reco_TrueHadTau_2019-summer.root";
      if (m_sVarNameRecoHadTau.length() == 0) m_sVarNameRecoHadTau = "TauScaleFactorReconstructionHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/RecoHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathRecoHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameRecoHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFDecayMode)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathDecayModeHadTau.empty()) m_sInputFilePathDecayModeHadTau = sDirectory+"DecayModeSubstructure_TrueHadTau_2019-summer.root";
      if (m_sVarNameDecayModeHadTau.length() == 0) m_sVarNameDecayModeHadTau = "TauScaleFactorDecayModeHadTau";

      std::string sJetIDWP = ConvertJetIDToString(m_iIDLevel);
      if (sJetIDWP.empty()) 
      {
        ATH_MSG_WARNING("Could not find valid ID working point. Skip ID efficiency corrections.");
        continue;
      }

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("DecayModeHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathDecayModeHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameDecayModeHadTau));
      ATH_CHECK(tTool->setProperty("UseTauSubstructure", true));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", sJetIDWP));
    }

    else if (iEfficiencyCorrectionType == SFTriggerHadTau)
    {
      std::string id;
      if ((m_iIDLevel >= 2 && m_iIDLevel <= 4) || (m_iIDLevel >= 11 && m_iIDLevel <= 15)) id = "BDT";
      if (m_iIDLevel >= 19 && m_iIDLevel <= 22) id = "RNN";
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathTriggerHadTau.empty())
      {
        if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
        if (m_bUseTriggerInclusiveEta) 
        {
	  // Determine the full input file name from the given trigger name.
	  // Triggers having "mediumRNN_tracktwoMVA are only part of 2018aftTS1.
	  // Every other trigger having "tracktwoEF" is only part of 2018.
	  // Every other trigger having "tau160_medium1" is only part of 2016.
	  // Every other trigger having "tau160" is only part of 2017/2018.
	  // Lastly check for other possible triggers, if this is not fulfilled the passed trigger is not supported.
          if (m_sTriggerName.find("mediumRNN_tracktwoMVA") != std::string::npos)
            m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/"+id+"/Trigger_TrueHadTau_2019-summer_data2018aftTS1"+GetTriggerSFMeasrementString()+m_sTriggerName+".root";
          else if (m_sTriggerName.find("tracktwoEF") != std::string::npos)
          {
            m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/"+id+"/Trigger_TrueHadTau_2019-summer_data2018"+GetTriggerSFMeasrementString()+m_sTriggerName+".root";
          }
          else if (m_sTriggerName.find("tau160_medium1") != std::string::npos)
          {
            m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/"+id+"/Trigger_TrueHadTau_2019-summer_data2016"+GetTriggerSFMeasrementString()+m_sTriggerName+".root";
          }
          else if ((m_sTriggerName.find("tau160") != std::string::npos) || (m_sTriggerName.find("tau60") != std::string::npos))
          {
            m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/"+id+"/Trigger_TrueHadTau_2019-summer_data1718"+GetTriggerSFMeasrementString()+m_sTriggerName+".root";
          }
          else if ((m_sTriggerName.find("tau125") != std::string::npos) || (m_sTriggerName.find("tau25") != std::string::npos) || (m_sTriggerName.find("tau35") != std::string::npos) || (m_sTriggerName.find("tau50") != std::string::npos) || (m_sTriggerName.find("tau80") != std::string::npos) )
          {
            m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/"+id+"/Trigger_TrueHadTau_2019-summer_data161718"+GetTriggerSFMeasrementString()+m_sTriggerName+".root";
          }
          else 
            ATH_MSG_ERROR("Your chosen trigger does not seem to be supported. Please set property \"TriggerName\" accordingly.");
        }
        else
        {
          ATH_MSG_ERROR("eta exclusive scale factors not available");
          return StatusCode::FAILURE;
        }
      }
      if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool", this);
      m_vTriggerEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathTriggerHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameTriggerHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
      ATH_CHECK(tTool->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
    }
    else
    {
      ATH_MSG_WARNING("unsupported EfficiencyCorrectionsType with enum "<<iEfficiencyCorrectionType);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TauEfficiencyCorrectionsTool::initializeTools_2018_summer()
{
  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/EfficiencyCorrections/";
  for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
  {
    if (iEfficiencyCorrectionType == SFJetIDHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathJetIDHadTau.empty()) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_2018-summer.root";
      if (m_sVarNameJetIDHadTau.length() == 0) m_sVarNameJetIDHadTau = "TauScaleFactorJetIDHadTau";

      std::string sJetIDWP = ConvertJetIDToString(m_iIDLevel);
      if (sJetIDWP.empty()) 
      {
        ATH_MSG_WARNING("Could not find valid ID working point. Skip ID efficiency corrections.");
        continue;
      }

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/JetIDHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathJetIDHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameJetIDHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", sJetIDWP));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRHadTau.empty()) m_sInputFilePathEleOLRHadTau = sDirectory+"EleOLR_TrueHadTau_2016-ichep.root";
      if (m_sVarNameEleOLRHadTau.length() == 0) m_sVarNameEleOLRHadTau = "TauScaleFactorEleOLRHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/EleOLRHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRElectron)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRElectron.empty()) m_sInputFilePathEleOLRElectron = sDirectory+"EleOLR_TrueElectron_2018-summer.root";
      if (m_sInputFilePathEleBDTElectron.empty()) m_sInputFilePathEleBDTElectron = sDirectory+"EleBDT_TrueElectron_2018-summer.root";
      if (m_sVarNameEleOLRElectron.length() == 0) m_sVarNameEleOLRElectron = "TauScaleFactorEleOLRElectron";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/EleOLRElectronTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRElectron));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertEleOLRToString(m_iOLRLevel)));
      
      if (m_iOLRLevel == TAUELEOLR)
      {
        ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRElectron));
        ATH_CHECK(tTool->setProperty("SplitMu", true));
      }
      else if (m_iOLRLevel == ELEBDTLOOSE || m_iOLRLevel == ELEBDTLOOSEPLUSVETO || 
        m_iOLRLevel == ELEBDTMEDIUM || m_iOLRLevel == ELEBDTMEDIUMPLUSVETO )
      {
        ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleBDTElectron));
        ATH_CHECK(tTool->setProperty("SplitMCCampaign", true));
        ATH_CHECK(tTool->setProperty("MCCampaign", m_sMCCampaign));
        if (m_sMCCampaign == "" && m_tPRWTool.empty())
          ATH_MSG_ERROR("One of these properties has to be set: \"MCCampaign\" or \"PileupReweightingTool\" ");
      }
      else 
      {
        ATH_MSG_ERROR("unsupported electron veto working point: " << ConvertEleOLRToString(m_iOLRLevel) ); 
      }
    }
    else if (iEfficiencyCorrectionType == SFRecoHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathRecoHadTau.empty()) m_sInputFilePathRecoHadTau = sDirectory+"Reco_TrueHadTau_mc16-prerec.root";
      if (m_sVarNameRecoHadTau.length() == 0) m_sVarNameRecoHadTau = "TauScaleFactorReconstructionHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/RecoHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathRecoHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameRecoHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFTriggerHadTau)
    {
      if (!m_bReadRandomRunNumber && m_tPRWTool.empty())   // use single setup
      {
        // only set vars if they differ from "", which means they have been configured by the user
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) 
          {
            if (m_sTriggerYear == "2015")
              m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2015_"+m_sTriggerName+"_etainc.root";
            else if (m_sTriggerYear == "2016")
            {
              m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2016"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
            }
            else if (m_sTriggerYear == "2017")
            {
              m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2017"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
            }
            else 
              ATH_MSG_ERROR("trigger recommendations are only provided for year 2015, 2016 and 2017. Please set property \"TriggerYear\" accordingly.");
          }
          else
          {
            ATH_MSG_ERROR("eta exclusive scale factors not available");
            return StatusCode::FAILURE;
          }
        }
        if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool", this);
        m_vTriggerEfficiencyTools.push_back(tTool);
        ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
      }
      else                      // setup two tools
      {
        if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

        // 2015 data
        std::string sInputFilePathTriggerHadTau("");
        if (m_sTriggerName != "HLT_tau160_medium1_tracktwo")
        {
          // only set vars if they differ from "", which means they have been configured by the user
          if (m_sInputFilePathTriggerHadTau.empty())
          {
            if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
            if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2015_"+m_sTriggerName+"_etainc.root";
            else
            {
              ATH_MSG_ERROR("eta exclusive scale factors not available");
              return StatusCode::FAILURE;
            }
          }
          else
            sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

          asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2015 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool_2015", this);
          m_vTriggerEfficiencyTools.push_back(tTool_2015);
          ATH_CHECK(tTool_2015->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
          ATH_CHECK(tTool_2015->setProperty("VarName", m_sVarNameTriggerHadTau));
          ATH_CHECK(tTool_2015->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
          ATH_CHECK(tTool_2015->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
          ATH_CHECK(tTool_2015->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
          ATH_CHECK(tTool_2015->setProperty("MaxRunNumber", 284484));
        }

        // 2016 data
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2016"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
          else
          {
            ATH_MSG_ERROR("eta exclusive scale factors not available");
            return StatusCode::FAILURE;
          }
        }
        else
          sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2016 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool_2016", this);
        m_vTriggerEfficiencyTools.push_back(tTool_2016);
        ATH_CHECK(tTool_2016->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool_2016->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool_2016->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool_2016->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool_2016->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
        ATH_CHECK(tTool_2016->setProperty("MinRunNumber", 296939));
        ATH_CHECK(tTool_2016->setProperty("MaxRunNumber", 311481));

        // 2017 data
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2017"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
          else
          {
            ATH_MSG_ERROR("eta exclusive scale factors not available");
            return StatusCode::FAILURE;
          }
        }
        else
          sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2017 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool_2017", this);
        m_vTriggerEfficiencyTools.push_back(tTool_2017);
        ATH_CHECK(tTool_2017->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool_2017->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool_2017->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool_2017->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool_2017->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
        ATH_CHECK(tTool_2017->setProperty("MinRunNumber", 324320));
      }
    }
    else
    {
      ATH_MSG_WARNING("unsupported EfficiencyCorrectionsType with enum "<<iEfficiencyCorrectionType);
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initializeTools_mc16_prerec()
{
  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/EfficiencyCorrections/";
  for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
  {
    if (iEfficiencyCorrectionType == SFJetIDHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathJetIDHadTau.empty()) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_mc16-prerec.root";
      if (m_sVarNameJetIDHadTau.length() == 0) m_sVarNameJetIDHadTau = "TauScaleFactorJetIDHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/JetIDHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathJetIDHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameJetIDHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertJetIDToString(m_iIDLevel)));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRHadTau.empty()) m_sInputFilePathEleOLRHadTau = sDirectory+"EleOLR_TrueHadTau_2016-ichep.root";
      if (m_sVarNameEleOLRHadTau.length() == 0) m_sVarNameEleOLRHadTau = "TauScaleFactorEleOLRHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/EleOLRHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRElectron)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRElectron.empty()) m_sInputFilePathEleOLRElectron = sDirectory+"EleOLR_TrueElectron_2017-moriond.root";
      if (m_sVarNameEleOLRElectron.length() == 0) m_sVarNameEleOLRElectron = "TauScaleFactorEleOLRElectron";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/EleOLRElectronTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRElectron));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRElectron));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertEleOLRToString(m_iOLRLevel)));
    }
    else if (iEfficiencyCorrectionType == SFRecoHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathRecoHadTau.empty()) m_sInputFilePathRecoHadTau = sDirectory+"Reco_TrueHadTau_mc16-prerec.root";
      if (m_sVarNameRecoHadTau.length() == 0) m_sVarNameRecoHadTau = "TauScaleFactorReconstructionHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/RecoHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathRecoHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameRecoHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFTriggerHadTau)
    {
      if (!m_bReadRandomRunNumber && m_tPRWTool.empty())   // use single setup
      {
        // only set vars if they differ from "", which means they have been configured by the user
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) 
          {
            if (m_sTriggerYear == "2015")
              m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2015_"+m_sTriggerName+"_etainc.root";
            else if (m_sTriggerYear == "2016")
            {
              m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2016"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
            }
            else if (m_sTriggerYear == "2017")
            {
              m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2017"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
            }
            else 
              ATH_MSG_ERROR("trigger recommendations are only provided for year 2015, 2016 and 2017. Please set property \"TriggerYear\" accordingly.");
          }
          else
          {
            ATH_MSG_ERROR("eta exclusive scale factors not available");
            return StatusCode::FAILURE;
          }
        }
        if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool", this);
        m_vTriggerEfficiencyTools.push_back(tTool);
        // ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::TauEfficiencyTriggerTool));
        ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
      }
      else // setup multiple tools
      {
        if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

        // 2015 data
        std::string sInputFilePathTriggerHadTau("");
        if (m_sTriggerName != "HLT_tau160_medium1_tracktwo")
        {
          // only set vars if they differ from "", which means they have been configured by the user
          if (m_sInputFilePathTriggerHadTau.empty())
          {
            if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
            if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2015_"+m_sTriggerName+"_etainc.root";
            else
            {
              ATH_MSG_ERROR("eta exclusive scale factors not available");
              return StatusCode::FAILURE;
            }
          }
          else
            sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

          asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2015 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool_2015", this);
          m_vTriggerEfficiencyTools.push_back(tTool_2015);
          ATH_CHECK(tTool_2015->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
          ATH_CHECK(tTool_2015->setProperty("VarName", m_sVarNameTriggerHadTau));
          ATH_CHECK(tTool_2015->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
          ATH_CHECK(tTool_2015->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
          ATH_CHECK(tTool_2015->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
          ATH_CHECK(tTool_2015->setProperty("MaxRunNumber", 284484));
        }

        // 2016 data
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2016"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
          else
          {
            ATH_MSG_ERROR("eta exclusive scale factors not available");
            return StatusCode::FAILURE;
          }
        }
        else
          sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2016 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool_2016", this);
        m_vTriggerEfficiencyTools.push_back(tTool_2016);
        ATH_CHECK(tTool_2016->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool_2016->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool_2016->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool_2016->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool_2016->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
        ATH_CHECK(tTool_2016->setProperty("MinRunNumber", 296939));
        ATH_CHECK(tTool_2016->setProperty("MaxRunNumber", 311481));

        // 2017 data
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2017"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
          else
          {
            ATH_MSG_ERROR("eta exclusive scale factors not available");
            return StatusCode::FAILURE;
          }
        }
        else
          sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2017 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool_2017", this);
        m_vTriggerEfficiencyTools.push_back(tTool_2017);
        ATH_CHECK(tTool_2017->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool_2017->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool_2017->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool_2017->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool_2017->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
        ATH_CHECK(tTool_2017->setProperty("MinRunNumber", 324320));
      }
    }
    else
    {
      ATH_MSG_WARNING("unsupported EfficiencyCorrectionsType with enum "<<iEfficiencyCorrectionType);
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initializeTools_2017_moriond()
{
  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/EfficiencyCorrections/";
  for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
  {
    if (iEfficiencyCorrectionType == SFJetIDHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathJetIDHadTau.empty()) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_2017-moriond.root";
      if (m_sVarNameJetIDHadTau.length() == 0) m_sVarNameJetIDHadTau = "TauScaleFactorJetIDHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/JetIDHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathJetIDHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameJetIDHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertJetIDToString(m_iIDLevel)));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRHadTau.empty()) m_sInputFilePathEleOLRHadTau = sDirectory+"EleOLR_TrueHadTau_2016-ichep.root";
      if (m_sVarNameEleOLRHadTau.length() == 0) m_sVarNameEleOLRHadTau = "TauScaleFactorEleOLRHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/EleOLRHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRElectron)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRElectron.empty()) m_sInputFilePathEleOLRElectron = sDirectory+"EleOLR_TrueElectron_2017-moriond.root";
      if (m_sVarNameEleOLRElectron.length() == 0) m_sVarNameEleOLRElectron = "TauScaleFactorEleOLRElectron";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/EleOLRElectronTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRElectron));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRElectron));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertEleOLRToString(m_iOLRLevel)));
    }
    else if (iEfficiencyCorrectionType == SFRecoHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathRecoHadTau.empty()) m_sInputFilePathRecoHadTau = sDirectory+"Reco_TrueHadTau_2016-ichep.root";
      if (m_sVarNameRecoHadTau.length() == 0) m_sVarNameRecoHadTau = "TauScaleFactorReconstructionHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/RecoHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathRecoHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameRecoHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFTriggerHadTau)
    {
      if (!m_bReadRandomRunNumber && m_tPRWTool.empty())   // use single setup
      {
        // only set vars if they differ from "", which means they have been configured by the user
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) 
          {
            if (m_sTriggerYear == "2015")
              m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2015_"+m_sTriggerName+"_etainc.root";
            else if (m_sTriggerYear == "2016")
            {
              m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2016"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
            }
            else if (m_sTriggerYear == "2017")
            {
              m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2017"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
            }
            else 
              ATH_MSG_ERROR("trigger recommendations are only provided for year 2015, 2016 and 2017. Please set property \"TriggerYear\" accordingly.");
          }
          else
          {
            ATH_MSG_ERROR("eta exclusive scale factors not available");
            return StatusCode::FAILURE;
          }
        }
        if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool", this);
        m_vTriggerEfficiencyTools.push_back(tTool);
        ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
      }
      else // setup multiple tools
      {
        if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

        // 2015 data
        std::string sInputFilePathTriggerHadTau("");
        if (m_sTriggerName != "HLT_tau160_medium1_tracktwo")
        {
          // only set vars if they differ from "", which means they have been configured by the user
          if (m_sInputFilePathTriggerHadTau.empty())
          {
            if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
            if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2015_"+m_sTriggerName+"_etainc.root";
            else
            {
              ATH_MSG_ERROR("eta exclusive scale factors not available");
              return StatusCode::FAILURE;
            }
          }
          else
            sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

          asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2015 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool_2015", this);
          m_vTriggerEfficiencyTools.push_back(tTool_2015);
          ATH_CHECK(tTool_2015->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
          ATH_CHECK(tTool_2015->setProperty("VarName", m_sVarNameTriggerHadTau));
          ATH_CHECK(tTool_2015->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
          ATH_CHECK(tTool_2015->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
          ATH_CHECK(tTool_2015->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
          ATH_CHECK(tTool_2015->setProperty("MaxRunNumber", 284484));
        }

        // 2016 data
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2016"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
          else
          {
            ATH_MSG_ERROR("eta exclusive scale factors not available");
            return StatusCode::FAILURE;
          }
        }
        else
          sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2016 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool_2016", this);
        m_vTriggerEfficiencyTools.push_back(tTool_2016);
        ATH_CHECK(tTool_2016->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool_2016->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool_2016->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool_2016->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool_2016->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
        ATH_CHECK(tTool_2016->setProperty("MinRunNumber", 296939));
        ATH_CHECK(tTool_2016->setProperty("MaxRunNumber", 311481));

        // 2017 data
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2017-moriond_data2017"+GetTriggerSFMeasrementString()+m_sTriggerName+"_etainc.root";
          else
          {
            ATH_MSG_ERROR("eta exclusive scale factors not available");
            return StatusCode::FAILURE;
          }
        }
        else
          sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2017 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TriggerHadTauTool_2017", this);
        m_vTriggerEfficiencyTools.push_back(tTool_2017);
        ATH_CHECK(tTool_2017->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool_2017->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool_2017->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool_2017->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool_2017->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
        ATH_CHECK(tTool_2017->setProperty("MinRunNumber", 324320));
      }
    }
    else
    {
      ATH_MSG_WARNING("unsupported EfficiencyCorrectionsType with enum "<<iEfficiencyCorrectionType);
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initializeTools_2016_fall()
{
  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/EfficiencyCorrections/";
  for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
  {
    if (iEfficiencyCorrectionType == SFJetIDHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathJetIDHadTau.empty()) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_2016-ichep.root";
      if (m_sVarNameJetIDHadTau.length() == 0) m_sVarNameJetIDHadTau = "TauScaleFactorJetIDHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("JetIDHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathJetIDHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameJetIDHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertJetIDToString(m_iIDLevel)));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRHadTau.empty()) m_sInputFilePathEleOLRHadTau = sDirectory+"EleOLR_TrueHadTau_2016-ichep.root";
      if (m_sVarNameEleOLRHadTau.length() == 0) m_sVarNameEleOLRHadTau = "TauScaleFactorEleOLRHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("EleOLRHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRElectron)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRElectron.empty()) m_sInputFilePathEleOLRElectron = sDirectory+"EleOLR_TrueElectron_2016-ichep.root";
      if (m_sVarNameEleOLRElectron.length() == 0) m_sVarNameEleOLRElectron = "TauScaleFactorEleOLRElectron";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("EleOLRElectronTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRElectron));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRElectron));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertEleOLRToString(m_iOLRLevel)));
    }
    else if (iEfficiencyCorrectionType == SFRecoHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathRecoHadTau.empty()) m_sInputFilePathRecoHadTau = sDirectory+"Reco_TrueHadTau_2016-ichep.root";
      if (m_sVarNameRecoHadTau.length() == 0) m_sVarNameRecoHadTau = "TauScaleFactorReconstructionHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("RecoHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathRecoHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameRecoHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFTriggerHadTau)
    {
      if (!m_bReadRandomRunNumber && m_tPRWTool.empty())   // use single setup
      {
        // only set vars if they differ from "", which means they have been configured by the user
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2016-fall_data"+m_sTriggerYear+"_"+m_sTriggerName+"_etainc.root";
          else
          {
            ATH_MSG_ERROR("eta exclusive scale factors not available");
            return StatusCode::FAILURE;
          }
        }
        if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TriggerHadTauTool", this);
        m_vTriggerEfficiencyTools.push_back(tTool);
        ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::TauEfficiencyTriggerTool));
        ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
      }
      else                      // setup two tools
      {
        if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

        std::string sInputFilePathTriggerHadTau("");
        if (m_sTriggerName != "HLT_tau160_medium1_tracktwo")
        {
          // only set vars if they differ from "", which means they have been configured by the user
          if (m_sInputFilePathTriggerHadTau.empty())
          {
            if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
            if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2016-fall_data2015_"+m_sTriggerName+"_etainc.root";
            else
            {
              ATH_MSG_ERROR("eta exclusive scale factors not available");
              return StatusCode::FAILURE;
            }
          }
          else
            sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

          asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2015 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TriggerHadTauTool_2015", this);
          m_vTriggerEfficiencyTools.push_back(tTool_2015);
          ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool_2015, TauAnalysisTools::TauEfficiencyTriggerTool));
          ATH_CHECK(tTool_2015->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
          ATH_CHECK(tTool_2015->setProperty("VarName", m_sVarNameTriggerHadTau));
          ATH_CHECK(tTool_2015->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
          ATH_CHECK(tTool_2015->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
          ATH_CHECK(tTool_2015->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
          ATH_CHECK(tTool_2015->setProperty("MaxRunNumber", 284484));
        }

        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2016-fall_data2016_"+m_sTriggerName+"_etainc.root";
          else
          {
            ATH_MSG_ERROR("eta exclusive scale factors not available");
            return StatusCode::FAILURE;
          }
        }
        else
          sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2016 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TriggerHadTauTool_2016", this);
        m_vTriggerEfficiencyTools.push_back(tTool_2016);
        ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool_2016, TauAnalysisTools::TauEfficiencyTriggerTool));
        ATH_CHECK(tTool_2016->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool_2016->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool_2016->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool_2016->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool_2016->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
        ATH_CHECK(tTool_2016->setProperty("MinRunNumber", 296939));
      }
    }
    else
    {
      ATH_MSG_WARNING("unsupported EfficiencyCorrectionsType with enum "<<iEfficiencyCorrectionType);
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initializeTools_2016_ichep()
{
  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/EfficiencyCorrections/";
  for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
  {
    if (iEfficiencyCorrectionType == SFJetIDHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathJetIDHadTau.empty()) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_2016-ichep.root";
      if (m_sVarNameJetIDHadTau.length() == 0) m_sVarNameJetIDHadTau = "TauScaleFactorJetIDHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("JetIDHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathJetIDHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameJetIDHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertJetIDToString(m_iIDLevel)));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRHadTau.empty()) m_sInputFilePathEleOLRHadTau = sDirectory+"EleOLR_TrueHadTau_2016-ichep.root";
      if (m_sVarNameEleOLRHadTau.length() == 0) m_sVarNameEleOLRHadTau = "TauScaleFactorEleOLRHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("EleOLRHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRElectron)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRElectron.empty()) m_sInputFilePathEleOLRElectron = sDirectory+"EleOLR_TrueElectron_2016-ichep.root";
      if (m_sVarNameEleOLRElectron.length() == 0) m_sVarNameEleOLRElectron = "TauScaleFactorEleOLRElectron";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("EleOLRElectronTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRElectron));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRElectron));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertEleOLRToString(m_iOLRLevel)));
    }
    else if (iEfficiencyCorrectionType == SFRecoHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathRecoHadTau.empty()) m_sInputFilePathRecoHadTau = sDirectory+"Reco_TrueHadTau_2016-ichep.root";
      if (m_sVarNameRecoHadTau.length() == 0) m_sVarNameRecoHadTau = "TauScaleFactorReconstructionHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("RecoHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathRecoHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameRecoHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFTriggerHadTau)
    {
      if (!m_bReadRandomRunNumber && m_tPRWTool.empty())   // use single setup
      {
        // only set vars if they differ from "", which means they have been configured by the user
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2016-ichep_data"+m_sTriggerYear+"_"+m_sTriggerName+"_etainc.root";
          else m_sInputFilePathTriggerHadTau                           = sDirectory+"Trigger/Trigger_TrueHadTau_2016-ichep_data"+m_sTriggerYear+"_"+m_sTriggerName+"_etaexcl.root";
        }
        if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TriggerHadTauTool", this);
        m_vTriggerEfficiencyTools.push_back(tTool);
        ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::TauEfficiencyTriggerTool));
        ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
      }
      else                      // setup two tools
      {
        if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

        // only set vars if they differ from "", which means they have been configured by the user
        std::string sInputFilePathTriggerHadTau("");
        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2016-ichep_data2015_"+m_sTriggerName+"_etainc.root";
          else                           sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2016-ichep_data2015_"+m_sTriggerName+"_etaexcl.root";
        }
        else
          sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2015 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TriggerHadTauTool_2015", this);
        m_vTriggerEfficiencyTools.push_back(tTool_2015);
        ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool_2015, TauAnalysisTools::TauEfficiencyTriggerTool));
        ATH_CHECK(tTool_2015->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool_2015->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool_2015->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool_2015->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool_2015->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
        // ATH_CHECK(tTool_2015->setProperty("MinRunNumber", 266904));
        ATH_CHECK(tTool_2015->setProperty("MaxRunNumber", 284484));

        if (m_sInputFilePathTriggerHadTau.empty())
        {
          if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
          if (m_bUseTriggerInclusiveEta) sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2016-ichep_data2016_"+m_sTriggerName+"_etainc.root";
          else                           sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_2016-ichep_data2016_"+m_sTriggerName+"_etaexcl.root";
        }
        else
          sInputFilePathTriggerHadTau = m_sInputFilePathTriggerHadTau;

        asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool_2016 = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TriggerHadTauTool_2016", this);
        m_vTriggerEfficiencyTools.push_back(tTool_2016);
        ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool_2016, TauAnalysisTools::TauEfficiencyTriggerTool));
        ATH_CHECK(tTool_2016->setProperty("InputFilePath", sInputFilePathTriggerHadTau));
        ATH_CHECK(tTool_2016->setProperty("VarName", m_sVarNameTriggerHadTau));
        ATH_CHECK(tTool_2016->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
        ATH_CHECK(tTool_2016->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
        ATH_CHECK(tTool_2016->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
        ATH_CHECK(tTool_2016->setProperty("MinRunNumber", 296939));
        // ATH_CHECK(tTool_2016->setProperty("MaxRunNumber", ));
      }
    }
    else
    {
      ATH_MSG_WARNING("unsupported EfficiencyCorrectionsType with enum "<<iEfficiencyCorrectionType);
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initializeTools_mc15_moriond()
{
  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/EfficiencyCorrections/";
  for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
  {
    if (iEfficiencyCorrectionType == SFJetIDHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathJetIDHadTau.empty()) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_mc15-moriond.root";
      if (m_sVarNameJetIDHadTau.length() == 0) m_sVarNameJetIDHadTau = "TauScaleFactorJetIDHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("JetIDHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathJetIDHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameJetIDHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertJetIDToString(m_iIDLevel)));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRHadTau.empty()) m_sInputFilePathEleOLRHadTau = sDirectory+"EleOLR_TrueHadTau_mc15-moriond.root";
      if (m_sVarNameEleOLRHadTau.length() == 0) m_sVarNameEleOLRHadTau = "TauScaleFactorEleOLRHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("EleOLRHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRElectron)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRElectron.empty()) m_sInputFilePathEleOLRElectron = sDirectory+"EleOLR_TrueElectron_mc15-moriond.root";
      if (m_sVarNameEleOLRElectron.length() == 0) m_sVarNameEleOLRElectron = "TauScaleFactorEleOLRElectron";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("EleOLRElectronTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRElectron));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRElectron));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertEleOLRToString(m_iOLRLevel)));
    }
    else if (iEfficiencyCorrectionType == SFRecoHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathRecoHadTau.empty()) m_sInputFilePathRecoHadTau = sDirectory+"Reco_TrueHadTau_mc15-prerec.root";
      if (m_sVarNameRecoHadTau.length() == 0) m_sVarNameRecoHadTau = "TauScaleFactorReconstructionHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("ReconstructionHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathRecoHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameRecoHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFTriggerHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathTriggerHadTau.empty())
      {
        if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
        if (m_bUseTriggerInclusiveEta) m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_mc15-prerec_"+m_sTriggerName+"_etainc.root";
        else m_sInputFilePathTriggerHadTau                           = sDirectory+"Trigger/Trigger_TrueHadTau_mc15-prerec_"+m_sTriggerName+"_etaexcl.root";
      }
      if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TriggerHadTauTool", this);
      m_vTriggerEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::TauEfficiencyTriggerTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathTriggerHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameTriggerHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
      ATH_CHECK(tTool->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
    }
    else
    {
      ATH_MSG_WARNING("unsupported EfficiencyCorrectionsType with enum "<<iEfficiencyCorrectionType);
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initializeTools_mc15_pre_recommendations()
{
  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/EfficiencyCorrections/";
  for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
  {
    if (iEfficiencyCorrectionType == SFJetIDHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathJetIDHadTau.empty()) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_mc15-prerec.root";
      if (m_sVarNameJetIDHadTau.length() == 0) m_sVarNameJetIDHadTau = "TauScaleFactorJetIDHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("JetIDHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathJetIDHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameJetIDHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertJetIDToString(m_iIDLevel)));
    }
    else if (iEfficiencyCorrectionType == SFEleOLRHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathEleOLRHadTau.empty()) m_sInputFilePathEleOLRHadTau = sDirectory+"EleOLR_TrueHadTau_mc15-prerec.root";
      if (m_sVarNameEleOLRHadTau.length() == 0) m_sVarNameEleOLRHadTau = "TauScaleFactorEleOLRHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("EleOLRHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleOLRHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFRecoHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathRecoHadTau.empty()) m_sInputFilePathRecoHadTau = sDirectory+"Reco_TrueHadTau_mc15-prerec.root";
      if (m_sVarNameRecoHadTau.length() == 0) m_sVarNameRecoHadTau = "TauScaleFactorReconstructionHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("ReconstructionHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::CommonEfficiencyTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathRecoHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameRecoHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFTriggerHadTau)
    {
      // only set vars if they differ from "", which means they have been configured by the user
      if (m_sInputFilePathTriggerHadTau.empty())
      {
        if (m_sTriggerName.empty()) ATH_MSG_FATAL("Property \"Trigger\" was not set, please provide a trigger name.");
        if (m_bUseTriggerInclusiveEta) m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/Trigger_TrueHadTau_mc15-prerec_"+m_sTriggerName+"_etainc.root";
        else m_sInputFilePathTriggerHadTau                           = sDirectory+"Trigger/Trigger_TrueHadTau_mc15-prerec_"+m_sTriggerName+"_etaexcl.root";
      }
      if (m_sVarNameTriggerHadTau.length() == 0) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TriggerHadTauTool", this);
      m_vTriggerEfficiencyTools.push_back(tTool);
      ATH_CHECK(ASG_MAKE_ANA_TOOL(*tTool, TauAnalysisTools::TauEfficiencyTriggerTool));
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathTriggerHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameTriggerHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertTriggerIDToString(m_iIDLevel)));
      ATH_CHECK(tTool->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
    }
    else
    {
      ATH_MSG_WARNING("unsupported EfficiencyCorrectionsType with enum "<<iEfficiencyCorrectionType);
    }
  }
  return StatusCode::SUCCESS;
}

// auto detection of simulation flavour, used to cross check configuration of tool
//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::beginInputFile()
{
  bool setup_tau_id = false;
  if (m_iIDLevel == JETIDBDTLOOSE || m_iIDLevel == JETIDBDTMEDIUM || m_iIDLevel == JETIDBDTTIGHT || m_iIDLevel == JETIDRNNLOOSE || m_iIDLevel == JETIDRNNMEDIUM || m_iIDLevel == JETIDRNNTIGHT) setup_tau_id = true;
  bool setup_eveto = false;
  if (m_iOLRLevel != OLRNONE && (m_iOLRLevel == ELEBDTLOOSE || m_iOLRLevel == ELEBDTMEDIUM)) setup_eveto = true;
  if (m_sRecommendationTag == "2019-summer" && (setup_tau_id || setup_eveto) )
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

//______________________________________________________________________________
std::string TauEfficiencyCorrectionsTool::ConvertJetIDToString(const int& iLevel)
{
  switch(iLevel)
  {
  case JETIDNONE:
    return "none";
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
  case JETIDBDTOTHER:
    return "jetbdtsigother";
    break;
  case JETIDLLHLOOSE:
    return "taujllhloose";
    break;
  case JETIDLLHMEDIUM:
    return "taujllhmedium";
    break;
  case JETIDLLHTIGHT:
    return "taujllhtight";
    break;
  case JETIDLLHFAIL:
    return "taujllh";
    break;
  case JETIDBDTFAIL:
    return "jetbdtsig";
    break;
  case JETIDRNNVERYLOOSE: 
    ATH_MSG_WARNING("Very loose RNN working point passed. Efficiency corrections for this working point are not supported.");
    return "";
  case JETIDRNNLOOSE: 
    return "jetrnnsigloose";
    break;
  case JETIDRNNMEDIUM: 
    return "jetrnnsigmedium";
    break;
  case JETIDRNNTIGHT: 
    return "jetrnnsigtight";
    break;
  default:
    assert(false && "No valid ID level passed. Breaking up ...");
    break;
  }
  return "";
}

std::string TauEfficiencyCorrectionsTool::ConvertEleOLRToString(const int& iLevel)
{
  switch(iLevel)
  {
  case TAUELEOLR:
    return "TauEleOLR";
    break;
  case ELELOOSELLHOLR:
    return "EleLooseLLHOLR";
    break;
  case ELEMEDIUMLLHOLR:
    return "EleMediumLLHOLR";
    break;
  case ELETIGHTLLHOLR:
    return "EleTightLLHOLR";
    break;
  case ELEBDTTIGHTPLUSVETO:
    return "eleBDTTightPlusVeto";
  case ELEBDTTIGHT:
    return "eleBDTTight";
  case ELEBDTMEDIUMPLUSVETO:
    return "eleBDTMediumPlusVeto";
  case ELEBDTMEDIUM:
    return "eleBDTMedium";
  case ELEBDTLOOSEPLUSVETO:
    return "eleBDTLoosePlusVeto";
  case ELEBDTLOOSE:
    return "eleBDTLoose";
  default:
    assert(false && "No valid electron OLR passed. Breaking up ...");
    break;
  }
  return "";
}

//______________________________________________________________________________
std::string TauEfficiencyCorrectionsTool::ConvertTriggerIDToString(const int& iLevel)
{
  switch(iLevel)
  {
  case JETIDBDTLOOSE:
    return "loose";
    break;
  case JETIDBDTMEDIUM:
    return "medium";
    break;
  case JETIDBDTTIGHT:
    return "tight";
    break;
  case JETIDRNNLOOSE:
    return "loose";
    break;
  case JETIDRNNMEDIUM:
    return "medium";
    break;
  case JETIDRNNTIGHT:
    return "tight";
    break;
  default:
    assert(false && "No valid Trigger ID level passed. Breaking up ...");
    break;
  }
  return "";
}

//______________________________________________________________________________
std::string TauEfficiencyCorrectionsTool::GetTriggerSFMeasrementString()
{
  std::string sMeasurement = "_comb_";

  if (m_sTriggerSFMeasurement=="Ztautau")
    sMeasurement = "_Ztt_";
  else if (m_sTriggerSFMeasurement=="ttbar")
    sMeasurement = "_ttbar_";
  else if (m_sTriggerSFMeasurement!="combined")
  {
    ATH_MSG_WARNING("Trigger scale factor measurement \'" << m_sTriggerSFMeasurement << "\' is not known. \'combined\' is used instead.");
  }

  return sMeasurement;
}

StatusCode TauEfficiencyCorrectionsTool::readRandomRunNumber()
{
  if (m_bReadRandomRunNumber && m_iRunNumber == 0)
  {
    static const SG::AuxElement::ConstAccessor<unsigned int> acc_rnd("RandomRunNumber");
    const xAOD::EventInfo* eventInfo{};
    if (!evtStore()->contains<xAOD::EventInfo>("EventInfo") || !evtStore()->retrieve(eventInfo, "EventInfo").isSuccess())
    {
      ANA_MSG_ERROR("Could not retrieve the xAOD::EventInfo with name EventInfo");
      return StatusCode::FAILURE;
    }

    if (!acc_rnd.isAvailable(*eventInfo))
    {
      ANA_MSG_ERROR("Failed to find the RandomRunNumber decoration. Please call the apply() method from the PileupReweightingTool beforehand in order to get period dependent SFs.");
      return StatusCode::FAILURE;
    }

    m_iRunNumber = acc_rnd(*eventInfo);

    ANA_MSG_VERBOSE("Read RandomRunNumber as " << m_iRunNumber);
  }

  return StatusCode::SUCCESS;
}

} // namespace TauAnalysisTools


