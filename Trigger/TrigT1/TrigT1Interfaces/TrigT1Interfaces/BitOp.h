// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_BITOP_H
#define TRIGT1INTERFACES_BITOP_H

// STL includes:
#include <string>

/**
 *  @short Utility class for integer bit operations
 *
 *         This is a utility class for integer bit operations.
 *         Operations that set or unset bits are done using 
 *         pointers and are inline, as they need to be fast.
 *         The print methods are not time-critical.
 *
 * @author Thorsten Wengler
 *
 * $Revision: 187728 $
 * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
 */
class BitOp {

public:
   /**
    * Utitlity function to print out the binary
    * representation of an input int or unsigned int 
    * value. Does not need to be fast.
    */
   static void printBin( unsigned int uintValue ) { printBinN( uintValue, 31 ); }
   static void printBin( int intValue ) { printBinN( intValue, 31 ); }
   static void printBinN( unsigned int uintValue, int nbits );
   static void printBinN( int intValue, int nbits );

   //! print selected bit range into string
   static const std::string printBits( const int value, const int startbit,
                                       const int endbit );

   /**
    * Check if a the given bit in the given unsigned int or
    * int value is set. True if yes. Uses pointer to the obejct
    * and is inline. Does not change the initial value given to
    * the function.
    */
   inline static bool isSet( const unsigned int *uintValue, int bit ) {
      return ( *uintValue == ( *uintValue | ( 1 << bit ) ) );
   }

   inline static bool isSet( const int *intValue, int bit ) {
      return ( *intValue == ( *intValue | ( 1 << bit ) ) );
   }

   /**
    * Set the given bit in the given unsigned int
    * or int value. Uses pointer to the object and is inline.
    */
   inline static void setBit( unsigned int *uintValue, int bit ) {
      *uintValue |= ( 1 << bit );
   }
   inline static void setBit( int *intValue, int bit ) {
      *intValue |= ( 1 << bit );
   }

   /**
    * Clear the given bit in the given integer. Uses pointer
    * to the object and is inline.
    */
   inline static void clearBit( unsigned int *uintValue, int bit ) {
      *uintValue |= ( 1 << bit ); *uintValue ^= ( 1 << bit );
   }
   inline static void clearBit( int *intValue, int bit ) {
      *intValue |= ( 1 << bit ); *intValue ^= ( 1 << bit );
   }

   /**
    * Superimpose the given integer wrd starting at bit stbit
    * onto integer or unsigned interger value.
    * Uses pointer to the object and is inline.
    * !! Bits already set in value are not reset !!
    */
   inline static void sImposeNBits( unsigned int *uintValue, int stbit, int wrd ) {
      *uintValue |= ( wrd << stbit );
   }
   inline static void sImposeNBits( int *intValue, int stbit, int wrd ) {
      *intValue |= ( wrd << stbit );
   }
   inline static void sImposeNBits( unsigned int *uintValue, int stbit,
                                    unsigned int wrd ) {
      *uintValue |= ( wrd << stbit );
   }
   inline static void sImposeNBits( int *intValue, int stbit, unsigned int wrd ) {
      *intValue |= ( wrd << stbit );
   }

   //! align given bits using start and end position into 32 bits
   static unsigned int alignBits( int value, int startbit, int endbit );
   //! create a 32 bit long mask with 1s from given start to end position
   static unsigned int createMask( int startbit, int endbit );

   /**
    * get the value in the input word represented by a bit pattern
    * given as a bitmask
    */
   static unsigned int getValue( const unsigned int * uintValue,
                                 const unsigned int mask );

}; // class BitOp

#endif // TRIGT1INTERFACES_BITOP_H
