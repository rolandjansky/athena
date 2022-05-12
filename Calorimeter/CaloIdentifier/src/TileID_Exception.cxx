/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/TileID_Exception.h"

#include <iostream>
#include <sstream>
#include <iomanip>

TileID_Exception::operator std::string () const 

{
  std::ostringstream text;

  text << "TileID_Exception - Error code: " << this->code() << std::endl;
  text << this->message() ;
  return text.str();
}
