/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ZDCSUBBLOCK_H
#define ZDCSUBBLOCK_H

#include <stdint.h>
#include <vector>

#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "ZdcSrcIdMap.h"


/**Zdc Sub Block base class
 *
 *
 *
 *  @author Peter Faulkner
 *
 *  Derived (copied !) from L1Calo Sub-Block base class.
 *
 *  Don't think we need this, as there is only one type of subblock (only PPM)
 *  being used by ZDC. Implement at once inside ZdcPpmSubBlock
 *
 *  Marco Leite
 *
 *  TRY_LINK_LVL1
 *
 */

class ZdcSubBlock {

 public:
   enum SubBlockWordType { HEADER,
						    DATA,
						    STATUS };

   enum DataFormats      { NEUTRAL         = 0,
							UNCOMPRESSED    = 1,
							COMPRESSED      = 2,
							SUPERCOMPRESSED = 3 };

   enum UnpackErrorType  { UNPACK_NONE,
							UNPACK_VERSION,
							UNPACK_FORMAT,
                            UNPACK_COMPRESSION_VERSION,
                            UNPACK_COMPRESSION_SLICES,
                            UNPACK_DATA_TRUNCATED,
                            UNPACK_SOURCE_ID,
                            UNPACK_WORD_ID };

   ZdcSubBlock();
   ~ZdcSubBlock();

   /// Clear all data
   void clear();

   /// Return number of data words
   int dataWords()   const;

   /// Store header data
   void setHeader(int wordId, int version, int format, int seqno, int crate,
                  int module, int slices2, int slices1);

   //  Return unpacked header data
   int wordId()      const;
   int version()     const;
   int format()      const;
   int seqno()       const;
   int slice()       const;
   int crate()       const;
   int module()      const;
   int slices2()     const;
   int slices1()     const;

   //  Return unpacked error status data
   uint32_t failingBCN()     const;
   bool     glinkTimeout()   const;
   bool     glinkDown()      const;
   bool     upstreamError()  const;
   bool     daqOverflow()    const;
   bool     bcnMismatch()    const;
   bool     glinkProtocol()  const;
   bool     glinkParity()    const;

   /// Return Sub-status word
   uint32_t subStatus()      const;

   /// Set the Bunch Crossing number (neutral format only)
   void setBunchCrossing(int bc);
   /// Return the Bunch Crossing number (neutral format only)
   int  bunchCrossing()      const;

   /// Input complete packed sub-block from ROD array
   // (OFFLINE_FRAGMENTS_NAMESPACE::PointerType = uint32_t*)
   OFFLINE_FRAGMENTS_NAMESPACE::PointerType read(
                      const OFFLINE_FRAGMENTS_NAMESPACE::PointerType beg,
	              const OFFLINE_FRAGMENTS_NAMESPACE::PointerType end);

   /// Output complete packed sub-block to ROD vector
   // (FullEventAssembler<L1CaloSrcIdMap>::RODDATA = vector<uint32_t>)
   void write(FullEventAssembler<ZdcSrcIdMap>::RODDATA* theROD) const;

   /// Store error status trailer
   void setStatus(uint32_t failingBCN, bool glinkTimeout, bool glinkDown,
                  bool upstreamError, bool daqOverflow, bool bcnMismatch,
		  bool glinkProtocol, bool glinkParity);

   /// Set DAQ FIFO Overflow bit in Sub-status word
   void setDaqOverflow(int bit = 1);
   /// Set G-Link Parity bit in Sub-status word
   void setGlinkParity(int bit = 1);

   /// Word identification
   static SubBlockWordType wordType(uint32_t word);
   /// Return wordID field from given header word
   static int wordId(uint32_t word);
   /// Return data format from given header word
   static int format(uint32_t word);
   /// Return seqno field from given header word
   static int seqno(uint32_t word);
   /// Return module field from given header word
   static int module(uint32_t word);

   //  Unpacking error code.  Set by derived classes
   /// Set the unpacking error code
   void     setUnpackErrorCode(int code);
   /// Return the unpacking error code
   int      unpackErrorCode() const;
   /// Return the unpacking error message for printing
   std::string unpackErrorMsg() const;

   //  Packing utilities
   /// Return the minimum number of bits needed for given data
   int      minBits(uint32_t datum) const;
   /// Return the parity bit for given data
   int      parityBit(int init, uint32_t datum, int nbits) const;
   /// Pack given data into given number of bits
   void     packer(uint32_t datum, int nbits);
   /// Flush the current data word padded with zeros
   void     packerFlush();
   /// Set continuous bit streaming for compressed formats
   void     setStreamed();
   /// Unpack given number of bits of data
   uint32_t unpacker(int nbits);
   /// Initialise unpacker
   void     unpackerInit();
   /// Return unpacker success flag
   bool     unpackerSuccess() const;

   //  Neutral format packing utilities
   /// Pack given neutral data from given pin
   void     packerNeutral(int pin, uint32_t datum, int nbits);
   /// Pack current G-Link parity bit for given pin
   void     packerNeutralParity(int pin);
   /// Unpack given number of bits of neutral data for given pin
   uint32_t unpackerNeutral(int pin, int nbits);
   /// Unpack and test G-Link parity bit for given pin
   bool     unpackerNeutralParityError(int pin);

