/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  , m_bIsData(false)
  , m_bIsConfigured(false)
  , m_iRunNumber(0)
  , m_iMu(0)
  , m_tTauSelectionToolHandle("")
  , m_tPRWTool("")
{
  declareProperty( "EfficiencyCorrectionTypes",    m_vEfficiencyCorrectionTypes    = {} );
  declareProperty( "InputFilePathRecoHadTau",      m_sInputFilePathRecoHadTau      = "" );
  declareProperty( "InputFilePathEleIDHadTau",     m_sInputFilePathEleIDHadTau     = "" );
  declareProperty( "InputFilePathEleIDElectron",   m_sInputFilePathEleIDElectron   = "" );
  declareProperty( "InputFilePathJetIDHadTau",     m_sInputFilePathJetIDHadTau     = "" );
  declareProperty( "InputFilePathTriggerHadTau",   m_sInputFilePathTriggerHadTau   = "" );
  declareProperty( "VarNameRecoHadTau",            m_sVarNameRecoHadTau            = "" );
  declareProperty( "VarNameEleIDHadTau",           m_sVarNameEleIDHadTau           = "" );
  declareProperty( "VarNameEleIDElectron",         m_sVarNameEleIDElectron         = "" );
  declareProperty( "VarNameJetIDHadTau",           m_sVarNameJetIDHadTau           = "" );
  declareProperty( "VarNameDecayModeHadTau",       m_sVarNameDecayModeHadTau       = "" );
  declareProperty( "VarNameTriggerHadTau",         m_sVarNameTriggerHadTau         = "" );
  declareProperty( "RecommendationTag",            m_sRecommendationTag            = "2019-summer" );
  declareProperty( "TriggerName",                  m_sTriggerName                  = "" );
  declareProperty( "AutoTriggerYear",              m_bReadRandomRunNumber          = false );
  declareProperty( "TriggerSFMeasurement",         m_sTriggerSFMeasurement         = "combined" ); // "combined", "Ztautau" or "ttbar"
  declareProperty( "UseTauSubstructure",           m_bUseTauSubstructure           = false );
  declareProperty( "UseHighPtUncert",              m_bUseHighPtUncert              = false );
  declareProperty( "JetIDLevel",                   m_iJetIDLevel                   = (int)JETIDNONE );
  declareProperty( "EleIDLevel",                   m_iEleIDLevel                   = (int)ELEIDNONE );
  declareProperty( "TriggerPeriodBinning",         m_iTriggerPeriodBinning         = (int)PeriodBinningAll );
  declareProperty( "MCCampaign",                   m_sMCCampaign                   = "" ); // MC16a, MC16d or MC16e
  declareProperty( "isAFII",	                   m_sAFII	                   = false );
  declareProperty( "SkipTruthMatchCheck",          m_bSkipTruthMatchCheck          = false );
  declareProperty( "TauSelectionTool",             m_tTauSelectionToolHandle );
  declareProperty( "PileupReweightingTool",        m_tPRWTool );
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
StatusCode TauEfficiencyCorrectionsTool::initializeWithTauSelectionTool(TauSelectionTool* tauSelectionTool)
{
  ATH_MSG_DEBUG( "Initializing TauEfficiencyCorrectionsTool using TauSelectionTool" );

  // add reco systematics in any case
  m_vEfficiencyCorrectionTypes.push_back(SFRecoHadTau);
  ATH_MSG_VERBOSE( "added SFRecoHadTau" );
  
  // use jet ID scale factors if TauSelectionTool applies jet ID cut
  if (tauSelectionTool->m_iSelectionCuts & CutJetIDWP) {
    m_iJetIDLevel = tauSelectionTool->m_iJetIDWP;

    if (m_iJetIDLevel == JETIDRNNLOOSE || m_iJetIDLevel == JETIDRNNMEDIUM || m_iJetIDLevel == JETIDRNNTIGHT) {
      m_vEfficiencyCorrectionTypes.push_back(SFJetIDHadTau);
      ATH_MSG_VERBOSE( "added SFJetIDHadTau" );
    }
    else {
      ATH_MSG_WARNING( "No JetID scale factor for working point " << m_iJetIDLevel );
    }
  }
  
  // use electron veto scale factors if TauSelectionTool applies electron veto
  if (tauSelectionTool->m_iSelectionCuts & CutEleIDWP) {
    m_iEleIDLevel = tauSelectionTool->m_iEleIDWP;

    // force m_iEleIDLevel to ELEIDNONE until RNN eVeto SFs are available  
    if (m_iEleIDLevel != ELEIDNONE) {
      ATH_MSG_WARNING("Scale factors for RNN eVeto are not available yet");
      m_iEleIDLevel = ELEIDNONE;
    }

    if (m_iEleIDLevel == ELEIDRNNLOOSE || m_iEleIDLevel == ELEIDRNNMEDIUM || m_iEleIDLevel == ELEIDRNNTIGHT) {	
      m_vEfficiencyCorrectionTypes.push_back(SFEleIDHadTau);
      ATH_MSG_VERBOSE( "added SFEleIDHadTau" );
      m_vEfficiencyCorrectionTypes.push_back(SFEleIDElectron);
      ATH_MSG_VERBOSE( "added SFEleIDElectron" );
    }
    // commented out until RNN eVeto SFs are available, to avoid double warnings
    /*
    else {
      ATH_MSG_WARNING( "No EleID scale factor for working point " << m_iEleIDLevel );
    }
    */
  }  
  
  if (m_bUseTauSubstructure) {
    ATH_MSG_WARNING("Scale factors for decay mode ID are not available yet");
    //m_vEfficiencyCorrectionTypes.push_back(SFDecayModeHadTau);
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initialize()
{
  ATH_MSG_INFO( "Initializing TauEfficiencyCorrectionsTool" );

  if (m_bSkipTruthMatchCheck)
    ATH_MSG_WARNING("Truth match check will be skipped. This is ONLY FOR TESTING PURPOSE!");

  if (!m_tPRWTool.empty())
    ATH_CHECK(m_tPRWTool.retrieve());

  // determine set of scale factors using TauSelectionTool
  if (!m_tTauSelectionToolHandle.empty())
  {
    if (!m_vEfficiencyCorrectionTypes.empty()) {
      ATH_MSG_ERROR("Either use a TauSelectionTool handle or EfficiencyCorrectionTypes, not both.");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(m_tTauSelectionToolHandle.retrieve());
    TauAnalysisTools::TauSelectionTool* tauSelectionTool = dynamic_cast<TauAnalysisTools::TauSelectionTool*>(&*m_tTauSelectionToolHandle);
    if(tauSelectionTool == nullptr) {
      ATH_MSG_ERROR("Could not retrieve TauSelectionTool");
      return StatusCode::FAILURE;
    }
    if (initializeWithTauSelectionTool(tauSelectionTool).isFailure()) {
      ATH_MSG_ERROR("Failed to determine scale factors using TauSelectionTool");
      return StatusCode::FAILURE;
    }
  }

  // configure default set of scale factors if neither TauSelectionTool nor EfficiencyCorrectionTypes are specified
  // FIXME: do we actually need this?
  if (m_vEfficiencyCorrectionTypes.empty())
  {
    m_vEfficiencyCorrectionTypes = { SFRecoHadTau };

    if (m_iJetIDLevel == JETIDRNNLOOSE || m_iJetIDLevel == JETIDRNNMEDIUM || m_iJetIDLevel == JETIDRNNTIGHT) {
      m_vEfficiencyCorrectionTypes.push_back(SFJetIDHadTau);
    }
    // Force m_iEleIDLevel to ELEIDNONE until RNN eVeto SFs are available  
    if (m_iEleIDLevel != ELEIDNONE) {
      ATH_MSG_WARNING("Scale factors for RNN eVeto are not available yet");
      m_iEleIDLevel = ELEIDNONE;
    }
    if (m_iEleIDLevel == ELEIDRNNLOOSE || m_iEleIDLevel == ELEIDRNNMEDIUM || m_iEleIDLevel == ELEIDRNNTIGHT) {	
      m_vEfficiencyCorrectionTypes.push_back(SFEleIDHadTau);
      m_vEfficiencyCorrectionTypes.push_back(SFEleIDElectron);
    }
    if (m_bUseTauSubstructure) {
      ATH_MSG_WARNING("Scale factors for decay mode ID are not available yet");
      //m_vEfficiencyCorrectionTypes.push_back(SFDecayModeHadTau);
    }
  }
  
  if (m_sRecommendationTag == "2019-summer") {
    ATH_CHECK(initializeTools_2019_summer());
  }
  else {
    ATH_MSG_ERROR("Unknown RecommendationTag " << m_sRecommendationTag);
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

  printConfig();

  return StatusCode::SUCCESS;
}

StatusCode TauEfficiencyCorrectionsTool::beginEvent()
{
  if (!m_bIsConfigured)
  {
    const xAOD::EventInfo* xEventInfo = nullptr;
    ATH_CHECK(evtStore()->retrieve(xEventInfo,"EventInfo"));
    m_bIsData = !(xEventInfo->eventType( xAOD::EventInfo::IS_SIMULATION));
    m_bIsConfigured = true;
  }

  if (m_bIsData)
    return StatusCode::SUCCESS;

  const xAOD::EventInfo* xEventInfo = nullptr;
  ATH_CHECK(evtStore()->retrieve(xEventInfo, "EventInfo"));
  m_iMu = xEventInfo->averageInteractionsPerCrossing();

  // FIXME: this should be harmonised for R22 recommendations
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
void TauEfficiencyCorrectionsTool::printConfig() const
{
  ATH_MSG_DEBUG( "TauEfficiencyCorrectionsTool with name " << name() << " is configured as follows:" );
  for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes) {
    ATH_MSG_DEBUG( "  EfficiencyCorrectionTypes " << iEfficiencyCorrectionType );
  }
  ATH_MSG_DEBUG( "  InputFilePathRecoHadTau " << m_sInputFilePathRecoHadTau );
  ATH_MSG_DEBUG( "  InputFilePathEleIDHadTau " << m_sInputFilePathEleIDHadTau );
  ATH_MSG_DEBUG( "  InputFilePathEleIDElectron " << m_sInputFilePathEleIDElectron );
  ATH_MSG_DEBUG( "  InputFilePathJetIDHadTau " << m_sInputFilePathJetIDHadTau );
  ATH_MSG_DEBUG( "  InputFilePathDecayModeHadTau " << m_sInputFilePathDecayModeHadTau );
  ATH_MSG_DEBUG( "  InputFilePathTriggerHadTau " << m_sInputFilePathTriggerHadTau );
  ATH_MSG_DEBUG( "  VarNameRecoHadTau " << m_sVarNameRecoHadTau );
  ATH_MSG_DEBUG( "  VarNameEleIDHadTau " << m_sVarNameEleIDHadTau );
  ATH_MSG_DEBUG( "  VarNameEleIDElectron " << m_sVarNameEleIDElectron );
  ATH_MSG_DEBUG( "  VarNameJetIDHadTau " << m_sVarNameJetIDHadTau );
  ATH_MSG_DEBUG( "  VarNameDecayModeHadTau " << m_sVarNameDecayModeHadTau );
  ATH_MSG_DEBUG( "  VarNameTriggerHadTau " << m_sVarNameTriggerHadTau );
  ATH_MSG_DEBUG( "  RecommendationTag " << m_sRecommendationTag );
  ATH_MSG_DEBUG( "  TriggerName " << m_sTriggerName );
  ATH_MSG_DEBUG( "  UseTauSubstructure " << m_bUseTauSubstructure );
  ATH_MSG_DEBUG( "  UseHighPtUncert " << m_bUseHighPtUncert );
  ATH_MSG_DEBUG( "  JetIDLevel " << m_iJetIDLevel );
  ATH_MSG_DEBUG( "  EleIDLevel " << m_iEleIDLevel );
  ATH_MSG_DEBUG( "  TriggerPeriodBinning " << m_iTriggerPeriodBinning );
  ATH_MSG_DEBUG( "  MCCampaign " << m_sMCCampaign );
  ATH_MSG_DEBUG( "  isAFII " << m_sAFII );
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
    if ( !(**it)->isSupportedRunNumber(m_iRunNumber) )
      continue;
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

/// returns: whether this tool is affected by the given systematics
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
StatusCode TauEfficiencyCorrectionsTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  for (auto it = m_vCommonEfficiencyTools.begin(); it != m_vCommonEfficiencyTools.end(); it++)
    if ((**it)->applySystematicVariation(sSystematicSet) == StatusCode::FAILURE)
      return StatusCode::FAILURE;
  for (auto it = m_vTriggerEfficiencyTools.begin(); it != m_vTriggerEfficiencyTools.end(); it++)
    if ((**it)->applySystematicVariation(sSystematicSet) == StatusCode::FAILURE)
      return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

//=================================PRIVATE-PART=================================

//______________________________________________________________________________
// this whole block is a place holder until we get R22 Run2 recommendations
// none of these SFs are valid for R22 MC, except possibly RNN ID to very coarse approximation
StatusCode TauEfficiencyCorrectionsTool::initializeTools_2019_summer()
{
  std::string sDirectory = "TauAnalysisTools/" + std::string(sSharedFilesVersion) + "/EfficiencyCorrections/";

  // initialise paths and SF names unless they have been configured by the user
  for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
  {
    if (iEfficiencyCorrectionType == SFJetIDHadTau)
    {
      if (m_sInputFilePathJetIDHadTau.empty()) {
	if (m_sAFII) m_sInputFilePathJetIDHadTau = sDirectory + "RNNID_TrueHadTau_2019-summer_AFII.root";
	else m_sInputFilePathJetIDHadTau = sDirectory + "RNNID_TrueHadTau_2019-summer.root";
      }
      if (m_sVarNameJetIDHadTau.empty()) m_sVarNameJetIDHadTau = "TauScaleFactorJetIDHadTau";

      std::string sJetIDWP = ConvertJetIDToString(m_iJetIDLevel);
      if (sJetIDWP.empty()) {
        ATH_MSG_WARNING("Could not find valid ID working point. Skip ID efficiency corrections.");
        continue;
      }

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/JetIDHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathJetIDHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameJetIDHadTau));
      //ATH_CHECK(tTool->setProperty("UseTauSubstructure", m_bUseTauSubstructure));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", sJetIDWP));
    }
    else if (iEfficiencyCorrectionType == SFEleIDHadTau)
    {
      // the path must be updated once RNN eVeto SFs are available
      if (m_sInputFilePathEleIDHadTau.empty()) m_sInputFilePathEleIDHadTau = sDirectory + "EleOLR_TrueHadTau_2016-ichep.root";
      if (m_sVarNameEleIDHadTau.empty()) m_sVarNameEleIDHadTau = "TauScaleFactorEleIDHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/EleIDHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleIDHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleIDHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFEleIDElectron)
    {
      // the path must be updated once RNN eVeto SFs are available
      if (m_sInputFilePathEleIDElectron.empty()) {
        if (m_sAFII) m_sInputFilePathEleIDElectron = sDirectory + "EleBDT_TrueElectron_2019-summer_AFII.root";
        else m_sInputFilePathEleIDElectron = sDirectory + "EleBDT_TrueElectron_2019-summer.root";
      }
      if (m_sVarNameEleIDElectron.empty()) m_sVarNameEleIDElectron = "TauScaleFactorEleIDElectron";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/EleIDElectronTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathEleIDElectron));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameEleIDElectron));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertEleIDToString(m_iEleIDLevel)));
      ATH_CHECK(tTool->setProperty("UseTauSubstructure", true));
    }
    else if (iEfficiencyCorrectionType == SFRecoHadTau)
    {
      if (m_sInputFilePathRecoHadTau.empty()) m_sInputFilePathRecoHadTau = sDirectory + "Reco_TrueHadTau_2019-summer.root";
      if (m_sVarNameRecoHadTau.empty()) m_sVarNameRecoHadTau = "TauScaleFactorReconstructionHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::CommonEfficiencyTool/RecoHadTauTool", this);
      m_vCommonEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathRecoHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameRecoHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
    }
    else if (iEfficiencyCorrectionType == SFDecayModeHadTau)
    {
      if (m_sInputFilePathDecayModeHadTau.empty()) m_sInputFilePathDecayModeHadTau = sDirectory + "DecayModeSubstructure_TrueHadTau_2019-summer.root";
      if (m_sVarNameDecayModeHadTau.empty()) m_sVarNameDecayModeHadTau = "TauScaleFactorDecayModeHadTau";

      std::string sJetIDWP = ConvertJetIDToString(m_iJetIDLevel);
      if (sJetIDWP.empty()) {
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
      if (m_sTriggerName.empty()) {
	ATH_MSG_ERROR("Property \"Trigger\" was not set, please provide a trigger name.");
	return StatusCode::FAILURE;  
      }
      if (m_sInputFilePathTriggerHadTau.empty()) {
	// Determine the input file name from the given trigger name.
	// Triggers having "mediumRNN_tracktwoMVA are only part of 2018aftTS1.
	// Every other trigger having "tracktwoEF" is only part of 2018.
	// Every other trigger having "tau160_medium1" is only part of 2016.
	// Every other trigger having "tau160" is only part of 2017/2018.
	// Lastly check for other possible triggers, if this is not fulfilled the passed trigger is not supported.
	if (m_sTriggerName.find("mediumRNN_tracktwoMVA") != std::string::npos) {
	  m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/RNN/Trigger_TrueHadTau_2019-summer_data2018aftTS1"+GetTriggerSFMeasurementString()+m_sTriggerName+".root";
	}
	else if (m_sTriggerName.find("tracktwoEF") != std::string::npos) {
	  m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/RNN/Trigger_TrueHadTau_2019-summer_data2018"+GetTriggerSFMeasurementString()+m_sTriggerName+".root";
	}
	else if (m_sTriggerName.find("tau160_medium1") != std::string::npos) {
	  m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/RNN/Trigger_TrueHadTau_2019-summer_data2016"+GetTriggerSFMeasurementString()+m_sTriggerName+".root";
	}
	else if ((m_sTriggerName.find("tau160") != std::string::npos) || (m_sTriggerName.find("tau60") != std::string::npos)) {
	  m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/RNN/Trigger_TrueHadTau_2019-summer_data1718"+GetTriggerSFMeasurementString()+m_sTriggerName+".root";
	}
	else if ((m_sTriggerName.find("tau125") != std::string::npos) || (m_sTriggerName.find("tau25") != std::string::npos) || (m_sTriggerName.find("tau35") != std::string::npos) || (m_sTriggerName.find("tau50") != std::string::npos) || (m_sTriggerName.find("tau80") != std::string::npos) ) {
	  m_sInputFilePathTriggerHadTau = sDirectory+"Trigger/RNN/Trigger_TrueHadTau_2019-summer_data161718"+GetTriggerSFMeasurementString()+m_sTriggerName+".root";
	}
	else {
	  ATH_MSG_ERROR("Trigger " << m_sTriggerName << " is not supported. Please fix \"TriggerName\" property.");        
	  return StatusCode::FAILURE;
	}
      }
      if (m_sVarNameTriggerHadTau.empty()) m_sVarNameTriggerHadTau = "TauScaleFactorTriggerHadTau";

      asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>* tTool = new asg::AnaToolHandle<ITauEfficiencyCorrectionsTool>("TauAnalysisTools::TauEfficiencyTriggerTool/TriggerHadTauTool", this);
      m_vTriggerEfficiencyTools.push_back(tTool);
      ATH_CHECK(tTool->setProperty("InputFilePath", m_sInputFilePathTriggerHadTau));
      ATH_CHECK(tTool->setProperty("VarName", m_sVarNameTriggerHadTau));
      ATH_CHECK(tTool->setProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck));
      ATH_CHECK(tTool->setProperty("WP", ConvertTriggerIDToString(m_iJetIDLevel)));
      ATH_CHECK(tTool->setProperty("PeriodBinning", (int)m_iTriggerPeriodBinning));
    }
    else {
      ATH_MSG_WARNING("unsupported EfficiencyCorrectionsType with enum " << iEfficiencyCorrectionType);
    }
  }

  return StatusCode::SUCCESS;
}

