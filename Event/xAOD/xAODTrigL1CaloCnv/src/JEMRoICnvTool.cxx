/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// EDM include(s):
#include "TrigT1CaloEvent/JEMRoICollection.h"
#include "xAODTrigL1Calo/JEMRoIContainer.h"
#include "xAODTrigL1Calo/JEMRoIAuxContainer.h"

// Local include(s):
#include "JEMRoICnvTool.h"

namespace xAODMaker {

   JEMRoICnvTool::JEMRoICnvTool( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IJEMRoICnvTool >( this );
   }


   /**
    * This is the important function of the tool. It takes the JEMRoI object
    * from the ESD and fills an xAOD::JEMRoIContainer with them.
    *
    * @param esd The ESD object to take the JEMRoI from
    * @param xaod The xAOD container to fill the JEMRoI into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode JEMRoICnvTool::convert( const JEMRoICollection* esd,
                                      xAOD::JEMRoIContainer* xaod) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the ESD objects:     
      JEMRoICollection::const_iterator itr = esd->begin();
      JEMRoICollection::const_iterator end = esd->end();
      for( ; itr != end; ++itr ) {
        
        xAOD::JEMRoI* x = new xAOD::JEMRoI();
        xaod->push_back( x );
        
        x->setRoiWord( (*itr)->roiWord() );
        
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
