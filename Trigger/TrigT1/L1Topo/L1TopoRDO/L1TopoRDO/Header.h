/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPORDO_HEADER_H
#define L1TOPORDO_HEADER_H

#include <iostream>
#include <cstdint>
#include <utility>

namespace L1Topo {

  /** @short Represents the L1Topo header word of the L1Topo DAQ data, with decoder and encoder
   *
   * Used to decode/encode the 32-bit word which contents include bunch crossing, FPGA, last block, active fibres and CRC.
   */  
  class Header {
  public:
    //! Construct from contents and encode word
    Header(const uint32_t version, const uint32_t active_fibres, const uint32_t payload_crc, const uint32_t fpga, const uint32_t last_block, const uint32_t bcn_sign, const uint32_t bcn_offset);
    //! Construct with word and decode contents
    Header(uint32_t word);
    //! access method
    uint32_t version() const;
    //! access method
    uint32_t active_fibres() const; 
    //! access method
    uint32_t payload_crc() const; 
    //! access method
    uint32_t fpga() const; 
    //! access method
    uint32_t last_block() const; 
    //! access method
    uint32_t bcn_sign() const;
    //! access method
    uint32_t bcn_offset() const;
    //! access method
    int bcn()  const;
    //! access method
    uint32_t word() const;
  protected:
    //! method used by constructor to decode word 
    void decode();
    //! method used by constructor to encode word 
    void encode();
  private:
    uint32_t m_version;
    uint32_t m_active_fibres; 
    uint32_t m_payload_crc; 
    uint32_t m_fpga; 
    uint32_t m_last_block; 
    uint32_t m_bcn_sign;
    uint32_t m_bcn_offset;
    int m_bcn;
    uint32_t m_word;
  };

  //! helper function to return a relative bunch crossing as a signed integer, given the sign bit and offset that are encoded in the header
  int signedBCN(const uint32_t sign, uint32_t offset);

  //! reverse helper function to return the sign and offset bits given a relative bunch crossing number
  std::pair<uint32_t,uint32_t> signAndOffsetBCN(const int bcn);

  //! Helper for printing
  std::ostream& operator<<(std::ostream&, const Header&);

} // namespace L1Topo



#endif // L1TOPORDO_HEADER_H
