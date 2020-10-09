/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchCrossing.cxx 618129 2014-09-23 11:37:00Z krasznaa $

// System include(s):
#include <iostream>

// Gaudi/Athena include(s):
#include "AsgMessaging/AsgMessaging.h"

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossing.h"

namespace Trig {

   //
   // Initialize the static variables:
   //
   const int BunchCrossing::MAX_BCID = 3564;
   const int BunchCrossing::BUNCH_SPACING = 25;

   /**
    * The TrigConf::BunchGroup objects store the BCIDs as signed integers, hence
    * this constructor.
    *
    * @param bcid The bunch crossing ID of the bunch crossing
    * @param intBeam1 The intensity of beam 1, or the intensity/luminosity
    *                 of the crossing
    * @param intBeam2 The intensity of beam 2
    */
   BunchCrossing::BunchCrossing( int bcid, float intBeam1, float intBeam2 )
      : m_bcid( bcid ), m_intensityBeam1( intBeam1 ),
        m_intensityBeam2( intBeam2 ) {

      if( m_bcid < 0 ) {
         m_bcid = MAX_BCID + m_bcid;
      }
      while( m_bcid >= MAX_BCID ) {
         m_bcid -= MAX_BCID;
      }
   }

   /**
    * The EventID objects store the BCIDs as unsigned integers, so this other
    * version has to exist as well. (In order to define clearly what the
    * compiler should do.)
    *
    * @param bcid The bunch crossing ID of the bunch crossing
    * @param intBeam1 The intensity of beam 1, or the intensity/luminosity
    *                 of the crossing
    * @param intBeam2 The intensity of beam 2
    */
   BunchCrossing::BunchCrossing( unsigned int bcid, float intBeam1,
                                 float intBeam2 )
      : m_bcid( static_cast< int >( bcid ) ), m_intensityBeam1( intBeam1 ),
        m_intensityBeam2( intBeam2 ) {

      if( m_bcid < 0 ) {
         m_bcid = MAX_BCID + m_bcid;
      }
      while( m_bcid >= MAX_BCID ) {
         m_bcid -= MAX_BCID;
      }
   }

   /**
    * The copy constructor makes sure that only important properites are copied.
    * The class at one point had some transient variables. But not anymore, so
    * this constructor is pretty useless. But at least it's not harmful...
    *
    * @param parent The object that is to be copied
    */
   BunchCrossing::BunchCrossing( const BunchCrossing& parent )
      : m_bcid( parent.m_bcid ), m_intensityBeam1( parent.m_intensityBeam1 ),
        m_intensityBeam2( parent.m_intensityBeam2 ) {

   }

   /**
    * Just like the copy constructor, this operator isn't strictly speaking
    * needed either. But if I left the copy constructor in, I should have this
    * here as well.
    *
    * @param parent The object whose properties are to be copied
    */
   BunchCrossing& BunchCrossing::operator=( const BunchCrossing& parent ) {

      // Check that it's not a self-assignment:
      if( &parent == this ) return *this;

      m_bcid = parent.m_bcid;
      m_intensityBeam1 = parent.m_intensityBeam1;
      m_intensityBeam2 = parent.m_intensityBeam2;

      return *this;
   }

   /**
    * The automatic conversion operators are here to be able to handle bunch crossing
    * objects like a simple integer value in the code. This makes using the STL code
    * much simpler in some places.
    *
    * @returns The BCID value of this object
    */
   BunchCrossing::operator int&() {

      return m_bcid;
   }

   /**
    * The automatic conversion operators are here to be able to handle bunch crossing
    * objects like a simple integer value in the code. This makes using the STL code
    * much simpler in some places.
    *
    * @returns The BCID value of this object (constant version)
    */
   BunchCrossing::operator const int&() const {

      return m_bcid;
   }

   /**
    * This operator is overriden to take the properties of the LHC bunches into
    * account.
    *
    * @param bc The bunch crossing that should be added to this one
    * @returns The updated bunch crossing object
    */
   BunchCrossing& BunchCrossing::operator+= ( const BunchCrossing& bc ) {

      m_bcid += bc.m_bcid;
      while( m_bcid >= MAX_BCID ) {
         m_bcid -= MAX_BCID;
      }

      return *this;
   }

