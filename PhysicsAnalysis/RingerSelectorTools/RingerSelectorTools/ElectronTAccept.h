/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronTAccept.h 670599 2015-05-28 14:15:35Z wsfreund $
#ifndef RINGER_STANDALONE

#ifndef RINGERSELECTORTOOLS_ELECTRONTACCEPT_H
#define RINGERSELECTORTOOLS_ELECTRONTACCEPT_H

// Athena includes:
#include "PATCore/AcceptInfo.h"

// Local includes:
#include "RingerSelectorTools/RingerSelectorToolsDefs.h"

namespace Ringer {

class BitdefElectron_v1;

/**
 * @class ElectronTAccept_v1
 * @brief Get electron defined TAccept and its masks.
 **/
class ElectronTAccept_v1 {
  public:
    /**
     * @brief Bit mask word used
     *
     * FIXME: Ask m_nBits to be encapsulated and a similar typedef like this to
     * be within TAccept
     **/
#ifdef XAOD_ANALYSIS
    typedef std::bitset<32> bitMskWord;
#else
    typedef std::bitset<asg::AcceptInfo::NBITS> bitMskWord;
#endif

    // Grant access to m_accept for BitdefElectron_v1
    friend class BitdefElectron_v1;

    /// Main methods
    /// @{
    /**
     * @brief Retrieve copy of the ElectronTAccept_v1 template
     **/
    static unsigned int getAppliedCutMsk(const Requirement cut,
        bool withCutIDTrack = false);
    /**
     * @brief Retrieve copy of the ElectronTAccept_v1 template
     **/
    static const asg::AcceptInfo& retrieveTAcceptTemplate(){ return m_accept;}
    /// @}


    /// Auxiliar methods
    /// @{
    /**
     * @brief Reset mask so that it doesn't apply any cut (IsEM mask):
     **/
    static void resetMsk(bitMskWord& word);
    /**
     * @brief Add loose applied cuts to bitmask (IsEM mask):
     **/
    static void addLooseAppliedCuts(bitMskWord& word);
    /**
     * @brief Add medium applied cuts to bitmask (IsEM mask):
     **/
    static void addMediumAppliedCuts(bitMskWord& word);
    /**
     * @brief Add tight applied cuts bitmask (IsEM mask):
     **/
    static void addTightAppliedCuts(bitMskWord& word);
    /**
     * @brief Add CutID Track applied cuts to bitmask (IsEM mask):
     **/
    static void addCutIDTrackAppliedCuts(bitMskWord& word);
    /// @}

  private:
    /**
     * @brief Standard ctor (ensure abstract class).
     **/
    ElectronTAccept_v1();

    /// The TAccept:
    static asg::AcceptInfo m_accept;
};

/**
 * @class BitdefElectron_v1
 * @brief Define electron bits (v1)
 **/
class BitdefElectron_v1 {
/*
 * NOTE:
 *
 *  Should new implementation inherit from this? That is, new bits
 *  can only be defined for positions that weren't yet defined:
 *
 *  Example:
 *
 *  \code
 *  class BitdefElectron_v2 : public BitdefElectron_v2
 *  {
 *    static const int NewBit;
 *  }
 *
 *  \endcode
 */

  public:
    /// The decision position set to true when no error occurs:
    static const int NoErrorBit;
    /// Whether CutID was run:
    static const int ExecCutID;
    /// The decision position of the CutID:
    static const int CutIDDec;
    /// The decision position of the Ringer algorithm's discrimation chain:
    static const int RingerChainDec;

    /// Number of bits:
    static unsigned NUsedBits() { return m_nUsedBits; }

  private:
    /**
     * @brief Declare bit to ElectronTAccept_v1 word
     **/
    static int declareBit(
        const char* cutName,
        const char* cutDescr)
    {
      int bit = ElectronTAccept_v1::m_accept.addCut(cutName,cutDescr);
      ++m_nUsedBits;
      return bit;
    }

    static unsigned m_nUsedBits;

    /**
     * @brief Standard ctor (ensure abstract class).
     **/
    BitdefElectron_v1();
};

// Definition of the current Electron TAccept version:
typedef ElectronTAccept_v1 ElectronTAccept;
// And the Electron Bitdef as well:
typedef BitdefElectron_v1  BitdefElectron;

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_ELECTRONTACCEPT_H


#endif // RINGER_STANDALONE
