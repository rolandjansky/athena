/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPORDO_MODULEID_H
#define L1TOPORDO_MODULEID_H

#include <cstdint>
#include <iostream>

namespace L1Topo {
  /** @short Represents the L1Topo module ID, with decoder and encoder
   *
   * Used to decode/encode the 16-bit module ID of L1Topo modules which makes up the first 16 bits of the 32-bit source ID that uniquely identifies a ROS and ROB fragment. 
   * Numbering scheme for the lower 8 bits is: rmmmssss, where 
   *    ssss is the S-link fibre number (four bits allowing values 0-15)
   *    mmm is the L1Topo module number (three bits allowing values 0-7)
   *    r is a one bit flag distingushing RoI fibres (1) from DAQ fibres (0).
   * Valid combinations according to the convention described by experts are:
   *    module 0 link 0 DAQ   0 000 0000 = 0x0000
   *    module 0 link 1 RoI   0 000 0001 = 0x0081
   *    module 1 link 0 DAQ   1 001 0000 = 0x0010
   *    module 1 link 1 RoI   0 001 0001 = 0x0091
   * Link 2 may be used for the new RoIB. 
   * A third module may be a later upgrade.
   */ 

  class ModuleID {
  public:
    //! Construct from constituent parts and encode word
    ModuleID(unsigned int link, unsigned int module, unsigned int roiDaq);
    //! Construct from word and decode constituent parts
    ModuleID(const uint16_t);
    //! access method
    uint16_t link() const;
    //! access method
    uint16_t module() const;
    //! access method
    uint16_t roiDaq() const;
    //! access method
    uint16_t id() const;
    //! True if this is a DAQ module
    bool isDAQ() const;
    //! True if this is a ROI module
    bool isROI() const;
protected:
    //! method used by constructor to decode word 
    void decode();
    //! method used by constructor to encode word 
    void encode();
  private:
    uint16_t m_link;
    uint16_t m_module;
    uint16_t m_roiDaq;
    uint16_t m_id;
  };

  //! print helpers
  std::ostream& operator<<(std::ostream& os, const ModuleID& );

} // namespace L1Topo

#endif // L1TOPORDO_MODULEID_H
