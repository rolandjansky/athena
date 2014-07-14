/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMSUBBLOCKV2_H
#define TRIGT1CALOBYTESTREAM_CPMSUBBLOCKV2_H

#include <stdint.h>
#include <vector>

#include "L1CaloSubBlock.h"

namespace LVL1BS {

/** Sub-Block class for CPM data post LS1.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version X.xxx                                          <<== CHECK
 *
 *  @author Peter Faulkner
 */

class CpmSubBlockV2 : public L1CaloSubBlock {

 public:
   CpmSubBlockV2();
   ~CpmSubBlockV2();

   /// Clear all data
   void clear();

   /// Store CPM header
   void setCpmHeader(int version, int format, int slice, int crate,
                     int module, int timeslices);
   /// Store trigger tower data
   void fillTowerData(int slice, int channel, int em, int had,
	                                      int emErr, int hadErr);

   /// Return Em data for given channel
   int  emData(int slice, int channel)   const;
   /// Return Had data for given channel
   int  hadData(int slice, int channel)  const;
   /// Return Em error for given channel
   int  emError(int slice, int channel)  const;
   /// Return Had error for given channel
   int  hadError(int slice, int channel) const;
   /// Return number of timeslices
   int  timeslices()                     const;
   /// Return true if there is tower data for given channel
   bool anyTowerData(int channel)        const;

   /// Pack data
   bool pack();
   /// Unpack data
   bool unpack();

 private:
   /// CPM header word ID
   static const int      s_wordIdVal       = 0xc;
   /// Data word length
   static const int      s_wordLength      = 32;
   //  Trigger tower data word bit positions and masks
   static const int      s_ttDataABit      = 0;
   static const int      s_ttDataBBit      = 9;
   static const int      s_parityABit      = 8;
   static const int      s_parityBBit      = 17;
   static const int      s_linkDownABit    = 19;
   static const int      s_linkDownBBit    = 20;
   static const int      s_pairBit         = 21;
   static const int      s_fpgaBit         = 23;
   static const int      s_dataIdBit       = 30;
   static const int      s_ttWordId        = 0x1;
   static const uint32_t s_ttDataMask      = 0xff;
   static const uint32_t s_pairPinMask     = 0x7f;
   static const uint32_t s_dataIdMask      = 0x3;
   //  G-Link/Neutral format
   static const int      s_pairsPerPin       = 4;
   static const int      s_wordsPerPin       = 8;
   static const int      s_ttBits            = 8;
   static const int      s_errBits           = 2;
   static const int      s_bcnBits           = 3;
   static const int      s_bcnPin            = 16;
   static const int      s_glinkPins         = 20;
   static const int      s_glinkBitsPerSlice = 84;

   /// Return data WordID
   int  dataId(uint32_t word) const;
   /// Return data for given channel and pin offset
   int  data(int slice, int channel, int offset)  const;
   /// Return error for given channel and pin offset
   int  error(int slice, int channel, int offset) const;
   /// Return data index appropriate to format
   int  index(int slice)      const;
   /// Resize a data vector according to format
   void resize(std::vector<uint32_t>& vec, int channels);

   /// Pack neutral data
   bool packNeutral();
   /// Pack uncompressed data
   bool packUncompressed();
   /// Unpack neutral data
   bool unpackNeutral();
   /// Unpack uncompressed data
   bool unpackUncompressed();

   /// Trigger tower data
   std::vector<uint32_t> m_ttData;
   /// Channel present flags vector
   std::vector<int> m_chanPresent;
   /// Number of Trigger tower channels per module
   int m_channels;

};

inline int CpmSubBlockV2::dataId(const uint32_t word) const
{
  return (word >> s_dataIdBit) & s_dataIdMask;
}

inline bool CpmSubBlockV2::anyTowerData(const int channel) const
{
  return m_chanPresent[channel];
}

} // end namespace

#endif
