/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RINGER_STANDALONE

#ifndef RINGERSELECTORTOOLS_EXTRADESCRIPTIONPATTERNS_H
#define RINGERSELECTORTOOLS_EXTRADESCRIPTIONPATTERNS_H

// Asg includes:
#include "AsgMessaging/MsgStream.h"

// std includes:
#include <bitset>

namespace Ringer {

/**
 * @class ExtraDescriptionPatterns
 * @brief Get electron defined TAccept and its masks.
 **/
class ExtraDescriptionPatterns {
  private:
    /**
     * @brief The bitmask indexes to be used
     **/
    enum bit_idxs {
      FEED_ETA = 0,
      FEED_ET = 1,
      FEED_PILEUP_ESTIMATION = 2,
      NIDXS
    };

  public:
    /**
     * @brief Bit mask word used
     **/
    typedef std::bitset<NIDXS> bitMskWord;

    /// Ctors
    /// @{
    ExtraDescriptionPatterns(){;}
    ExtraDescriptionPatterns(unsigned long mask):m_bitset(mask){;}
    /// @}

    /// Main methods
    /// @{
    /// Flag to whether feed eta or not to the dicriminator
    ExtraDescriptionPatterns& setFeedEta(bool flag = true) { m_bitset[FEED_ETA] = flag; return *this; }
    /// Flag to whether feed Et or not to the dicriminator
    ExtraDescriptionPatterns& setFeedEt(bool flag = true) { m_bitset[FEED_ET] = flag; return *this; }
    /// Flag to whether feed pile-up or not to the dicriminator
    ExtraDescriptionPatterns& setFeedPileupEstimation(bool flag = true) { m_bitset[FEED_PILEUP_ESTIMATION] = flag; return *this; }
    /// Returns whether to feed eta to the dicriminator
    bool feedEta() const { return m_bitset[FEED_ETA]; }
    /// Returns whether to feed Et to the dicriminator
    bool feedEt() const { return m_bitset[FEED_ET]; }
    /// Returns whether to feed pile-up to the dicriminator
    bool feedPileupEstimation() const { return m_bitset[FEED_PILEUP_ESTIMATION]; }
    /// @}

    /// Redirect methods to the bitset
    /// @{
    std::basic_string<char> to_string() const { return m_bitset.to_string(); }
    unsigned long to_ulong() const { return m_bitset.to_ulong(); }
    unsigned long long to_ullong() const { return m_bitset.to_ullong(); }
    ExtraDescriptionPatterns& reset() { m_bitset.reset(); return *this; }
    ExtraDescriptionPatterns& reset(size_t pos) { m_bitset.reset(pos); return *this; }
    /// @}

    /**
     * Set boolean representation as true if any bit is set.
     **/
    explicit operator bool() const{
      return m_bitset.any();
    }

    ExtraDescriptionPatterns& operator=( const ExtraDescriptionPatterns& rv ) {
      this->m_bitset = rv.m_bitset;
      return *this;
    }

  private:
    bitMskWord m_bitset;

};


} // namespace Ringer

// =============================================================================
/**
 * Provide operator<< to ExtraDescriptionPatterns
 **/
MsgStream& operator<<( MsgStream& out,  const Ringer::ExtraDescriptionPatterns& extraPat );

#endif // RINGERSELECTORTOOLS_EXTRADESCRIPTIONPATTERNS_H


#endif // RINGER_STANDALONE
