/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTHPTDCREADOUT_H
#define MUONBYTESTREAM_MDTHPTDCREADOUT_H

#include <stdint.h>

#include "MdtReadOut.h"

// Decoding methods for MDT Hptdc raw data words
// S. Rosati, J. Meyer Apr 2017

class MdtHptdcReadOut : public MdtReadOut {
private:
    // Raw data word
    uint32_t m_dataWord;
    // Data word header
    uint16_t m_wordHeader;

    // Header info: event counter, bunch crossing ID and tdc number
    uint16_t m_tdcId;
    uint16_t m_ecnt;
    uint16_t m_bcId;

    // Footer info: words counter
    uint16_t m_wcnt;

    // Data info:
    uint16_t m_channel;
    uint16_t m_coarse;
    uint16_t m_fine;
    uint16_t m_width;
    bool m_errflag;

    bool m_leading;

    // Define the data structure and the word header values

    // Word header Position and values
    // The word header occupies the 4 highest bits of each TDC data word
    static constexpr uint16_t HEADERPOS = 28;
    static constexpr uint16_t HEADERBITS = 0xf;

    // Beginning of TDC: two possible header values
    // for channels 00-15 and 16-17 respectively
    static constexpr uint16_t BOTVALUE1 = 0xa;
    static constexpr uint16_t BOTVALUE2 = 0xb;

    // End of TDC
    static constexpr uint16_t EOTVALUE = 0xc;

    // TDC single measurement
    static constexpr uint16_t TSMVALUE_LEAD = 0x4;
    static constexpr uint16_t TSMVALUE_TAIL = 0x5;

    // TDC combined measurement
    static constexpr uint16_t TCMVALUE = 0x9;

    // TDC error status
    static constexpr uint16_t TESVALUE = 0x6;

public:
    // Constructor and destructor
    MdtHptdcReadOut();
    ~MdtHptdcReadOut() =default;

    // Methods for decoding and encoding dataWords
    void decodeWord(uint32_t dataWord);
    void encodeWord(uint16_t headerVal);

    // Methods to identify the word type
    // Beginning of TDC
    bool is_BOT() const { return ((m_wordHeader == BOTVALUE1) || (m_wordHeader == BOTVALUE2)); };
    // End of TDC
    bool is_EOT() const { return (m_wordHeader == EOTVALUE); };
    // TDC single measurement
    bool is_TSM() const { return ((m_wordHeader == TSMVALUE_LEAD) || (m_wordHeader == TSMVALUE_TAIL)); };
    // TDC combined measurement
    bool is_TCM() const { return (m_wordHeader == TCMVALUE); };
    // TDC error status
    bool is_TES() const { return (m_wordHeader == TESVALUE); };

    // Methods to retrieve the decoded word content
    uint16_t tdcId() const { return m_tdcId; }
    uint16_t ecnt() const { return m_ecnt; }
    uint16_t bcId() const { return m_bcId; }

    uint16_t channel() const { return m_channel; }
    uint16_t coarse() const { return m_coarse; }
    uint16_t fine() const { return m_fine; }
    uint16_t width() const { return m_width; }
    bool errflag() const { return m_errflag; }
    bool isLeading() const { return m_leading; }

    // Methods to encode data in 32 bits words

    uint32_t makeBOT(uint16_t tdcId, uint16_t ecnt, uint16_t bcid);
    uint32_t makeEOT(uint16_t tdcId, uint16_t ecnt, uint16_t wcnt);
    uint32_t makeTSM(uint16_t tdcId, uint16_t channel, bool leading, uint16_t coarse, uint16_t fine);

    uint32_t makeTCM(uint16_t tdcId, uint16_t channel, uint16_t width, uint16_t coarse, uint16_t fine);

private:
    void setZero();
};

#endif  // MUONBYTESTREAM_MDTHPTDCREADOUT_H
