/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigT2ZdcSignalsCnvTool.cxx 608072 2014-07-23 00:27:40Z azemla $

// EDM include(s):
#include "xAODTrigMinBias/TrigT2ZdcSignalsContainer.h"
#include "xAODTrigMinBias/TrigT2ZdcSignals.h"

#include "TrigCaloEvent/TrigT2ZdcSignalsContainer.h"
// Local include(s):
#include "TrigT2ZdcSignalsCnvTool.h"

namespace xAODMaker {
  
  TrigT2ZdcSignalsCnvTool::TrigT2ZdcSignalsCnvTool( const std::string& type,
						const std::string& name,
						const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
    
    // Declare the interface(s) provided by the tool:
    declareInterface< ITrigT2ZdcSignalsCnvTool >( this );
  }
  
  StatusCode TrigT2ZdcSignalsCnvTool::initialize() {
    
    // Greet the user:
    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  /**
   * This is the important function of the tool. It takes the old EDM objects
   * from a TrigT2ZdcSignalsContainer, and fills an xAOD::TrigT2ZdcSignalsContainer with them.
   *
   * @param aod The AOD object to take the TrigT2ZdcSignals from
   * @param xaod The xAOD container to fill the TrigT2ZdcSignals into
   * @returns <code>StatusCode::SUCCESS</code> if all went fine,
   *          something else if not
   */
  StatusCode TrigT2ZdcSignalsCnvTool::convert( const TrigT2ZdcSignalsContainer* aod,
					     xAOD::TrigT2ZdcSignalsContainer* xaod ) {
    
    // A small sanity check. The output container should really be empty...
    if( !xaod->empty() ) {
      ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
      << xaod->size() << ")" );
    }
    
    TrigT2ZdcSignalsContainer::const_iterator t2zdcsig_iter 	= aod->begin();
    TrigT2ZdcSignalsContainer::const_iterator t2zdcsig_iter_end	= aod->end();
    
    ATH_MSG_VERBOSE("AOD TrigT2ZdcSignalsContainer size:: "<< aod->size());
    
    for(;t2zdcsig_iter != t2zdcsig_iter_end; ++t2zdcsig_iter){
      // Create a (single) output xAOD object.
      xAOD::TrigT2ZdcSignals* xT2ZdcSignals = new xAOD::TrigT2ZdcSignals();
      xaod->push_back(xT2ZdcSignals);
      
      // 
      xT2ZdcSignals->setTriggerEnergies( (*t2zdcsig_iter)-> triggerEnergies() );
      xT2ZdcSignals->setTriggerTimes( (*t2zdcsig_iter) -> triggerTimes() );
      
      ATH_MSG_VERBOSE("AOD vs xAOD::triggerEnergies size:: "<< (*t2zdcsig_iter)->triggerEnergies().size()<<" "<<xT2ZdcSignals->triggerEnergies().size());
      ATH_MSG_VERBOSE("AOD vs xAOD:triggerTimes size:: "<< (*t2zdcsig_iter)->triggerTimes().size()<<" "<<xT2ZdcSignals->triggerTimes().size());
    }
    // Return gracefully:
    return StatusCode::SUCCESS;
					     }
					     
} // namespace xAODMaker
