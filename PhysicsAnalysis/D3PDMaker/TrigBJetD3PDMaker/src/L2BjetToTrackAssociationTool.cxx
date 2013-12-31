/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// STL include(s):
#include <vector>
#include <string>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigParticle/TrigL2BjetContainer.h"

// Trigger include(s):
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"

// Local include(s):
#include "L2BjetToTrackAssociationTool.h"

namespace D3PD {

   L2BjetToTrackAssociationTool::L2BjetToTrackAssociationTool( const std::string &type,
                                                               const std::string &name,
                                                               const IInterface *parent )
      : Base( type, name, parent ),
        m_valid( false ),
        m_trigDec( "TrigDec::TrigDecisionTool/TrigDecisionTool" ) {

      declareProperty( "TrigDecisionTool", m_trigDec,
                       "The tool to access TrigDecision" );
   }

   StatusCode L2BjetToTrackAssociationTool::initialize() {

      CHECK( Base::initialize() );

      // get TrigDecisionTool
      CHECK( m_trigDec.retrieve() );

      return StatusCode::SUCCESS;
   }

   StatusCode L2BjetToTrackAssociationTool::reset( const TrigL2Bjet &p ) {

      const TrigL2Bjet* bjet = &p;

      ATH_MSG_DEBUG( "Looking L2Track association to TrigL2Bjet at ("
                     << bjet->eta() << ", " << bjet->phi() << ")" );

      std::vector< std::string > l2Chains  = m_trigDec->getListOfTriggers( "(L2_.b.*|L2_mu4_j10_a4tc_EFFS|L2_mu4_L1J10_matched|L2_mu4_L1J20_matched|L2_mu4_L1J30_matched|L2_mu4_L1J50_matched|L2_mu4_L1J75_matched|L2_4j30_a4tc_EFFS|L2_mu4_j100_a4tc_EFFS_L1matched)" );
      std::vector< std::string >::const_iterator l2ChainNameItr = l2Chains.begin();
      std::vector< std::string >::const_iterator l2ChainNameEnd = l2Chains.end();

      bool found = false;
      m_valid = false;

      for( ; l2ChainNameItr != l2ChainNameEnd; ++l2ChainNameItr ) {

         Trig::FeatureContainer fx = m_trigDec->features( ( *l2ChainNameItr ),
                                                          TrigDefs::alsoDeactivateTEs );
         Trig::FeatureContainer::combination_const_iterator cx    = fx.getCombinations().begin();
         Trig::FeatureContainer::combination_const_iterator cxEnd = fx.getCombinations().end();
         for( ; cx != cxEnd; ++cx ) {

            std::vector< Trig::Feature< TrigL2Bjet > > trigL2Bjet =
               cx->get< TrigL2Bjet >( "L2BjetFex", TrigDefs::alsoDeactivateTEs );
            std::vector< Trig::Feature< TrigL2Bjet > >::const_iterator trkL2bI   = trigL2Bjet.begin();
            std::vector< Trig::Feature< TrigL2Bjet > >::const_iterator trkL2bEnd = trigL2Bjet.end();
            for( ; trkL2bI != trkL2bEnd; ++trkL2bI ) {
               const TrigL2Bjet* L2b = trkL2bI->cptr();
               if( bjet == L2b ) {
                  REPORT_MESSAGE( MSG::VERBOSE )
                     << "Found match with TrigL2Bjet at (" << L2b->eta() << ", "
                     << L2b->phi() << ")";
                  found = true;
                  break;
               }
            }

            if( found ) {
               std::vector< Trig::Feature< TrigInDetTrackCollection > > inDetTrackFeature =
                  cx->get< TrigInDetTrackCollection >( "TrigSiTrack_Jet",
                                                       TrigDefs::alsoDeactivateTEs );
               std::vector< Trig::Feature< TrigInDetTrackCollection > >::const_iterator
                  trkFeatureI = inDetTrackFeature.begin();
               std::vector< Trig::Feature< TrigInDetTrackCollection > >::const_iterator
                  trkFeatureEnd = inDetTrackFeature.end();
               for( ; trkFeatureI != trkFeatureEnd; ++trkFeatureI ) {
                  const TrigInDetTrackCollection* L2tr = trkFeatureI->cptr();
                  if( L2tr ) {
                     m_trkBegin = L2tr->begin();
                     m_trkEnd = L2tr->end();
                     m_valid = true;

                     REPORT_MESSAGE( MSG::VERBOSE )
                        << "Found " << L2tr->size() << " TrigInDetTracks at TrigSiTrack_Jet";

                     TrigInDetTrackCollection::const_iterator aa = m_trkBegin;
                     TrigInDetTrackCollection::const_iterator bb = m_trkEnd;
                     for( ; aa != bb; ++aa ) {
                        if( *aa ) {
                           if( ( *aa )->param() )
                              REPORT_MESSAGE( MSG::VERBOSE )
                                 << "   * Track - (" << ( *aa )->param()->eta() << ", "
                                 << ( *aa )->param()->phi0() << ")";
                        }
                     }
                     break;
                  }
               }

               std::vector< Trig::Feature< TrigInDetTrackCollection > > inDetTrackFeatureID =
                  cx->get< TrigInDetTrackCollection >( "TrigIDSCAN_Jet",
                                                       TrigDefs::alsoDeactivateTEs );
               std::vector< Trig::Feature< TrigInDetTrackCollection > >::const_iterator
                  trkFeatureI_ID = inDetTrackFeatureID.begin();
               std::vector< Trig::Feature< TrigInDetTrackCollection > >::const_iterator
                  trkFeatureEnd_ID = inDetTrackFeatureID.end();
               for( ; trkFeatureI_ID != trkFeatureEnd_ID; ++trkFeatureI_ID ) {
                  const TrigInDetTrackCollection* L2tr = trkFeatureI_ID->cptr();
                  if( L2tr ) {
                     m_trkBegin = L2tr->begin();
                     m_trkEnd = L2tr->end();
                     m_valid = true;

                     REPORT_MESSAGE( MSG::VERBOSE )
                        << "Found " << L2tr->size() << " TrigInDetTracks at TrigIDSCAN_Jet";
                     TrigInDetTrackCollection::const_iterator aa = m_trkBegin;
                     TrigInDetTrackCollection::const_iterator bb = m_trkEnd;
                     for( ; aa != bb; ++aa ) {
                        if( *aa ) {
                           if( ( *aa )->param() )
                              REPORT_MESSAGE( MSG::VERBOSE )
                                 << "   * Track - (" << ( *aa )->param()->eta() << ", "
                                 << ( *aa )->param()->phi0() << ")";
                        }
                     }
                     break;
                  }
               }
               break;
            }

         }

         if( found && m_valid )
            break;

      }

      return StatusCode::SUCCESS;
   }

   const TrigInDetTrack* L2BjetToTrackAssociationTool::next() {

      if( ! m_valid )
         return 0;
      if( m_trkBegin == m_trkEnd )
         return 0;

      const TrigInDetTrack *track = *m_trkBegin;
      ++m_trkBegin;

      return track;
   }

} // namespace D3PD
