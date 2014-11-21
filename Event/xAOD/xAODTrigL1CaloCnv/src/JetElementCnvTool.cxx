/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElementCnvTool.cxx 576052 2013-12-18 09:13:50Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/JetElementCollection.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/JetElementAuxContainer.h"

// Local include(s):
#include "JetElementCnvTool.h"

namespace xAODMaker {

   JetElementCnvTool::JetElementCnvTool( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IJetElementCnvTool >( this );
   }

   StatusCode JetElementCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the JetElement object
    * from the ESD and fills an xAOD::JetElementContainer with them.
    *
    * @param esd The ESD object to take the JetElement from
    * @param xaod The xAOD container to fill the JetElement into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode JetElementCnvTool::convert( const JetElementCollection* esd,
                                          xAOD::JetElementContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( xaod->size() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      JetElementCollection::const_iterator itr = esd->begin();
      JetElementCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::JetElement* x = new xAOD::JetElement();
        xaod->push_back( x );
        
        x->setPhi( (*itr)->phi() );
        x->setEta( (*itr)->eta() );
        x->setKey( (*itr)->key() );
        x->setPeak( (*itr)->peak() );
        x->setEmEnergyVec( (*itr)->emEnergyVec() );
        x->setHadEnergyVec( (*itr)->hadEnergyVec() );
        x->setEmErrorVec( (*itr)->emErrorVec() );
        x->setHadErrorVec( (*itr)->hadErrorVec() );
        x->setLinkErrorVec( (*itr)->linkErrorVec() );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
