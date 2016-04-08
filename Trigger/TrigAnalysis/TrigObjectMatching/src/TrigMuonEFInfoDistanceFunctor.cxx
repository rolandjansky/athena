/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// STL include(s):
#include <cmath>

// Local include(s):
#include "TrigObjectMatching/TrigMuonEFInfoDistanceFunctor.h"

namespace TrigMatch {

   int TrigMuonEFInfoHelper::m_trackToUse = useSpectrometerTrack;

   void TrigMuonEFInfoHelper::setTrackToUse( int trackToUse ) {

      m_trackToUse = trackToUse;
      return;
   }

   int TrigMuonEFInfoHelper::trackToUse() {

      return m_trackToUse;
   }

   float TrigMuonEFInfoHelper::calculateInfoInfoDistance( const TrigMuonEFInfo *left,
                                                          const TrigMuonEFInfo *right ) {

      // When comparing objects of the same type, in many cases we want to find
      // out if we are looking at the same object with two pointers. This is needed
      // for mainly technical reasons...
      if( trackToUse() == useObjectPointer ) {
         if( left == right ) {
            return 0.0;
         } else {
            return 1000.0;
         }
      }

      // first, we need to figure out how the tracks are stored
      // there are basically two options:
      // In older datasets (15.1.0 and earlier), we use the legacy track methods
      // In newer ones, we access the track container
      // Try the newer method first, and fall back to the legacy method it doesn't work

      const TrigMuonEFInfoTrackContainer *leftTrackContainer  = left->TrackContainer();
      const TrigMuonEFInfoTrackContainer *rightTrackContainer = right->TrackContainer();

      if( ( ! leftTrackContainer ) || leftTrackContainer->empty() ||
          ( ! rightTrackContainer ) || rightTrackContainer->empty() ) {

         // if no track container, try the legacy methods
         // check if we can use the legacy track methods
         if( left->hasLegacyTrack() && right->hasLegacyTrack() ) {
            const TrigMuonEFTrack* left_trk = 0;
            const TrigMuonEFTrack* right_trk = 0;
            switch( trackToUse() ) {
            case TrigMatch::useSpectrometerTrack:
               left_trk  = left->legacySpectrometerTrack();
               right_trk = right->legacySpectrometerTrack();
               break;
            case TrigMatch::useExtrapolatedTrack:
               left_trk  = left->legacyExtrapolatedTrack();
               right_trk = right->legacyExtrapolatedTrack();
               break;
            case TrigMatch::useCombinedTrack:
               left_trk  = ( TrigMuonEFTrack* ) left->legacyCombinedTrack();
               right_trk = ( TrigMuonEFTrack* ) right->legacyCombinedTrack();
               break;
            default:
               REPORT_MESSAGE_WITH_CONTEXT( MSG::WARNING, "TrigMuonEFInfoHelper" )
                  << "Track type " << trackToUse() << " not supported for legacy objects";
               return -1.;
               break;
            }
            return DeltaRDistanceFunctor< TrigMuonEFTrack,
                                          TrigMuonEFTrack >::deltaR( left_trk, right_trk );
         }
         else {
            REPORT_MESSAGE_WITH_CONTEXT( MSG::WARNING, "TrigMuonEFInfoHelper" )
               << "Didn't find any tracks belonging to the provided TrigMuonEFInfo object";
            return -1.;
         }
      }

      // if we're here, we should use the track container tracks
      TrigMuonEFInfoTrackContainer::const_iterator left_itr = leftTrackContainer->begin();
      TrigMuonEFInfoTrackContainer::const_iterator left_end = leftTrackContainer->end();
      TrigMuonEFInfoTrackContainer::const_iterator right_itr = rightTrackContainer->begin();
      TrigMuonEFInfoTrackContainer::const_iterator right_end = rightTrackContainer->end();

      float minDr = 1000.0;

      switch( trackToUse() ) {

      case TrigMatch::useSpectrometerTrack:
         for( ; left_itr != left_end; ++left_itr ) {
            if( ( *left_itr )->MuonType() != TrigMuonEFInfoTrack::MuonTypeEFMuon )
               continue;
            for( ; right_itr != right_end; ++right_itr ) {
               if( ( *right_itr )->MuonType() != TrigMuonEFInfoTrack::MuonTypeEFMuon )
                  continue;
               float dr = DeltaRDistanceFunctor< TrigMuonEFTrack,
                  TrigMuonEFTrack >::deltaR( (  *left_itr )->SpectrometerTrack(),
                                             (  *right_itr )->SpectrometerTrack() );
               if( dr < minDr ) {
                  minDr = dr;
               }
            }
         }
         break;
      case TrigMatch::useExtrapolatedTrack:
         for( ; left_itr != left_end; ++left_itr ) {
            if( ( *left_itr)->MuonType() != TrigMuonEFInfoTrack::MuonTypeEFMuon )
               continue;
            for( ; right_itr != right_end; ++right_itr ) {
               if( ( *right_itr)->MuonType() != TrigMuonEFInfoTrack::MuonTypeEFMuon )
                  continue;
               float dr = DeltaRDistanceFunctor< TrigMuonEFTrack,
                  TrigMuonEFTrack >::deltaR( ( *left_itr )->ExtrapolatedTrack(),
                                             ( *right_itr )->ExtrapolatedTrack() );
               if( dr < minDr ) {
                  minDr = dr;
               }
            }
         }
         break;
      case TrigMatch::useCombinedTrack:
         for( ; left_itr != left_end; ++left_itr ) {
            if( ( *left_itr )->MuonType() != TrigMuonEFInfoTrack::MuonTypeEFMuon )
               continue;
            for( ; right_itr != right_end; ++right_itr ) {
               if( ( *right_itr )->MuonType() != TrigMuonEFInfoTrack::MuonTypeEFMuon )
                  continue;
               float dr = DeltaRDistanceFunctor< TrigMuonEFCbTrack,
                  TrigMuonEFCbTrack >::deltaR( ( *left_itr)->CombinedTrack(),
                                               ( *right_itr)->CombinedTrack() );
               if( dr < minDr ) {
                  minDr = dr;
               }
            }
         }
         break;
      case TrigMatch::useMuGirlTrack:
         for( ; left_itr != left_end; ++left_itr ) {
            if( ( *left_itr )->MuonType() != TrigMuonEFInfoTrack::MuGirlMuon )
               continue;
            for( ; right_itr != right_end; ++right_itr ) {
               if( ( *right_itr )->MuonType() != TrigMuonEFInfoTrack::MuGirlMuon )
                  continue;
               float dr = DeltaRDistanceFunctor< TrigMuonEFCbTrack,
                  TrigMuonEFCbTrack >::deltaR( ( *left_itr )->CombinedTrack(),
                                               ( *right_itr )->CombinedTrack() );
               if( dr < minDr ) {
                  minDr = dr;
               }
            }
         }
         break;
      case TrigMatch::useAnyTrack:
         for( ; left_itr != left_end; ++left_itr ) {
            for( ; right_itr != right_end; ++right_itr ) {

               float dr1 = 1000.0;
               if( (  *left_itr )->SpectrometerTrack() &&
                   (  *right_itr )->SpectrometerTrack() ) {
                  dr1 = DeltaRDistanceFunctor< TrigMuonEFTrack,
                     TrigMuonEFTrack >::deltaR( (  *left_itr )->SpectrometerTrack(),
                                                (  *right_itr )->SpectrometerTrack() );
               }
               float dr2 = 1000.0;
               if( (  *left_itr )->ExtrapolatedTrack() &&
                   (  *right_itr )->ExtrapolatedTrack() ) {
                  dr2 = DeltaRDistanceFunctor< TrigMuonEFTrack,
                     TrigMuonEFTrack >::deltaR( (  *left_itr )->ExtrapolatedTrack(),
                                                (  *right_itr )->ExtrapolatedTrack() );
               }
               float dr3 = 1000.0;
               if( (  *left_itr )->CombinedTrack() &&
                   (  *right_itr )->CombinedTrack() ) {
                  dr3 = DeltaRDistanceFunctor< TrigMuonEFCbTrack,
                     TrigMuonEFCbTrack >::deltaR( (  *left_itr )->CombinedTrack(),
                                                  (  *right_itr )->CombinedTrack() );
               }
               float dr = std::min( dr1, std::min( dr2, dr3 ) );
               if( dr < minDr ) {
                  minDr = dr;
               }
            }
         }
         break;
      case TrigMatch::useMuGirlTagTrack:
         for( ; left_itr != left_end; ++left_itr ) {
            if( ( *left_itr )->MuonType() != TrigMuonEFInfoTrack::MuGirlMuonTag )
               continue;
            for( ; right_itr != right_end; ++right_itr ) {
               if( ( *right_itr )->MuonType() != TrigMuonEFInfoTrack::MuGirlMuonTag )
                  continue;
               float dr = DeltaRDistanceFunctor< TrigMuonEFCbTrack,
                  TrigMuonEFCbTrack >::deltaR( ( *left_itr )->CombinedTrack(),
                                               ( *right_itr )->CombinedTrack() );
               if( dr < minDr ) {
                  minDr = dr;
               }
            }
         }
         break;
      case TrigMatch::useAnyMuGirlTrack:
         for( ; left_itr != left_end; ++left_itr ) {
            if( ( ( *left_itr )->MuonType() != TrigMuonEFInfoTrack::MuGirlMuon ) &&
                ( ( *left_itr )->MuonType() != TrigMuonEFInfoTrack::MuGirlMuonTag ) )
               continue;
            const short muon_type = ( *left_itr )->MuonType();
            for( ; right_itr != right_end; ++right_itr ) {
               if( ( *right_itr )->MuonType() != muon_type )
                  continue;
               float dr = DeltaRDistanceFunctor< TrigMuonEFCbTrack,
                  TrigMuonEFCbTrack >::deltaR( ( *left_itr )->CombinedTrack(),
                                               ( *right_itr )->CombinedTrack() );
               if( dr < minDr ) {
                  minDr = dr;
               }
            }
         }
         break;
      default:
         REPORT_MESSAGE_WITH_CONTEXT( MSG::WARNING, "TrigMuonEFInfoHelper" )
            << "Track type " << trackToUse() << " unknown!";
         minDr = -1.0;
         break;
      }

      return minDr;
   }

} // namespace TrigMatch
