/*
 
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 
 */
 
#ifndef PIXELDIGITIZATION_Utilities_H
 
#define PIXELDIGITIZATION_Utilities_H
 

 
#include <string>
 
#include <tuple>
 
#include <vector>
 
#include <utility>
 

 
struct BichselData;
 

 
namespace PixelDigitization{
 
  std::string
 
  formBichselDataFileName(int particleType, unsigned int nCols);
 
  
 
  BichselData
 
  getBichselDataFromFile(const std::string & fullFilename);
 
  
 
  std::tuple<double, double, double>
 
  parseThreeDoubles(const std::string & line);
 
  
 
  std::pair<int, int>
 
  fastSearch(const std::vector<double> & vec, double item);
 
  
 
}
 

 
#endif
