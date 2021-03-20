/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMHitsCnvTool.cxx 576052 2013-12-18 09:13:50Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/JEMHitsCollection.h"
#include "xAODTrigL1Calo/JEMHitsContainer.h"
#include "xAODTrigL1Calo/JEMHitsAuxContainer.h"

// Local include(s):
#include "JEMHitsCnvTool.h"

namespace xAODMaker {

   JEMHitsCnvTool::JEMHitsCnvTool( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IJEMHitsCnvTool >( this );
   }

   StatusCode JEMHitsCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the JEMHits object
    * from the ESD and fills an xAOD::JEMHitsContainer with them.
    *
    * @param esd The ESD object to take the JEMHits from
    * @param xaod The xAOD container to fill the JEMHits into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode JEMHitsCnvTool::convert( const JEMHitsCollection* esd,
                                       xAOD::JEMHitsContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      JEMHitsCollection::const_iterator itr = esd->begin();
      JEMHitsCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::JEMHits* x = new xAOD::JEMHits();
        xaod->push_back( x );
        
        x->setCrate( (*itr)->crate() );
        x->setModule( (*itr)->module() );
        x->setPeak( (*itr)->peak() );
        x->setJetHitsVec( (*itr)->JetHitsVec() );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
