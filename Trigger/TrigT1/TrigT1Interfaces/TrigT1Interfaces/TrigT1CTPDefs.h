/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1INTERFACES_TRIGT1CTPDEFS_H
#define TRIGT1INTERFACES_TRIGT1CTPDEFS_H

// std include(s):
#include <stdint.h>

// STL include(s):
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

namespace LVL1CTP {

  /**
   * 
   *   @short Class holding common constants for the CTP simulation
   *
   *          All CTP constants are taken from CTPfragment/CTPdataformat.h
   *          from tdaq-common project.
   *
   *  @author Wolfgang Ehrenfeld (wolfgang.ehrenfeld@desy.de)
   *
   * @version \$Id: TrigT1CTPDefs.h 621285 2014-10-11 14:51:48Z stelzer $
   *
   */

  class TrigT1CTPDefs {

  public:
    
    // constants:

  };

  //! helper function to dump a number in hex format
  inline const std::string convertToHex(const uint32_t word) 
  {
    std::ostringstream s;
    s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << word;
    // s << std::dec << std::setfill(' ');
    return s.str();
  }

  //! helper function to dump a vector of numbers in hex format
  inline const std::string convertToHex(const std::vector<uint32_t> words) 
  {
    std::ostringstream s;

    // switch to hex format
    s << std::hex <<  std::setfill( '0' );

    if (words.size() > 0) 
      s << " 0x" << std::setw(8) << words[0];
    for (size_t i(1); i < words.size(); ++i) 
      s << ", 0x" << std::setw(8) << words[i];

    // switch back to normal format
    s  << std::dec << std::setfill(' ');

    return s.str();
  }

  //! operator to dump a vector of numbers
  inline std::ostream& operator<<(std::ostream& os, std::vector<unsigned int> rhs) 
  {
    for (std::vector<unsigned int>::const_iterator i(rhs.begin()); i != rhs.end(); ++i) {
      os << " " << *i;
    }

    return os;
  }

  //! helper function to dump raw CTP content
  const std::string convertRawCTPData(const std::vector<uint32_t>& data, const bool longFormat = false, int ctpVersionNumber = 4);

} // namespace LVL1CTP

#endif // TRIGT1INTERFACES_TRIGT1CTPDEFS_H
