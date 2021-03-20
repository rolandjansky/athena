/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMEtSumsCnvTool.cxx 575995 2013-12-17 16:56:45Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/CMMEtSumsCollection.h"
#include "xAODTrigL1Calo/CMMEtSumsContainer.h"
#include "xAODTrigL1Calo/CMMEtSumsAuxContainer.h"

// Local include(s):
#include "CMMEtSumsCnvTool.h"

namespace xAODMaker {

   CMMEtSumsCnvTool::CMMEtSumsCnvTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ICMMEtSumsCnvTool >( this );
   }

   StatusCode CMMEtSumsCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the CMMEtSums object
    * from the ESD and fills an xAOD::CMMEtSumsContainer with them.
    *
    * @param esd The ESD object to take the CMMEtSums from
    * @param xaod The xAOD container to fill the CMMEtSums into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode CMMEtSumsCnvTool::convert( const CMMEtSumsCollection* esd,
                                         xAOD::CMMEtSumsContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      CMMEtSumsCollection::const_iterator itr = esd->begin();
      CMMEtSumsCollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::CMMEtSums* x = new xAOD::CMMEtSums();
        xaod->push_back( x );
        
        x->setCrate( (*itr)->crate() );
        x->setDataID( (*itr)->dataID() );
        x->setPeak( (*itr)->peak() );
        x->addEt( (*itr)->EtVec() , (*itr)->EtErrorVec() );
        x->addEx( (*itr)->ExVec() , (*itr)->ExErrorVec() );
        x->addEy( (*itr)->EyVec() , (*itr)->EyErrorVec() );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
