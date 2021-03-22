/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetEtRoICnvTool.cxx 575028 2013-12-11 14:30:06Z krasznaa $

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/JetET_ROI.h"
#include "xAODTrigger/JetEtRoI.h"

// Local include(s):
#include "JetEtRoICnvTool.h"

namespace xAODMaker {

   JetEtRoICnvTool::JetEtRoICnvTool( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IJetEtRoICnvTool >( this );
   }

   StatusCode JetEtRoICnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the jet-Et RoI object
    * from a LVL1_ROI container, and fills an xAOD::JetEtRoI with it.
    *
    * @param aod The AOD object to take the jet-Et RoI from
    * @param xaod The xAOD object to fill with information
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode JetEtRoICnvTool::convert( const LVL1_ROI* aod,
                                        xAOD::JetEtRoI* xaod ) {

      // If there is no RoI to convert:
      if( aod->getJetEtROIs().empty() ) {
         // This happens unfortunately. Let's discuss with L1Calo about how
         // we should handle this...
         ATH_MSG_DEBUG( "No JetEt RoI received on the input" );
         return StatusCode::SUCCESS;
      }

      // Another sanity check on the input:
      if( aod->getJetEtROIs().size() != 1 ) {
         ATH_MSG_WARNING( "There is supposed ot be exactly one JetEt RoI "
                          "in the event." );
         ATH_MSG_WARNING( "Instead received "
                          << aod->getJetEtROIs().size() );
      }

      // Convert the one RoI:
      const JetET_ROI& roi = aod->getJetEtROIs()[ 0 ];
      xaod->setRoIWord( roi.getROIWord() );
      for( size_t i = 0; i < roi.getThresholds().size(); ++i ) {
         xaod->addThreshold( roi.getThresholds()[ i ] );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
