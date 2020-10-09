/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTUtilities.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>



  // Called on demand

std::string TRTUtilities::NumberToString(int number)
{
  std::string digitAsString[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
  std::string numberAsString = "";

  for ( ; ; )
  {
    numberAsString = digitAsString[number % 10] + numberAsString;
    number /= 10;
    if (number == 0)
      break;
  }

  return numberAsString;
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
