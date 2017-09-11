/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL include(s):
#include <algorithm>
#include <functional>
#include <cmath>

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingToolBase.h"
#include "unary_compose.h"
#include "SetPrint.h"
#include "count_bunch_neighbors.h"

namespace Trig {

   /**
    * The class needs an explicit constructor because some of the member variables
    * really need to be initialized at construction.
    */
   BunchCrossingToolBase::BunchCrossingToolBase( const std::string& name )
      : asg::AsgMetadataTool( name ),
        m_filledBunches(), m_singleBunches(), m_unpairedBunches(),
        m_bunchTrains() {

      /// Declare the properties of the tool:
      declareProperty( "MaxBunchSpacing", m_maxBunchSpacing = 150 );
      declareProperty( "FrontLength", m_frontLength = 300 );
      declareProperty( "TailLength", m_tailLength = 300 );
   }

   bool BunchCrossingToolBase::isFilled( bcid_type bcid ) const {

      // Check if this is a filled bunch crossing:
      if( m_filledBunches.find( bcid ) != m_filledBunches.end() ) {
         return true;
      } else {
         return false;
      }
   }

   bool BunchCrossingToolBase::isInTrain( bcid_type bcid ) const {

      // Look for this BCID in the list of bunch trains:
      std::set< BunchTrain >::const_iterator itr = m_bunchTrains.begin();
      std::set< BunchTrain >::const_iterator end = m_bunchTrains.end();
      for( ; itr != end; ++itr ) {
         if( itr->find( bcid ) != itr->end() ) {
            return true;
         }
      }

      return false;
   }

   bool BunchCrossingToolBase::isUnpaired( bcid_type bcid ) const {

      // Check if this is an unpaired bunch crossing:
      if( m_unpairedBunches.find( bcid ) != m_unpairedBunches.end() ) {
         return true;
      } else {
         return false;
      }
   }

   bool BunchCrossingToolBase::isBeam1( bcid_type bcid ) const {

      // Check if this is a filled bunch:
      std::set< BunchCrossing >::const_iterator itr_fill =
         m_filledBunches.find( bcid );
      if( itr_fill != m_filledBunches.end() ) {
         return true;
      }

      // Check if this is an unpaired bunch with the bunch in beam 1:
      std::set< BunchCrossing >::const_iterator itr_unp =
         m_unpairedBunches.find( bcid );
      if( ( itr_unp != m_unpairedBunches.end() ) &&
          ( itr_unp->intensityBeam1() > 0.001 ) ) {
         return true;
      } else {
         return false;
      }
   }

   bool BunchCrossingToolBase::isBeam2( bcid_type bcid ) const {

      // Check if this is a filled bunch:
      std::set< BunchCrossing >::const_iterator itr_fill =
         m_filledBunches.find( bcid );
      if( itr_fill != m_filledBunches.end() ) {
         return true;
      }

      // Check if this is an unpaired bunch with the bunch in beam 1:
      std::set< BunchCrossing >::const_iterator itr_unp =
         m_unpairedBunches.find( bcid );
      if( ( itr_unp != m_unpairedBunches.end() ) &&
          ( itr_unp->intensityBeam2() > 0.001 ) ) {
         return true;
      } else {
         return false;
      }
   }

   float BunchCrossingToolBase::bcIntensity( bcid_type bcid,
                                             BeamType type ) const {

      // Check if this is a colliding bunch:
      std::set< BunchCrossing >::const_iterator itr;
      if( ( itr = m_filledBunches.find( bcid ) ) != m_filledBunches.end() ) {
         switch( type ) {
         case Crossing:
            if( itr->intensityBeam2() > 0.001 ) {
               ATH_MSG_WARNING( "Crossing intensity not available, ask for "
                                << "separate beam intensities instead" );
               return 0.0;
            } else {
               return itr->intensityBeam1();
            }
         case Beam1:
            if( std::abs( itr->intensityBeam2() ) < 0.001 ) {
               ATH_MSG_WARNING( "Separate beam intensities not available, ask "
                                << "for the crossing intensity instead" );
               return 0.0;
            } else {
               return itr->intensityBeam1();
            }
            break;
         case Beam2:
            return itr->intensityBeam2();
            break;
         default:
            ATH_MSG_ERROR( "Unknown intensity type requested (" << type
                           << ")" );
            return -1.0;
         }
      }

      // Check if this is an unpaired bunch:
      if( ( itr = m_unpairedBunches.find( bcid ) ) !=
          m_unpairedBunches.end() ) {
         switch( type ) {
         case Beam1:
            return itr->intensityBeam1();
            break;
         case Beam2:
            return itr->intensityBeam2();
            break;
         case Crossing:
            ATH_MSG_WARNING( "Crossing intensity requested for unpaired bunch ("
                             << "bcid=" << bcid << ")" );
            return 0.0;
         default:
            ATH_MSG_ERROR( "Unknown intensity type requested (" << type
                           << ")" );
            return -1.0;
         }
      }

      // If neither, then its intensity is 0.0 by definition:
      return 0.0;
   }

