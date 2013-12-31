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
#include "TrigParticle/TrigEFBjetContainer.h"

// Trigger include(s):
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"

// Local include(s):
#include "EFBjetToTrackAssociationTool.h"

namespace D3PD {

   EFBjetToTrackAssociationTool::EFBjetToTrackAssociationTool( const std::string &type,
                                                               const std::string &name,
                                                               const IInterface *parent )
      : Base( type, name, parent ),
        m_valid( false ),
        m_trigDec( "TrigDec::TrigDecisionTool/TrigDecisionTool" ) {

      declareProperty( "TrigDecisionTool", m_trigDec,
                       "The tool to access TrigDecision" );
   }

   StatusCode EFBjetToTrackAssociationTool::initialize() {

      CHECK( Base::initialize() );

      // get TrigDecisionTool
      CHECK( m_trigDec.retrieve() );

      return StatusCode::SUCCESS;
   }

   StatusCode EFBjetToTrackAssociationTool::reset( const TrigEFBjet &p ) {

      const TrigEFBjet* bjet = &p;

      std::vector< std::string > efChains  = m_trigDec->getListOfTriggers( "(EF_.b.*|EF_mu4_j10_a4tc_EFFS|EF_mu4_L1J10_matched|EF_mu4_L1J20_matched|EF_mu4_L1J30_matched|EF_mu4_L1J50_matched|EF_mu4_L1J75_matched|EF_4j30_a4tc_EFFS|EF_mu4_j100_a4tc_EFFS_L1matched)" );

      std::vector< std::string >::const_iterator efChainNameItr = efChains.begin();
      std::vector< std::string >::const_iterator efChainNameEnd = efChains.end();

      bool found = false;
      m_valid = false;

      for( ; efChainNameItr != efChainNameEnd; ++efChainNameItr ) {

         Trig::FeatureContainer fx = m_trigDec->features( ( *efChainNameItr ),
                                                          TrigDefs::alsoDeactivateTEs );
         Trig::FeatureContainer::combination_const_iterator cx    = fx.getCombinations().begin();
         Trig::FeatureContainer::combination_const_iterator cxEnd = fx.getCombinations().end();
         for( ; cx != cxEnd; ++cx ) {

            std::vector< Trig::Feature< TrigEFBjet > > trigEFBjet =
               cx->get< TrigEFBjet >( "EFBjetFex", TrigDefs::alsoDeactivateTEs );
            std::vector< Trig::Feature< TrigEFBjet > >::const_iterator trkEFbI = trigEFBjet.begin();
            std::vector< Trig::Feature< TrigEFBjet > >::const_iterator trkEFbEnd = trigEFBjet.end();
            for( ; trkEFbI != trkEFbEnd; ++trkEFbI ) {
               const TrigEFBjet* EFb = trkEFbI->cptr();
               if( bjet == EFb ) { // there should be a better way of doing this!
                  found = true;
                  break;
               }
            }

            if( found ) {
               std::vector< Trig::Feature< Rec::TrackParticleContainer > > trackFeature =
                  cx->get< Rec::TrackParticleContainer >( "InDetTrigParticleCreation_Bjet_EFID",
                                                          TrigDefs::alsoDeactivateTEs );
               std::vector< Trig::Feature< Rec::TrackParticleContainer > >::const_iterator
                  trkFeatureI = trackFeature.begin();
               std::vector< Trig::Feature< Rec::TrackParticleContainer > >::const_iterator
                  trkFeatureEnd = trackFeature.end();
               for( ; trkFeatureI != trkFeatureEnd; ++trkFeatureI ) {
                  const Rec::TrackParticleContainer* EFtr = trkFeatureI->cptr();
                  if( EFtr ) {
                     m_trkBegin = EFtr->begin();
                     m_trkEnd = EFtr->end();
                     m_valid = true;
                     break;
                  }
               }
               break;
            } // if found
         } // for all combinations
    
         if( found ) break;

      } // for all chains

      return StatusCode::SUCCESS;
   }

   const Rec::TrackParticle* EFBjetToTrackAssociationTool::next() {

      if( ! m_valid )
         return 0;
      if( m_trkBegin == m_trkEnd )
         return 0;

      const Rec::TrackParticle *track = *m_trkBegin;
      ++m_trkBegin;

      return track;
   }

} // namespace D3PD
