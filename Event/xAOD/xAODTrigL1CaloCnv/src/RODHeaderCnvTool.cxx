/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeaderCnvTool.cxx 576052 2013-12-18 09:13:50Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/RODHeaderCollection.h"
#include "xAODTrigL1Calo/RODHeaderContainer.h"
#include "xAODTrigL1Calo/RODHeaderAuxContainer.h"

// Local include(s):
#include "RODHeaderCnvTool.h"

namespace xAODMaker {

   RODHeaderCnvTool::RODHeaderCnvTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IRODHeaderCnvTool >( this );
   }

   StatusCode RODHeaderCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the RODHeader object
    * from the ESD and fills an xAOD::RODHeaderContainer with them.
    *
    * @param esd The ESD object to take the RODHeader from
    * @param xaod The xAOD container to fill the RODHeader into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode RODHeaderCnvTool::convert( const RODHeaderCollection* esd,
                                         xAOD::RODHeaderContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( xaod->size() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      RODHeaderCollection::const_iterator itr = esd->begin();
      RODHeaderCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::RODHeader* x = new xAOD::RODHeader();
        xaod->push_back( x );
        
        x->setVersion( (*itr)->version() );
        x->setSourceId( (*itr)->sourceID() );
        x->setRun( (*itr)->run() );
        x->setLvl1Id( (*itr)->extendedL1ID() );
        x->setBcid( (*itr)->bunchCrossing() );
        x->setTrigType( (*itr)->l1TriggerType() );
        x->setDetType( (*itr)->detEventType() );
        x->setStatusWords( (*itr)->statusWords() );
        x->setPayloadSize( (*itr)->payloadSize() );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
