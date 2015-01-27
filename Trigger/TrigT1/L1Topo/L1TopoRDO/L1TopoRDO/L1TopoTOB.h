/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPORDO_L1TOPOTOB_H
#define L1TOPORDO_L1TOPOTOB_H

#include <iostream>
#include <cstdint>

namespace L1Topo {

  /** @short Represents the L1Topo TOB word of the L1Topo ROI data, with decode and encoder
   *
   * Used to decode/encode the 32-bit word that contains the trigger bits and overflow bits sent to the CTP by L1Topo. 
   */
  class L1TopoTOB {
  public:
    //! Construct from contents and encode word
    L1TopoTOB(const int32_t ctp_signal, uint32_t overflow_bits, uint32_t trigger_bits);
    //! Construct with word and decode contents
    L1TopoTOB(const int32_t word);
    //! access method for ctp signal bits
    uint32_t ctp_signal() const;
    //! accessor method for overflow bits
    uint32_t overflow_bits() const; 
    //! accessor method for trigger bits
    uint32_t trigger_bits() const;
    //! accessor method for data word
    uint32_t word() const;
    //! index bit, 0 or 1, part ofthe ctp_signal
    uint32_t index() const;
    //! clock cycle bit, 0 or 1, part ofthe ctp_signal
    uint32_t clock() const;
    //! fpga number bit , 0 or 1, part ofthe ctp_signal
    uint32_t fpga() const;
  protected:
    //! method used by constructor to decode word 
    void decode();
    //! method used by constructor to encode word 
    void encode();
  private:    
    uint32_t m_ctp_signal;
    uint32_t m_overflow_bits; 
    uint32_t m_trigger_bits; 
    uint32_t m_word;
  };

} // namespace L1Topo

//! Helper for printing
std::ostream& operator<<(std::ostream&, const L1Topo::L1TopoTOB&);

#endif // L1TOPORDO_L1TOPOTOB_H
