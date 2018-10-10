/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1Interfaces/LucidCTP.h"

#include <sstream>
#include <iomanip>

#include "TrigT1Interfaces/BitOp.h"

namespace LVL1 {

  LucidCTP::LucidCTP( unsigned int word0 )
    : m_cableWord0(word0)
  {  
  }

  const std::string LucidCTP::dump() const
  {
    std::ostringstream s;

    s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << m_cableWord0
      << std::dec << std::setfill(' ') << std::endl;

    return s.str();
  }

  const std::string LucidCTP::print() const
  {
    std::ostringstream s;

    s << std::setfill( '0' )
      << " Side C:" << std::setw(1) << BitOp::printBits(m_cableWord0, 0, 0)
      << "  Side A:" << std::setw(1) << BitOp::printBits(m_cableWord0, 1, 1)
      << std::setfill(' ') << std::endl;

    return s.str();
  }

}