// auto detection of simulation flavour, used to cross check configuration of tool
//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::beginInputFile()
{
  // FIXME: policy for Fullsim and AtlFast3 scale factors to be reassessed in R22
  if (inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData")) {
    const xAOD::FileMetaData* fmd = nullptr;
    ATH_CHECK( inputMetaStore()->retrieve( fmd, "FileMetaData" ) );
    std::string simType("");
    bool result = fmd->value( xAOD::FileMetaData::simFlavour , simType );
    // if no result -> no simFlavor metadata, so must be data
    if(result) boost::to_upper(simType);
    
    if (simType.find("ATLFASTII")!=std::string::npos && !m_sAFII)
      ATH_MSG_WARNING("Input file is fast simulation but you are _not_ using AFII corrections and uncertainties, you should set \"isAFII\" to \"true\"");
    else if (simType.find("FULLG4")!=std::string::npos && m_sAFII)
      ATH_MSG_WARNING("Input file is full simulation but you are using AFII corrections and uncertainties, you should set \"isAFII\" to \"false\"");
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
std::string TauEfficiencyCorrectionsTool::ConvertJetIDToString(const int iLevel) const
{
  switch(iLevel)
    {
    case JETIDNONE:
      return "none";
    case JETIDRNNVERYLOOSE: 
      ATH_MSG_WARNING("Efficiency corrections for JETIDRNNVERYLOOSE working point are not supported.");
      return "";
    case JETIDRNNLOOSE: 
      return "jetrnnsigloose";
    case JETIDRNNMEDIUM: 
      return "jetrnnsigmedium";
    case JETIDRNNTIGHT: 
      return "jetrnnsigtight";
    default:
      ATH_MSG_WARNING("No valid JetID level passed.");
      return "";
    }
}

std::string TauEfficiencyCorrectionsTool::ConvertEleIDToString(const int iLevel) const
{
  switch(iLevel)
    {
    case ELEIDRNNTIGHT:
      return "eleRNNTight";
    case ELEIDRNNMEDIUM:
      return "eleRNNMedium";
    case ELEIDRNNLOOSE:
      return "eleRNNLoose";
    default:
      ATH_MSG_WARNING("No valid EleID level passed.");
      return "";
    }
}

//______________________________________________________________________________
std::string TauEfficiencyCorrectionsTool::ConvertTriggerIDToString(const int iLevel) const
{
  switch(iLevel)
    {
    case JETIDRNNLOOSE:
      return "loose";
    case JETIDRNNMEDIUM:
      return "medium";
    case JETIDRNNTIGHT:
      return "tight";
    default:
      ATH_MSG_WARNING("No valid TriggerID level passed.");
      return "";
    }
}

//______________________________________________________________________________
std::string TauEfficiencyCorrectionsTool::GetTriggerSFMeasurementString() const
{
  std::string sMeasurement = "_comb_";

  if (m_sTriggerSFMeasurement == "Ztautau")
    sMeasurement = "_Ztt_";
  else if (m_sTriggerSFMeasurement == "ttbar")
    sMeasurement = "_ttbar_";
  else if (m_sTriggerSFMeasurement != "combined")
    ATH_MSG_WARNING("Trigger scale factor measurement \'" << m_sTriggerSFMeasurement << "\' is not supported. \'combined\' is used instead.");

  return sMeasurement;
}

StatusCode TauEfficiencyCorrectionsTool::readRandomRunNumber()
{
  if (m_bReadRandomRunNumber && m_iRunNumber == 0)
  {
    static const SG::AuxElement::ConstAccessor<unsigned int> acc_rnd("RandomRunNumber");
    const xAOD::EventInfo* eventInfo = nullptr;
    if (!evtStore()->contains<xAOD::EventInfo>("EventInfo") || !evtStore()->retrieve(eventInfo, "EventInfo").isSuccess())
    {
      ANA_MSG_ERROR("Could not retrieve EventInfo");
      return StatusCode::FAILURE;
    }

    if (!acc_rnd.isAvailable(*eventInfo))
    {
      ANA_MSG_ERROR("Failed to find the RandomRunNumber decoration. Call the apply() method from the PileupReweightingTool beforehand to get period dependent SFs.");
      return StatusCode::FAILURE;
    }

    m_iRunNumber = acc_rnd(*eventInfo);

    ANA_MSG_VERBOSE("Read RandomRunNumber as " << m_iRunNumber);
  }

  return StatusCode::SUCCESS;
}

} // namespace TauAnalysisTools
