/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// /***************************************************************************
//                           EnergyTrigger.cxx  -  description
//                              -------------------
//     begin                : Wed Dec 13 2000
//     copyright            : (C) 2000 by moyse
//     email                : moyse@heppch.ph.qmw.ac.uk
//  ***************************************************************************/
//
//    updated: June 20, 2001 M. Wielers
//             move to Storegate
//
//    updated: Sep 10 2007, Alan Watson
//             use L1EtTools for trigger algorithm emulation
//
//================================================
// EnergyTrigger class Implementation
// ================================================
//
//
//

#include "TrigT1CaloSim/EnergyTrigger.h"


namespace LVL1 {


//--------------------------------
// Constructors and destructors
//--------------------------------

EnergyTrigger::EnergyTrigger
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator )
{
  declareProperty( "JetElementLocation",   m_JetElementLocation ) ;
  declareProperty(  "EnergyRoILocation",   m_energyRoILocation );
  declareProperty(  "EnergyCTPLocation",   m_energyCTPLocation );
  declareProperty(  "JEMEtSumsLocation",   m_jemEtSumsLocation );
}

// Destructor
EnergyTrigger::~EnergyTrigger() {
  
	
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode EnergyTrigger::initialize()
{
  ATH_MSG_WARNING("EnergyTrigger is DEPRECATED in Run2");
  return StatusCode::SUCCESS ;
}

//---------------------------------
// finalise()
//---------------------------------

StatusCode EnergyTrigger::finalize()
{
   
   ATH_MSG_INFO( "Finalizing" );
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode EnergyTrigger::execute( )
{
  return StatusCode::SUCCESS ;
}


} // end of LVL1 namespace bracket
