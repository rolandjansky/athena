/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMROISUBBLOCKV2_H
#define TRIGT1CALOBYTESTREAM_CPMROISUBBLOCKV2_H

#include <vector>

#include "L1CaloSubBlock.h"

namespace LVL1 {
  class CPMTobRoI;
}

namespace LVL1BS {

/** Sub-Block class for CPM RoI data (neutral format) post LS1.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version X.xxx                                         <<== CHECK
 *
 *  @author Peter Faulkner
 */

class CpmRoiSubBlockV2 : public L1CaloSubBlock {

 public:
   CpmRoiSubBlockV2();
   ~CpmRoiSubBlockV2();

   /// Clear all data
   void clear();

   /// Store header
   void setRoiHeader(int version, int crate, int module);
   /// Store RoI
   void fillRoi(LVL1::CPMTobRoI roi);

   /// Return RoI for given chip and location (left/right) and type (em/tau)
   LVL1::CPMTobRoI roi(int chip, int loc, int type) const;

   /// Pack data
   bool pack();
   /// Unpack data
   bool unpack();

 private:
   /// Header word ID
   static const int s_wordIdVal         = 0xc;
   //  G-Link/Neutral format
   static const int s_glinkPins         = 16;
   static const int s_energyLen         = 8;
   static const int s_isolLen           = 5;
   static const int s_locationLen       = 2;
   static const int s_bunchCrossingBits = 12;

   /// Pack neutral data
   bool packNeutral();
   /// Unpack neutral data
   bool unpackNeutral();

   /// RoI words
   std::vector<LVL1::CPMTobRoI> m_roiData;

};

} // end namespace

#endif
