/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMCPHitsCnvTool.cxx 575874 2013-12-17 12:39:55Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/CMMCPHitsCollection.h"
#include "xAODTrigL1Calo/CMMCPHitsContainer.h"
#include "xAODTrigL1Calo/CMMCPHitsAuxContainer.h"

// Local include(s):
#include "CMMCPHitsCnvTool.h"

namespace xAODMaker {

   CMMCPHitsCnvTool::CMMCPHitsCnvTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ICMMCPHitsCnvTool >( this );
   }

   StatusCode CMMCPHitsCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the CMMCPHits object
    * from the ESD and fills an xAOD::CMMCPHitsContainer with them.
    *
    * @param esd The ESD object to take the CMMCPHits from
    * @param xaod The xAOD container to fill the CMMCPHits into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode CMMCPHitsCnvTool::convert( const CMMCPHitsCollection* esd,
                                         xAOD::CMMCPHitsContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      CMMCPHitsCollection::const_iterator itr = esd->begin();
      CMMCPHitsCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::CMMCPHits* x = new xAOD::CMMCPHits();
        xaod->push_back( x );
        
        x->setCrate( (*itr)->crate() );
        x->setDataID( (*itr)->dataID() );
        x->setPeak( (*itr)->peak() );
        x->addHits( (*itr)->HitsVec0() , (*itr)->HitsVec1() , (*itr)->ErrorVec0() , (*itr)->ErrorVec1() );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
