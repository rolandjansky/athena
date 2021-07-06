/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTUtilities.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>
#include <string>



  // Called on demand

std::string TRTUtilities::NumberToString(int number)
{
  return std::to_string (number);
}


  // Called on demand

void TRTUtilities::PrintTubeParameters(TRTOutputFile& f, double outerRadius, double length)
{
  std::ofstream& output = f.GetReference();

  output << "  outerRadius=" << outerRadius << " mm";
  output << "  length=" << length << " mm" << std::endl;
}


  // Called on demand

void TRTUtilities::PrintTubeParameters(TRTOutputFile& f,
                                       double innerRadius,
  double outerRadius, double length)
{
  std::ofstream& output = f.GetReference();

  output << "  innerRadius=" << innerRadius << " mm";
  output << "  outerRadius=" << outerRadius << " mm";
  output << "  length=" << length << " mm" << std::endl;
}


  // Called on demand

void TRTUtilities::PrintTubeParameters(TRTOutputFile& f,
                                       double innerRadius,
  double outerRadius, double length, double position)
{
  std::ofstream& output = f.GetReference();

  output << "  innerRadius=" << innerRadius << " mm";
  output << "  outerRadius=" << outerRadius << " mm";
  output << "  length=" << length << " mm" << std::endl;
  output << "  position=" << position << " mm" << std::endl;
}
