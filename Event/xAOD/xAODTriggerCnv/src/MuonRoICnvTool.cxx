/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonRoICnvTool.cxx 575028 2013-12-11 14:30:06Z krasznaa $

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/MuonRoI.h"

// Local include(s):
#include "MuonRoICnvTool.h"

namespace xAODMaker {

   MuonRoICnvTool::MuonRoICnvTool( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IMuonRoICnvTool >( this );
   }

   StatusCode MuonRoICnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the muon RoI objects
    * from a LVL1_ROI container, and fills an xAOD::MuonRoIContainer with them.
    *
    * @param aod The AOD object to take the muon RoIs from
    * @param xaod The xAOD container to fill the RoIs into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode MuonRoICnvTool::convert( const LVL1_ROI* aod,
                                       xAOD::MuonRoIContainer* xaod ) {

      // A small sanity check. The output container should really be empty...
      if( !xaod->empty() ) {
         ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                          << xaod->size() << ")" );
      }

      // Loop over the AOD objects:
      LVL1_ROI::muons_type::const_iterator aod_itr = aod->getMuonROIs().begin();
      LVL1_ROI::muons_type::const_iterator aod_end = aod->getMuonROIs().end();
      for( ; aod_itr != aod_end; ++aod_itr ) {

         // Add a new muon RoI object to the output:
         xAOD::MuonRoI* roi = new xAOD::MuonRoI();
         xaod->push_back( roi );

         // Now fill it with data:
         roi->initialize( aod_itr->getROIWord(),
                          aod_itr->getEta(), aod_itr->getPhi(),
                          aod_itr->getThrName(), aod_itr->getThrValue() );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
