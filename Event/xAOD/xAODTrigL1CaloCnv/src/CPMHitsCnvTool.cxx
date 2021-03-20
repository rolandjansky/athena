/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMHitsCnvTool.cxx 576052 2013-12-18 09:13:50Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/CPMHitsCollection.h"
#include "xAODTrigL1Calo/CPMHitsContainer.h"
#include "xAODTrigL1Calo/CPMHitsAuxContainer.h"

// Local include(s):
#include "CPMHitsCnvTool.h"

namespace xAODMaker {

   CPMHitsCnvTool::CPMHitsCnvTool( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ICPMHitsCnvTool >( this );
   }

   StatusCode CPMHitsCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the CPMHits object
    * from the ESD and fills an xAOD::CPMHitsContainer with them.
    *
    * @param esd The ESD object to take the CPMHits from
    * @param xaod The xAOD container to fill the CPMHits into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode CPMHitsCnvTool::convert( const CPMHitsCollection* esd,
                                       xAOD::CPMHitsContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      CPMHitsCollection::const_iterator itr = esd->begin();
      CPMHitsCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::CPMHits* x = new xAOD::CPMHits();
        xaod->push_back( x );
        
        x->setCrate( (*itr)->crate() );
        x->setModule( (*itr)->module() );
        x->setPeak( (*itr)->peak() );
        x->addHits( (*itr)->HitsVec0() , (*itr)->HitsVec1()  );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
