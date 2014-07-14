/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMROISUBBLOCK_H
#define TRIGT1CALOBYTESTREAM_CPMROISUBBLOCK_H

#include <vector>

#include "L1CaloSubBlock.h"

namespace LVL1 {
  class CPMRoI;
}

namespace LVL1BS {

/** Sub-Block class for CPM RoI data (neutral format).
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version 1.09h
 *
 *  @author Peter Faulkner
 */

class CpmRoiSubBlock : public L1CaloSubBlock {

 public:
   CpmRoiSubBlock();
   ~CpmRoiSubBlock();

   /// Clear all data
   void clear();

   /// Store header
   void setRoiHeader(int version, int crate, int module);
   /// Store RoI
   void fillRoi(LVL1::CPMRoI roi);

   /// Return RoI for given chip and location (left/right)
   LVL1::CPMRoI roi(int chip, int loc) const;

   /// Pack data
   bool pack();
   /// Unpack data
   bool unpack();

 private:
   /// Header word ID
   static const int s_wordIdVal         = 0xc;
   //  G-Link/Neutral format
   static const int s_glinkPins         = 16;
   static const int s_hitsLen           = 16;
   static const int s_errorLen          = 2;
   static const int s_locationLen       = 2;
   static const int s_bunchCrossingBits = 12;

   /// Pack neutral data
   bool packNeutral();
   /// Unpack neutral data
   bool unpackNeutral();

   /// RoI words
   std::vector<LVL1::CPMRoI> m_roiData;

};

} // end namespace

#endif
