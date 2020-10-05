/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTUtilities_hh
#define TRTUtilities_hh

#include "globals.hh"
#include <fstream>
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class TRTOutputFile;


namespace TRTUtilities
{
  std::string NumberToString(int);
  
  void PrintTubeParameters(TRTOutputFile& f, double, double);
  void PrintTubeParameters(TRTOutputFile& f, double, double, double);
  void PrintTubeParameters(TRTOutputFile& f, double, double, double, double);
}


#endif
