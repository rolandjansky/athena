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
      assert( strip <=N_STRIPS_PER_SIDE);

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

   /** Get the geometrical chip ID for the given strip.
    * @param the strip_id.
    * @return the geometrical chip ID which is in the range 0..5 (for both sides and numbering is not swapped if the phi readout detection is swapped)
    */
   inline constexpr unsigned int getGeometricalChipID(unsigned int strip) {
      assert( strip < N_CHIPS_PER_SIDE * N_STRIPS_PER_CHIP );
      return strip/N_STRIPS_PER_CHIP;
   }

   /** Get the physical chip ID for the given strip.
    * @param sct_id the SCT ID helper
    * @param the strip_id.
    * @return the geometrical chip ID which is in the range 0..5 (for both sides and numbering is not swapped if the phi readout detection is swapped)
    */
   inline unsigned int getGeometricalChipID(const SCT_ID &sct_id, const Identifier& stripId) {
      return getGeometricalChipID(static_cast<unsigned int>(sct_id.strip(stripId)));
   }

   /** Get the geometrical chip ID from a physica chip ID.
    * @param side the module side as returned by SCT_ID::side
    * @param swap true if the phi readout direction is swapped (returned by InDetDD::SiDetectorElement::swapPhiReadoutDirection )
    * @param physical_chip_id which must be in the range 0..5 for side 0 or 6..11 for side 1
    * @return the geometrical chip ID which is in the range 0..5 (for both sides and numbering is not swapped if the phi readout detection is swapped)
    */
   inline constexpr unsigned int getGeometricalFromPhysicalChipID(unsigned int side, bool swap, unsigned int physical_chip_id) {
      // side 0      :  0 ->  0   5 ->  5
      // side 0 swap :  0 ->  5   5 ->  0
      // side 1      :  6 ->  0  11 ->  5
      // side 1 swap :  6 ->  5  11 ->  0
      assert( (side==0 && physical_chip_id<6) || (side ==1 && physical_chip_id<12));
      return side==0
         ? ( swap ?   N_CHIPS_PER_SIDE - 1 - physical_chip_id : physical_chip_id)
         : ( swap ? 2*N_CHIPS_PER_SIDE - 1 - physical_chip_id : physical_chip_id - N_CHIPS_PER_SIDE);
   }

   /** Get the geometrical chip ID from a physical chip ID.
    * @param sct_id the SCT ID helper
    * @param siElement the SiDetectorElement of the module which contains the given chip
    * @param physical_chip_id which must be in the range 0..5 for side 0 or 6..11 for side 1
    * @return the geometrical chip ID which is in the range 0..5 (for both sides and numbering is not swapped if the phi readout detection is swapped)
    */
   inline unsigned int getGeometricalFromPhysicalChipID(const SCT_ID &sct_id, const InDetDD::SiDetectorElement &siElement, unsigned int physical_chip_id) {
      return getGeometricalFromPhysicalChipID(static_cast<unsigned int>(sct_id.side(siElement.identify())),
                                              siElement.swapPhiReadoutDirection(),
                                              physical_chip_id);
   }

   /** Get the physical chip ID from a geometrical chip ID.
    * @param side the module side as returned by SCT_ID::side
    * @param swap true if the phi readout direction is swapped (returned by InDetDD::SiDetectorElement::swapPhiReadoutDirection )
    * @param geometrical_chip_id the geometrical chip id which must be in the range 0..5
    * @return physical chip ID which is in the range 0..5 for side0 and 6..11 for side 1  and the numbering scheme is swapped if the phi readout direction is swapped.
    */
   inline constexpr unsigned int getPhysicalFromGeometricalChipID(unsigned int side, bool swap, unsigned int geometrical_chip_id) {
      // side 0      :  0 ->  0   5 ->  5
      // side 0 swap :  0 ->  5   5 ->  0
      // side 1      :  0 ->  6   5 -> 11
      // side 1 swap :  0 -> 11   5 ->  6
      assert( geometrical_chip_id<6 && side<2);
      return side==0
         ? ( swap ?   N_CHIPS_PER_SIDE - 1 - geometrical_chip_id : geometrical_chip_id)
         : ( swap ? 2*N_CHIPS_PER_SIDE - 1 - geometrical_chip_id : geometrical_chip_id + N_CHIPS_PER_SIDE);
   }

   /** Get the physical chip ID from a geometrical chip ID.
    * @param sct_id the SCT ID helper
    * @param siElement the SiDetectorElement of the module which contains the given chip
    * @param geometrical_chip_id the geometrical chip id which must be in the range 0..5
    * @return physical chip ID which is in the range 0..5 for side0 and 6..11 for side 1  and the numbering scheme is swapped if the phi readout direction is swapped.
    */
   inline unsigned int getPhysicalFromGeometricalChipID(const SCT_ID &sct_id, const InDetDD::SiDetectorElement &siElement, unsigned int geometrical_chip_id) {
      return getPhysicalFromGeometricalChipID(static_cast<unsigned int>(sct_id.side(siElement.identify())),
                                              siElement.swapPhiReadoutDirection(),
                                              geometrical_chip_id);
   }

   /** Convert a word in which each bit represents the status of a certain physical chip to a word in which each bit represents a geometrical chip
    * @param side the module side as returned by SCT_ID::side
    * @param swap true if the phi readout direction is swapped (returned by InDetDD::SiDetectorElement::swapPhiReadoutDirection )
    * @param physical_chip_flags a word in which bits 0..5 or 6..11 represent the status of a certain chip on the given module where each bit corresponds to the physical chip ID
    * @return  a word in which bits 0..5 represent the status of a certain chip on the given module where each bit corresponds to the geometrical chip ID
    */
   inline constexpr unsigned int getGeometricalFromPhysicalChipFlags(unsigned int side, bool swap, unsigned int physical_chip_flags) {
      assert( side<2 );
      // side 0  0..5  -> 0..5 or 5..0
      // side 1  6..11 -> 0..5 or 5..0
      unsigned int chip_flags = ( side==0 ? physical_chip_flags : physical_chip_flags>>N_CHIPS_PER_SIDE);
      if (swap) {
         unsigned int chip_flags_out=0;
         // bit 0 -> 5
         // bit 5 -> 0
         for (unsigned int chip_i=SCT::N_CHIPS_PER_SIDE; chip_i-->0;) {
            chip_flags_out <<=1;
            chip_flags_out  |= (chip_flags & 1);
            chip_flags >>=1;
         }
         chip_flags = chip_flags_out;
      }
      return chip_flags;
   }

   /** Convert a word in which each bit represents the status of a certain physical chip to a word in which each bit represents a geometrical chip
    * @param sct_id the SCT ID helper
    * @param siElement the SiDetectorElement of the module of the chips represented by the given word
    * @param physical_chip_flags a word in which bits 0..5 or 6..11 represent the status of a certain chip on the given module where each bit corresponds to the physical chip ID
    * @return a word in which bits 0..5 represent the status of a certain chip on the given module where each bit corresponds to the geometrical chip ID
    */
   inline unsigned int getGeometricalFromPhysicalChipFlags(const SCT_ID &sct_id, const InDetDD::SiDetectorElement &siElement, unsigned int physical_chip_flags) {
      return getGeometricalFromPhysicalChipFlags(static_cast<unsigned int>(sct_id.side(siElement.identify())),
                                                 siElement.swapPhiReadoutDirection(),
                                                 physical_chip_flags);
   }
}
#endif