   IBunchCrossingTool::BunchCrossingType
   BunchCrossingToolBase::bcType( bcid_type bcid ) const {

      // First the obvious check:
      if( ! isFilled( bcid ) ) {
         // Check if it's an unpaired bunch:
         if( isUnpaired( bcid ) ) {
            return Unpaired;
         }
         // If the previous bunch crossing is the tail of a bunch train:
         if( ! distanceFromTail( bcid - 1, BunchCrossings ) ) {
            return FirstEmpty;
         }
         // Check if it's in the middle of a bunch train:
         std::set< BunchTrain >::const_iterator itr = m_bunchTrains.begin();
         std::set< BunchTrain >::const_iterator end = m_bunchTrains.end();
         for( ; itr != end; ++itr ) {
            if( itr->isInside( bcid ) ) {
               return MiddleEmpty;
            }
         }
         // If none of the above are true, it has to be a "simple" empty bunch:
         return Empty;
      }

      // Now we know that the bunch has to be a filled one...

      // If it's not in a train, it has to be a single filled bunch:
      if( ! isInTrain( bcid ) ) return Single;

      // Let's check if it is close to the front of a bunch train:
      int distance = distanceFromFront( bcid, NanoSec );
      if( ( distance >= 0 ) && ( distance <= m_frontLength ) ) {
         return Front;
      }
      // Now let's check if it's close to the tail of a bunch train:
      distance = distanceFromTail( bcid, NanoSec );
      if( ( distance >= 0 ) && ( distance <= m_tailLength ) ) {
         return Tail;
      }

      // If none of the above are true, it has to be in the middle of a train:
      return Middle;
   }

   /**
    * This is one of the most tricky functions of this class.
    *
    * When the user wants to ask the distance of a bunch crossing from the front
    * of its train in units of nano seconds or bunch crossings, the main part of
    * the logic is propagated to the algebra defined for the BunchCrossing
    * class.
    *
    * But when the user wants to know the distance in terms of filled bunches,
    * the code has to treat 3 different bunch train configurations:
    *
    *  - First is when the bunch train doesn't spread across the "BCID
    *    turnover". The code in this case just has to see how many steps it is
    *    from <code>bunchtrain->begin()</code>.
    *  - If the bunch train spreads across the "BCID turnover", and the bcid is
    *    after BCID==0, the code has to count the filled bunches from
    *    <code>bunchtrain->train_front()</code> to
    *    <code>bunchtrain->end()</code>, plus the filled bunches from
    *    <code>bunchtrain->begin()</code> to the bcid in question.
    *  - If the bunch train spreads across the "BCID turnover", and the bcid is
    *    "before" BCID==0, the code has to count the filled bunches from
    *    <code>bunchtrain->train_front()</code> to the bcid in question.
    *
    * @param bcid The bcid that should be checked
    * @param type The type of the requested return value
    * @returns The distance of the bcid in question from the front of its bunch
    *          train
    */
   int
   BunchCrossingToolBase::distanceFromFront( bcid_type bcid,
                                             BunchDistanceType type ) const {

      // Look for this BCID in the list of bunch trains:
      std::set< BunchTrain >::const_iterator itr = m_bunchTrains.begin();
      std::set< BunchTrain >::const_iterator end = m_bunchTrains.end();
      for( ; itr != end; ++itr ) {
         BunchTrain::const_iterator element;
         if( ( element = itr->find( bcid ) ) != itr->end() ) {
            switch( type ) {

            case NanoSec:
               return BunchCrossing::BUNCH_SPACING * ( BunchCrossing( bcid ) -
                                                       *( itr->train_front() ) );
               break;
            case BunchCrossings:
               return ( BunchCrossing( bcid ) - *( itr->train_front() ) );
               break;
            case FilledBunches:
               if( *( itr->train_front() ) > *( itr->train_back() ) ) {
                  if( BunchCrossing( bcid ) <= *( itr->train_back() ) ) {
                     return ( std::count_if( itr->begin(), element,
                                             std::bind1st( std::not_equal_to< BunchCrossing >(),
                                                           *element ) ) +
                              std::count_if( itr->train_front(), itr->end(),
                                             std::bind1st( std::not_equal_to< BunchCrossing >(),
                                                           *element ) ) );
                  } else {
                     return std::count_if( itr->train_front(), element,
                                           std::bind1st( std::not_equal_to< BunchCrossing >(),
                                                         *element ) );
                  }
               } else {
                  return std::count_if( itr->begin(), element,
                                        std::bind1st( std::not_equal_to< BunchCrossing >(),
                                                      *element ) );
               }
               break;
            default:
               ATH_MSG_ERROR( "BunchDistanceType not understood!" );
               return -1;
            }
         }
      }

      // If the bunch crossing is not part of a train:
      return -1;
   }

