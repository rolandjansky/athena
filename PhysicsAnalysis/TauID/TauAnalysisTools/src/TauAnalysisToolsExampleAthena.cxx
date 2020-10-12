/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODTau/TauJetContainer.h"

// Local include(s):
#include "TauAnalysisToolsExampleAthena.h"

using namespace TauAnalysisTools;

TauAnalysisToolsExampleAthena::TauAnalysisToolsExampleAthena( const std::string& name, ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc )
  , m_selTool( "TauAnalysisTools::TauSelectionTool/TauSelectionTool", this )
  , m_smearTool( "TauAnalysisTools::TauSmearingTool/TauSmearingTool", this )
  , m_effTool( "TauAnalysisTools::TauEfficiencyCorrectionsTool/TauEfficiencyCorrectionsTool", this )
{
  declareProperty( "SGKey", m_sgKey = "TauRecContainer" );

  declareProperty( "TauSelectionTool", m_selTool );
  declareProperty( "TauSmearingTool", m_smearTool );
  declareProperty( "TauEfficiencyTool", m_effTool );
}

StatusCode TauAnalysisToolsExampleAthena::initialize()
{
  // Greet the user:
  ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "SGKey = " << m_sgKey );
  ATH_MSG_DEBUG( "TauSelectionTool  = " << m_selTool );
  ATH_MSG_DEBUG( "TauSmearingTool   = " << m_smearTool );
  ATH_MSG_DEBUG( "TauEfficiencyTool = " << m_effTool );

  // Retrieve the tools:
  ATH_CHECK( m_selTool.retrieve() );
  ATH_CHECK( m_smearTool.retrieve() );
  ATH_CHECK( m_effTool.retrieve() );

  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode TauAnalysisToolsExampleAthena::execute()
{
  // Retrieve the taus:
  const xAOD::TauJetContainer* taus = 0;
  ATH_CHECK( evtStore()->retrieve( taus, m_sgKey ) );
  ATH_MSG_INFO( "Number of taus: " << taus->size() );

  // Loop over them:
  xAOD::TauJetContainer::const_iterator tau_itr = taus->begin();
  xAOD::TauJetContainer::const_iterator tau_end = taus->end();
  for( ; tau_itr != tau_end; ++tau_itr )
  {

    ATH_MSG_DEBUG( "  current tau: eta = " << ( *tau_itr )->eta()
                   << ", phi = " << ( *tau_itr )->phi()
                   << ", pt = " << ( *tau_itr )->pt() );

    // Select "good" taus:
    if( ! m_selTool->accept( **tau_itr ) ) continue;

    // Print some info about the selected tau:
    ATH_MSG_INFO( "  Selected tau: eta = " << ( *tau_itr )->eta()
                  << ", phi = " << ( *tau_itr )->phi()
                  << ", pt = " << ( *tau_itr )->pt() );

    // copy constant objects to non-constant
    xAOD::TauJet* tau = 0;
    tau = new xAOD::TauJet();
    tau->makePrivateStore( **tau_itr );

    ATH_CHECK (m_effTool->applyEfficiencyScaleFactor(*tau));

    ATH_MSG_INFO( "  sf = " << tau->auxdata< double >( "TauScaleFactorJetID" ) );

    ATH_CHECK (m_smearTool->applyCorrection(*tau));
    ATH_MSG_INFO( "Unsmeared tau pt " << tau->pt() << " Smeared tau pt: " << tau->p4().Pt());
  }

  // Return gracefully:
  return StatusCode::SUCCESS;
}
