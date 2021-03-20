/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoICnvTool.cxx 629847 2014-11-20 15:41:02Z masik $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EnergySum_ROI.h"
#include "xAODTrigger/EnergySumRoI.h"

// Local include(s):
#include "EnergySumRoICnvTool.h"

namespace xAODMaker {

   EnergySumRoICnvTool::EnergySumRoICnvTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IEnergySumRoICnvTool >( this );
   }

   StatusCode EnergySumRoICnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the energy-sum RoI object
    * from a LVL1_ROI container, and fills an xAOD::EnergySumRoI with it.
    *
    * @param aod The AOD object to take the energy-sum RoI from
    * @param xaod The xAOD object to fill with information
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode EnergySumRoICnvTool::convert( const LVL1_ROI* aod,
                                            xAOD::EnergySumRoI* xaod ) {

      // If there is no RoI to convert:
      if( aod->getEnergySumROIs().empty() ) {
         ATH_MSG_WARNING( "No EnergySum RoI received on the input" );
         return StatusCode::SUCCESS;
      }

      // A sanity check on the input:
      if( aod->getEnergySumROIs().size() != 1 ) {
         ATH_MSG_WARNING( "There is supposed ot be exactly one EnergySum RoI "
                          "in the event." );
         ATH_MSG_WARNING( "Instead received "
                          << aod->getEnergySumROIs().size() );
      }

      // Convert the one RoI:
      const EnergySum_ROI& roi = aod->getEnergySumROIs()[ 0 ];
      xaod->initialize( roi.getROIWord0(), roi.getROIWord1(), roi.getROIWord2(),
                        roi.getEnergyX(), roi.getEnergyY(), roi.getEnergyT() );
      for( size_t i = 0; i < roi.getThresholds().size(); ++i ) {
         xaod->addThreshold( roi.getThresholds()[ i ] );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
