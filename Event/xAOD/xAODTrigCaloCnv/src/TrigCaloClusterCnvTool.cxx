/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterCnvTool.cxx 592334 2014-04-10 11:01:56Z krasznaa $

// EDM include(s):
#include "TrigCaloEvent/TrigCaloCluster.h"
#include "TrigCaloEvent/TrigCaloClusterContainer.h"
#include "xAODTrigCalo/TrigCaloCluster.h"
#include "xAODTrigCalo/TrigCaloClusterContainer.h"

// Local include(s):
#include "TrigCaloClusterCnvTool.h"

namespace xAODMaker {

   TrigCaloClusterCnvTool::TrigCaloClusterCnvTool( const std::string& type,
						   const std::string& name,
						   const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ITrigCaloClusterCnvTool >( this );
   }

   StatusCode TrigCaloClusterCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the TrigCaloCluster object
    * from a TrigCaloCluster container, and fills an xAOD::TrigCaloCluster with it.
    *
    * @param aod The AOD object to take the TrigCaloCluster from
    * @param xaod The xAOD object to fill with information
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode
   TrigCaloClusterCnvTool::convert( const TrigCaloClusterContainer* aod,
                                    xAOD::TrigCaloClusterContainer* xaod ) {

      ATH_MSG_DEBUG( "Size of the xAOD container before loop: "
                     << xaod->size() );

      // Number of available samplings:
      const unsigned int samplings = 25; // Is this really right?...

      // Loop over the AOD objects:
      TrigCaloClusterContainer::const_iterator aod_itr = aod->begin();
      TrigCaloClusterContainer::const_iterator aod_end = aod->end();

      for( ; aod_itr != aod_end; ++aod_itr ) {

         // Create the xAOD object:
         xAOD::TrigCaloCluster* cluster = new xAOD::TrigCaloCluster();
         xaod->push_back( cluster );

         cluster->setRawEnergy     ( ( *aod_itr )->rawEnergy() );
         cluster->setRawEt         ( ( *aod_itr )->rawEt() );
         cluster->setRawEta        ( ( *aod_itr )->rawEta() );
         cluster->setRawPhi        ( ( *aod_itr )->rawPhi() );
         cluster->setRoIword       ( ( *aod_itr )->RoIword() );
         cluster->setNCells        ( ( *aod_itr )->nCells() );
         cluster->setClusterQuality( ( *aod_itr )->quality() );

         for( unsigned int i = 0; i < samplings; ++i ) {
            const CaloSampling::CaloSample sample =
               static_cast< CaloSampling::CaloSample >( i );
            const float value = ( *aod_itr )->rawEnergy( sample );
            if( ! cluster->setRawEnergy( sample, value ) ) {
               ATH_MSG_WARNING( "Unable to set raw energy for sampling "
                                << sample );
            }
         }

         ATH_MSG_DEBUG( "rawEnergy: "
                        << ( *aod_itr )->rawEnergy() << "; "
                        << "rawEt: "
                        << ( *aod_itr )->rawEt() << "; "
                        << "rawEta: "
                        << ( *aod_itr )->rawEta() << "; "
                        << "rawPhi: "
                        << ( *aod_itr )->rawPhi() << "; "
                        << std::hex << "m_roiWord: 0x"
                        << ( *aod_itr )->RoIword() << "; " << std::dec
                        << "numberUsedCells: "
                        << ( *aod_itr )->nCells() << "; "
                        << "quality: "
                        << ( *aod_itr )->quality() );

         if( msg().level() <= MSG::DEBUG ) {
            for( unsigned int i = 0; i < samplings; ++i ) {
               const CaloSampling::CaloSample sample =
                  static_cast< CaloSampling::CaloSample >( i );
               ATH_MSG_DEBUG( "rawEnergySample : "
                              << CaloSampling::getSamplingName( sample )
                              << " = "
                              << ( *aod_itr )->rawEnergy( sample ) << ";" );
            }
         }

      }

      ATH_MSG_DEBUG( "Size of the xAOD container after loop: "
                     << xaod->size() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
