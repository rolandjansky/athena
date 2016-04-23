/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

#include "xAODJet/JetContainer.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"

#include "ToolTester.h"

BTagToolTester::BTagToolTester( const std::string & name, ISvcLocator * svcLoc) 
  : AthAlgorithm( name, svcLoc ),
    m_effTool( "BTaggingEfficiencyTool/BTaggingEfficiencyTool", this ) 
{
  declareProperty( "SGKey", m_sgKey = "AntiKt4LCTopoJets" );
  declareProperty( "BTaggingEfficiencyTool", m_effTool);
}

StatusCode BTagToolTester::initialize() {
  ATH_MSG_INFO( "Initialising - Package version : " << PACKAGE_VERSION);
  ATH_MSG_DEBUG( "SGKey = " << m_sgKey);
  ATH_MSG_DEBUG( "BTaggingEfficiencyTool = " << m_effTool );
  
  ATH_CHECK( m_effTool.retrieve() );
  
  return StatusCode::SUCCESS;
}

StatusCode BTagToolTester::execute() {
  const xAOD::JetContainer * jets = 0;
  ATH_CHECK( evtStore()->retrieve( jets, m_sgKey));
  ATH_MSG_INFO( "Number of jets: " << jets->size());
  
  for( xAOD::JetContainer::const_iterator itr = jets->begin();
       itr != jets->end(); ++itr) {
    const xAOD::Jet * jet = *itr;
    ATH_MSG_INFO( " Jet : eta = " << jet->eta() << ", phi = " << jet->phi() << ", pt = " << jet->pt() );
    
    float eff = 0.;
    float sf = 0.;
    
    
    if( ! m_effTool->getEfficiency(*jet,eff)){
      ATH_MSG_WARNING( "Failed to get b-jet efficiency");
      return StatusCode::FAILURE;
    }
    if( ! m_effTool->getScaleFactor(*jet,sf)) {
      ATH_MSG_WARNING( "Failed to get b-jet scale factor");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "eff = " << eff << " sf = " << sf );
  }
  
  return StatusCode::SUCCESS;
}
