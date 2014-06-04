/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/LArID_Exception.h"

#include <sstream>


LArID_Exception::operator std::string () 

{
  std::ostringstream s;

  s << "LArID_Exception - Error code: " << this->code() << " \n "
    << this->message();
  return s.str();
}
