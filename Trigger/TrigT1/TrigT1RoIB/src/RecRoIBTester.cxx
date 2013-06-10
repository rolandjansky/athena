/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RecRoIBTester.cxx 550415 2013-06-10 09:36:28Z pottgen $

// STL includes:
#include <string>
#include <vector>
#include <iomanip>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// TrigT1 includes:
#include "TrigT1Result/RecRoIBResult.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecJetRoI.h"
//#include "TrigT1Interfaces/TriggerThreshold.h"
//#include "TrigT1Interfaces/TriggerThresholdValue.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"

// Local include(s):
#include "RecRoIBTester.h"

namespace ROIB {

   //--------------
   // Constructor
   //--------------
   RecRoIBTester::RecRoIBTester( const std::string& name, ISvcLocator* pSvcLocator )
      : AthAlgorithm( name, pSvcLocator ) {

   }

   //---------------------------------
   // initialize()
   //---------------------------------
   StatusCode RecRoIBTester::initialize() {

      ATH_MSG_INFO( "Initializing " << name() << " algorithm " 
                    << " - package version " << PACKAGE_VERSION );

      return StatusCode::SUCCESS;
   }

   //---------------------------------
   // finalize()
   //---------------------------------
   StatusCode RecRoIBTester::finalize() {

      ATH_MSG_INFO( "Finalizing " << name()
                    << " - package version " << PACKAGE_VERSION );

      return StatusCode::SUCCESS;
   }

