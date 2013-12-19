/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTowerCnvTool.cxx 576328 2013-12-19 16:09:32Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

// Local include(s):
#include "TriggerTowerCnvTool.h"

namespace xAODMaker {

   TriggerTowerCnvTool::TriggerTowerCnvTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ITriggerTowerCnvTool >( this );
   }

   StatusCode TriggerTowerCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the TriggerTower object
    * from the ESD and fills an xAOD::TriggerTowerContainer with them.
    *
    * @param esd The ESD object to take the TriggerTower from
    * @param xaod The xAOD container to fill the TriggerTower into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode TriggerTowerCnvTool::convert( const TriggerTowerCollection* esd,
                                            xAOD::TriggerTowerContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( xaod->size() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      TriggerTowerCollection::const_iterator itr = esd->begin();
      TriggerTowerCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::TriggerTower* x = new xAOD::TriggerTower();
        xaod->push_back( x );
        
        x->setCoord( (*itr)->eta() , (*itr)->phi() );
        x->setKey( (*itr)->key() );
        x->addEM(
                 (*itr)->emADC(),
                 (*itr)->emLUT(),
                 (*itr)->emBCIDvec(),
                 (*itr)->emBCIDext(),
                 (*itr)->emError(),
                 (*itr)->emPeak(),
                 (*itr)->emADCPeak()
                );
        
        x->addHad(
                 (*itr)->hadADC(),
                 (*itr)->hadLUT(),
                 (*itr)->hadBCIDvec(),
                 (*itr)->hadBCIDext(),
                 (*itr)->hadError(),
                 (*itr)->hadPeak(),
                 (*itr)->hadADCPeak()
                 );        
        
      }      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
