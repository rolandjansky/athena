/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchTrain.cxx 625753 2014-11-03 13:40:15Z krasznaa $

// System include(s):
#include <cstdlib>
#include <iostream>

// Gaudi/Athena include(s):
#include "AsgMessaging/AsgMessaging.h"

// Local include(s):
#include "TrigBunchCrossingTool/BunchTrain.h"
#include "SetPrint.h"

namespace Trig {

   /**
    * Default constructor, creating an empty bunch train.
    */
   BunchTrain::BunchTrain()
      : std::set< BunchCrossing >(), m_spacing( -1 ),
        m_front(), m_back(), m_gapFound( false ) {

   }

   /**
    * The internal iterators don't remain valid if we just use the default
    * copy constructor. Hence when the object is copied, the validation
    * has to be run again.
    *
    * @param parent The original BunchTrain object
    */
   BunchTrain::BunchTrain( const BunchTrain& parent )
      : std::set< BunchCrossing >( parent ), m_spacing( -1 ),
        m_front(), m_back(), m_gapFound( false ) {

      validate();
   }

   BunchTrain& BunchTrain::operator= ( const BunchTrain& rhs ) {

      // Protect against self-assignment:
      if( this == &rhs ) {
         return *this;
      }

      // Do the heavy lifting in the base class's assignment operator:
      std::set< BunchCrossing >::operator=( rhs );

      // Reset the cached variables:
      m_front = end();
      m_back  = end();
      m_gapFound = false;

      // Cache the variables:
      validate();

      // Return the updated object:
      return *this;
   }

   /**
    * The spacing between the bunches in the train is calculated by the
    * BunchTrain::validate() function, so you should only trust this value once
    * the validation ran on this object.
    *
    * The value is stored in nanoseconds to maybe make the meaning of the value
    * a bit more obvious.
    *
    * @returns The bunch spacing in nanoseconds, or -1 if the object has not been
    *          validated yet
    */
   int BunchTrain::spacing() const {

      return m_spacing;
   }

   /**
    * This function is used by the code to determine if a new bunch crossing should
    * be merged into this bunch train or not. The calculation of the distance of a
    * bunch crossing from a bunch train is pretty simple. If the train is empty, or
    * the bunch crossing is already in it, the distance is 0. Otherwise the code
    * calculates the distance of the reference bunch crossing from all the bunch
    * crossings which are already in the train, and takes the smallest value. It's
    * not the fastest method, but it's the only reliable method that I've found...
    *
    * @param bc The reference bunch crossing
    * @returns The distance of the reference bunch crossing from this bunch train
    */
   int BunchTrain::distance( const BunchCrossing& bc ) const {

      // If the train is empty, by definition the distance is 0:
      if( ! size() ) return 0;

      // If the BC is already in the train, it has a distance of 0 from it:
      if( find( bc ) != end() ) return 0;

      // Calculate the BC's distance from all the bunches in the train, and take the
      // smallest one:
      int min_distance = BunchCrossing::MAX_BCID;
      BunchTrain::const_iterator itr = begin();
      for( ; itr != end(); ++itr ) {
         int diff;
         if( ( diff = itr->distance( bc ) ) < min_distance ) {
            min_distance = diff;
         }
      }

      return min_distance;
   }

   /**
    * This function checks if the specified bunch crossing is "inside" this bunch
    * train. This can mean two different things. Either the bunch crossing is a
    * filled bunch crossing which is part of the train, or the bunch crossing is
    * an empty bunch crossing which is between the front and the tail of this
    * train.
    *
    * @param bc The reference bunch crossing
    * @returns <code>true</code> if the reference bunch crossing is "inside" the
    *          train, or <code>false</code> if it isn't
    */
   bool BunchTrain::isInside( const BunchCrossing& bc ) const {

      // If it's a filled bunch crossing in this train, then it's inside of it:
      if( find( bc ) != end() ) return true;

      // Check if the bunch crossing is an empty bunch crossing inside
      // this train:
      if( m_gapFound ) {
         if( ( ( *begin() < bc ) && ( *train_back() > bc ) ) ||
             ( ( *train_front() < bc ) && ( *( --end() ) > bc ) ) ) {
            return true;
         }
      } else {
         if( ( *train_front() < bc ) && ( *train_back() > bc ) ) {
            return true;
         }
      }

      // If none of the above are true, then the reference is not inside
      // of this train:
      return false;
   }

   /**
    * The returned iterator always points to the first bunch of the train.
    *
    * Note however, that can only be "logically" the first bunch of the train.
    * When a train stretches over the "BCID turnover" region, the underlying
    * <code>std::set</code> object will order the bunches like this:
    *
    *  0, 1, 2, 3562, 3563
    *
    * In this case 3562 is the first bunch of the train, and this function will
    * return an iterator pointing to that element.
    *
    * This iterator is determined in the validate() function, so the code must
    * always call validate() at least once before using this iterator.
    *
    * @returns The element that is logically the front of the train
    */
   BunchTrain::const_iterator BunchTrain::train_front() const {

      if( m_gapFound ) {
         return m_front;
      } else {
         return begin();
      }
   }

