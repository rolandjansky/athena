// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef L1CALOFEXBYTESTREAM_JFEXBITS_H
#define L1CALOFEXBYTESTREAM_JFEXBITS_H

#include <cstdint>

namespace LVL1::jFEXBits {

/************     TOB,XTOB Counter Trailer     ************/

    ///Masking for TOB/xTOB Counter Trailer of jEM, jTau, jJ and jLJ
    static constexpr uint32_t TOB_COUNTS_6b = 0x3f;
    ///Masking for TOB Counter Trailer of jXE and jTE
    static constexpr uint32_t TOB_COUNTS_1b = 0x1;
    ///Bit positions for TOB Counter Trailer
    static constexpr uint32_t jJ_TOB_COUNTS   = 1;
    static constexpr uint32_t jLJ_TOB_COUNTS  = 7;
    static constexpr uint32_t jTau_TOB_COUNTS = 13;
    static constexpr uint32_t jEM_TOB_COUNTS  = 19;
    static constexpr uint32_t jTE_TOB_COUNTS  = 26;
    static constexpr uint32_t jXE_TOB_COUNTS  = 27;

/************     jFEX to ROD Header     ************/

    ///Masking words
    static constexpr uint32_t ROD_HEADER_3b  = 0x7;
    static constexpr uint32_t ROD_HEADER_8b  = 0xff;
    static constexpr uint32_t ROD_HEADER_9b  = 0x1ff;
    static constexpr uint32_t ROD_HEADER_12b = 0xfff;
    static constexpr uint32_t ROD_HEADER_24b = 0xffffff;
    
    ///Bit positions for jFEX to ROB Trailer
    static constexpr uint32_t L1CALO_STREAM_ID_ROD_HEADER = 0;
    static constexpr uint32_t L1CALO_STREAM_ID_SLOT_ROD_HEADER = 0;
    static constexpr uint32_t L1CALO_STREAM_ID_FPGA_ROD_HEADER = 4;
    static constexpr uint32_t L1CALO_STREAM_ID_INFO_ROD_HEADER = 6;
    static constexpr uint32_t BCN_ROD_HEADER              = 8;
    static constexpr uint32_t CRC_ROD_HEADER              = 20;
    static constexpr uint32_t VERS_ROD_HEADER             = 29;

    static constexpr uint32_t L1ID_ROD_HEADER  = 0;
    static constexpr uint32_t ECRID_ROD_HEADER = 24;

/************     jFEX to ROD Trailer     ************/

    ///Masking words
    static constexpr uint32_t ROD_TRAILER_2b  = 0x3;
    static constexpr uint32_t ROD_TRAILER_4b  = 0xf;
    static constexpr uint32_t ROD_TRAILER_6b  = 0x3f;
    static constexpr uint32_t ROD_TRAILER_16b = 0xffff;
    static constexpr uint32_t ROD_TRAILER_20b = 0xfffff;
    
    ///Bit positions for jFEX to ROB Trailer
    static constexpr uint32_t PAYLOAD_ROD_TRAILER = 0;
    static constexpr uint32_t FPGA_ROD_TRAILER    = 18;
    static constexpr uint32_t jFEX_ROD_TRAILER    = 20;
    static constexpr uint32_t RO_ROD_TRAILER      = 24;
    static constexpr uint32_t TSN_ROD_TRAILER     = 28;
    
    static constexpr uint32_t ERROR_ROD_TRAILER   = 0;
    static constexpr uint32_t CRC_ROD_TRAILER     = 12;
    

} // namespace LVL1::jFEXBits

#endif // L1CALOFEXBYTESTREAM_JFEXBITS_H
