/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// EDM include(s):
#include "TrigSteeringEvent/TrigPassBitsCollection.h"
#include "xAODTrigger/TrigPassBitsContainer.h"

// Local include(s):
#include "TrigPassBitsCnvTool.h"

namespace xAODMaker {

  TrigPassBitsCnvTool::TrigPassBitsCnvTool( const std::string& type,
					    const std::string& name,
					    const IInterface* parent )
    : AthAlgTool( type, name, parent ) {

    // Declare the interface(s) provided by the tool:
    declareInterface< ITrigPassBitsCnvTool >( this );
  }

  StatusCode TrigPassBitsCnvTool::initialize() {

    // Greet the user:
    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  /**
   * This is the important function of the tool. It takes the TrigPassBits object
   * from a TrigPassBits collection, and fills an xAOD::TrigPassBitsContainer with it.
   *
   * @param aod The AOD collection to take the TrigPassBits from
   * @param xaod The xAOD object to fill with converted TrigPassBits
   * @returns <code>StatusCode::SUCCESS</code> if all went fine,
   *          something else if not
   */
  StatusCode TrigPassBitsCnvTool::convert( const TrigPassBitsCollection* aod,
					   xAOD::TrigPassBitsContainer* xaod ) const
  {
    ATH_MSG_VERBOSE( "in TrigPassBitsCnvTool::convert" );
    // If there is no TrigPassBits to convert:
    if( aod->empty() ) {
      ATH_MSG_DEBUG( "No TrigPassBits objects received on the input" );
      return StatusCode::SUCCESS;
    }
    
    ATH_MSG_VERBOSE( "in TrigPassBitsCnvTool::convert, size of the input container is " << aod->size() );
    // Convert:

    TrigPassBitsCollection::const_iterator it = aod->begin();
    TrigPassBitsCollection::const_iterator it_end = aod->end();
    // here we just create container with empty xAOD TrigPassBits, of the same size as the AOD equivalent container
    // the actual bit setting is made by Trigger/TrigEvent/TrigBSExtraction in TrigHLTtoxAODConversion::execute()
    // by StatusCode TrigBStoxAODTool::setTrigPassBits(HLT::Navigation* nav);
    // thus we are sure that after rewiring the navigation the new trigpassbits are in the proper place
    for( ; it!=it_end; ++it) { 
      
      xAOD::TrigPassBits* xaod_tpb = nullptr; // new xAOD::TrigPassBits(); 
      xaod->push_back(xaod_tpb);
    }
    

    ATH_MSG_VERBOSE( "exiting TrigPassBitsCnvTool::convert, converted container has " << xaod->size() << " elements" );    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }

} // namespace xAODMaker
