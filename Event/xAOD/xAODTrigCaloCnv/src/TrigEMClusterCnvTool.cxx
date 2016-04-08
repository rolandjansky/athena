/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterCnvTool.cxx 592334 2014-04-10 11:01:56Z krasznaa $

// EDM include(s):
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

// Local include(s):
#include "TrigEMClusterCnvTool.h"

namespace xAODMaker {

   TrigEMClusterCnvTool::TrigEMClusterCnvTool( const std::string& type,
					       const std::string& name,
					       const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ITrigEMClusterCnvTool >( this );
   }

   StatusCode TrigEMClusterCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the TrigEMCluster object
    * from a TrigEMCluster container, and fills an xAOD::TrigEMCluster with it.
    *
    * @param aod The AOD object to take the TrigEMCluster from
    * @param xaod The xAOD object to fill with information
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode
   TrigEMClusterCnvTool::convert( const TrigEMClusterContainer* aod,
                                  xAOD::TrigEMClusterContainer* xaod ) {

      ATH_MSG_DEBUG( "Size of the xAOD container before loop: "
                     << xaod->size() );

      // Number of available samplings:
      const unsigned int samplings = 25; // Is this really right?...

      // Loop over the AOD objects:
      TrigEMClusterContainer::const_iterator aod_itr = aod->begin();
      TrigEMClusterContainer::const_iterator aod_end = aod->end();

      for( ; aod_itr != aod_end; ++aod_itr ) {

         // Create the xAOD object:
         xAOD::TrigEMCluster* cluster = new xAOD::TrigEMCluster();
         xaod->push_back( cluster );

         cluster->setRawEnergy     ( ( *aod_itr )->rawEnergy() );
         cluster->setRawEt         ( ( *aod_itr )->rawEt() );
         cluster->setRawEta        ( ( *aod_itr )->rawEta() );
         cluster->setRawPhi        ( ( *aod_itr )->rawPhi() );
         cluster->setRoIword       ( ( *aod_itr )->RoIword() );
         cluster->setNCells        ( ( *aod_itr )->nCells() );
         cluster->setClusterQuality( ( *aod_itr )->quality() );

         cluster->setEnergy ( ( *aod_itr )->energy() );
         cluster->setEt     ( ( *aod_itr )->et() );
         cluster->setEta    ( ( *aod_itr )->eta() );
         cluster->setPhi    ( ( *aod_itr )->phi() );
         cluster->setE237   ( ( *aod_itr )->e237() );
         cluster->setE277   ( ( *aod_itr )->e277() );
         cluster->setFracs1 ( ( *aod_itr )->fracs1() );
         cluster->setWeta2  ( ( *aod_itr )->weta2() );
         cluster->setEhad1  ( ( *aod_itr )->ehad1() );
         cluster->setEta1   ( ( *aod_itr )->Eta1() );
         cluster->setEmaxs1 ( ( *aod_itr )->emaxs1() );
         cluster->setE2tsts1( ( *aod_itr )->e2tsts1() );
         cluster->setE233   ( ( *aod_itr )->e233() );
         cluster->setWstot  ( ( *aod_itr )->wstot() );

         for( unsigned int i = 0; i < samplings; ++i ) {
            const CaloSampling::CaloSample sample =
               static_cast< CaloSampling::CaloSample >( i );
            const float rawValue = ( *aod_itr )->rawEnergy( sample );
            if( ! cluster->setRawEnergy( sample, rawValue ) ) {
               ATH_MSG_WARNING( "Unable to set raw energy for sampling "
                                << sample );
            }
            const float value = ( *aod_itr )->energy( sample );
            if( ! cluster->setEnergy( sample, value ) ) {
               ATH_MSG_WARNING( "Unable to set energy for sampling "
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
                        << ( *aod_itr )->quality() << ";" );

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

         ATH_MSG_DEBUG( "energy: "
                        << ( *aod_itr )->energy() << "; "
                        << "et: "
                        << ( *aod_itr )->et() << "; "
                        << "eta: "
                        << ( *aod_itr )->eta() << "; "
                        << "phi: "
                        << ( *aod_itr )->phi() << "; "
                        << "e237: "
                        << ( *aod_itr )->e237() << "; "
                        << "e277: "
                        << ( *aod_itr )->e277() << "; "
                        << "fracs1: "
                        << ( *aod_itr )->fracs1() << "; "
                        << "weta2: "
                        << ( *aod_itr )->weta2() << "; "
                        << "ehad1: "
                        << ( *aod_itr )->ehad1() << "; "
                        << "Eta1: "
                        << ( *aod_itr )->Eta1() << "; "
                        << "emaxs1: "
                        << ( *aod_itr )->emaxs1() << "; "
                        << "e2tsts1: "
                        << ( *aod_itr )->e2tsts1() << "; "
                        << "e233: "
                        << ( *aod_itr )->e233() << "; "
                        << "wstot: "
                        << ( *aod_itr )->wstot() );

         if( msg().level() <= MSG::DEBUG ) {
            for( unsigned int i = 0; i < samplings; ++i ) {
               const CaloSampling::CaloSample sample =
                  static_cast< CaloSampling::CaloSample >( i );
               ATH_MSG_DEBUG( "energySample : "
                              << CaloSampling::getSamplingName( sample )
                              << " = "
                              << ( *aod_itr )->energy( sample ) << ";" );
            }
         }

      }

      ATH_MSG_DEBUG( "Size of the xAOD container after loop: "
                     << xaod->size() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
