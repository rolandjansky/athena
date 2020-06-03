/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1Interfaces/ZdcCTP.h"

// STL includes(s):
#include <sstream>
#include <iomanip>

// local includes
#include "TrigT1Interfaces/BitOp.h"

namespace LVL1 {

  ZdcCTP::ZdcCTP( unsigned int word0 )
    : m_cableWord0(word0)
  {  
  }

  const std::string ZdcCTP::dump() const
  {
    std::ostringstream s;

    s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << m_cableWord0
      << std::dec << std::setfill(' ') << std::endl;

    return s.str();
  }

  const std::string ZdcCTP::print() const
  {
    std::ostringstream s;

    s << std::setfill( '0' )
      << " bits:" << std::setw(6) << BitOp::printBits(m_cableWord0, 0, 20)
      << std::setfill(' ') << std::endl;

    return s.str();
  }

}