 private:
   //  Constants.
   //  Header and status ID
   static const int      s_headerBit        = 30;
   static const int      s_statusBit        = 28;
   static const uint32_t s_headerMask       = 0x3;
   static const uint32_t s_statusMask       = 0x1;
   static const uint32_t s_headerVal        = 0x3;
   static const uint32_t s_statusVal        = 0x1;
   //  Header word data positions and masks
   static const int      s_wordIdBit        = 28;
   static const int      s_versionBit       = 25;
   static const int      s_formatBit        = 22;
   static const int      s_seqnoBit         = 16;
   static const int      s_crateBit         = 12;
   static const int      s_moduleBit        = 8;
   static const int      s_slices2Bit       = 3;
   static const int      s_slices1Bit       = 0;
   static const uint32_t s_wordIdMask       = 0xf;
   static const uint32_t s_versionMask      = 0x7;
   static const uint32_t s_formatMask       = 0x7;
   static const uint32_t s_seqnoMask        = 0x3f;
   static const uint32_t s_crateMask        = 0xf;
   static const uint32_t s_moduleMask       = 0xf;
   static const uint32_t s_slices2Mask      = 0x1f;
   static const uint32_t s_slices1Mask      = 0x7;
   //  Status word data positions and masks
   static const int      s_failingBcnBit    = 22;
   static const int      s_glinkTimeoutBit  = 7;
   static const int      s_glinkDownBit     = 6;
   static const int      s_upstreamErrorBit = 4;
   static const int      s_daqOverflowBit   = 3;
   static const int      s_bcnMismatchBit   = 2;
   static const int      s_glinkProtocolBit = 1;
   static const int      s_glinkParityBit   = 0;
   static const uint32_t s_failingBcnMask   = 0x3f;
   //  Packing word sizes and masks
   static const int      s_maxWordBits      = 32;
   static const int      s_maxStreamedBits  = 31;
   static const uint32_t s_maxWordMask      = 0xffffffff;
   static const uint32_t s_maxStreamedMask  = 0x7fffffff;
   //  Neutral packing
   static const int      s_maxPins          = 20;
   static const uint32_t s_glinkDavSet      = 0x400000;

   /// Sub-Block Header
   uint32_t m_header;
   /// Sub-Block Status Trailer
   uint32_t m_trailer;
   /// Bunch Crossing number (neutral format only)
   int m_bunchCrossing;
   /// Unpacking error code
   int m_unpackError;
   //  Used for bit-packing
   uint32_t m_bitword;
   int      m_currentBit;
   int      m_maxBits;
   uint32_t m_maxMask;
   bool     m_unpackerFlag;
   std::vector<uint32_t>::const_iterator m_dataPos;
   std::vector<uint32_t>::const_iterator m_dataPosEnd;
   //  Used for neutral bit packing
   std::vector<int> m_currentPinBit;
   std::vector<int> m_oddParity;
   /// Current number of data words
   int      m_dataWords;
   /// Sub-Block data
   std::vector<uint32_t> m_data;

};

inline int ZdcSubBlock::dataWords() const
{
  return m_dataWords;
}

inline int ZdcSubBlock::wordId() const
{
  return (m_header >> s_wordIdBit) & s_wordIdMask;
}

inline int ZdcSubBlock::version() const
{
  return (m_header >> s_versionBit) & s_versionMask;
}

inline int ZdcSubBlock::format() const
{
  return (m_header >> s_formatBit) & s_formatMask;
}

inline int ZdcSubBlock::seqno() const
{
  return (m_header >> s_seqnoBit) & s_seqnoMask;
}

inline int ZdcSubBlock::slice() const
{
  return seqno();
}

inline int ZdcSubBlock::crate() const
{
  return (m_header >> s_crateBit) & s_crateMask;
}

inline int ZdcSubBlock::module() const
{
  return (m_header >> s_moduleBit) & s_moduleMask;
}

inline int ZdcSubBlock::slices2() const
{
  return (m_header >> s_slices2Bit) & s_slices2Mask;
}

inline int ZdcSubBlock::slices1() const
{
  return (m_header >> s_slices1Bit) & s_slices1Mask;
}

inline uint32_t ZdcSubBlock::failingBCN() const
{
  return (m_trailer >> s_failingBcnBit) & s_failingBcnMask;
}

inline bool ZdcSubBlock::glinkTimeout() const
{
  return m_trailer & (0x1 << s_glinkTimeoutBit);
}

inline bool ZdcSubBlock::glinkDown() const
{
  return m_trailer & (0x1 << s_glinkDownBit);
}

inline bool ZdcSubBlock::upstreamError() const
{
  return m_trailer & (0x1 << s_upstreamErrorBit);
}

inline bool ZdcSubBlock::daqOverflow() const
{
  return m_trailer & (0x1 << s_daqOverflowBit);
}

inline bool ZdcSubBlock::bcnMismatch() const
{
  return m_trailer & (0x1 << s_bcnMismatchBit);
}

inline bool ZdcSubBlock::glinkProtocol() const
{
  return m_trailer & (0x1 << s_glinkProtocolBit);
}

inline bool ZdcSubBlock::glinkParity() const
{
  return m_trailer & (0x1 << s_glinkParityBit);
}

inline uint32_t ZdcSubBlock::subStatus() const
{
  return m_trailer;
}

inline void ZdcSubBlock::setBunchCrossing(const int bc)
{
  if (bc) m_bunchCrossing = bc;
}

inline int ZdcSubBlock::bunchCrossing() const
{
  return m_bunchCrossing;
}

inline void ZdcSubBlock::setUnpackErrorCode(const int code)
{
  m_unpackError = code;
}

inline int ZdcSubBlock::unpackErrorCode() const
{
  return m_unpackError;
}

inline void ZdcSubBlock::setStreamed()
{
  m_maxBits = s_maxStreamedBits;
  m_maxMask = s_maxStreamedMask;
}

inline bool ZdcSubBlock::unpackerSuccess() const
{
  return m_unpackerFlag;
}

#endif
