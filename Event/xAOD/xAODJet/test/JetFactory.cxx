/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s).
#include "JetFactory.h"

// EDM include(s).
#include "xAODJet/JetAuxContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

// Framework include(s).
#ifndef XAOD_STANDALONE
#  include "SGTools/TestStore.h" 
#else
#  include "xAODRootAccess/Init.h"
#  include "xAODRootAccess/TEvent.h"
#  include "xAODRootAccess/TStore.h"
#  include "xAODRootAccess/TActiveStore.h"
#endif

// System include(s).
#undef NDEBUG
#include <cassert>
#include <mutex>

namespace {

   /// Objects implementing a transient object store
   std::once_flag storeInitStatus;
#ifdef XAOD_STANDALONE
   static std::unique_ptr< xAOD::TEvent > event;
   static std::unique_ptr< xAOD::TStore > store;
#else
   std::unique_ptr< SGTest::TestStore > store;
#endif // XAOD_STANDALONE

#ifndef XAOD_STANDALONE
   void record1 ATLAS_NOT_THREAD_SAFE( xAOD::IParticleContainer* ipc,
                                       const std::string& name ) {

      store->record( ipc, name );
      return;
   }

   void record1 ATLAS_NOT_THREAD_SAFE( SG::IConstAuxStore* auxStore,
                                       const std::string& name ) {

      store->record( auxStore, name );
      return;
   }
#endif // not XAOD_STANDALONE

   /// Record an object, with a given name/key, into the transient store
   template< typename T >
   void record ATLAS_NOT_THREAD_SAFE( std::unique_ptr< T > cont,
                                      const std::string& name ) {

#ifndef XAOD_STANDALONE
      std::call_once( storeInitStatus,
                      []( std::unique_ptr< SGTest::TestStore >& store ) {
                         store = SGTest::getTestStore();
                      }, store );
      T* ptr = cont.get();
      store->record( std::move( cont ), name );
      record1( ptr, name );
#else
      std::call_once( storeInitStatus,
                      []( std::unique_ptr< xAOD::TEvent >& event,
                          std::unique_ptr< xAOD::TStore >& store ) {
                         xAOD::Init().ignore();
                         event = std::make_unique< xAOD::TEvent >();
                         store = std::make_unique< xAOD::TStore >();
                      }, event, store );

      store->record( std::move( cont ), name ).ignore();
#endif // not XAOD_STANDALONE
   }

} // private namespace

namespace xAOD {
   namespace JetTests {

      xAOD::JetContainer&
      createEmptyJetContainer ATLAS_NOT_THREAD_SAFE( const std::string& name )  {

         // Create the jet container.
         auto jetCont = std::make_unique< xAOD::JetContainer >();
         auto aux = std::make_unique< xAOD::JetAuxContainer >();
         jetCont->setStore( aux.get() );

         // Record it.
         xAOD::JetContainer* jetContPtr = jetCont.get();
         record( std::move( jetCont ), name );
         record( std::move( aux ), name + "Aux." );

         // Return a reference to it.
         return *jetContPtr;
      }

      void fillJets( xAOD::JetContainer & jetCont,
                     const std::vector< xAOD::JetFourMom_t >& jet4moms ) {

         for( const xAOD::JetFourMom_t& v : jet4moms ) {
	          jetCont.push_back( new xAOD::Jet() );
	          xAOD::Jet* jet = jetCont.back();
            jet->setJetP4( v );
         }
      }

      void fillJets( xAOD::JetContainer& jetCont,
                     const std::vector< xAOD::JetFourMom_t >& jet4moms,
                     const std::map< std::string,
                                     std::vector< float > >& jetAttMapF,
                     const std::map< std::string,
                                     std::vector< int > >& jetAttMapI,
                     const std::map< std::string,
                                     std::vector< xAOD::JetFourMom_t > >&
                        jetAttMap4M ) {

         // Make some checks.
         for( const auto& floatAtts : jetAttMapF ) {
            assert( jet4moms.size() == floatAtts.second.size() );
         }
         for( const auto& intAtts : jetAttMapI ) {
            assert( jet4moms.size() == intAtts.second.size() );
         }
         for( const auto& fmomAtts : jetAttMap4M ) {
            assert( jet4moms.size() == fmomAtts.second.size() );
         }

         // Create and set up the jets.
         for( std::size_t i = 0; i < jet4moms.size(); ++i ) {
            jetCont.push_back( new xAOD::Jet() );
            xAOD::Jet *jet = jetCont.back();
            jet->setJetP4( jet4moms[ i ] );
            for( const auto& attpair : jetAttMapF ) {
               jet->setAttribute( attpair.first, attpair.second[ i ] );
            }
            for( const auto& attpair : jetAttMapI ) {
               jet->setAttribute( attpair.first, attpair.second[ i ] );
            }
            for( const auto& attpair : jetAttMap4M ) {
               jet->setAttribute( attpair.first, attpair.second[ i ] );
            }
         }
      }