   /**
    * This is one of the most tricky functions of this class.
    *
    * When the user wants to ask the distance of a bunch crossing from the tail
    * of its train in units of nano seconds or bunch crossings, the main part of
    * the logic is propagated to the algebra defined for the BunchCrossing
    * class.
    *
    * But when the user wants to know the distance in terms of filled bunches,
    * the code has to treat 3 different bunch train configurations:
    *
    *  - First is when the bunch train doesn't spread across the "BCID
    *    turnover". The code in this case just has to see how many steps it is
    *    from <code>bunchtrain->end()</code>.
    *  - If the bunch train spreads across the "BCID turnover", and the bcid is
    *    "before" BCID==0, the code has to count the filled bunches from
    *    the bcid in question to <code>bunchtrain->end()</code>,
    *    plus the filled bunches from <code>bunchtrain->begin()</code> to
    *    <code>bunchtrain->train_back()</code>.
    *  - If the bunch train spreads across the "BCID turnover", and the bcid is
    *    after BCID==0, the code has to count the filled bunches from
    *    the bcid in question to <code>bunchtrain->train_back()</code>.
    *
    * @param bcid The bcid that should be checked
    * @param type The type of the requested return value
    * @returns The distance of the bcid in question from the tail of its bunch
    *          train
    */
   int BunchCrossingToolBase::distanceFromTail( bcid_type bcid,
                                                BunchDistanceType type ) const {

      // Look for this BCID in the list of bunch trains:
      std::set< BunchTrain >::const_iterator itr = m_bunchTrains.begin();
      std::set< BunchTrain >::const_iterator end = m_bunchTrains.end();
      for( ; itr != end; ++itr ) {
         BunchTrain::const_iterator element;
         if( ( element = itr->find( bcid ) ) != itr->end() ) {
            switch( type ) {

            case NanoSec:
               return BunchCrossing::BUNCH_SPACING * ( *( itr->train_back() ) -
                                                       BunchCrossing( bcid ) );
               break;
            case BunchCrossings:
               return ( *( itr->train_back() ) - BunchCrossing( bcid ) );
               break;
            case FilledBunches:
               if( *( itr->train_front() ) > *( itr->train_back() ) ) {
                  if( BunchCrossing( bcid ) > *( itr->train_back() ) ) {
                     return ( std::count_if( element, itr->end(),
                                             std::bind1st( std::not_equal_to< BunchCrossing >(),
                                                           *element ) ) +
                              std::count_if( itr->begin(), ++( itr->train_back() ),
                                             std::bind1st( std::not_equal_to< BunchCrossing >(),
                                                           *element ) ) );
                  } else {
                     return std::count_if( element, ++( itr->train_back() ),
                                           std::bind1st( std::not_equal_to< BunchCrossing >(),
                                                         *element ) );
                  }
               } else {
                  return std::count_if( element, itr->end(),
                                        std::bind1st( std::not_equal_to< BunchCrossing >(),
                                                      *element ) );
               }
               break;
            default:
               ATH_MSG_ERROR( "BunchDistanceType not understood!" );
               return -1;
            }
         }
      }

      // If the bunch crossing is not part of a train:
      return -1;
   }

   /**
    * The function first finds the train that the specified BCID is in. Then
    * it calculates the size of the gap before this train. It can return the
    * size of the gap either in nanoseconds or in BCIDs (25 ns steps).
    *
    * @param bcid The bcid whose train should be investigated
    * @param type The type of the requested return value
    * @returns The gap before the train of the specified bcid
    */
   int BunchCrossingToolBase::gapBeforeTrain( bcid_type bcid,
                                              BunchDistanceType type ) const {

      // Find this BCID in the list of bunch trains:
      std::set< BunchTrain >::const_iterator itr = m_bunchTrains.begin();
      std::set< BunchTrain >::const_iterator end = m_bunchTrains.end();
      std::set< BunchTrain >::const_iterator train = m_bunchTrains.end();
      for( ; itr != end; ++itr ) {
         if( itr->find( bcid ) != itr->end() ) {
            train = itr;
            break;
         }
      }

      // If we didn't find this BCID in a train, let's return right away:
      if( train == end ) {
         return -1;
      }

      // Search for the first filled bunch before the front of this train:
      BunchCrossing train_tail = *( train->train_front() );
      --train_tail;
      while( ! isFilled( train_tail ) ) {
         --train_tail;
      }

      // Now return the results:
      switch( type ) {

      case NanoSec:
         return BunchCrossing::BUNCH_SPACING *
            train->train_front()->distance( train_tail );
         break;
      case BunchCrossings:
         return train->train_front()->distance( train_tail );
         break;
      default:
         ATH_MSG_ERROR( "You can only use NanoSec or BunchCrossings for type "
                        "for gapBeforeTrain" );
         return -1;
      }

      // This should actually never be reached:
      return -1;
   }

   /**
    * The function first finds the train that the specified BCID is in. Then
    * it calculates the size of the gap after this train. It can return the
    * size of the gap either in nanoseconds or in BCIDs (25 ns steps).
    *
    * @param bcid The bcid whose train should be investigated
    * @param type The type of the requested return value
    * @returns The gap after the train of the specified bcid
    */
   int BunchCrossingToolBase::gapAfterTrain( bcid_type bcid,
                                             BunchDistanceType type ) const {

      // Find this BCID in the list of bunch trains:
      std::set< BunchTrain >::const_iterator itr = m_bunchTrains.begin();
      std::set< BunchTrain >::const_iterator end = m_bunchTrains.end();
      std::set< BunchTrain >::const_iterator train = m_bunchTrains.end();
      for( ; itr != end; ++itr ) {
         if( itr->find( bcid ) != itr->end() ) {
            train = itr;
            break;
         }
      }

      // If we didn't find this BCID in a train, let's return right away:
      if( train == end ) {
         return -1;
      }

      // Search for the first filled bunch before the front of this train:
      BunchCrossing train_front = *( train->train_back() );
      ++train_front;
      while( ! isFilled( train_front ) ) {
         ++train_front;
      }

      // Now return the results:
      switch( type ) {

      case NanoSec:
         return BunchCrossing::BUNCH_SPACING *
            train_front.distance( *( train->train_back() ) );
         break;
      case BunchCrossings:
         return train_front.distance( *( train->train_back() ) );
         break;
      default:
         ATH_MSG_ERROR( "You can only use NanoSec or BunchCrossings for type "
                        "for gapAfterTrain" );
         return -1;
      }

      // This should actually never be reached:
      return -1;
   }

