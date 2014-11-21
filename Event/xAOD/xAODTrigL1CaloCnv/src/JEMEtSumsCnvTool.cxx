/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSumsCnvTool.cxx 576052 2013-12-18 09:13:50Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/JEMEtSumsCollection.h"
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/JEMEtSumsAuxContainer.h"

// Local include(s):
#include "JEMEtSumsCnvTool.h"

namespace xAODMaker {

   JEMEtSumsCnvTool::JEMEtSumsCnvTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IJEMEtSumsCnvTool >( this );
   }

   StatusCode JEMEtSumsCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the JEMEtSums object
    * from the ESD and fills an xAOD::JEMEtSumsContainer with them.
    *
    * @param esd The ESD object to take the JEMEtSums from
    * @param xaod The xAOD container to fill the JEMEtSums into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode JEMEtSumsCnvTool::convert( const JEMEtSumsCollection* esd,
                                         xAOD::JEMEtSumsContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( xaod->size() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      JEMEtSumsCollection::const_iterator itr = esd->begin();
      JEMEtSumsCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::JEMEtSums* x = new xAOD::JEMEtSums();
        xaod->push_back( x );
        
        x->setCrate( (*itr)->crate() );
        x->setModule( (*itr)->module() );
        x->setPeak( (*itr)->peak() );
        x->setEtVec( (*itr)->EtVec() );
        x->setExVec( (*itr)->ExVec() );
        x->setEyVec( (*itr)->EyVec() );  
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
