// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BitOp.h 796872 2017-02-13 15:03:25Z fwinkl $
#ifndef TRIGT1MUCTPI_BITOP_H
#define TRIGT1MUCTPI_BITOP_H

#include <string>
#include "CxxUtils/bitscan.h"

namespace LVL1MUCTPI {

  /**
   *******************************************************************
   *
   *    $Date: 2017-02-13 16:03:25 +0100 (Mon, 13 Feb 2017) $
   *
   *    @short Utility class for integer bit operations
   *
   *           This is a utility class for integer bit operations
   *           Operations that set or unset bits are done using
   *           Pointers and are inline, as they need to be fast.
   *           The print methods are not time-critical.
   *
   *      @see
   *   @author  Author: Thorsten Wengler
   *  @version $Revision: 796872 $
   *
   *******************************************************************
   */
  class BitOp {

  public:
    BitOp();
    ~BitOp();

    /**
     * Utitlity function to print out the binary
     * representation of an input int or unsigned int
     * value. Does not need to be fast.
     */
    static void printBin( unsigned int uintValue ) { printBinN( uintValue, 31 ); }
    static void printBin( int intValue ) { printBinN( intValue, 31 ); }
    static void printBin( unsigned int uintValue, std::string& destination ) { printBinN( uintValue, 31, destination ); }
    static void printBin( int intValue, std::string& destination ) { printBinN( intValue, 31, destination ); }
    static void printBinN( unsigned int uintValue, int nbits );
    static void printBinN( int intValue, int nbits );
    static void printBinN( unsigned int uintValue, int nbits, std::string& destination );
    static void printBinN( int intValue, int nbits, std::string& destination );

    /**
     * Check if a the given bit in the given unsigned int or
     * int value is set. True if yes. Uses pointer to the obejct
     * and is inline. Does not change the initial value given to
     * the function.
     */
    static inline bool isSet(const unsigned int *uintValue, int bit)
      { return ( *uintValue == ( *uintValue | ( 1 << bit ) ) ); }

    static inline bool isSet(const int *intValue, int bit)
      { return ( *intValue == ( *intValue | ( 1 << bit ) ) ); }

    /**
     * Set the given bit in the given unsigned int
     * or int value. Uses pointer to the object and is inline.
     */
    static inline void setBit(unsigned int *uintValue, int bit)
      { *uintValue |= ( 1 << bit ); }

    static inline void setBit(int *intValue, int bit)
      { *intValue |= ( 1 << bit ); }

    /**
     * Clear the given bit in the given integer. Uses pointer
     * to the object and is inline.
     */
    static inline void clearBit(unsigned int *uintValue, int bit)
      { *uintValue |= ( 1 << bit ); *uintValue ^=  ( 1 << bit ); }

    static inline void clearBit(int *intValue, int bit)
      { *intValue |= ( 1 << bit ); *intValue ^=  ( 1 << bit ); }

    /**
     * Superimpose the given integer wrd starting at bit stbit
     * onto integer or unsigned interger value.
     * Uses pointer to the object and is inline.
     * !! Bits already set in value are not reset !!
     */
    static inline void sImposeNBits(unsigned int *uintValue, int stbit, int wrd)
      { *uintValue |= ( wrd << stbit ); }

    static inline void sImposeNBits(int *intValue, int stbit, int wrd)
      { *intValue |= ( wrd << stbit ); }

    static inline void sImposeNBits(unsigned int *uintValue, int stbit, unsigned int wrd)
      { *uintValue |= ( wrd << stbit ); }

    static inline void sImposeNBits(int *intValue, int stbit, unsigned int wrd)
      { *intValue |= ( wrd << stbit ); }

    /**
     * get the value in the input word represented by a bit pattern
     * given as a bitmask
     */
    static inline unsigned int getValue(const unsigned int* uintValue, const unsigned int mask)
    {
      return (*uintValue & mask) >> CxxUtils::count_trailing_zeros(mask);
    }

  }; // class BitOp

} // namespace LVL1MUCTPI

#endif  // TRIGT1MUCTPI_BITOP_H