   /**
    * The function creates a smart BunchCrossing out of the BCID provided, then
    * it goes looking for the previous bunch crossing of the specified type.
    * Finally it just calculates the distance between the two bunch crossings in
    * the requested units. It can return the size of the gap either in
    * nanoseconds or in BCIDs (25 ns steps).
    *
    * @param bcid The bcid that should be investigated
    * @param dtype The type of the requested return value
    * @param ftype The type of the previous bunch to consider
    * @returns The gap before the specified bcid
    */
   int BunchCrossingToolBase::gapBeforeBunch( bcid_type bcid,
                                              BunchDistanceType dtype,
                                              BunchFillType ftype ) const {

      // Construct this "smart" BCID:
      const BunchCrossing bunch( bcid );

      // Search for the first previous bunch that fulfills the requirement:
      BunchCrossing prev_bunch( bunch );
      --prev_bunch;
      int loop_counter = 0;
      switch( ftype ) {

      case CollidingBunch:
         // There should always be filled bunches in the configuration, but
         // let's make sure that we don't get into an endless loop:
         while( ( ! isFilled( prev_bunch ) ) &&
                ( loop_counter < BunchCrossing::MAX_BCID ) ) {
            --prev_bunch;
            ++loop_counter;
         }
         if( loop_counter == BunchCrossing::MAX_BCID ) {
            ATH_MSG_ERROR( "Failed to calculate gap before BCID "
                           << bcid << " to a filled bunch! This shouldn't have "
                           << "happened!" );
            return -1;
         }
         break;
      case UnpairedBunch:
         // There are no unpaired bunches in every configuration, so make sure
         // we don't get into an endless loop:
         while( ( ! isUnpaired( prev_bunch ) ) &&
                ( loop_counter < BunchCrossing::MAX_BCID ) ) {
            --prev_bunch;
            ++loop_counter;
         }
         // Return "-1" if there are no unpaired bunches in the configuration:
         if( loop_counter == BunchCrossing::MAX_BCID ) {
            return -1;
         }
         break;
      case UnpairedBeam1:
         // There are no unpaired bunches from beam 1 in every configuration, so
         // make sure we don't get into an endless loop:
         while( ( ! ( isUnpaired( prev_bunch ) && isBeam1( prev_bunch ) ) ) &&
                ( loop_counter < BunchCrossing::MAX_BCID ) ) {
            --prev_bunch;
            ++loop_counter;
         }
         // Return "-1" if there are no unpaired bunches from beam 1 in the
         // configuration:
         if( loop_counter == BunchCrossing::MAX_BCID ) {
            return -1;
         }
         break;
      case UnpairedBeam2:
         // There are no unpaired bunches from beam 2 in every configuration, so
         // make sure we don't get into an endless loop:
         while( ( ! ( isUnpaired( prev_bunch ) && isBeam2( prev_bunch ) ) ) &&
                ( loop_counter < BunchCrossing::MAX_BCID ) ) {
            --prev_bunch;
            ++loop_counter;
         }
         // Return "-1" if there are no unpaired bunches from beam 2 in the
         // configuration:
         if( loop_counter == BunchCrossing::MAX_BCID ) {
            return -1;
         }
         break;
      case EmptyBunch:
         // There should always be empty bunches in the configuration, but let's
         // make sure that we don't get into an endless loop:
         while( ( isFilled( prev_bunch ) || isUnpaired( prev_bunch ) ) &&
                ( loop_counter < BunchCrossing::MAX_BCID ) ) {
            --prev_bunch;
            ++loop_counter;
         }
         if( loop_counter == BunchCrossing::MAX_BCID ) {
            ATH_MSG_ERROR( "Failed to calculate gap before BCID "
                           << bcid << " to an empty bunch! This shouldn't have "
                           << "happened!" );
            return -1;
         }
         break;
      default:
         ATH_MSG_ERROR( "Unknown bunch fill type specified: "
                        << ftype );
         return -1;
      }

      // Now return the results:
      switch( dtype ) {

      case NanoSec:
         return BunchCrossing::BUNCH_SPACING * bunch.gapFrom( prev_bunch );
         break;
      case BunchCrossings:
         return bunch.gapFrom( prev_bunch );
         break;
      default:
         ATH_MSG_ERROR( "You can only use NanoSec or BunchCrossings for type "
                        "for gapBeforeBunch" );
         return -1;
      }

      // This should actually never be reached:
      return -1;
   }

