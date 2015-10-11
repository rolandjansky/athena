/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "PATInterfaces/SystematicRegistry.h"

// Local include(s):
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/SharedFilesVersion.h"

namespace TauAnalysisTools
{

//______________________________________________________________________________
TauEfficiencyCorrectionsTool::TauEfficiencyCorrectionsTool( const std::string& sName )
  : asg::AsgTool( sName ),
    m_iEfficiencyCorrectionType(SFJetID),
    m_vEfficiencyCorrectionTypes(),
    m_vCommonEfficiencyTools(),
    m_sInputFilePath(""),
    m_sInputFilePathRecoHadTau(""),
    m_sInputFilePathEleOLRHadTau(""),
    m_sInputFilePathJetIDHadTau(""),
    m_sInputFilePathContJetIDHadTau(""),
    m_sInputFilePathEleIDHadTau(""),
    m_sVarNameBase(""),
    m_sVarNameRecoHadTau(""),
    m_sVarNameEleOLRHadTau(""),
    m_sVarNameJetIDHadTau(""),
    m_sVarNameContJetIDHadTau(""),
    m_sVarNameEleIDHadTau(""),
    m_sRecommendationTag("mc15-pre-recommendations"),
    m_bSkipTruthMatchCheck(false),
    m_bNoMultiprong(false),
    m_bUseIDExclusiveSF(false),
    m_bUseInclusiveEta(false),
    m_bUsePtBinnedSF(false),
    m_bUseHighPtUncert(false),
    m_iIDLevel(JETIDBDTTIGHT),
    m_iEVLevel(ELEIDBDTLOOSE),
    m_iOLRLevel(OLRLOOSEPP),
    m_iContSysType(TOTAL),
    m_tTauSelectionTool(0)
{
  declareProperty( "EfficiencyCorrectionTypes", m_vEfficiencyCorrectionTypes );
  declareProperty( "InputFilePathRecoHadTau", m_sInputFilePathRecoHadTau );
  declareProperty( "InputFilePathEleOLRHadTau", m_sInputFilePathEleOLRHadTau );
  declareProperty( "InputFilePathJetIDHadTau", m_sInputFilePathJetIDHadTau );
  declareProperty( "InputFilePathContJetIDHadTau", m_sInputFilePathContJetIDHadTau );
  declareProperty( "InputFilePathEleIDHadTau", m_sInputFilePathEleIDHadTau );
  declareProperty( "VarNameRecoHadTau", m_sVarNameRecoHadTau );
  declareProperty( "VarNameEleOLRHadTau", m_sVarNameEleOLRHadTau );
  declareProperty( "VarNameJetIDHadTau", m_sVarNameJetIDHadTau );
  declareProperty( "VarNameContJetIDHadTau", m_sVarNameContJetIDHadTau );
  declareProperty( "VarNameEleIDHadTau", m_sVarNameEleIDHadTau );
  declareProperty( "RecommendationTag", m_sRecommendationTag );

  declareProperty( "UseIDExclusiveSF", m_bUseIDExclusiveSF );
  declareProperty( "UseInclusiveEta", m_bUseInclusiveEta );
  declareProperty( "UsePtBinnedSF", m_bUsePtBinnedSF );
  declareProperty( "UseHighPtUncert", m_bUseHighPtUncert );
  declareProperty( "IDLevel", m_iIDLevel );
  declareProperty( "EVLevel", m_iEVLevel );
  declareProperty( "OLRLevel", m_iOLRLevel );
  declareProperty( "ContSysType", m_iContSysType );

  declareProperty( "SkipTruthMatchCheck", m_bSkipTruthMatchCheck );

  // deprecated properties
  declareProperty( "EfficiencyCorrectionType", m_iEfficiencyCorrectionType );
  declareProperty( "InputFilePath", m_sInputFilePath );
  declareProperty( "VarNameBase", m_sVarNameBase );
}

//______________________________________________________________________________
TauEfficiencyCorrectionsTool::TauEfficiencyCorrectionsTool( const std::string& sName, TauSelectionTool* tTauSelectionTool )
  : TauEfficiencyCorrectionsTool( sName)
{
  m_tTauSelectionTool = tTauSelectionTool;
}

//______________________________________________________________________________
TauEfficiencyCorrectionsTool::~TauEfficiencyCorrectionsTool()
{
}

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initializeWithTauSelectionTool()
{
  // Greet the user:
  ATH_MSG_DEBUG( "Initialising TauEfficiencyCorrectionsTool using TauSelectionTool" );

  // configure the tool depending on cuts used in the TauSelectionTool if
  // EfficiencyCorrectionTypes have not been set
  if (m_sRecommendationTag == "mc15-pre-recommendations" and m_vEfficiencyCorrectionTypes.size() == 0)
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
    // use electron OLR scale factors if TauSelectionTool applies electron OLR
    // cut
    if (m_tTauSelectionTool->m_iSelectionCuts & CutEleOLR and m_tTauSelectionTool->m_bEleOLR)
    {
      m_vEfficiencyCorrectionTypes.push_back(SFEleOLRHadTau);
      ATH_MSG_VERBOSE( "added SFEleOLRHadTau" );
    }
  }
  else if (m_sRecommendationTag == "mc12-final" and m_vEfficiencyCorrectionTypes.size() == 0)
  {
    // use jet ID scale factors if TauSelectionTool applies jet ID cut and use
    // the same workingpoint
    if (m_tTauSelectionTool->m_iSelectionCuts & CutJetIDWP)
    {
      m_iIDLevel = m_tTauSelectionTool->m_iJetIDWP;
      m_vEfficiencyCorrectionTypes.push_back(SFJetID);
    }
    // use electron ID scale factors if TauSelectionTool applies electron ID cut
    // and use the same workingpoint
    if (m_tTauSelectionTool->m_iSelectionCuts & CutEleBDTWP)
    {
      m_iEVLevel = m_tTauSelectionTool->m_iEleBDTWP;
      m_vEfficiencyCorrectionTypes.push_back(SFEleID);
    }
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauEfficiencyCorrectionsTool::initialize()
{
  // Greet the user:
  ATH_MSG_INFO( "Initialising TauEfficiencyCorrectionsTool" );

  if (m_bSkipTruthMatchCheck)
    ATH_MSG_WARNING("Truth match check will be skipped. This is ONLY FOR TESTING PURPOSE!");

  if (m_tTauSelectionTool)
    if (initializeWithTauSelectionTool().isFailure())
    {
      ATH_MSG_FATAL("problem in initializing with TauSelectionTool");
      return StatusCode::FAILURE;
    }

  // configure default set of variations if not set by the constructor using TauSelectionTool or the user
  if (m_sRecommendationTag == "mc15-pre-recommendations" and m_vEfficiencyCorrectionTypes.size() == 0)
    m_vEfficiencyCorrectionTypes = {SFRecoHadTau,
                                    SFJetIDHadTau,
                                    SFEleOLRHadTau
                                   };
  else if (m_sRecommendationTag == "mc12-final" and m_vEfficiencyCorrectionTypes.size() == 0)
    m_vEfficiencyCorrectionTypes = {SFJetID,
                                    SFEleID
                                   };

  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/EfficiencyCorrections/";

  if (m_sRecommendationTag == "mc15-pre-recommendations")
  {
    for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
    {
      if (iEfficiencyCorrectionType == SFJetIDHadTau)
      {
        // only set vars if they differ from "", which means they have been configured by the user
        if (m_sInputFilePathJetIDHadTau.empty()) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_mc15-prerec.root";
        if (m_sVarNameJetIDHadTau.length() == 0) m_sVarNameJetIDHadTau = "TauScaleFactorJetIDHadTau";
        CommonEfficiencyTool* tTool = new CommonEfficiencyTool(name()+"_JetIDHadTauTool", m_sInputFilePathJetIDHadTau, m_sVarNameJetIDHadTau, m_bSkipTruthMatchCheck, ConvertJetIDToString(m_iIDLevel));
        m_vCommonEfficiencyTools.push_back(tTool);
      }
      if (iEfficiencyCorrectionType == SFEleOLRHadTau)
      {
        // only set vars if they differ from "", which means they have been configured by the user
        if (m_sInputFilePathEleOLRHadTau.empty()) m_sInputFilePathEleOLRHadTau = sDirectory+"EleOLR_TrueHadTau_mc15-prerec.root";
        if (m_sVarNameEleOLRHadTau.length() == 0) m_sVarNameEleOLRHadTau = "TauScaleFactorEleOLRHadTau";

        CommonEfficiencyTool* tTool = new CommonEfficiencyTool(name()+"_EleOLRHadTauTool", m_sInputFilePathEleOLRHadTau, m_sVarNameEleOLRHadTau, m_bSkipTruthMatchCheck);
        m_vCommonEfficiencyTools.push_back(tTool);
      }
      // else if (iEfficiencyCorrectionType == SFEleOLRElectron)
      // {
      // 	// only set vars if they differ from "", which means they have been configured by the user
      // 	if (m_sInputFilePath.empty()) m_sInputFilePath = sDirectory+"EleOLR.root";
      // 	if (m_sVarNameBase.length() == 0) m_sVarNameBase = "TauScaleFactorEleOLRElectron";
      // 	m_bNoMultiprong = true;
      // 	m_bCheckTruth = false;
      // 	m_tCommonEfficiencyTool.reset(new TauEfficiencyEleOLRTool(name()+"TauEfficiencyEleOLRElectronTool"));
      // }
      //else
      else if (iEfficiencyCorrectionType == SFRecoHadTau)
      {
        // only set vars if they differ from "", which means they have been configured by the user
        if (m_sInputFilePathRecoHadTau.empty()) m_sInputFilePathRecoHadTau = sDirectory+"Reco_TrueHadTau_mc15-prerec.root";
        if (m_sVarNameRecoHadTau.length() == 0) m_sVarNameRecoHadTau = "TauScaleFactorReconstructionHadTau";

        CommonEfficiencyTool* tTool = new CommonEfficiencyTool(name()+"_ReconstructionHadTauTool", m_sInputFilePathRecoHadTau, m_sVarNameRecoHadTau, m_bSkipTruthMatchCheck);
        m_vCommonEfficiencyTools.push_back(tTool);
      }
    }
  }
  else if (m_sRecommendationTag == "mc12-final")
  {
    for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
    {
      if (iEfficiencyCorrectionType == SFJetID or iEfficiencyCorrectionType == SFJetIDHadTau )
      {
        // only set vars if they differ from "", which means they have been configured by the user
        if (m_sInputFilePathJetIDHadTau.empty())
        {
          if (m_bUsePtBinnedSF)
          {
            if (m_bUseInclusiveEta) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_mc12-final_ptbin_etainc.root";
            else                    m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_mc12-final_ptbin_etabin.root";
          }
          else
          {
            if (m_bUseInclusiveEta) m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_mc12-final_ptinc_etainc.root";
            else                    m_sInputFilePathJetIDHadTau = sDirectory+"JetID_TrueHadTau_mc12-final_ptinc_etabin.root";
          }
        }
        if (m_sVarNameJetIDHadTau.length() == 0) m_sVarNameJetIDHadTau = "TauScaleFactorJetIDHadTau";
        CommonEfficiencyTool* tTool = new TauEfficiencyJetIDTool(name()+"_JetIDHadTauTool", m_sInputFilePathJetIDHadTau, m_sVarNameJetIDHadTau);
        tTool->setParent(this);
        m_vCommonEfficiencyTools.push_back(tTool);

      }
      else if (iEfficiencyCorrectionType == SFContJetID)
      {
        // only set vars if they differ from "", which means they have been configured by the user
        if (m_sInputFilePathContJetIDHadTau.empty()) m_sInputFilePathContJetIDHadTau = sDirectory+"ContJetIDSF.root";
        if (m_sVarNameContJetIDHadTau.length() == 0) m_sVarNameContJetIDHadTau = "TauScaleFactorContJetIDHadTau";
        CommonEfficiencyTool* tTool = new TauEfficiencyContJetIDTool(name()+"TauEfficiencyContJetIDHadTauTool", m_sInputFilePathContJetIDHadTau, m_sVarNameContJetIDHadTau);
        tTool->setParent(this);
        m_vCommonEfficiencyTools.push_back(tTool);
      }
      else if (iEfficiencyCorrectionType == SFEleID or iEfficiencyCorrectionType == SFEleIDHadTau)
      {
        // only set vars if they differ from "", which means they have been configured by the user
        if (m_sInputFilePathEleIDHadTau.empty()) m_sInputFilePathEleIDHadTau = sDirectory+"EleID_TrueHadTau_mc12-final.root";
        if (m_sVarNameEleIDHadTau.length() == 0) m_sVarNameEleIDHadTau = "TauScaleFactorEleIDHadTau";
        CommonEfficiencyTool* tTool = new TauEfficiencyEleIDTool(name()+"TauEfficiencyEleIDHadTauTool", m_sInputFilePathEleIDHadTau, m_sVarNameEleIDHadTau);
        tTool->setParent(this);
        m_vCommonEfficiencyTools.push_back(tTool);
      }
    }
  }
  else
  {
    ATH_MSG_FATAL("Unknown RecommendationTag: "<<m_sRecommendationTag);
    return StatusCode::FAILURE;
  }

  for (auto tCommonEfficiencyTool : m_vCommonEfficiencyTools)
  {
    tCommonEfficiencyTool->msg().setLevel( this->msg().level() );
    tCommonEfficiencyTool->setParent(this);
    ATH_CHECK(tCommonEfficiencyTool->initialize());
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
void TauEfficiencyCorrectionsTool::printConfig(bool bAlways)
{
  if (bAlways)
  {
    for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
      ATH_MSG_ALWAYS( "EfficiencyCorrectionTypes " << iEfficiencyCorrectionType );
    ATH_MSG_ALWAYS( "InputFilePathRecoHadTau " << m_sInputFilePathRecoHadTau );
    ATH_MSG_ALWAYS( "InputFilePathEleOLRHadTau " << m_sInputFilePathEleOLRHadTau );
    ATH_MSG_ALWAYS( "InputFilePathJetIDHadTau " << m_sInputFilePathJetIDHadTau );
    ATH_MSG_ALWAYS( "InputFilePathContJetIDHadTau " << m_sInputFilePathContJetIDHadTau );
    ATH_MSG_ALWAYS( "InputFilePathEleIDHadTau " << m_sInputFilePathEleIDHadTau );
    ATH_MSG_ALWAYS( "VarNameRecoHadTau " << m_sVarNameRecoHadTau );
    ATH_MSG_ALWAYS( "VarNameEleOLRHadTau " << m_sVarNameEleOLRHadTau );
    ATH_MSG_ALWAYS( "VarNameJetIDHadTau " << m_sVarNameJetIDHadTau );
    ATH_MSG_ALWAYS( "VarNameContJetIDHadTau " << m_sVarNameContJetIDHadTau );
    ATH_MSG_ALWAYS( "VarNameEleIDHadTau " << m_sVarNameEleIDHadTau );
    ATH_MSG_ALWAYS( "RecommendationTag " << m_sRecommendationTag );

    ATH_MSG_ALWAYS( "UseIDExclusiveSF " << m_bUseIDExclusiveSF );
    ATH_MSG_ALWAYS( "UseInclusiveEta " << m_bUseInclusiveEta );
    ATH_MSG_ALWAYS( "UsePtBinnedSF " << m_bUsePtBinnedSF );
    ATH_MSG_ALWAYS( "UseHighPtUncert " << m_bUseHighPtUncert );
    ATH_MSG_ALWAYS( "IDLevel " << m_iIDLevel );
    ATH_MSG_ALWAYS( "EVLevel " << m_iEVLevel );
    ATH_MSG_ALWAYS( "OLRLevel " << m_iOLRLevel );
    ATH_MSG_ALWAYS( "ContSysType " << m_iContSysType );
  }
  else
  {
    ATH_MSG_DEBUG( "TauEfficiencyCorrectionsTool with name " << name() << " is configured as follows:" );
    for (auto iEfficiencyCorrectionType : m_vEfficiencyCorrectionTypes)
      ATH_MSG_DEBUG( "EfficiencyCorrectionTypes " << iEfficiencyCorrectionType );
    ATH_MSG_DEBUG( "InputFilePathRecoHadTau " << m_sInputFilePathRecoHadTau );
    ATH_MSG_DEBUG( "InputFilePathEleOLRHadTau " << m_sInputFilePathEleOLRHadTau );
    ATH_MSG_DEBUG( "InputFilePathJetIDHadTau " << m_sInputFilePathJetIDHadTau );
    ATH_MSG_DEBUG( "InputFilePathContJetIDHadTau " << m_sInputFilePathContJetIDHadTau );
    ATH_MSG_DEBUG( "InputFilePathEleIDHadTau " << m_sInputFilePathEleIDHadTau );
    ATH_MSG_DEBUG( "VarNameRecoHadTau " << m_sVarNameRecoHadTau );
    ATH_MSG_DEBUG( "VarNameEleOLRHadTau " << m_sVarNameEleOLRHadTau );
    ATH_MSG_DEBUG( "VarNameJetIDHadTau " << m_sVarNameJetIDHadTau );
    ATH_MSG_DEBUG( "VarNameContJetIDHadTau " << m_sVarNameContJetIDHadTau );
    ATH_MSG_DEBUG( "VarNameEleIDHadTau " << m_sVarNameEleIDHadTau );
    ATH_MSG_DEBUG( "RecommendationTag " << m_sRecommendationTag );

    ATH_MSG_DEBUG( "UseIDExclusiveSF " << m_bUseIDExclusiveSF );
    ATH_MSG_DEBUG( "UseInclusiveEta " << m_bUseInclusiveEta );
    ATH_MSG_DEBUG( "UsePtBinnedSF " << m_bUsePtBinnedSF );
    ATH_MSG_DEBUG( "UseHighPtUncert " << m_bUseHighPtUncert );
    ATH_MSG_DEBUG( "IDLevel " << m_iIDLevel );
    ATH_MSG_DEBUG( "EVLevel " << m_iEVLevel );
    ATH_MSG_DEBUG( "OLRLevel " << m_iOLRLevel );
    ATH_MSG_DEBUG( "ContSysType " << m_iContSysType );
  }
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyCorrectionsTool::getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
    double& eff )
{
  eff = 1.;
  for (auto tCommonEfficiencyTool : m_vCommonEfficiencyTools)
  {
    double dToolEff = 1.;
    CP::CorrectionCode tmpCorrectionCode = tCommonEfficiencyTool->getEfficiencyScaleFactor(xTau, dToolEff);
    if (tmpCorrectionCode != CP::CorrectionCode::Ok)
      return tmpCorrectionCode;
    eff *= dToolEff;
  }
  return CP::CorrectionCode::Ok;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyCorrectionsTool::applyEfficiencyScaleFactor( const xAOD::TauJet& xTau )
{
  for (auto tCommonEfficiencyTool : m_vCommonEfficiencyTools)
  {
    CP::CorrectionCode tmpCorrectionCode = tCommonEfficiencyTool->applyEfficiencyScaleFactor(xTau);
    if (tmpCorrectionCode != CP::CorrectionCode::Ok)
      return tmpCorrectionCode;
  }
  return CP::CorrectionCode::Ok;
}

/// returns: whether this tool is affected by the given systematis
//______________________________________________________________________________
bool TauEfficiencyCorrectionsTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  for (auto tCommonEfficiencyTool : m_vCommonEfficiencyTools)
    if (tCommonEfficiencyTool->isAffectedBySystematic(systematic))
      return true;
  return false;
}

/// returns: the list of all systematics this tool can be affected by
//______________________________________________________________________________
CP::SystematicSet TauEfficiencyCorrectionsTool::affectingSystematics() const
{
  CP::SystematicSet sAffectingSystematics;
  for (auto tCommonEfficiencyTool : m_vCommonEfficiencyTools)
    sAffectingSystematics.insert(tCommonEfficiencyTool->affectingSystematics());
  return sAffectingSystematics;
}

/// returns: the list of all systematics this tool recommends to use
//______________________________________________________________________________
CP::SystematicSet TauEfficiencyCorrectionsTool::recommendedSystematics() const
{
  CP::SystematicSet sRecommendedSystematics;
  for (auto tCommonEfficiencyTool : m_vCommonEfficiencyTools)
    sRecommendedSystematics.insert(tCommonEfficiencyTool->recommendedSystematics());
  return sRecommendedSystematics;
}

//______________________________________________________________________________
CP::SystematicCode TauEfficiencyCorrectionsTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  for (auto tCommonEfficiencyTool : m_vCommonEfficiencyTools)
    if (tCommonEfficiencyTool->applySystematicVariation(sSystematicSet) == CP::SystematicCode::Unsupported)
      return CP::SystematicCode::Unsupported;
  return CP::SystematicCode::Ok;
}

//=================================PRIVATE-PART=================================
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
  default:
    assert(false && "No valid ID level passed. Breaking up ...");
    break;
  }
  return "";
}

} // namespace TauAnalysisTools
