/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMRoICnvTool.cxx 576052 2013-12-18 09:13:50Z morrisj $

// EDM include(s):
#include "TrigT1CaloEvent/CPMRoICollection.h"
#include "xAODTrigL1Calo/CPMRoIContainer.h"
#include "xAODTrigL1Calo/CPMRoIAuxContainer.h"

// Local include(s):
#include "CPMRoICnvTool.h"

namespace xAODMaker {

   CPMRoICnvTool::CPMRoICnvTool( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ICPMRoICnvTool >( this );
   }

   StatusCode CPMRoICnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the CPMRoI object
    * from the ESD and fills an xAOD::CPMRoIContainer with them.
    *
    * @param esd The ESD object to take the CPMRoI from
    * @param xaod The xAOD container to fill the CPMRoI into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode CPMRoICnvTool::convert( const CPMRoICollection* esd,
                                      xAOD::CPMRoIContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      CPMRoICollection::const_iterator itr = esd->begin();
      CPMRoICollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::CPMRoI* x = new xAOD::CPMRoI();
        xaod->push_back( x );
        
        x->setRoiWord( (*itr)->roiWord() );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