   /**
    * The function creates a smart BunchCrossing out of the BCID provided, then
    * it goes looking for the next bunch crossing of the specified type. Finally
    * it just calculates the distance between the two bunch crossings in the
    * requested units. It can return the size of the gap either in nanoseconds
    * or in BCIDs (25 ns steps).
    *
    * @param bcid The bcid that should be investigated
    * @param dtype The type of the requested return value
    * @param ftype The type of the previous bunch to consider
    * @returns The gap after the specified bcid
    */
   int BunchCrossingToolBase::gapAfterBunch( bcid_type bcid,
                                             BunchDistanceType dtype,
                                             BunchFillType ftype ) const {

      // Construct this "smart" BCID:
      const BunchCrossing bunch( bcid );

      // Search for the first next bunch that fulfills the requirement:
      BunchCrossing next_bunch( bunch );
      ++next_bunch;
      int loop_counter = 0;
      switch( ftype ) {

      case CollidingBunch:
         // There should always be filled bunches in the configuration, but
         // let's make sure that we don't get into an endless loop:
         while( ( ! isFilled( next_bunch ) ) &&
                ( loop_counter < BunchCrossing::MAX_BCID ) ) {
            ++next_bunch;
            ++loop_counter;
         }
         if( loop_counter == BunchCrossing::MAX_BCID ) {
            ATH_MSG_ERROR( "Failed to calculate gap after BCID "
                           << bcid << " to a filled bunch! This shouldn't have "
                           << "happened!" );
            return -1;
         }
         break;
      case UnpairedBunch:
         // There are no unpaired bunches in every configuration, so make sure
         // we don't get into an endless loop:
         while( ( ! isUnpaired( next_bunch ) ) &&
                ( loop_counter < BunchCrossing::MAX_BCID ) ) {
            ++next_bunch;
            ++loop_counter;
         }
         // Return "-1" if there are no unpaired bunches in the configuration:
         if( loop_counter == BunchCrossing::MAX_BCID ) {
            return -1;
         }
         break;
      case UnpairedBeam1:
         // There are no unpaired bunches from beam 1 in every configuration, so
         // make sure we don't get into an endless loop:
         while( ( ! ( isUnpaired( next_bunch ) && isBeam1( next_bunch ) ) ) &&
                ( loop_counter < BunchCrossing::MAX_BCID ) ) {
            ++next_bunch;
            ++loop_counter;
         }
         // Return "-1" if there are no unpaired bunches from beam 1 in the
         // configuration:
         if( loop_counter == BunchCrossing::MAX_BCID ) {
            return -1;
         }
         break;
      case UnpairedBeam2:
         // There are no unpaired bunches from beam 2 in every configuration, so
         // make sure we don't get into an endless loop:
         while( ( ! ( isUnpaired( next_bunch ) && isBeam2( next_bunch ) ) ) &&
                ( loop_counter < BunchCrossing::MAX_BCID ) ) {
            ++next_bunch;
            ++loop_counter;
         }
         // Return "-1" if there are no unpaired bunches from beam 2 in the
         // configuration:
         if( loop_counter == BunchCrossing::MAX_BCID ) {
            return -1;
         }
         break;
      case EmptyBunch:
         // There should always be empty bunches in the configuration, but let's
         // make sure that we don't get into an endless loop:
         while( ( isFilled( next_bunch ) || isUnpaired( next_bunch ) ) &&
                ( loop_counter < BunchCrossing::MAX_BCID ) ) {
            ++next_bunch;
            ++loop_counter;
         }
         if( loop_counter == BunchCrossing::MAX_BCID ) {
            ATH_MSG_ERROR( "Failed to calculate gap after BCID "
                           << bcid << " to an empty bunch! This shouldn't have "
                           << "happened!" );
            return -1;
         }
         break;
      default:
         ATH_MSG_ERROR( "Unknown bunch fill type specified: "
                        << ftype );
         return -1;
      }

      // Now return the results:
      switch( dtype ) {

      case NanoSec:
         return BunchCrossing::BUNCH_SPACING * bunch.gapTo( next_bunch );
         break;
      case BunchCrossings:
         return bunch.gapTo( next_bunch );
         break;
      default:
         ATH_MSG_ERROR( "You can only use NanoSec or BunchCrossings for type "
                        "for gapBeforeBunch" );
         return -1;
      }

      // This should actually never be reached:
      return -1;
   }

   std::vector< bool >
   BunchCrossingToolBase::bunchesInFront( bcid_type bcid,
                                          int bunches ) const {

      // The only thing we have to be careful about is the bunches near the
      // "turnover" region of the BCIDs. That's why I use the BunchCrossing
      // class here:
      std::vector< bool > result;
      for( int i = 0; i < bunches; ++i ) {
         result.push_back( isFilled( BunchCrossing( bcid ) -
                                     BunchCrossing( i ) ) );
      }

      return result;
   }

   std::vector< bool >
   BunchCrossingToolBase::bunchesAfter( bcid_type bcid,
                                        int bunches ) const {

      // The only thing we have to be careful about is the bunches near the
      // "turnover" region of the BCIDs. That's why I use the BunchCrossing
      // class here:
      std::vector< bool > result;
      for( int i = 0; i < bunches; ++i ) {
         result.push_back( isFilled( BunchCrossing( bcid ) +
                                     BunchCrossing( i ) ) );
      }

      return result;
   }

   std::vector< float >
   BunchCrossingToolBase::bunchIntInFront( bcid_type bcid,
                                           int bunches,
                                           BeamType type ) const {

      std::vector< float > result;
      for( int i = 0; i < bunches; ++i ) {
         std::set< BunchCrossing >::const_iterator itr =
            m_filledBunches.find( BunchCrossing( bcid ) - BunchCrossing( i ) );
         if( itr != m_filledBunches.end() ) {
            switch( type ) {
            case Beam1:
            case Crossing:
               result.push_back( itr->intensityBeam1() );
               break;
            case Beam2:
               result.push_back( itr->intensityBeam2() );
               break;
            default:
               ATH_MSG_ERROR( "Unknown intensity type requested ("
                              << type << ")" );
               return result;
            }
         } else {
            result.push_back( 0.0 );
         }
      }

      return result;
   }

   std::vector< float >
   BunchCrossingToolBase::bunchIntAfter( bcid_type bcid,
                                         int bunches,
                                         BeamType type ) const {

      std::vector< float > result;
      for( int i = 0; i < bunches; ++i ) {
         std::set< BunchCrossing >::const_iterator itr =
            m_filledBunches.find( BunchCrossing( bcid ) + BunchCrossing( i ) );
         if( itr != m_filledBunches.end() ) {
            switch( type ) {
            case Beam1:
            case Crossing:
               result.push_back( itr->intensityBeam1() );
               break;
            case Beam2:
               result.push_back( itr->intensityBeam2() );
               break;
            default:
               ATH_MSG_ERROR( "Unknown intensity type requested ("
                              << type << ")" );
               return result;
            }
         } else {
            result.push_back( 0.0 );
         }
      }

      return result;
   }

   unsigned int BunchCrossingToolBase::numberOfFilledBunches() const {

      return m_filledBunches.size();
   }

   unsigned int BunchCrossingToolBase::numberOfUnpairedBunches() const {

      return m_unpairedBunches.size();
   }

   unsigned int BunchCrossingToolBase::numberOfBunchTrains() const {

      return m_bunchTrains.size();
   }

