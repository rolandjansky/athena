/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/test/exctrace2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009
 * @brief Test trace backs for caught exceptions --- using the collector.
 */


#include <string>
#include "stdlib.h"

int main (int /*argc*/, char** argv)
{
  std::string cmd = argv[0];
  std::string::size_type pos = cmd.rfind ('/');
  if (pos != std::string::npos)
    cmd = cmd.substr (0, pos+1);
  else
    cmd = "./";
  cmd = "LD_PRELOAD=libexctrace_collector.so " + cmd;
  cmd += "exctrace1_test.exe";

  system (cmd.c_str());
  
  return 0;
}
