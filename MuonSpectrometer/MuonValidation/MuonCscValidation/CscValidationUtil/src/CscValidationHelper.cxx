/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscValidationHelper.cxx

#include "CscValidationUtil/CscValidationHelper.h"

#include <sstream>

using std::string;
using std::ostringstream;

string CscValidationHelper::
csc_name(int istation, int zsec, int phisec, int wlay, bool measphi) {
  ostringstream ssout;
  if ( istation == 1 ) ssout << "CSS";
  else if ( istation == 2 ) ssout << "CSL";
  else return "invalid station";
  if ( zsec == -1 ) ssout << "-";
  else if ( zsec == 1 ) ssout << "+";
  else return "invalid z sector";
  if ( phisec>0 && phisec<9 ) ssout << phisec;
  else return "invalid phi sector";
  if ( wlay == 0 ) return ssout.str();
  if ( wlay>0 && wlay<5 ) ssout << "." << wlay;
  else return "invalid wire layer";
  if ( measphi ) ssout << "phi";
  else ssout << "eta";
  return ssout.str();
}
