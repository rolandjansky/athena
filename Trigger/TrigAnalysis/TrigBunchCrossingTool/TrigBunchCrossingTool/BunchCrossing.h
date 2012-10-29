// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchCrossing.h 520130 2012-10-04 07:14:13Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_BUNCHCROSSING_H
#define TRIGBUNCHCROSSINGTOOL_BUNCHCROSSING_H

namespace Trig {

   // Forward declaration(s):
   class MsgLogger;

   /**
    *  @short A smart integer class representing bunch crossings
    *
    *         There can be a maximum of 3564 bunches in the LHC. And BCID
    *         #0 and #3563 are just beside each other. To have the STL
    *         algorithms take this into account, objects of this class
    *         behave as "smart" integer variables, which respect the properties
    *         of the LHC.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 520130 $
    * $Date: 2012-10-04 09:14:13 +0200 (Thu, 04 Oct 2012) $
    */
   class BunchCrossing {

   public:
      /// Constructor with a value
      BunchCrossing( int bcid = 0, float intBeam1 = 1.0, float intBeam2 = 1.0 );
      /// Constructor with an unsigned value
      BunchCrossing( unsigned int bcid, float intBeam1 = 1.0,
                     float intBeam2 = 1.0 );
      /// Copy constructor
      BunchCrossing( const BunchCrossing& parent );

      /// Assignment operator
      BunchCrossing& operator=( const BunchCrossing& parent );

      /// Minimum spacing between the bunches, in nanoseconds
      static const int BUNCH_SPACING;
      /// The maximum number of bunches that can be in the LHC
      static const int MAX_BCID;

      /// Automatic conversion operator
      operator int&();
      /// Automatic conversion operator (constant version)
      operator const int&() const;

      /// Operator adding another BunchCrossing object
      BunchCrossing& operator+= ( const BunchCrossing& bc );
      /// Operator subtracting another BunchCrossing object
      BunchCrossing& operator-= ( const BunchCrossing& bc );

      /// Operator pushing the object to the next bunch crossing
      BunchCrossing& operator++ ();
      /// Operator pushing the object to the next bunch crossing
      BunchCrossing  operator++ ( int );

      /// Operator pushing the object to the previous bunch crossing
      BunchCrossing& operator-- ();
      /// Operator pushing the object to the previous bunch crossing
      BunchCrossing  operator-- ( int );

      /// The distance from another bunch crossing
      int distance( const BunchCrossing& bc ) const;
      /// Distance from a previous bunch crossing
      int gapFrom( const BunchCrossing& bc ) const;
      /// Distance to a following bunch crossing
      int gapTo( const BunchCrossing& bc ) const;

      /// Get the BCID of this bunch crossing
      int bcid() const;
      /// Set the BCID of this bunch crossing
      void setBCID( int bcid );

      /// Get the "intensity" of beam 1 in this bunch crossing
      float intensityBeam1() const;
      /// Set the "intensity" of beam 1 in this bunch crossing
      void setIntensityBeam1( float intensity );

      /// Get the "intensity" of beam 2 in this bunch crossing
      float intensityBeam2() const;
      /// Set the "intensity" of beam 2 in this bunch crossing
      void setIntensityBeam2( float intensity );

      /// Equality operator for bunch crossings
      bool operator== ( const BunchCrossing& bc ) const;

   private:
      int m_bcid; ///< The BCID of this bunch crossing
      float m_intensityBeam1; ///< Intensity of the bunch in "beam 1" some measure
      float m_intensityBeam2; ///< Intensity of the bunch in "beam 2" some measure

   }; // class BunchCrossing

   /// Operator summing two BunchCrossing objects
   BunchCrossing operator+ ( const BunchCrossing& bc1, const BunchCrossing& bc2 );
   /// Operator subtracting two BunchCrossing objects
   BunchCrossing operator- ( const BunchCrossing& bc1, const BunchCrossing& bc2 );

   /// Function calculating the distance of two bunch crossings
   int distance( const BunchCrossing bc1, const BunchCrossing bc2 );

   /// Output operator for printing BunchCrossing objects
   MsgLogger& operator<< ( MsgLogger& out, const BunchCrossing& bc );

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_BUNCHCROSSING_H
