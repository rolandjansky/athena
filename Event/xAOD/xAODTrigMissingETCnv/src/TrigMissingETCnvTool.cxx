/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingETCnvTool.cxx 785790 2016-11-22 16:48:26Z ssnyder $

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODTrigMissingET/TrigMissingET.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "TrigMissingEtEvent/TrigMissingET.h"

// Local include(s):
#include "TrigMissingETCnvTool.h"

namespace xAODMaker {

  TrigMissingETCnvTool::TrigMissingETCnvTool( const std::string& type,
					      const std::string& name,
					      const IInterface* parent )
    : AthAlgTool( type, name, parent ) {

    // Declare the interface(s) provided by the tool:
    declareInterface< ITrigMissingETCnvTool >( this );
  }

  StatusCode TrigMissingETCnvTool::initialize() {

    // Greet the user:
    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  /**
   * This is where the translation actually happens.
   *
   * @param aod The AOD object to take the muon RoIs from
   * @param xaod The xAOD container to fill the RoIs into
   * @returns <code>StatusCode::SUCCESS</code> if all went fine,
   *          something else if not
   */
  StatusCode
  TrigMissingETCnvTool::convert( const TrigMissingETContainer* aod,
                                 xAOD::TrigMissingETContainer* xaod ) const {

    // A small sanity check. The output container should really be empty...
    if( !xaod->empty() ) {
      ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                       << xaod->size() << ")" );
    }

    TrigMissingETContainer::const_iterator aod_itr = aod->begin();
    TrigMissingETContainer::const_iterator aod_end = aod->end();
    for( ; aod_itr != aod_end; ++aod_itr ) {

       // Create a (single) output xAOD object.
       auto *xMET = new xAOD::TrigMissingET();
       xaod->push_back(xMET);

       // Transfer the simple items
       xMET->setEx( ( *aod_itr )->ex() );
       xMET->setEy( ( *aod_itr )->ey() );
       xMET->setEz( ( *aod_itr )->ez() );
       xMET->setSumEt( ( *aod_itr )->sumEt() ); 
       xMET->setSumE( ( *aod_itr )->sumE() );
       xMET->setFlag( ( *aod_itr )->getFlag() );
       xMET->setRoiWord( ( *aod_itr )->RoIword() );
       // ...

       // Get the list of components
       unsigned int nComp ( ( *aod_itr )->getNumOfComponents());
       std::vector< std::string > componentNames;
       for (unsigned int i = 0; i < nComp; i++) {
          componentNames.push_back( ( *aod_itr )->getNameOfComponent( i ) );
       }
       xMET->defineComponents( componentNames );
       for (unsigned int i = 0; i < nComp; i++) {
          xMET->setExComponent( i, ( *aod_itr )->getExComponent( i ) );
          xMET->setEyComponent( i, ( *aod_itr )->getEyComponent( i ) );
          xMET->setEzComponent( i, ( *aod_itr )->getEzComponent( i ) );
          xMET->setSumEtComponent( i, ( *aod_itr )->getSumEtComponent( i ) );
          xMET->setSumEComponent( i, ( *aod_itr )->getSumEComponent( i ) );
          xMET->setCalib0Component( i, ( *aod_itr )->getComponentCalib0( i ) );
          xMET->setCalib1Component( i, ( *aod_itr )->getComponentCalib1( i ) );
          xMET->setSumOfSignsComponent( i, ( *aod_itr )->getSumOfSigns( i ) );
          xMET->setUsedChannelsComponent( i, ( *aod_itr )->getUsedChannels( i ) );
          xMET->setStatusComponent( i, ( *aod_itr )->getStatus( i ) );
       }

    }

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

} // namespace xAODMaker
