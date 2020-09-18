/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPORDO_FIBRE_H
#define L1TOPORDO_FIBRE_H

#include <vector>
#include <cstdint>
#include <iostream>

namespace L1Topo {

  /** @short Represents the L1Topo fibre word of the L1Topo DAQ header, with decoder and encoder
   *
   * Used to decode/encode the 32-bit word whch contains up to 5 x 5-bit fibres, each comprising a 1-bit status flag and 4-bit size/count
   * This word is repeated until the number of fibres in the L1Topo::Header block have been given.
   * If any fibres asserted their status flag, then a separate fibre status word will follow.
   */  


  class Fibre {
  public:
    //! Construct from contents and encode word: vectors of up to 5 status flags and sizes. They will be inserted left to right in the order the are given. The correct ordering is left to the user. Note that status is encoded as a single bit as specified in the data format, copied from the least significant bit of the given integer. Similarly, the size is truncated to the least significant 4 bits of the given integer.  
    Fibre(std::vector<uint32_t> &&status, std::vector<uint32_t> &&count);
    //! Construct from word and encode contents
    Fibre(const uint32_t word);
    //! access method
    const std::vector<uint32_t>& status() const;
    //! access method
    const std::vector<uint32_t>& count() const;
    //! access method
    uint32_t word() const;
  protected:
    //! method used by constructor to decode word 
    void decode();
    //! method used by constructor to encode word 
    void encode();
  private:
    //! fibre status
    std::vector<uint32_t> m_status;
    //! fibre counts
    std::vector<uint32_t> m_count;
    //! The raw data representation
    uint32_t m_word;
  };

  //! Helper for printing
  std::ostream& operator<<(std::ostream&, const Fibre&);

} // namespace L1Topo

#endif // L1TOPORDO_FIBRE_H
