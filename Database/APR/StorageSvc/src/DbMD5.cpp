/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/MD5.h"

#include <string>

void genMD5(const std::string& s, void* code)    {
   MD5 checkSum((unsigned char*)s.c_str(), s.length());
   checkSum.raw_digest((unsigned char*)code);
}