   int
   BunchCrossingToolBase::bunchTrainSpacing( BunchDistanceType type ) const {

      // Check if there are bunch trains in the current configurations:
      if( m_bunchTrains.size() ) {
         switch( type ) {
         case NanoSec:
            return m_bunchTrains.begin()->spacing();
            break;
         case BunchCrossings:
            return ( m_bunchTrains.begin()->spacing() / 
                     BunchCrossing::BUNCH_SPACING );
            break;
         case FilledBunches:
            ATH_MSG_ERROR( "Function should not be called with argument: "
                           "FilledBunches" );
            return -1;
            break;
         default:
            ATH_MSG_ERROR( "Function called with unknown argument: " << type );
            return -1;
         }
      } else {
         // Return -1 if there are no bunch trains in the configuration:
         return -1;
      }

      ATH_MSG_FATAL( "The code should never reach this line. Check the code!" );
      return -1;
   }

   /**
    * This function takes care of selecting the single bunches from the paired
    * bunches. These will then not be taken into account in the train finding
    * algorithm.
    *
    * The bunch intensity parameter is optional. If it's not specified, the code
    * will assign an intensity of "1.0" to all the bunch crossings.
    *
    * @param bunches The paired bunches
    * @param bunch_int The "intensities" of the paired bunches
    * @returns <code>StatusCode::SUCCESS</code> if the interpretation was
    *          successful, <code>StatusCode::FAILURE</code> otherwise
    */
   StatusCode BunchCrossingToolBase::
   loadSingleBunches( const std::vector< int >& bunches,
                      const std::vector< float >& bunch_int1,
                      const std::vector< float >& bunch_int2 ) {

      // Do a small sanity check:
      if( ( ( bunches.size() != bunch_int1.size() ) && bunch_int1.size() ) ||
          ( ( bunches.size() != bunch_int2.size() ) && bunch_int2.size() ) ) {
         ATH_MSG_ERROR( "Received vectors of different sizes for the bunch "
                        "IDs and bunch intensities\n"
                        "Function can not work like this..." );
         return StatusCode::FAILURE;
      }
      if( ! bunch_int1.size() ) {
         ATH_MSG_DEBUG( "Not using bunch intensity for the calculation" );
      }
      if( ( ! bunch_int2.size() ) && bunch_int1.size() ) {
         ATH_MSG_DEBUG( "Using 'bunch crossing intensity' for the "
                        "calculation" );
      }

      //
      // Calculate the allowed maximum BCID separation between single bunches:
      //
      const int maxBCSpacing = bunchSpacing( bunches );

      // Reset the cache:
      m_filledBunches.clear();
      m_singleBunches.clear();

      //
      // Loop over the paired bunch crossings:
      //
      std::vector< int >::const_iterator b_itr = bunches.begin();
      std::vector< int >::const_iterator b_end = bunches.end();
      std::vector< float >::const_iterator i1_itr = bunch_int1.begin();
      std::vector< float >::const_iterator i2_itr = bunch_int2.begin();
      for( ; b_itr != b_end; ++b_itr ) {

         // Evaluate the intensity of this paired bunch:
         const float intensity1 = bunch_int1.size() ? *i1_itr : 1.0;
         const float intensity2 = bunch_int2.size() ? *i2_itr : 0.0;

         // It's definitely a filled bunch, so let's remember it as such:
         m_filledBunches.insert( BunchCrossing( *b_itr, intensity1,
                                                intensity2 ) );

         ATH_MSG_VERBOSE( "Evaluating bunch crossing: " << *b_itr );

         //
         // This is some STL magic. This expression counts how many of the
         // paired bunches fulfill:
         //
         //      distance( ref_bcid,  bcid ) <= maxBCSpacing
         //
         // Since the calculation takes the reference bcid into account as well,
         // the count is always >= 1. I have to use the specialised
         // distance(...) function, because a regular
         //
         //      bcid - maxBCSpacing <= ref_bcid <= bcid + maxBCSpacing
         //
         // expression wouldn't give the correct answer at the "turnover" of the
         // bcid numbering. (When evaluating bcid 1 and
         // BunchCrossing::MAX_BCID.)
         //
         int neighbours =
            std::count_if( bunches.begin(), bunches.end(),
                           compose1( std::bind2nd( std::less_equal< int >(),
                                                   maxBCSpacing ),
                                     std::bind1st( std::ptr_fun( Trig::distance ),
                                                   *b_itr ) ) );

         //
         // Now decide if we want to consider this bunch crossing as a single
         // bunch or not:
         //
         ATH_MSG_VERBOSE( "  Bunch neighbours: " << neighbours );
         if( neighbours == 1 ) {
            ATH_MSG_VERBOSE( "  Bunch crossing " << *b_itr
                             << " seems to be a single bunch" );
            m_singleBunches.insert( BunchCrossing( *b_itr, intensity1,
                                                   intensity2 ) );
         }

         // Only step through the intensity vector(s) if it has some elements:
         if( bunch_int1.size() ) ++i1_itr;
         if( bunch_int2.size() ) ++i2_itr;
      }

      //
      // Finally some debugging message for the end:
      //
      ATH_MSG_DEBUG( "Single bunches found: " << m_singleBunches );

      return StatusCode::SUCCESS;
   }

