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

TauEfficiencyCorrectionsTool::TauEfficiencyCorrectionsTool( const std::string& sName )
  : asg::AsgTool( sName ),
    m_iEfficiencyCorrectionType(SFJetID),
    m_mCommonEfficiencyTool(),
    m_tCommonEfficiencyTool(0),
    m_sInputFilePath(""),
    m_sVarNameBase(""),
    m_bUseIDExclusiveSF(false),
    m_bUseInclusiveEta(false),
    m_bUsePtBinnedSF(false),
    m_bUseHighPtUncert(false),
    m_iSysDirection(0),
    m_iIDLevel(JETIDBDTTIGHT),
    m_iEVLevel(ELEIDBDTLOOSE),
    m_iOLRLevel(OLRLOOSEPP),
    m_iContSysType(TOTAL)
{
  declareProperty( "EfficiencyCorrectionType", m_iEfficiencyCorrectionType );
  declareProperty( "InputFilePath", m_sInputFilePath );
  declareProperty( "VarNameBase", m_sVarNameBase );
  declareProperty( "UseIDExclusiveSF", m_bUseIDExclusiveSF );
  declareProperty( "UseInclusiveEta", m_bUseInclusiveEta );
  declareProperty( "UsePtBinnedSF", m_bUsePtBinnedSF );
  declareProperty( "UseHighPtUncert", m_bUseHighPtUncert );
  declareProperty( "SysDirection", m_iSysDirection );
  declareProperty( "IDLevel", m_iIDLevel );
  declareProperty( "EVLevel", m_iEVLevel );
  declareProperty( "OLRLevel", m_iOLRLevel );
  declareProperty( "ContSysType", m_iContSysType );
}

TauEfficiencyCorrectionsTool::TauEfficiencyCorrectionsTool( const std::string& sName, TauSelectionTool* tTauSelectionTool )
  : TauEfficiencyCorrectionsTool( sName)
{
  // set jet ID level in depending on config of jet ID cut and if the cut was applied
  if (tTauSelectionTool->m_iSelectionCuts & CutJetIDWP)
    m_iIDLevel = tTauSelectionTool->m_iJetIDWP;
  // set ele ID level in depending on config of ele ID cut and if the cut was applied
  if (tTauSelectionTool->m_iSelectionCuts & CutEleBDTWP)
    m_iEVLevel = tTauSelectionTool->m_iEleBDTWP;
}


TauEfficiencyCorrectionsTool::~TauEfficiencyCorrectionsTool()
{
  delete m_tCommonEfficiencyTool;
}