      void fillStandardTestJets( JetContainer& jetCont ) {

         static const std::vector< xAOD::JetFourMom_t > jet4moms {
            xAOD::JetFourMom_t{ 40000, 0, 1, 10000 },
            xAOD::JetFourMom_t{ 40000, 0.01, 2, 10000 },
            xAOD::JetFourMom_t{ 40000, 0.01, 2, 0. },
            xAOD::JetFourMom_t{ 40000, 0.2, 2, 10000. },
            xAOD::JetFourMom_t{ 40000, 1.0, 2, 10000. },
            xAOD::JetFourMom_t{ 40000, 1.001, 2, 10000. },
            xAOD::JetFourMom_t{ 40000, 2., 2, 10000. },
            xAOD::JetFourMom_t{ 40000, 3., 2, 10000. },
            xAOD::JetFourMom_t{ 40000, 3.22, 2, 10000. },
            xAOD::JetFourMom_t{ 40000, 4., 2, 10000. },
            xAOD::JetFourMom_t{ 40000, 5., 2, 10000. },
            xAOD::JetFourMom_t{ 40000, 5.5, 2, 10000. },

            xAOD::JetFourMom_t{ 0.1, 1.0, 2, 10000. },
            xAOD::JetFourMom_t{ 100000, 1.0, 2, 10000. },
            xAOD::JetFourMom_t{ 500000, 1.0, 2, 10000. },
            xAOD::JetFourMom_t{ 1000000, 1.0, 2, 10000. },
            xAOD::JetFourMom_t{ 10000000, 1.0, 2, 10000. },

            xAOD::JetFourMom_t{ 100000, 1.0, 2, 100000. },
            xAOD::JetFourMom_t{ 100000, 1.0, 2, 1000000. }
         };

         fillJets( jetCont, jet4moms,
                   { { "Width", std::vector< float >( jet4moms.size(),
                                                      0.123 ) } },
                   { { "NumTrkPt1000", std::vector< int >( jet4moms.size(),
                                                           5 ) } },
                   {} );

         // Set also some JetScale so they can be tested.
         // We don't need these scales to have precise signification, just
         // they are relatively close to the default scale, as what one could
         // expect for a real jets (typically a calibrated LCTopo jets)
         for( xAOD::Jet* jet: jetCont ) {
            jet->setJetP4( xAOD::JetScale::JetConstitScaleMomentum,
                           0.9 * jet->jetP4() );
            jet->setJetP4( xAOD::JetScale::JetEMScaleMomentum,
                           0.7 * jet->jetP4() );
         }
      }

      xAOD::CaloClusterContainer&
      createEmptyCaloClusterContainer ATLAS_NOT_THREAD_SAFE( const std::string& name ) {

         // Create the jet container.
         auto clustCont = std::make_unique< xAOD::CaloClusterContainer >();
         auto aux = std::make_unique< xAOD::CaloClusterAuxContainer >();
         clustCont->setStore( aux.get() );

         // Record it.
         xAOD::CaloClusterContainer* clustContPtr = clustCont.get();
         record( std::move( clustCont ), name );
         record( std::move( aux ), name + "Aux." );

         // Return a reference to it.
         return *clustContPtr;
      }

      void fillStandardTestClusters( CaloClusterContainer& clustCont ) {

         static constexpr std::size_t N_CLUSTERS = 10;
         for( std::size_t i = 0; i < N_CLUSTERS; ++i ) {
           
            clustCont.push_back( new xAOD::CaloCluster() );
            xAOD::CaloCluster* cl = clustCont.back();

            cl->setE( 1000.f + i * 1.1f );
            cl->setEta( -5.f + i * 0.1f );
            cl->setPhi( 0.f );
            cl->setM( 0.f );

            cl->setRawE( 500.f + i * 2.2f );
            cl->setRawEta( -5.f + i * 0.1 );
            cl->setRawPhi( 0.f );
            cl->setRawM( 0.f );
         }
      }

      void shutDown ATLAS_NOT_THREAD_SAFE() {

#ifdef XAOD_STANDALONE
         event.reset();
         store.reset();
#else
         store.reset();
#endif // XAOD_STANDALONE
      }

   } // namespace JetTests
} // namespace xAOD