   /**
    * This function takes care of identifying the bunch trains in the
    * configuration. The algorithm is quite simple. It starts off with all the
    * filled bunches that have not been identified as single bunches by the
    * loadSingleBunches(...) function. It takes the first available bunch
    * crossing, then loops over the rest of them. When it finds a BC that's
    * "close enough" to the current bunch train, then the bunch train is
    * extended. From there on the algorithm continues with this extended bunch
    * train. When the loop reaches the end of the available bunches, the created
    * bunch train is added to the cache, and the algorithms starts again with
    * the first still available bunch crossing.
    *
    * The bunch intensity parameter is optional. If it's not specified, the code
    * will assign an intensity of "1.0" to all the bunch crossings.
    *
    * @param bunches The filled bunch crossings
    * @param bunch_int The "intensities" of the paired bunches
    * @returns <code>StatusCode::SUCCESS</code> if the interpretation was
    *          successful, <code>StatusCode::FAILURE</code> otherwise
    */
   StatusCode BunchCrossingToolBase::
   loadBunchTrains( const std::vector< int >& bunches,
                    const std::vector< float >& bunch_int1,
                    const std::vector< float >& bunch_int2 ) {

      // Do a small sanity check:
      if( ( ( bunches.size() != bunch_int1.size() ) && bunch_int1.size() ) ||
          ( ( bunches.size() != bunch_int2.size() ) && bunch_int2.size() ) ) {
         ATH_MSG_ERROR( "Received vectors of different sizes for the bunch "
                        "IDs and bunch intensities\n"
                        "Function can not work like this..." );
         return StatusCode::FAILURE;
      }
      if( ! bunch_int1.size() ) {
         ATH_MSG_DEBUG( "Not using bunch intensity for the calculation" );
      }
      if( ( ! bunch_int2.size() ) && bunch_int1.size() ) {
         ATH_MSG_DEBUG( "Using 'bunch crossing intensity' for the "
                        "calculation" );
      }

      //
      // Calculate the allowed maximum BCID separation between single bunches:
      //
      const int maxBCSpacing = bunchSpacing( bunches );

      // Reset the cache:
      m_bunchTrains.clear();

      //
      // Create a cache of the bunches which have not been identified as a
      // single bunch:
      //
      std::set< BunchCrossing > cache;
      std::vector< int >::const_iterator b_itr = bunches.begin();
      std::vector< int >::const_iterator b_end = bunches.end();
      std::vector< float >::const_iterator i1_itr = bunch_int1.begin();
      std::vector< float >::const_iterator i2_itr = bunch_int2.begin();
      for( ; b_itr != b_end; ++b_itr ) {
         if( std::find( m_singleBunches.begin(), m_singleBunches.end(),
                        BunchCrossing( *b_itr ) ) == m_singleBunches.end() ) {
            cache.insert( BunchCrossing( *b_itr,
                                         ( float )( bunch_int1.size() ? *i1_itr :
                                                    1.0 ),
                                         ( float )( bunch_int2.size() ? *i2_itr :
                                                    0.0 ) ) );
         }
         if( bunch_int1.size() ) ++i1_itr;
         if( bunch_int2.size() ) ++i2_itr;
      }

      ATH_MSG_VERBOSE( "Bunches considered for trains: " << cache );

      //
      // Continue the loop until we have unassigned bunches:
      //
      while( cache.size() ) {

         // Create a new bunch train object:
         BunchTrain bt;
         bt.insert( *cache.begin() );

         // Try finding attachable bunches until no other attachable bunch can
         // be found:
         size_t prev_size = 0;
         while( prev_size != cache.size() ) {

            // Let's remember the size of the cache:
            prev_size = cache.size();

            // Find all the bunches that should be a part of this train:
            std::set< BunchCrossing >::const_iterator c_itr = cache.begin();
            std::set< BunchCrossing >::const_iterator c_end = cache.end();
            for( ; c_itr != c_end; ++c_itr ) {
               if( bt.distance( *c_itr ) <= maxBCSpacing ) {
                  ATH_MSG_VERBOSE( "Adding BunchCrossing " << *c_itr
                                   << " to Bunch Train " << bt );
                  bt.insert( *c_itr );
               }
            }

            // Now remove the selected bunches from the cache:
            BunchTrain::const_iterator itr = bt.begin();
            BunchTrain::const_iterator end = bt.end();
            for( ; itr != end; ++itr ) {
               cache.erase( *itr );
            }
         }

         // Finally, remember this train:
         if( ! bt.validate() ) {
            ATH_MSG_ERROR( "Found a strange bunch train: " << bt );
            ATH_MSG_ERROR( "Keeping in it the list of trains!" );
         }
         m_bunchTrains.insert( bt );
      }

      //
      // Check if the spacing in the bunch trains is the same. (It should be for
      // all real configurations.)
      //
      std::set< BunchTrain >::const_iterator train_itr = m_bunchTrains.begin();
      std::set< BunchTrain >::const_iterator train_end = m_bunchTrains.end();
      int spacing = -1;
      for( ; train_itr != train_end; ++train_itr ) {
         if( spacing < 0 ) {
            spacing = train_itr->spacing();
            continue;
         }
         if( train_itr->spacing() != spacing ) {
            ATH_MSG_WARNING( "The spacing seems to be different between the "
                             "trains" );
            ATH_MSG_WARNING( "This should probably not happen" );
         }
      }

      ATH_MSG_DEBUG( "Bunch trains found: " << m_bunchTrains );

      return StatusCode::SUCCESS;
   }