StatusCode TauEfficiencyCorrectionsTool::initialize()
{
  // Greet the user:
  ATH_MSG_INFO( "Initialising TauEfficiencyCorrectionsTool" );
  ATH_MSG_DEBUG( "Efficiency correction type " << m_iEfficiencyCorrectionType );

  std::string sDirectory = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/EfficiencyCorrections/";

  if (m_iEfficiencyCorrectionType == SFJetID)
  {
    // only set vars if they differ from "", which means they have been configured by the user
    if (m_sInputFilePath.empty()) m_sInputFilePath = sDirectory+"JetIDSF.root";
    if (m_sVarNameBase.length() == 0) m_sVarNameBase = "TauScaleFactorJetID";
    m_tCommonEfficiencyTool = new TauEfficiencyJetIDTool("TauEfficiencyJetIDTool");
    // if ( sc.isFailure() ) return sc;
  }
  if (m_iEfficiencyCorrectionType == SFContJetID)
  {
    // only set vars if they differ from "", which means they have been configured by the user
    if (m_sInputFilePath.empty()) m_sInputFilePath = sDirectory+"ContJetIDSF.root";
    if (m_sVarNameBase.length() == 0) m_sVarNameBase = "TauScaleFactorContJetID";
    m_tCommonEfficiencyTool = new TauEfficiencyContJetIDTool("TauEfficiencyContJetIDTool");
  }
  if (m_iEfficiencyCorrectionType == SFEleID)
  {
    // only set vars if they differ from "", which means they have been configured by the user
    if (m_sInputFilePath.empty()) m_sInputFilePath = sDirectory+"EleIDSF.root";
    if (m_sVarNameBase.length() == 0) m_sVarNameBase = "TauScaleFactorEleID";
    m_tCommonEfficiencyTool = new TauEfficiencyEleIDTool("TauEfficiencyEleIDTool");
  }

  if (!m_tCommonEfficiencyTool)
  {
    ATH_MSG_FATAL("unknown Efficiency correction type "<<m_iEfficiencyCorrectionType);
    return StatusCode::FAILURE;
  }
  // only set vars if they differ from "", which means they have been configured by the user

  m_tCommonEfficiencyTool->msg().setLevel( this->msg().level() );
  m_tCommonEfficiencyTool->setParent(this);
  ATH_CHECK(m_tCommonEfficiencyTool->initialize());

  // Add the affecting systematics to the global registry
  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  if (!registry.registerSystematics(*this))
  {
    ATH_MSG_ERROR ("Unable to register the systematics");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

void TauEfficiencyCorrectionsTool::printConfig()
{
  ATH_MSG_ALWAYS( "EfficiencyCorrectionType " << m_iEfficiencyCorrectionType );
  ATH_MSG_ALWAYS( "InputFilePath " << m_sInputFilePath );
  ATH_MSG_ALWAYS( "VarNameBase " << m_sVarNameBase );
  ATH_MSG_ALWAYS( "UseIDExclusiveSF " << m_bUseIDExclusiveSF );
  ATH_MSG_ALWAYS( "UseInclusiveEta " << m_bUseInclusiveEta );
  ATH_MSG_ALWAYS( "UsePtBinnedSF " << m_bUsePtBinnedSF );
  ATH_MSG_ALWAYS( "UseHighPtUncert " << m_bUseHighPtUncert );
  ATH_MSG_ALWAYS( "SysDirection " << m_iSysDirection );
  ATH_MSG_ALWAYS( "IDLevel " << m_iIDLevel );
  ATH_MSG_ALWAYS( "EVLevel " << m_iEVLevel );
  ATH_MSG_ALWAYS( "OLRLevel " << m_iOLRLevel );
  ATH_MSG_ALWAYS( "ContSysType " << m_iContSysType );

}

CP::CorrectionCode TauEfficiencyCorrectionsTool::getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
    double& eff )
{

  return m_tCommonEfficiencyTool->getEfficiencyScaleFactor(xTau, eff);
}

CP::CorrectionCode TauEfficiencyCorrectionsTool::applyEfficiencyScaleFactor( const xAOD::TauJet& xTau )
{
  return m_tCommonEfficiencyTool->applyEfficiencyScaleFactor(xTau);
}

CP::CorrectionCode TauEfficiencyCorrectionsTool::getEfficiencyScaleFactorStatUnc( const xAOD::TauJet& xTau,
    double& eff )
{
  ATH_MSG_ERROR("getEfficiencyScaleFactorStatUnc: This function is deprecated, please use the recommendedSystematic() funtcionality in combination with applySystematicVariation(...) instead");
  return m_tCommonEfficiencyTool->getEfficiencyScaleFactorStatUnc(xTau, eff);
}

CP::CorrectionCode TauEfficiencyCorrectionsTool::applyEfficiencyScaleFactorStatUnc( const xAOD::TauJet& xTau )
{
  ATH_MSG_ERROR("applyEfficiencyScaleFactorStatUnc: This function is deprecated, please use the recommendedSystematic() funtcionality in combination with applySystematicVariation(...) instead");
  return m_tCommonEfficiencyTool->applyEfficiencyScaleFactorStatUnc(xTau);
}

CP::CorrectionCode TauEfficiencyCorrectionsTool::getEfficiencyScaleFactorSysUnc( const xAOD::TauJet& xTau,
    double& eff )
{
  ATH_MSG_ERROR("getEfficiencyScaleFactorSysUnc: This function is deprecated, please use the recommendedSystematic() functionality in combination with applySystematicVariation(...) instead");
  return m_tCommonEfficiencyTool->getEfficiencyScaleFactorSysUnc(xTau, eff);
}

CP::CorrectionCode TauEfficiencyCorrectionsTool::applyEfficiencyScaleFactorSysUnc( const xAOD::TauJet& xTau )
{
  ATH_MSG_ERROR("applyEfficiencyScaleFactorSysUnc: This function is deprecated, please use the recommendedSystematic() funtcionality in combination with applySystematicVariation(...) instead");
  return m_tCommonEfficiencyTool->applyEfficiencyScaleFactorSysUnc(xTau);
}

/// returns: whether this tool is affected by the given systematis
bool TauEfficiencyCorrectionsTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  return m_tCommonEfficiencyTool->isAffectedBySystematic( systematic );
}

/// returns: the list of all systematics this tool can be affected by
CP::SystematicSet TauEfficiencyCorrectionsTool::affectingSystematics() const
{
  return m_tCommonEfficiencyTool->affectingSystematics();
}

/// returns: the list of all systematics this tool recommends to use
CP::SystematicSet TauEfficiencyCorrectionsTool::recommendedSystematics() const
{
  return m_tCommonEfficiencyTool->recommendedSystematics();
}

CP::SystematicCode TauEfficiencyCorrectionsTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  return m_tCommonEfficiencyTool->applySystematicVariation( sSystematicSet );
}


} // namespace TauAnalysisTools
