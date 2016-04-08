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
    L1TopoTOB(const int32_t ctp_signal, uint32_t overflow_bits, uint32_t trigger_bits, uint32_t error_bits);
    //! Construct from contents and encode word, without CRC error bits (they will be 0 by default)
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
    //! access to five bits which flag CRC errors on the input links. They are available as individual flags below.
    uint32_t error_bits() const;
    //! flag true if EM TOB input links have errors (bit 16)
    bool crc_EM() const;
    //! flag true if Tau TOB input links have errors (bit 17)
    bool crc_Tau() const;
    //! flag true if Muon TOB input links have errors (bit 18)
    bool crc_Muon()  const;
    //! flag true if Jet TOB input links have errors (bit 19)
    bool crc_Jet() const;
    //! flag true if Energy TOB input links have errors (bit 20)
    bool crc_Energy()  const;
  protected:
    //! method used by constructor to decode word 
    void decode();
    //! method used by constructor to encode word 
    void encode();
  private:    
    uint32_t m_ctp_signal;
    uint32_t m_overflow_bits; 
    uint32_t m_trigger_bits; 
    uint32_t m_error_bits;
    uint32_t m_word;
  };

  //! Helper for printing
  std::ostream& operator<<(std::ostream&, const L1TopoTOB&);
  
  //! Comparison operators, based on word
  bool operator==(const L1TopoTOB&, const L1TopoTOB&);
  //! Comparison operators, based on word
  bool operator!=(const L1TopoTOB&, const L1TopoTOB&);
  //! Comparison operators, based on word
  bool operator< (const L1TopoTOB&, const L1TopoTOB&);
  //! Comparison operators, based on word
  bool operator> (const L1TopoTOB&, const L1TopoTOB&);

} // namespace L1Topo


#endif // L1TOPORDO_L1TOPOTOB_H