   /**
    * This function just caches the unpaired bunches internally. It doesn't have
    * to do anything as fancy as the other two load functions, it just takes the
    * BCIDs as they are.
    *
    * @param bunches The unpaired bunch crossings
    * @param bunch_int The "intensities" of the unpaired bunches
    * @returns <code>StatusCode::SUCCESS</code> if the caching was successful,
    *          <code>StatusCode::FAILURE</code> otherwise
    */
   StatusCode BunchCrossingToolBase::
   loadUnpairedBunches( const std::vector< int >& beam1,
                        const std::vector< int >& beam2,
                        const std::vector< float >& bunch_int1,
                        const std::vector< float >& bunch_int2 ) {

      // Do a small sanity check:
      if( ( ( beam1.size() != bunch_int1.size() ) && bunch_int1.size() ) ||
          ( ( beam2.size() != bunch_int2.size() ) && bunch_int2.size() ) ) {
         ATH_MSG_ERROR( "Received vectors of different sizes for the bunch "
                        "IDs and bunch intensities\n"
                        "Function can not work like this..." );
         return StatusCode::FAILURE;
      }
      if( ( ! bunch_int1.size() ) && ( ! bunch_int2.size() ) ) {
         ATH_MSG_DEBUG( "Not using bunch intensity for the calculation" );
      }

      // Reset the cache:
      m_unpairedBunches.clear();

      //
      // Add the unpaired bunches from beam 1:
      //
      std::vector< int >::const_iterator b_itr = beam1.begin();
      std::vector< int >::const_iterator b_end = beam1.end();
      std::vector< float >::const_iterator i_itr = bunch_int1.begin();
      for( ; b_itr != b_end; ++b_itr ) {

         // Evaluate the intensity of this unpaired bunch:
         const float intensity = bunch_int1.size() ? *i_itr : 1.0;

         // Nothing fancy to do, just put it in the cache:
         m_unpairedBunches.insert( BunchCrossing( *b_itr, intensity, 0.0 ) );

         // Only step through the intensity vector if it has some elements:
         if( bunch_int1.size() ) ++i_itr;
      }

      //
      // Add the unpaired bunches from beam 2:
      //
      b_itr = beam2.begin();
      b_end = beam2.end();
      i_itr = bunch_int2.begin();
      for( ; b_itr != b_end; ++b_itr ) {

         // Evaluate the intensity of this unpaired bunch:
         const float intensity = bunch_int2.size() ? *i_itr : 1.0;

         // Check if this BCID is already known as an unpaired BCID:
         std::set< BunchCrossing >::iterator itr =
            m_unpairedBunches.find( *b_itr );
         if( itr != m_unpairedBunches.end() ) {
            // Modify the BCID not to correspond to a particular beam. Most
            // implementations don't treat beam 1 and beam 2 separately.
            BunchCrossing bc( *itr );
            bc.setIntensityBeam2( intensity );
            m_unpairedBunches.erase( itr );
            m_unpairedBunches.insert( bc );
         } else {
            // Nothing fancy to do, just put it in the cache:
            m_unpairedBunches.insert( BunchCrossing( *b_itr, 0.0, intensity ) );
         }

         // Only step through the intensity vector if it has some elements:
         if( bunch_int2.size() ) ++i_itr;
      }

      //
      // Finally some debugging message for the end:
      //
      ATH_MSG_DEBUG( "Unpaired bunches found: " << m_unpairedBunches );

      return StatusCode::SUCCESS;
   }

   /**
    * This function is used to print the overall configuration in a format
    * very similar to how atlas-runqery.cern.ch started showing this information
    * recently.
    */
   void BunchCrossingToolBase::printConfig() const {

      ATH_MSG_INFO( "No. of coll. bunches   : " << m_filledBunches.size() );
      ATH_MSG_INFO( "No. of unpaired bunches: " << m_unpairedBunches.size() );
      ATH_MSG_INFO( "No. of bunch trains    : " << m_bunchTrains.size() );
      if( m_bunchTrains.size() ) {
         ATH_MSG_INFO( "Bunch spacing in trains: "
                       << m_bunchTrains.begin()->spacing()
                       << " ns" );
      }

      return;
   }

   /**
    * This function tries to figure out what's the right bunch spacing for the
    * current configuration, before the code would try to properly interpret
    * the configuration.
    *
    * This allows us to set a "MaxBunchSpacing" property of a relatively large
    * number, but still be able to interpret configurations with very small gaps
    * between bunch trains. (Like the 8b4e 2017 configurations.)
    *
    * @param bunches The filled bunches in the current configuration
    * @return The apparent bunch spacing of the configuration in bunch crossings
    */
   int BunchCrossingToolBase::
   bunchSpacing( const std::vector< int >& bunches ) const {

      // The maximum BC spacing to start from.
      const int maxSpacing = m_maxBunchSpacing / BunchCrossing::BUNCH_SPACING;

      // Iterate downwards from the maximum spacing, searching for the minimum
      // spacing with which bunches exist.
      int result = maxSpacing;
      for( ; result > 0; --result ) {

         // Test how many bunches have neighbors inside of the current window.
         const int nbunches =
            std::count_if( bunches.begin(), bunches.end(),
                           count_bunch_neighbors( bunches, result ) );
         ATH_MSG_VERBOSE( "Number of bunches with " << result
                          << " BC neighbors: " << nbunches );

         // If none of them do, then we're finished.
         if( ! nbunches ) {
            // If we're not at the max spacing anymore, then it means that in
            // the previous step there were still bunches with neighbors in this
            // window. That's the spacing we need then.
            //
            // But if we're still at the maximum spacing, then let's just keep
            // that as the final answer.
            if( result != maxSpacing ) {
               ++result;
            }
            break;
         }
      }
      // If we went down to 0, then the right answer is 1. This is just how this
      // algorithm works...
      if( result == 0 ) {
         result = 1;
      }
      ATH_MSG_DEBUG( "Bunch spacing: " << result << " BCs" );

      // Return the smallest spacing found:
      return result;
   }

} // namespace Trig
