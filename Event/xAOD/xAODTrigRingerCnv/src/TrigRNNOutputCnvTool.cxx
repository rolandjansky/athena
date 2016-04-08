/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRNNOutputCnvTool.cxx 592334 2014-04-10 11:01:56Z krasznaa $

// EDM include(s):
#include "TrigCaloEvent/TrigRNNOutput.h"
#include "TrigCaloEvent/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"

// Local include(s):
#include "TrigRNNOutputCnvTool.h"

namespace xAODMaker {

   TrigRNNOutputCnvTool::TrigRNNOutputCnvTool( const std::string& type,
					       const std::string& name,
					       const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ITrigRNNOutputCnvTool >( this );
   }

   StatusCode TrigRNNOutputCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the RingerRNNOutput object
    * from a RingerRNNOutput container, and fills an xAOD::RingerRNNOutput with it.
    *
    * @param aod The AOD object to take the RingerRNNOutput from
    * @param xaod The xAOD object to fill with information
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode
   TrigRNNOutputCnvTool::convert( const TrigRNNOutputContainer* aod,
                                  xAOD::TrigRNNOutputContainer* xaod ) {

      ATH_MSG_DEBUG( "Size of the xAOD container before loop: "
                     << xaod->size() );

      // Loop over the AOD objects:
      TrigRNNOutputContainer::const_iterator aod_itr = aod->begin();
      TrigRNNOutputContainer::const_iterator aod_end = aod->end();

      for( ; aod_itr != aod_end; ++aod_itr ) {

         // Create the xAOD object:
         xAOD::TrigRNNOutput* rnn = new xAOD::TrigRNNOutput();
         xaod->push_back( rnn );
          
          rnn->setRnnDecision( ((*aod_itr)->output())[0] );
          //rnn->setEt( ((*aod_itr)->cluster())->et() );
          rnn->auxdata<float>("et") = ((*aod_itr)->cluster())->et();
      }

      ATH_MSG_DEBUG( "Size of the xAOD container after loop: "
                     << xaod->size() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
