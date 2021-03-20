/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTauRoICnvTool.cxx 575028 2013-12-11 14:30:06Z krasznaa $

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/EmTauRoI.h"

// Local include(s):
#include "EmTauRoICnvTool.h"

namespace xAODMaker {

   EmTauRoICnvTool::EmTauRoICnvTool( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IEmTauRoICnvTool >( this );
   }

   StatusCode EmTauRoICnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the em/tau RoI objects
    * from a LVL1_ROI container, and fills an xAOD::EmTauRoIContainer with them.
    *
    * @param aod The AOD object to take the em/tau RoIs from
    * @param xaod The xAOD container to fill the RoIs into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode EmTauRoICnvTool::convert( const LVL1_ROI* aod,
                                        xAOD::EmTauRoIContainer* xaod ) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the AOD objects:
      LVL1_ROI::emtaus_type::const_iterator aod_itr = aod->getEmTauROIs().begin();
      LVL1_ROI::emtaus_type::const_iterator aod_end = aod->getEmTauROIs().end();
      for( ; aod_itr != aod_end; ++aod_itr ) {

         // Add a new em/tau RoI object to the output:
         xAOD::EmTauRoI* roi = new xAOD::EmTauRoI();
         xaod->push_back( roi );

         // Now fill it with data:
         roi->initialize( aod_itr->getROIWord(),
                          aod_itr->getEta(), aod_itr->getPhi() );
         roi->setCore( aod_itr->getCore() );
         roi->setEmClus( aod_itr->getEMClus() );
         roi->setTauClus( aod_itr->getTauClus() );
         roi->setEmIsol( aod_itr->getEMIsol() );
         roi->setHadIsol( aod_itr->getHadIsol() );
         roi->setHadCore( aod_itr->getHadCore() );
         for( size_t i = 0; i < aod_itr->getThresholdNames().size(); ++i ) {
            roi->addThreshold( aod_itr->getThresholdNames()[ i ],
                               aod_itr->getThresholdValues()[ i ] );
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