   /**
    * This operator is overriden to take the properties of the LHC bunches into
    * account.
    *
    * @param bc The bunch crossing that should be subtracted from this one
    * @returns The updated bunch crossing object
    */
   BunchCrossing& BunchCrossing::operator-= ( const BunchCrossing& bc ) {

      m_bcid -= bc.m_bcid;
      if( m_bcid < 0 ) {
         m_bcid = MAX_BCID + m_bcid;
      }

      return *this;
   }

   /**
    * This operator can be used when you want to iterate over bunch crossings.
    * It makes sure that the bunch crossings follow each other in the right order.
    *
    * @returns A reference to the incremented object
    */
   BunchCrossing& BunchCrossing::operator++ () {

      return operator+=( 1 );
   }

   /**
    * This operator can be used when you want to iterate over bunch crossings.
    * It makes sure that the bunch crossings follow each other in the right order.
    *
    * @returns The object's state before the increment
    */
   BunchCrossing BunchCrossing::operator++ ( int ) {

      BunchCrossing result = *this;
      operator+=( 1 );
      return result;
   }

   /**
    * This operator can be used when you want to iterate over bunch crossings.
    * It makes sure that the bunch crossings follow each other in the right order.
    *
    * @returns A reference to the decreased object
    */
   BunchCrossing& BunchCrossing::operator-- () {

      return operator-=( 1 );
   }

   /**
    * This operator can be used when you want to iterate over bunch crossings.
    * It makes sure that the bunch crossings follow each other in the right order.
    *
    * @returns The object's state before the decrease
    */
   BunchCrossing BunchCrossing::operator-- ( int ) {

      BunchCrossing result = *this;
      operator-=( 1 );
      return result;
   }

   /**
    * Function calculating the absolute value of the distance of two bunches.
    * If you just use something like <code>std::abs( b1 - b2 )</code>, the
    * result will depend on the order of the two bunches. Sometimes that's
    * the expected behaviour, but for simple distance calculations this function
    * should be used.
    *
    * @param bc The other BunchCrossing
    * @returns The distance of the specified bunch crossing from this one in BCIDs
    */
   int BunchCrossing::distance( const BunchCrossing& bc ) const {

      BunchCrossing ibc = *this - bc;
      if( ibc.m_bcid > ( MAX_BCID / 2 ) ) {
         return ( MAX_BCID - ibc.m_bcid );
      } else {
         return ibc.m_bcid;
      }
   }

   /**
    * As it turns out, the <code>distance(...)</code> function can not be used
    * when calculating the gaps between bunches, as the "direction" of the gap
    * matters a lot in this case. (So, the gap can be larger than half of the
    * LHC ring.)
    *
    * In the end, this is just a nicely named function on top of the arithmetic
    * that was designed for this class.
    *
    * @param bc The "previous" bunch to which the distance should be calculated
    * @returns The gap before this bunch to the specified one in BCIDs
    */
   int BunchCrossing::gapFrom( const BunchCrossing& bc ) const {

      return ( *this - bc );
   }

   /**
    * As it turns out, the <code>distance(...)</code> function can not be used
    * when calculating the gaps between bunches, as the "direction" of the gap
    * matters a lot in this case. (So, the gap can be larger than half of the
    * LHC ring.)
    *
    * In the end, this is just a nicely named function on top of the arithmetic
    * that was designed for this class.
    *
    * @param bc The "next" bunch to which the distance should be calculated
    * @returns The gap after this bunch to the specified one in BCIDs
    */
   int BunchCrossing::gapTo( const BunchCrossing& bc ) const {

      return ( bc - *this );
   }

   /**
    * Some parts of the code have to access the BCID and the bunch crossing intensity
    * explicitly, it's just nicer (and more readable) to do it using a function than
    * to use the dereferencing operator (*) in all those cases.
    *
    * @returns The BCID of this bunch crossing
    */
   int BunchCrossing::bcid() const {

      return m_bcid;
   }

   /**
    * This function is not used anywhere in the code I think, but for completeness's sake
    * it makes sense to have it here.
    *
    * @param bcid The BCID that this object should describe
    */
   void BunchCrossing::setBCID( int bcid ) {

      m_bcid = bcid;
      return;
   }

   /**
    * The intensity of the beam in a bunch crossing means a bit different things
    * for data and for MC, but in both cases it can be described by a floating
    * point number.
    *
    * @returns The "intensity" of beam 1 in this bunch crossing
    */
   float BunchCrossing::intensityBeam1() const {

      return m_intensityBeam1;
   }

