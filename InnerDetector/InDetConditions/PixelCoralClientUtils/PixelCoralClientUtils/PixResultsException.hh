/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _CAN_PixResultsException_hh_
#define _CAN_PixResultsException_hh_

#include <exception>
#include <string>

namespace CAN {

class  PixResultsException : public std::exception {
public:
  PixResultsException(const std::string& msg) : m_msg(msg) {}
  ~PixResultsException() throw() {}
  const char* what() const throw() { return m_msg.c_str(); }
  
private:
  std::string m_msg;
};
} // namespace CAN

#endif
