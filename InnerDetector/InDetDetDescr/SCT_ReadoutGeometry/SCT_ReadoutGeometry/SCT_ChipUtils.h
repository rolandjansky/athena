/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/* Dear emacs, this is -*-c++-*- */
#ifndef SCT_CHIPUTILS_H
#define SCT_CHIPUTILS_H

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include <stdexcept>

// Utilities to help with SCT chip IDs
namespace SCT {
   constexpr unsigned int N_CHIPS_PER_SIDE = 6;
   constexpr unsigned int N_SIDES = 2;
   constexpr unsigned int N_STRIPS_PER_CHIP = 128;
   constexpr unsigned int N_STRIPS_PER_SIDE = N_STRIPS_PER_CHIP*N_CHIPS_PER_SIDE;

   /** Get the physical chip ID for the given strip.
    * @param side the module side as returned by SCT_ID::side
    * @param swap true if the phi readout direction is swapped (returned by InDetDD::SiDetectorElement::swapPhiReadoutDirection )
    * @param strip the strip id as returned by SCT_ID::strip
    * @return the physical chip ID which is in the range 0..11
    */
   inline constexpr unsigned int getChip(unsigned int side, bool swap, unsigned int strip) {
      // Get strip number
      assert( strip>=0 && strip <=N_STRIPS_PER_SIDE);

      // Conversion from strip to chip (specific for present SCT)
      unsigned int chip{static_cast<unsigned int>(strip)/N_STRIPS_PER_CHIP}; // One ABCD chip reads 128 strips
      // Relation between chip and offline strip is determined by the swapPhiReadoutDirection method.
      // If swap is false
      //  offline strip:   0            767
      //  chip on side 0:  0  1  2  3  4  5
      //  chip on side 1: 11 10  9  8  7  6
      // If swap is true
      //  offline strip:   0            767
      //  chip on side 0:  5  4  3  2  1  0
      //  chip on side 1:  6  7  8  9 10 11
      if (side==0) {
         chip = swap ? (N_CHIPS_PER_SIDE        -1) - chip :                    chip;
      } else {
         chip = swap ? (N_CHIPS_PER_SIDE*N_SIDES-1) - chip : N_CHIPS_PER_SIDE + chip;
      }
      return chip;
   }

   /** Get the physical chip ID for the given strip.
    * @param sct_id the SCT ID helper
    * @param siElement the SiDetectorElement of the module which contains the given strip
    * @param the strip_id (must be on the given SiDetectorElement)
    * @return the physical chip ID which is in the range 0..11
    */
   inline unsigned int getChip(const SCT_ID &sct_id, const InDetDD::SiDetectorElement &siElement, const Identifier& stripId) {
      // Get strip number
      const int strip{sct_id.strip(stripId)};
      if (strip<0 or static_cast<unsigned int>(strip)>=N_STRIPS_PER_SIDE) {
         throw std::range_error("Invalid SCT strip");
      }

      return getChip(static_cast<unsigned int>(sct_id.side(stripId)),
                     siElement.swapPhiReadoutDirection(),
                     strip);
   }
}
#endif
