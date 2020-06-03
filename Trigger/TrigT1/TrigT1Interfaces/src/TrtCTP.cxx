/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1Interfaces/TrtCTP.h"

// STL includes(s):
#include <sstream>
#include <iomanip>

// local includes
#include "TrigT1Interfaces/BitOp.h"

namespace LVL1 {

  TrtCTP::TrtCTP( unsigned int word0 )
    : m_cableWord0(word0)
  {  
  }

  const std::string TrtCTP::dump() const
  {
    std::ostringstream s;

    s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << m_cableWord0
      << std::dec << std::setfill(' ') << std::endl;

    return s.str();
  }

  const std::string TrtCTP::print() const
  {
    std::ostringstream s;

    s << std::setfill( '0' )
      << "TRT bit pattern: " << BitOp::printBits(m_cableWord0, 0, 31)
      << std::setfill(' ') << std::endl;

    return s.str();
  }

}