   /**
    * The returned iterator always points to the last bunch of the train.
    *
    * Note that it is a valid iterator, which actually points to the end of the
    * train. So you shouldn't use it in for loops like end().
    *
    * The rest of the logic is the same as with train_front().
    *
    * @see train_front
    * @returns The element that is logically at the back of the train
    */
   BunchTrain::const_iterator BunchTrain::train_back() const {

      if( m_gapFound ) {
         return m_back;
      } else {
         return ( --end() );
      }
   }

   /**
    * The function checks if the bunches in the train have equal spacing in between
    * them. This should be the case if everything went correctly with the code...
    *
    * It also checks whether the train stretches over the "BCID turnover" or not.
    * If it does, it sets the m_front and m_back variables correctly. If it does not,
    * then the train doesn't have to be treated in a special way.
    *
    * @returns <code>true</code> if the validation passes,
    *          <code>false</code> otherwise
    */
   bool BunchTrain::validate() {

      /// Helper object to print logging messages:
      static asg::AsgMessaging logger( "Trig::BunchTrain" );
      logger.msg().setLevel( MSG::INFO ); // This is a temporary hack until AsgTools is updated...

      // Reset the gap flag:
      m_gapFound = false;

      // Loop over all the bunch crossings of this train:
      BunchTrain::const_iterator itr = begin();
      size_t bunch_pos = 0;
      BunchTrain::const_iterator next;
      for( ; itr != end(); ++itr, ++bunch_pos ) {
         // Set up an iterator to the next bunch crossing in the train:
         next = itr; ++next;
         if( next == end() ) continue;
         // Calculate the spacing between these two:
         int diff = BunchCrossing::BUNCH_SPACING * next->distance( *itr );
         if( m_spacing < 0 ) { // If these are the first two bunches of the train
            m_spacing = diff;
         }
         // Check if it is the same as the stored value:
         else if( diff != m_spacing ) {
            // If not, there can be a two valid reasons for it. First is that we've been seeing
            // the correct bunch spacing so far, and we just reached the gap:
            if( ( ! m_gapFound ) && ( diff > m_spacing ) ) {
               m_gapFound = true;
               m_front = next;
               m_back  = itr;
            }
            // Or that the spacing between the first two bunches was actually the gap,
            // and not the train's bunch spacing. (For trains like: [0, 3561, 3562, 3563])
            else if( ( bunch_pos == 1 ) && ( ! m_gapFound ) && ( diff < m_spacing ) ) {
               m_gapFound = true;
               m_front = itr;
               m_back = itr; --m_back;
               m_spacing = diff;
            }
            // Or finally it could be that the configuration is just weird...
            else {
               logger.msg() << MSG::WARNING << "Bunches don't appear to have "
                            << "equal spacing!" << endmsg;
               return false;
            }
         }
      }

      // Report what we found out about this train:
      if( m_gapFound ) {
         logger.msg() << MSG::VERBOSE
                      << "Gap found in train! train_front = "
                      << *train_front() << "; train_back = "
                      << *train_back() << endmsg;
      } else {
         logger.msg() << MSG::VERBOSE
                      << "Gap not found in train! train_front = "
                      << *train_front() << "; train_back = "
                      << *train_back() << endmsg;
      }

      return true;
   }

   /**
    * This operator is here to be able to put BunchTrain objects into ordered
    * containers like <code>std::set</code> and friends.
    *
    * @param bt1 One bunch train object
    * @param bt2 Another bunch train object
    * @returns <code>true</code> if bt1 is considered "lower" than bt2, or
    *          <code>false</code> otherwise
    */
   bool operator< ( const BunchTrain& bt1, const BunchTrain& bt2 ) {

      if( ! bt2.size() ) return false;
      if( ! bt1.size() ) return true;

      return ( ( *bt1.begin() ) < ( *bt2.begin() ) );
   }

} // namespace Trig

/**
 * Operator for printing the configuration of bunch trains in a readable
 * format.
 *
 * @param stream A standard std::ostream object
 * @param bt The bunch train to print
 * @returns The same stream object that it received
 */
std::ostream& operator<< ( std::ostream& stream, const Trig::BunchTrain& bt ) {

   // Take advantage of the output operator defined for std::set objects:
   stream << "[spacing: " << bt.spacing() << "; bunches: "
          << static_cast< const std::set< Trig::BunchCrossing >& >( bt )
          << "; front: " << *bt.train_front() << "; back: "
          << *bt.train_back() << "]";

   return stream;
}

/**
 * Operator for printing the configuration of bunch trains in a readable
 * format.
 *
 * @param stream A functional MsgStream object
 * @param bt The bunch train to print
 * @returns The same stream object that it received
 */
MsgStream& operator<< ( MsgStream& stream, const Trig::BunchTrain& bt ) {

   // Take advantage of the output operator defined for std::set objects:
   stream << "[spacing: " << bt.spacing() << "; bunches: "
          << static_cast< const std::set< Trig::BunchCrossing >& >( bt )
          << "; front: " << *bt.train_front() << "; back: "
          << *bt.train_back() << "]";

   return stream;
}
