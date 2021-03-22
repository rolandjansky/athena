/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigT2MbtsBitsCnvTool.cxx 785778 2016-11-22 16:26:31Z ssnyder $

// EDM include(s):
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"

#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"
// Local include(s):
#include "TrigT2MbtsBitsCnvTool.h"

namespace xAODMaker {
  
  TrigT2MbtsBitsCnvTool::TrigT2MbtsBitsCnvTool( const std::string& type,
						const std::string& name,
						const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
    
    // Declare the interface(s) provided by the tool:
    declareInterface< ITrigT2MbtsBitsCnvTool >( this );
  }
  
  StatusCode TrigT2MbtsBitsCnvTool::initialize() {
    
    // Greet the user:
    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  /**
   * This is the important function of the tool. It takes the old EDM objects
   * from a TrigT2MbtsBitsContainer, and fills an xAOD::TrigT2MbtsBitsContainer with them.
   *
   * @param aod The AOD object to take the TrigT2MbtsBits from
   * @param xaod The xAOD container to fill the TrigT2MbtsBits into
   * @returns <code>StatusCode::SUCCESS</code> if all went fine,
   *          something else if not
   */
  StatusCode TrigT2MbtsBitsCnvTool::convert( const TrigT2MbtsBitsContainer* aod,
					     xAOD::TrigT2MbtsBitsContainer* xaod ) const {
    
    // A small sanity check. The output container should really be empty...
    if( !xaod->empty() ) {
      ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
      << xaod->size() << ")" );
    }
    
    TrigT2MbtsBitsContainer::const_iterator t2mbts_iter 	= aod->begin();
    TrigT2MbtsBitsContainer::const_iterator t2mbts_iter_end	= aod->end();
    
    ATH_MSG_VERBOSE("AOD TrigT2MbtsBitsContainer size:: "<< aod->size());
    
    for(;t2mbts_iter != t2mbts_iter_end; ++t2mbts_iter){
      // Create a (single) output xAOD object.
      xAOD::TrigT2MbtsBits* xT2MbtsBits = new xAOD::TrigT2MbtsBits();
      xaod->push_back(xT2MbtsBits);
      
      // 
      xT2MbtsBits->setTriggerEnergies( (*t2mbts_iter)-> triggerEnergies() );
      xT2MbtsBits->setTriggerTimes( (*t2mbts_iter) -> triggerTimes() );
      
      ATH_MSG_VERBOSE("AOD vs xAOD::triggerEnergies size:: "<< (*t2mbts_iter)->triggerEnergies().size()<<" "<<xT2MbtsBits->triggerEnergies().size());
      ATH_MSG_VERBOSE("AOD vs xAOD:triggerTimes size:: "<< (*t2mbts_iter)->triggerTimes().size()<<" "<<xT2MbtsBits->triggerTimes().size());
    }
    // Return gracefully:
    return StatusCode::SUCCESS;
					     }
					     
} // namespace xAODMaker
