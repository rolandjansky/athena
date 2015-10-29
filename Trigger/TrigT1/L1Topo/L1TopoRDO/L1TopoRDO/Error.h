/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPORDO_ERROR_H
#define L1TOPORDO_ERROR_H

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

namespace L1Topo {

  //! The different types of error that can be flagged in the L1TopoRDO
  enum class Error { ROB_ERROR=0, ROD_ERROR, SLINK_STATUS_ERROR };
  //! Define the text to print for each error type above - bad design to put it in the header file?
  static const std::map<Error, std::string> errorText = {
    {Error::ROB_ERROR,"ROB Error"},
    {Error::ROD_ERROR,"ROD Error"},
    {Error::SLINK_STATUS_ERROR,"S-Link Status Error"}
  };

  //! Helper to print errors as text rather than numbers. 
  std::ostream& operator<<(std::ostream&, const Error);
  
  //! Helper to print vectors of errors
  std::ostream& operator<<(std::ostream&, const std::vector<Error>&);

} // namespace L1Topo


#endif // L1TOPORDO_ERROR_H
