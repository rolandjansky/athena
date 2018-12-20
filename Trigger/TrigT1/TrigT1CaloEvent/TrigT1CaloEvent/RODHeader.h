/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_RODHEADER_H
#define TRIGT1CALO_RODHEADER_H

#include <stdint.h>
#include <vector>

namespace LVL1 {

/** ROD Header data and status words.
 *
 *  @author Peter Faulkner
 */

class RODHeader {

 public:

   RODHeader();
   RODHeader(uint32_t version, uint32_t sourceId, uint32_t run,
             uint32_t lvl1Id,  uint32_t bcId,     uint32_t trigType, 
	     uint32_t detType, const std::vector<uint32_t>& statusWords,
	     uint32_t nData);

   ~RODHeader();
   
   // Header data
   int version()        const;
   int  majorVersion()  const;
   int  minorVersion()  const;
   int sourceID()       const;
   int  subDetectorID() const;
   int  moduleID()      const;
   int   crate()        const;
   int   sLink()        const;
   int   dataType()     const;
   int run()            const;
   int  runType()       const;
   int  runNumber()     const;
   int extendedL1ID()   const;
   int  ecrID()         const;
   int  l1ID()          const;
   int bunchCrossing()  const;
   int l1TriggerType()  const;
   int detEventType()   const;
   int  orbitCount()    const;
   int  stepNumber()    const;
   int  stepType()      const;

   // Status bits - word 1
   bool bcnMismatch()        const;
   bool gLinkTimeout()       const;
   bool dataTransportError() const;
   bool rodFifoOverflow()    const;
   bool lvdsLinkError()      const;
   bool cmmParityError()     const;
   bool gLinkError()         const;
   // Status bits - word 2
   bool limitedRoISet()      const;
   bool triggerTypeTimeout() const;

   // Status words
   const std::vector<uint32_t>& statusWords() const;

   // Payload
   int payloadSize()    const;

 private:

   uint32_t m_version;
   uint32_t m_sourceId;
   uint32_t m_run;
   uint32_t m_lvl1Id;
   uint32_t m_bcId;
   uint32_t m_trigType;
   uint32_t m_detType;
   std::vector<uint32_t> m_statusWords;
   uint32_t m_payloadSize;

};

inline int RODHeader::version() const
{
  return m_version;
}

inline int RODHeader::majorVersion() const
{
  return (m_version >> 16) & 0xffff;
}

inline int RODHeader::minorVersion() const
{
  return m_version & 0xffff;
}

inline int RODHeader::sourceID() const
{
  return m_sourceId;
}

inline int RODHeader::subDetectorID() const
{
  return (m_sourceId >> 16) & 0xff;
}

inline int RODHeader::moduleID() const
{
  return m_sourceId & 0xffff;
}

inline int RODHeader::crate() const
{
  return m_sourceId & 0xf;
}

inline int RODHeader::sLink() const
{
  return (m_sourceId >> 4) & 0x3;
}

inline int RODHeader::dataType() const
{
  return (m_sourceId >> 7) & 0x1;
}

inline int RODHeader::run() const
{
  return m_run;
}

inline int RODHeader::runType() const
{
  return (m_run >> 24) & 0xff;
}

inline int RODHeader::runNumber() const
{
  return m_run & 0xffffff;
}

inline int RODHeader::extendedL1ID() const
{
  return m_lvl1Id;
}

inline int RODHeader::ecrID() const
{
  return (m_lvl1Id >> 24) & 0xff;
}

inline int RODHeader::l1ID() const
{
  return m_lvl1Id & 0xffffff;
}

inline int RODHeader::bunchCrossing() const
{
  return m_bcId;
}

inline int RODHeader::l1TriggerType() const
{
  return m_trigType;
}

inline int RODHeader::detEventType() const
{
  return m_detType;
}

inline int RODHeader::orbitCount() const
{
  return (m_detType >> 16) & 0xffff;
}

inline int RODHeader::stepNumber() const
{
  return (m_detType >> 4) & 0xfff;
}

inline int RODHeader::stepType() const
{
  return m_detType & 0xf;
}

inline bool RODHeader::bcnMismatch() const
{
  return (m_statusWords.empty()) ? false : m_statusWords[0] & 0x1;
}

inline bool RODHeader::gLinkTimeout() const
{
  return (m_statusWords.empty()) ? false : m_statusWords[0] & 0x4;
}

inline bool RODHeader::dataTransportError() const
{
  return (m_statusWords.empty()) ? false : m_statusWords[0] & 0x8;
}

inline bool RODHeader::rodFifoOverflow() const
{
  return (m_statusWords.empty()) ? false : m_statusWords[0] & 0x10;
}

inline bool RODHeader::lvdsLinkError() const
{
  return (m_statusWords.empty()) ? false : m_statusWords[0] & 0x10000;
}

inline bool RODHeader::cmmParityError() const
{
  return (m_statusWords.empty()) ? false : m_statusWords[0] & 0x20000;
}

inline bool RODHeader::gLinkError() const
{
  return (m_statusWords.empty()) ? false : m_statusWords[0] & 0x40000;
}

inline bool RODHeader::limitedRoISet() const
{
  return (m_statusWords.size() < 2) ? false : m_statusWords[1] & 0x2;
}

inline bool RODHeader::triggerTypeTimeout() const
{
  return (m_statusWords.size() < 2) ? false : m_statusWords[1] & 0x10000;
}

inline const std::vector<uint32_t>& RODHeader::statusWords() const
{
  return m_statusWords;
}

inline int RODHeader::payloadSize() const
{
  return m_payloadSize;
}

} // end namespace

#ifndef RODHeader_ClassDEF_H
#include "TrigT1CaloEvent/RODHeader_ClassDEF.h"
#endif

#endif
