/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEMROISUBBLOCKV2_H
#define TRIGT1CALOBYTESTREAM_JEMROISUBBLOCKV2_H

#include <vector>

#include "L1CaloSubBlock.h"

namespace LVL1 {
  class JEMTobRoI;
}

namespace LVL1BS {

/** Sub-Block class for JEM RoI data (neutral format) post-LS1.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version X.xxx                                           //<< CHECK
 *
 *  @author Peter Faulkner
 */

class JemRoiSubBlockV2 : public L1CaloSubBlock {

 public:
   JemRoiSubBlockV2();
   ~JemRoiSubBlockV2();

   /// Clear all data
   void clear();

   /// Store header
   void setRoiHeader(int version, int crate, int module);
   /// Store RoI
   void fillRoi(LVL1::JEMTobRoI roi);

   /// Return RoI for given frame
   LVL1::JEMTobRoI roi(int frame) const;

   /// Pack data
   bool pack();
   /// Unpack data
   bool unpack();

 private:
   /// Header word ID
   static const int s_wordIdVal         = 0xc;
   //  G-Link/Neutral format
   static const int s_frames            = 8;
   static const int s_framesPerPin      = 4;
   static const int s_bunchCrossingPin  = 2;
   static const int s_energySmallBits   = 9;
   static const int s_energyLargeBits   = 10;
   static const int s_locationBits      = 2;
   static const int s_bunchCrossingBits = 12;
   static const int s_paddingBits       = 32;

   /// Pack neutral data
   bool packNeutral();
   /// Unpack neutral data
   bool unpackNeutral();

   /// RoIs
   std::vector<LVL1::JEMTobRoI> m_roiData;

};

} // end namespace

#endif
