/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoICnvTool.cxx 575028 2013-12-11 14:30:06Z krasznaa $

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/JetRoI.h"

// Local include(s):
#include "JetRoICnvTool.h"

namespace xAODMaker {

   JetRoICnvTool::JetRoICnvTool( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IJetRoICnvTool >( this );
   }

   StatusCode JetRoICnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the jet RoI objects
    * from a LVL1_ROI container, and fills an xAOD::JetRoIContainer with them.
    *
    * @param aod The AOD object to take the jet RoIs from
    * @param xaod The xAOD container to fill the RoIs into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode JetRoICnvTool::convert( const LVL1_ROI* aod,
                                      xAOD::JetRoIContainer* xaod ) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the AOD objects:
      LVL1_ROI::jets_type::const_iterator aod_itr = aod->getJetROIs().begin();
      LVL1_ROI::jets_type::const_iterator aod_end = aod->getJetROIs().end();
      for( ; aod_itr != aod_end; ++aod_itr ) {

         // Add a new jet RoI object to the output:
         xAOD::JetRoI* roi = new xAOD::JetRoI();
         xaod->push_back( roi );

         // Now fill it with data:
         roi->initialize( aod_itr->getROIWord(),
                          aod_itr->getEta(), aod_itr->getPhi() );
         roi->setEt4x4( aod_itr->getET4x4() );
         roi->setEt6x6( aod_itr->getET6x6() );
         roi->setEt8x8( aod_itr->getET8x8() );
         for( size_t i = 0; i < aod_itr->getThresholdNames().size(); ++i ) {
            roi->addThreshold( aod_itr->getThresholdNames()[ i ],
                               aod_itr->getThresholdValues()[ i ] );
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
