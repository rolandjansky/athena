/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArIdentifier/LArOnlID_Exception.h"

#include <stdio.h>


LArOnlID_Exception::operator std::string () 

{

  char * l_str = new char[200];
  std::string newline(" \n ") ;
  std::string errorMessage ;
  sprintf(l_str,"LArOnlID_Exception - Error code: %d ", this->code());
  errorMessage += std::string(l_str);
  errorMessage += newline ;
  errorMessage += this->message() ;
  delete[] l_str ;
  return errorMessage ;

}
