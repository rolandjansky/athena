/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMJetHitsCnvTool.cxx 576052 2013-12-18 09:13:50Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/CMMJetHitsCollection.h"
#include "xAODTrigL1Calo/CMMJetHitsContainer.h"
#include "xAODTrigL1Calo/CMMJetHitsAuxContainer.h"

// Local include(s):
#include "CMMJetHitsCnvTool.h"

namespace xAODMaker {

   CMMJetHitsCnvTool::CMMJetHitsCnvTool( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ICMMJetHitsCnvTool >( this );
   }

   StatusCode CMMJetHitsCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the CMMJetHits object
    * from the ESD and fills an xAOD::CMMJetHitsContainer with them.
    *
    * @param esd The ESD object to take the CMMJetHits from
    * @param xaod The xAOD container to fill the CMMJetHits into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode CMMJetHitsCnvTool::convert( const CMMJetHitsCollection* esd,
                                          xAOD::CMMJetHitsContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      CMMJetHitsCollection::const_iterator itr = esd->begin();
      CMMJetHitsCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::CMMJetHits* x = new xAOD::CMMJetHits();
        xaod->push_back( x );
        
        x->setCrate( (*itr)->crate() );
        x->setDataID( (*itr)->dataID() );
        x->setPeak( (*itr)->peak() );
        x->addHits( (*itr)->HitsVec() ,  (*itr)->ErrorVec()  );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
