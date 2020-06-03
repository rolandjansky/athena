/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1Interfaces/BcmCTP.h"

// STL includes(s):
#include <sstream>
#include <iomanip>

// local includes
#include "TrigT1Interfaces/BitOp.h"

namespace LVL1 {

  BcmCTP::BcmCTP( unsigned int word0 )
    : m_cableWord0(word0)
  {  
  }

  const std::string BcmCTP::dump() const
  {
    std::ostringstream s;

    s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << m_cableWord0
      << std::dec << std::setfill(' ') << std::endl;

    return s.str();
  }

  const std::string BcmCTP::print() const
  {
    std::ostringstream s;

    s << std::setfill( '0' )
      << " Side-to-side:" << std::setw(6) << BitOp::printBits(m_cableWord0, 0, 1)
      << " Wide:" << std::setw(6) << BitOp::printBits(m_cableWord0, 2, 2)
      << " A-side multiplicities:" << std::setw(6) << BitOp::printBits(m_cableWord0, 3, 4)
      << " C-side multiplicities:" << std::setw(6) << BitOp::printBits(m_cableWord0, 5, 6)
      << std::setfill(' ') << std::endl;

    return s.str();
  }

}

