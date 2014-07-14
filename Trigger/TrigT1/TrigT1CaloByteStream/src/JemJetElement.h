/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEMJETELEMENT_H
#define TRIGT1CALOBYTESTREAM_JEMJETELEMENT_H

#include <stdint.h>

namespace LVL1BS {

/** JEM jet element dataword class.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version 1.06d
 *
 *  @author Peter Faulkner
 */

class JemJetElement {

 public:
   JemJetElement(uint32_t word);
   JemJetElement(int chan, int emDat, int hadDat,
                 int emParErr, int hadParErr, int linkErr);

   //  Return jet element data
   int      channel()   const;
   int      emData()    const;
   int      hadData()   const;
   int      emParity()  const;
   int      hadParity() const;
   int      linkError() const;
   int      pair()      const;
   int      pin()       const;
   int      wordId()    const;
   uint32_t data()      const;

 private:
   //  Jet Element data word bit positions and masks
   static const int      s_emDataBit     = 0;
   static const int      s_emParityBit   = 9;
   static const int      s_hadDataBit    = 10;
   static const int      s_hadParityBit  = 19;
   static const int      s_linkErrorBit  = 20;
   static const int      s_pairBit       = 23;
   static const int      s_pinBit        = 25;
   static const int      s_wordIdBit     = 30;
   static const int      s_jeWordId      = 0x1;
   static const int      s_pairsPerPin   = 3;
   static const int      s_pairOffset    = 1;
   static const uint32_t s_emDataMask    = 0x1ff;
   static const uint32_t s_emParityMask  = 0x1;
   static const uint32_t s_hadDataMask   = 0x1ff;
   static const uint32_t s_hadParityMask = 0x1;
   static const uint32_t s_linkErrorMask = 0x3;
   static const uint32_t s_pairMask      = 0x3;
   static const uint32_t s_pinMask       = 0x1f;
   static const uint32_t s_wordIdMask    = 0x3;

   /// Jet element data
   uint32_t m_data;

};

inline int JemJetElement::channel() const
{
  return s_pairsPerPin * pin() + pair() - s_pairOffset;
}

inline int JemJetElement::emData() const
{
  return (m_data >> s_emDataBit) & s_emDataMask;
}

inline int JemJetElement::hadData() const
{
  return (m_data >> s_hadDataBit) & s_hadDataMask;
}

inline int JemJetElement::emParity() const
{
  return (m_data >> s_emParityBit) & s_emParityMask;
}

inline int JemJetElement::hadParity() const
{
  return (m_data >> s_hadParityBit) & s_hadParityMask;
}

inline int JemJetElement::linkError() const
{
  return (m_data >> s_linkErrorBit) & s_linkErrorMask;
}

inline int JemJetElement::pair() const
{
  return (m_data >> s_pairBit) & s_pairMask;
}

inline int JemJetElement::pin() const
{
  return (m_data >> s_pinBit) & s_pinMask;
}

inline int JemJetElement::wordId() const
{
  return (m_data >> s_wordIdBit) & s_wordIdMask;
}

inline uint32_t JemJetElement::data() const
{
  return m_data;
}

} // end namespace

#endif
