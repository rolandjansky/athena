/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1CaloTriggerTowerDecoratorAlg.cxx 652902 2015-03-10 11:06:57Z morrisj $

// Local include(s):
#include "TrigT1CaloCalibTools/L1CaloTriggerTowerDecoratorAlg.h"

// EDM include(s):
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

namespace LVL1 
{

  L1CaloTriggerTowerDecoratorAlg::L1CaloTriggerTowerDecoratorAlg( const std::string& name, ISvcLocator* svcLoc ) :
    AthAlgorithm( name, svcLoc ),
    m_sgKey_TriggerTowers("xAODTriggerTowers"),
    m_caloCellEnergy("CaloCellEnergy"),
    m_caloCellET("CaloCellET"),
    m_caloCellEnergyByLayer("CaloCellEnergyByLayer"),
    m_caloCellETByLayer ("CaloCellETByLayer"),  
    m_caloCellsQuality ("CaloCellQuality"),   
    m_ttTools(nullptr)
  {  
    declareProperty( "sgKey_TriggerTowers" , m_sgKey_TriggerTowers = "xAODTriggerTowers" );
    
    declareProperty( "DecorName_caloCellEnergy" , m_caloCellEnergy = "CaloCellEnergy" );
    declareProperty( "DecorName_caloCellET" , m_caloCellET = "CaloCellET" );
    declareProperty( "DecorName_caloCellEnergyByLayer" , m_caloCellEnergyByLayer = "CaloCellEnergyByLayer" );
    declareProperty( "DecorName_caloCellETByLayer" , m_caloCellETByLayer = "CaloCellETByLayer" );
    declareProperty( "DecorName_caloCellQuality" , m_caloCellsQuality = "CaloCellQuality" );    
    
    declareProperty( "TriggerTowerTools", m_ttTools );    
  }

  StatusCode L1CaloTriggerTowerDecoratorAlg::initialize()
  {
    CHECK( m_ttTools.retrieve() );
    
    // Return gracefully:
    return StatusCode::SUCCESS;  
  }

  StatusCode L1CaloTriggerTowerDecoratorAlg::execute()
  {
    //ATH_MSG_INFO( "StoreGate contents:\n\n" << evtStore()->dump() ); 
 
    // Shall I proceed?
    if (evtStore()->contains<xAOD::TriggerTowerContainer>( m_sgKey_TriggerTowers )) {
       
      CHECK( m_ttTools->initCaloCells() );
  //     CHECK( m_ttTools->initDatabase() );
        
      const xAOD::TriggerTowerContainer* tts(nullptr);
      CHECK( evtStore()->retrieve( tts , m_sgKey_TriggerTowers ) );
      for (const auto x : *tts) {      
    
        x->auxdecor< float >( m_caloCellEnergy ) = m_ttTools->caloCellsEnergy( *x );
        x->auxdecor< float >( m_caloCellET ) = m_ttTools->caloCellsET( *x );
        x->auxdecor< std::vector<float> >( m_caloCellEnergyByLayer ) = m_ttTools->caloCellsEnergyByLayer( *x );
        x->auxdecor< std::vector<float> >( m_caloCellETByLayer ) = m_ttTools->caloCellsETByLayer( *x );   
        x->auxdecor< float >( m_caloCellsQuality ) = m_ttTools->caloCellsQuality( *x );
        

      }
    } // Trigger towers present in Storegate
        
    // Return gracefully:
    return StatusCode::SUCCESS;  
  }
}
