/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTowerCnvTool.cxx 576328 2013-12-19 16:09:32Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/CPMTowerCollection.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "xAODTrigL1Calo/CPMTowerAuxContainer.h"

// Local include(s):
#include "CPMTowerCnvTool.h"

namespace xAODMaker {

   CPMTowerCnvTool::CPMTowerCnvTool( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ICPMTowerCnvTool >( this );
   }

   StatusCode CPMTowerCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the CPMTower object
    * from the ESD and fills an xAOD::CPMTowerContainer with them.
    *
    * @param esd The ESD object to take the CPMTower from
    * @param xaod The xAOD container to fill the CPMTower into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode CPMTowerCnvTool::convert( const CPMTowerCollection* esd,
                                        xAOD::CPMTowerContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( xaod->size() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      CPMTowerCollection::const_iterator itr = esd->begin();
      CPMTowerCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::CPMTower* x = new xAOD::CPMTower();
        xaod->push_back( x );
        
        x->setEmEnergyVec( (*itr)->emEnergyVec() );
        x->setHadEnergyVec( (*itr)->hadEnergyVec() );
        x->setEmErrorVec( (*itr)->emErrorVec() );
        x->setHadErrorVec( (*itr)->hadErrorVec() );
        x->setEta( (*itr)->eta() );
        x->setPhi( (*itr)->phi() );
        x->setPeak( (*itr)->peak() );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