   /**
    * @see BunchCrossing::intensity()
    * @param intensity The value that the beam 1 intensity should be set to
    */
   void BunchCrossing::setIntensityBeam1( float intensity ) {

      if( intensity >= 0.0 ) {
         m_intensityBeam1 = intensity;
      } else {
         asg::AsgMessaging logger( "Trig::BunchCrossing" );
         logger.msg() << MSG::ERROR
                      << "Trying to set beam 1 intensity to negative number ("
                      << intensity << "). Using 0.0 instead." << endmsg;
         m_intensityBeam1 = 0.0;
      }
      return;
   }

   /**
    * The intensity of the beam in a bunch crossing means a bit different things
    * for data and for MC, but in both cases it can be described by a floating
    * point number.
    *
    * @returns The "intensity" of beam 2 in this bunch crossing
    */
   float BunchCrossing::intensityBeam2() const {

      return m_intensityBeam2;
   }

   /**
    * @see BunchCrossing::intensity()
    * @param intensity The value that the beam 2 intensity should be set to
    */
   void BunchCrossing::setIntensityBeam2( float intensity ) {

      if( intensity >= 0.0 ) {
         m_intensityBeam2 = intensity;
      } else {
         asg::AsgMessaging logger( "Trig::BunchCrossing" );
         logger.msg() << MSG::ERROR
                      << "Trying to set beam 2 intensity to negative number ("
                      << intensity << "). Using 0.0 instead." << endmsg;
         m_intensityBeam2 = 0.0;
      }
      return;
   }

   /**
    * The compiler would probably be able to do this same thing automatically thanks
    * to the automatic conversion operators, but I wanted to be sure. The equality of
    * two BunchCrossing objects should not depend on the intensity stored in them,
    * only the BCIDs that they describe.
    *
    * @param bc The bunch crossing object that this object should be compared to
    * @returns <code>true</code> if the BCID of the two objects is the same,
    *          <code>false</code> otherwise
    */
   bool BunchCrossing::operator== ( const BunchCrossing& bc ) const {

      return ( m_bcid == bc.m_bcid );
   }

   /**
    * Convenience operator taking advantage of the += operator defined in the
    * BunchCrossing class. Note that using += is much quicker.
    *
    * @param bc1 One bunch crossing object
    * @param bc2 Another bunch crossing object
    * @returns The sum of the two bunch crossings
    */
   BunchCrossing operator+ ( const BunchCrossing& bc1, const BunchCrossing& bc2 ) {

      BunchCrossing result( bc1 );
      result += bc2;

      return result;
   }

   /**
    * Convenience operator taking advantage of the -= operator defined in the
    * BunchCrossing class. Note that using -= is much quicker.
    *
    * @param bc1 One bunch crossing object
    * @param bc2 Another bunch crossing object
    * @returns The difference of the two bunch crossings (absolute value depends on order!)
    */
   BunchCrossing operator- ( const BunchCrossing& bc1, const BunchCrossing& bc2 ) {

      BunchCrossing result( bc1 );
      result -= bc2;

      return result;
   }

   /**
    * I need this function only for technical reasons. It just calls
    * BunchCrossing::distance under the hood...
    *
    * Unfortunately the function can't receive constant references because
    * of the way some of STL is written. :-/
    *
    * @param bc1 One BunchCrossing object
    * @param bc2 Another BunchCrossing object
    * @returns The absolute distance of the two objects
    */
   int distance( const BunchCrossing bc1, const BunchCrossing bc2 ) {

      return bc1.distance( bc2 );
   }

} // namespace Trig

/**
 * This operator is used to print the configuration of a BunchCrossing object
 * in a nice way.
 *
 * @param out A standard std::ostream object
 * @param bc The BunchCrossing object that we want to display
 * @returns The same stream object to be able to chain output statements together
 */
std::ostream& operator<< ( std::ostream& out, const Trig::BunchCrossing& bc ) {

   out << "[id:" << bc.bcid() << ";int1:" << bc.intensityBeam1()
       << ";int2:" << bc.intensityBeam2() << "]";

   return out;
}

/**
 * This operator is used to print the configuration of a BunchCrossing object
 * in a nice way.
 *
 * @param out A functional MsgStream object
 * @param bc The BunchCrossing object that we want to display
 * @returns The same stream object to be able to chain output statements together
 */
MsgStream& operator<< ( MsgStream& out, const Trig::BunchCrossing& bc ) {

   out << "[id:" << bc.bcid() << ";int1:" << bc.intensityBeam1()
       << ";int2:" << bc.intensityBeam2() << "]";

   return out;
}
