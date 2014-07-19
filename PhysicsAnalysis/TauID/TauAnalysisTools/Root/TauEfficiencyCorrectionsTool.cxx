/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

namespace TauAnalysisTools {

  TauEfficiencyCorrectionsTool::TauEfficiencyCorrectionsTool( const std::string& name )
    : asg::AsgTool( name ),
      m_iEfficiencyCorrectionType(SFJetID),
#ifdef ASGTOOL_ATHENA
      m_sSharepath("../share/"),
#else
      m_sSharepath("../share/EfficiencyCorrections/"),
#endif
      m_sFileName(""),
      m_bUseIDExclusiveSF(false),
      m_bUseInclusiveEta(false),
      m_bUsePtBinnedSF(false),
      m_bUseHighPtUncert(false),
      m_iSysDirection(1),
      m_iIDLevel(JETIDBDTLOOSE)
  {
    declareProperty( "EfficiencyCorrectionType", m_iEfficiencyCorrectionType );
    declareProperty( "SharePath", m_sSharepath );
    declareProperty( "FileName", m_sFileName );
    declareProperty( "UseIDExclusiveSF", m_bUseIDExclusiveSF );
    declareProperty( "UseInclusiveEta", m_bUseInclusiveEta );
    declareProperty( "UsePtBinnedSF", m_bUsePtBinnedSF );
    declareProperty( "UseHighPtUncert", m_bUseHighPtUncert );
    declareProperty( "SysDirection", m_iSysDirection );
    declareProperty( "IDLevel", m_iIDLevel );
  }

  StatusCode TauEfficiencyCorrectionsTool::initialize() {
    // Greet the user:
    ATH_MSG_INFO( "Initialising TauEfficiencyCorrectionsTool" );
    ATH_MSG_DEBUG( "Efficiency correction type " << m_iEfficiencyCorrectionType );
    
    if (m_iEfficiencyCorrectionType == SFJetID){
#ifdef ASGTOOL_ATHENA
      ATH_CHECK(setProperty( "FileName" , m_sFileName = "JetIDSF.root"));
#else
      setProperty( "FileName" , m_sFileName = "JetIDSF.root");
#endif
      m_tCommonEfficiencyTool = new TauEfficiencyJetIDTool("TauEfficiencyJetIDTool");
      StatusCode sc = m_tCommonEfficiencyTool->configure(this->getPropertyMgr());
      if ( sc.isFailure() ) return sc;
    }
#ifdef DEVEL
    if (m_iEfficiencyCorrectionType == SFContJetID){
      ATH_CHECK(setProperty( "FileName" , m_sFileName = "ContJetIDSF.root")); 
      m_tCommonEfficiencyTool = new TauEfficiencyContJetIDTool("TauEfficiencyContJetIDTool", this->getPropertyMgr());
    }
    if (m_iEfficiencyCorrectionType == SFEleVeto){
      ATH_CHECK(setProperty( "FileName" , m_sFileName = "EleIDSF.root")); 
      m_tCommonEfficiencyTool = new TauEfficiencyEleIDTool("TauEfficiencyContJetIDTool", this->getPropertyMgr());
    }
#endif // DEVEL
    return StatusCode::SUCCESS;
  }

  CorrectionCode TauEfficiencyCorrectionsTool::getEfficiencyScaleFactor( const xAOD::TauJet& xTau,
									 double& eff ) {
    
    m_tCommonEfficiencyTool->getEfficiencyScaleFactor(xTau, eff);
    return CorrectionCode::OK;
  }
  
  CorrectionCode TauEfficiencyCorrectionsTool::applyEfficiencyScaleFactor( xAOD::TauJet& xTau ) {
    return m_tCommonEfficiencyTool->applyEfficiencyScaleFactor(xTau);
  }
  
  CorrectionCode TauEfficiencyCorrectionsTool::getEfficiencyScaleFactorStatUnc( const xAOD::TauJet& xTau,
										double& eff ) {

    m_tCommonEfficiencyTool->getEfficiencyScaleFactorStatUnc(xTau, eff);
    return CorrectionCode::OK;
  }

  CorrectionCode TauEfficiencyCorrectionsTool::applyEfficiencyScaleFactorStatUnc( xAOD::TauJet& xTau ) {
    return m_tCommonEfficiencyTool->applyEfficiencyScaleFactorStatUnc(xTau);
  }

  CorrectionCode TauEfficiencyCorrectionsTool::getEfficiencyScaleFactorSysUnc( const xAOD::TauJet& xTau,
									       double& eff ) {

    m_tCommonEfficiencyTool->getEfficiencyScaleFactorSysUnc(xTau, eff);
    return CorrectionCode::OK;
  }

  CorrectionCode TauEfficiencyCorrectionsTool::applyEfficiencyScaleFactorSysUnc( xAOD::TauJet& xTau ) {
    return m_tCommonEfficiencyTool->applyEfficiencyScaleFactorSysUnc(xTau);
  }

} // namespace TauAnalysisTools
