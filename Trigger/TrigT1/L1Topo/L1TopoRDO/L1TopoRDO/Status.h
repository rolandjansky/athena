/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPORDO_STATUS_H
#define L1TOPORDO_STATUS_H

#include <cstdint>
#include <iostream>


namespace L1Topo {
  /** @short Represents the L1Topo status word of the L1Topo DAQ header, with decoder and encoder
   *
   * Used to decode/encode the 32-bit word whch contains just two 1-bit flags: overflow flag and CRC mismatch flag.
   */  


  class Status {
  public:
    //! Construct from contents and encode word
    Status(const uint32_t overflow, const uint32_t crc);
    //! Construct from word and encode contents
    Status(const uint32_t word);
    //! access method
    uint32_t overflow() const;
    //! access method
    uint32_t crc() const;
    //! access method
    uint32_t word() const;
  protected:
    //! method used by constructor to decode word 
    void decode();
    //! method used by constructor to encode word 
    void encode();
  private:
    //! overflow flag
    uint32_t m_overflow;
    //! CRC mismatch flag
    uint32_t m_crc;
    //! The raw data representation
    uint32_t m_word;
  };
  std::ostream& operator<<(std::ostream&, const Status&);
  
} // namespace L1Topo

//! Helper for printing

#endif // L1TOPORDO_FIBRE_H