   //----------------------------------------------
   // execute() method called once per event
   //----------------------------------------------
   StatusCode RecRoIBTester::execute( ) {

      // Return right away if we're not going to print anything anyway:
      if( ! msgLvl( MSG::DEBUG ) ) {
         return StatusCode::SUCCESS;
      }

      // Exec message
      ATH_MSG_DEBUG( "====================================" );
      ATH_MSG_DEBUG( "Execution of RecRoIB test algorithm." );
      ATH_MSG_DEBUG( "====================================" );

      // Get RoIB RDO object back from SG:
      const RecRoIBResult* roib_rdo = 0;
      CHECK( evtStore()->retrieve( roib_rdo ) );

      //
      // Test RoIB RDO object from store:
      //
      ATH_MSG_DEBUG( "==============================" );
      ATH_MSG_DEBUG( "Printing EmTau RoI information" );
      ATH_MSG_DEBUG( "==============================" );

      std::vector< LVL1::RecEmTauRoI >::const_iterator emtau_itr =
         roib_rdo->recEmTauRoIVector().begin();
      std::vector< LVL1::RecEmTauRoI >::const_iterator emtau_end =
         roib_rdo->recEmTauRoIVector().end();
      for( ; emtau_itr != emtau_end; ++emtau_itr ) {

         ATH_MSG_DEBUG( "  --> The EmTau RoI " << std::ios::hex << emtau_itr->roiWord()
                        << ' ' << std::ios::dec << emtau_itr->phi() << ' '
                        << std::ios::dec << emtau_itr->eta() );

         std::vector< unsigned int >::const_iterator thr_itr =
            emtau_itr->thresholdsPassed()->begin();
         std::vector< unsigned int >::const_iterator thr_end =
            emtau_itr->thresholdsPassed()->end();
         for( ; thr_itr != thr_end; ++thr_itr ) {
            ATH_MSG_DEBUG( "    - passed threshold (number / GeV) : "
                           << *thr_itr << " / " << emtau_itr->triggerThreshold( *thr_itr )
                           << " ( " << emtau_itr->emIsolation( *thr_itr )
                           << " / " << emtau_itr->hadIsolation( *thr_itr )
                           << " / " << emtau_itr->hadCoreIsolation( *thr_itr )
                           << " )" );
         }
      }

      ATH_MSG_DEBUG( "=============================" );
      ATH_MSG_DEBUG( "Printing Muon RoI information" );
      ATH_MSG_DEBUG( "=============================" );

      std::vector< LVL1::RecMuonRoI >::const_iterator muon_itr =
         roib_rdo->recMuonRoIVector().begin();
      std::vector< LVL1::RecMuonRoI >::const_iterator muon_end =
         roib_rdo->recMuonRoIVector().end();
      for( ; muon_itr != muon_end; ++muon_itr ) {

         ATH_MSG_DEBUG( "  --> The Muon RoI " << muon_itr->roiWord() << ' '
                        << muon_itr->phi() << ' ' << muon_itr->eta() << ' '
                        << muon_itr->sysID() << ' ' << muon_itr->subsysID() << ' '
                        << muon_itr->sectorID() );

         if( muon_itr->getThresholdValue() == 0 ) {
            REPORT_MESSAGE( MSG::WARNING )
               << "     --- threshold number " << muon_itr->getThresholdNumber()
               << " was not configured in thresholds' XML file";
         } else {
            ATH_MSG_DEBUG( "    - passed threshold (number / GeV) : "
                           << muon_itr->getThresholdNumber() << " / "
                           << muon_itr->getThresholdValue() );
         }
      }

      ATH_MSG_DEBUG( "============================" );
      ATH_MSG_DEBUG( "Printing Jet RoI information" );
      ATH_MSG_DEBUG( "============================" );

      std::vector< LVL1::RecJetRoI >::const_iterator jet_itr =
         roib_rdo->recJetRoIVector().begin();
      std::vector< LVL1::RecJetRoI >::const_iterator jet_end =
         roib_rdo->recJetRoIVector().end();
      for( ; jet_itr != jet_end; ++jet_itr ) {

         static LVL1::JEPRoIDecoder jep_decoder;

         // if energy
         if( jep_decoder.roiType( jet_itr->roiWord() ) > 2 ) {

            ATH_MSG_DEBUG( " The energy situation : of type "
                           << jep_decoder.roiType( jet_itr->roiWord() )
                           << " has the RoI word " << jet_itr->roiWord() );

            if( jep_decoder.roiType( jet_itr->roiWord() ) == 3 ) {

               ATH_MSG_DEBUG( " --- energy X = "
                              << jep_decoder.energyX( jet_itr->roiWord() ) );

            } else if( jep_decoder.roiType( jet_itr->roiWord() ) == 4 ) {

               ATH_MSG_DEBUG( " --- energy Y = "
                              << jep_decoder.energyY( jet_itr->roiWord() ) );
               //FIXME ... this should work now, but please test (ATW)
               const std::vector< unsigned int > sumPassed =
                  jep_decoder.etSumThresholdsPassed( jet_itr->roiWord() );
               if( sumPassed.size() ) {
                  ATH_MSG_DEBUG( " --- Etsum thresholds passed :" );
                  std::vector< unsigned int >::const_iterator thr_itr =
                     sumPassed.begin();
                  std::vector< unsigned int >::const_iterator thr_end =
                     sumPassed.end();
                  for( ; thr_itr != thr_end; ++thr_itr ) {
                     ATH_MSG_DEBUG( "         " << *thr_itr );
                  }
               }

            } else if( jep_decoder.roiType( jet_itr->roiWord() ) == 5 ) {

               ATH_MSG_DEBUG( " energy T = "
                              << jep_decoder.energyT( jet_itr->roiWord() ) );
               //FIXME ... this should work now, but please test (ATW)
               const std::vector< unsigned int > missPassed =
                  jep_decoder.etMissThresholdsPassed( jet_itr->roiWord() );
               if( missPassed.size() ) {
                  ATH_MSG_DEBUG( " --- Etmiss thresholds passed :" );
                  std::vector< unsigned int >::const_iterator thr_itr =
                     missPassed.begin();
                  std::vector< unsigned int >::const_iterator thr_end =
                     missPassed.end();
                  for( ; thr_itr != thr_end; ++thr_itr ) {
                     ATH_MSG_DEBUG( "         " << *thr_itr );
                  }
               }
            }

         }
         // if we are talking about jet RoIs with type = 1
         else if( jep_decoder.roiType( jet_itr->roiWord() ) == 1 ) {

            ATH_MSG_DEBUG( " The Jet RoI " << jet_itr->roiWord() << ' '
                           << jet_itr->phi() << ' ' << jet_itr->eta() );

            // You need to ask the RecJetRoI whether it is a central or forward jet.
            // thresholdPassed() will supply the vector of threshold numbers for
            // whichever type it is
            std::vector< unsigned int >::const_iterator thr_itr =
               jet_itr->thresholdsPassed()->begin();
            std::vector< unsigned int >::const_iterator thr_end =
               jet_itr->thresholdsPassed()->end();
            for( ; thr_itr != thr_end; ++thr_itr ) {
               if( ! jet_itr->isForwardJet() ) {
                  ATH_MSG_DEBUG( "- passed threshold (number / GeV) : "
                                 << *thr_itr << " / "
                                 << jet_itr->triggerThreshold( *thr_itr ) << " ( "
                                 << jet_itr->windowSize( *thr_itr ) << " /" );
               }
               else {
                  ATH_MSG_DEBUG( "- passed forward threshold (number / GeV) : "
                                 << *thr_itr << " / "
                                 << jet_itr->triggerThreshold( *thr_itr ) << " ( "
                                 << jet_itr->windowSize( *thr_itr ) << " /" );
               }
            }

         }
         // if we are talking about the summed jet et word.
         else if( jep_decoder.roiType( jet_itr->roiWord() ) == 2 ) {

            // FIXME code not yet provided to get thre threshold and the value.
            ATH_MSG_DEBUG( " The Summed Jet Et RoI is " << jet_itr->roiWord()
                           << " This still needs some code to fill it!" );

         }

      }

      // Return happily!
      return StatusCode::SUCCESS;
   }

} // end of  namespace bracket

