/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEMROISUBBLOCK_H
#define TRIGT1CALOBYTESTREAM_JEMROISUBBLOCK_H

#include <vector>

#include "L1CaloSubBlock.h"

namespace LVL1 {
  class JEMRoI;
}

namespace LVL1BS {

/** Sub-Block class for JEM RoI data (neutral format).
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version 1.09h
 *
 *  @author Peter Faulkner
 */

class JemRoiSubBlock : public L1CaloSubBlock {

 public:
   JemRoiSubBlock();
   ~JemRoiSubBlock();

   /// Clear all data
   void clear();

   /// Store header
   void setRoiHeader(int version, int crate, int module);
   /// Store RoI
   void fillRoi(LVL1::JEMRoI roi);

   /// Return RoI for given frame and forward
   LVL1::JEMRoI roi(int frame, int forward) const;

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
   static const int s_hitsBits          = 8;
   static const int s_locationBits      = 2;
   static const int s_saturationBits    = 1;
   static const int s_bunchCrossingBits = 12;
   static const int s_paddingBits       = 32;

   /// Pack neutral data
   bool packNeutral();
   /// Unpack neutral data
   bool unpackNeutral();

   /// RoIs
   std::vector<LVL1::JEMRoI> m_roiData;

};

} // end namespace

#endif
