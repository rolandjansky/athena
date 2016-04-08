/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1CaloTriggerTowerDecoratorAlg.cxx 684325 2015-07-20 13:47:57Z amazurov $

// TrigT1 common definitions
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

// Local include(s):
#include "TrigT1CaloCalibTools/L1CaloTriggerTowerDecoratorAlg.h"

// EDM include(s):
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

namespace LVL1 
{

  L1CaloTriggerTowerDecoratorAlg::L1CaloTriggerTowerDecoratorAlg( const std::string& name, ISvcLocator* svcLoc ) :
    AthAlgorithm( name, svcLoc ),
    m_sgKey_TriggerTowers(LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation),
    m_caloCellEnergy("CaloCellEnergy"),
    m_caloCellET("CaloCellET"),
    m_caloCellEnergyByLayer("CaloCellEnergyByLayer"),
    m_caloCellETByLayer ("CaloCellETByLayer"),
    m_caloCellsQuality ("CaloCellQuality")
  {  
    declareProperty( "sgKey_TriggerTowers" , m_sgKey_TriggerTowers = LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation );
 
    declareProperty( "DecorName_caloCellEnergy" , m_caloCellEnergy = "CaloCellEnergy" );
    declareProperty( "DecorName_caloCellET" , m_caloCellET = "CaloCellET" );
    declareProperty( "DecorName_caloCellEnergyByLayer" , m_caloCellEnergyByLayer = "CaloCellEnergyByLayer" );
    declareProperty( "DecorName_caloCellETByLayer" , m_caloCellETByLayer = "CaloCellETByLayer" );
    declareProperty( "DecorName_caloCellQuality" , m_caloCellsQuality = "CaloCellQuality" );    
    
    declareProperty( "TriggerTowerTools", m_ttTools );    
  }

  StatusCode L1CaloTriggerTowerDecoratorAlg::initialize()
  {
    ATH_MSG_INFO("TrigT1CaloCalibTools/L1CaloTriggerTowerDecoratorAlg::initialize()" );
    CHECK( m_ttTools.retrieve() );
    
    // Return gracefully:
    return StatusCode::SUCCESS;  
  }

  StatusCode L1CaloTriggerTowerDecoratorAlg::execute()
  {
 
    // Shall I proceed?
    if (evtStore()->contains<xAOD::TriggerTowerContainer>( m_sgKey_TriggerTowers )) {
       
      CHECK( m_ttTools->initCaloCells() );
        
      const xAOD::TriggerTowerContainer* tts(nullptr);
      CHECK( evtStore()->retrieve( tts , m_sgKey_TriggerTowers ) );
      for (const auto x : *tts) {      
    
        // Don't need CellEnergy or CellET, figure them out from the byLayer sums. Waste of disk space.
        // x->auxdecor< float >( m_caloCellEnergy ) = m_ttTools->caloCellsEnergy( *x );
        // x->auxdecor< float >( m_caloCellET ) = m_ttTools->caloCellsET( *x );
        x->auxdecor< std::vector<float> >( m_caloCellEnergyByLayer ) = m_ttTools->caloCellsEnergyByLayer( *x );
        x->auxdecor< std::vector<float> >( m_caloCellETByLayer ) = m_ttTools->caloCellsETByLayer( *x );   
        x->auxdecor< float >( m_caloCellsQuality ) = m_ttTools->caloCellsQuality( *x );
        

      }
    } // Trigger towers present in Storegate
        
    // Return gracefully:
    return StatusCode::SUCCESS;  
  }
}
