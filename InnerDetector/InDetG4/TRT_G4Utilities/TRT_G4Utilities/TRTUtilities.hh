/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTUtilities_hh
#define TRTUtilities_hh

#include "globals.hh"

class TRTOutputFile;


namespace TRTUtilities
{
  std::string NumberToString(int);
  
  void PrintTubeParameters(TRTOutputFile& f, double, double);
  void PrintTubeParameters(TRTOutputFile& f, double, double, double);
  void PrintTubeParameters(TRTOutputFile& f, double, double, double, double);
}


#endif
