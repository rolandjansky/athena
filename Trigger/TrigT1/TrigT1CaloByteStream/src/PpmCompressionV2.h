/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMCOMPRESSION_H
#define TRIGT1CALOBYTESTREAM_PPMCOMPRESSION_H

namespace LVL1BS {

class PpmSubBlockV2;

/** PPM Compressed Format Version 1.04 packing and unpacking utilities.
 *
 *  Based on:
 *
 *  "ATLAS L1Calo Pre-processor compressed S-Link data formats",
 *   Version 1.7, D.P.C.Sankey.
 *
 *  @author Peter Faulkner
 */

class PpmCompressionV2 {

 public:
   PpmCompressionV2();
   ~PpmCompressionV2();

   /// Pack data
   static bool pack(PpmSubBlockV2& subBlock);
   /// Unpack data
   static bool unpack(PpmSubBlockV2& subBlock);

 private:
   static const int s_formatsV0    = 6;
   static const int s_lowerRange   = 12;
   static const int s_upperRange   = 3;
   static const int s_formats      = 7;
   static const int s_fadcRange    = 15;
   static const int s_peakOnly     = 4;
   static const int s_lutDataBits  = 8;
   static const int s_lutBcidBits  = 3;
   static const int s_fadcDataBits = 10;
   static const int s_glinkPins    = 16;
   static const int s_statusBits   = 5;
   static const int s_errorBits    = 6;
   static const int s_statusMask   = 0x1f;

   static bool unpackV100(PpmSubBlockV2& subBlock);
   static bool unpackV101(PpmSubBlockV2& subBlock);
   static bool unpackV104(PpmSubBlockV2& subBlock);

};

} // end namespace

#endif
