/*
 
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 
*/
 

 
#include "PixelDigitizationUtilities.h"
 
#include "BichselData.h"
 
#include <fstream>
 
#include <exception>
 
#include <regex>
 
#include <algorithm> //std::lower_bound
 
#include <iterator>  //std::distance
 
#include <limits>
 

 

 
namespace PixelDigitization{
 
  std::string
 
  formBichselDataFileName(int particleType, unsigned int nCols){
 
    const std::string prefix{"PixelDigitization/Bichsel_"};
 
    const std::string suffix{".dat"};
 
    const std::string particleTypeString = std::to_string(particleType);
 
    const std::string nColString= (nCols == 1) ? "" : "_" + std::to_string(nCols) + "steps";
 
    return prefix + particleTypeString + nColString + suffix;
 
  }
 
  
 
  BichselData
 
  getBichselDataFromFile(const std::string & fullFilename){
 
    std::ifstream inputFile(fullFilename);
 
    if (not inputFile){
 
      std::string errmsg = "getBichselDataFromFile: File " + fullFilename +" could not be opened.";
 
      throw std::runtime_error(errmsg);
 
    }
 
    BichselData iData;
 
    std::string thisLine;
 
    thisLine.reserve(60); //expecting maximum line length to be 53, allow some margin
 
    while (getline(inputFile, thisLine)) {
 
      const auto [logBetaGamma, logCollisionEnergy, logCrossSection] = parseThreeDoubles(thisLine);
 
      iData.addEntry(logBetaGamma, logCollisionEnergy, logCrossSection);
 
    }
 
    iData.logHighestCrossSectionsVector.push_back(iData.logIntegratedCrossSectionsVectorOfVector.back().back());
 
    return iData;
 
  }
 
  
 
  std::tuple<double, double, double>
 
  parseThreeDoubles(const std::string & line){
 
    std::tuple<double, double, double> result{0., 0.,0.};
 
    std::regex threeDoublesRx("^([-+.0-9eE]+)\\s+([-+.0-9eE]+)\\s+([-+.0-9eE]+)$");
 
    std::smatch x3;
 
    bool foundDoubles=std::regex_match(line, x3, threeDoublesRx);
 
    if (foundDoubles){
 
      try{
 
        //std::stod might throw a out_of_range or an invalid_argument error
 
        //both of which are subclasses of logic_error
 
        result = {std::stod(x3[1]), std::stod(x3[2]), std::stod(x3[3])};
 
      } catch (std::logic_error & e){
 
        const std::string msg("parseThreeDoubles: error in parsing a number in "+ line);
 
        throw(std::runtime_error(msg));
 
      }
 
    } else {
 
      const std::string msg("parseThreeDoubles: error in parsing the line " + line);
 
      throw(std::runtime_error(msg));
 
    }
 
    return result;
 
  }
 
  
 
  std::pair<int, int>
 
  fastSearch(const std::vector<double> & vec, double item){
 
    std::pair<int, int> output{-1, -1};
 
    if (vec.empty()) return output;
 
    int index_low = 0;
 
    int index_up = vec.size() - 1;
 

 
    if ((item < vec[index_low]) || (item > vec[index_up])) {
 
      return output;
 
    } else if (item == vec[index_low]) {
 
      output.first = index_low;
 
      output.second = index_low;
 
      return output;
 
    } else if (item == vec[index_up]) {
 
      output.first = index_up;
 
      output.second = index_up;
 
      return output;
 
    }
 

 
    while ((index_up - index_low) != 1) {
 
      int index_middle = int(1.0 * (index_up + index_low) / 2.);
 
      if (item < vec[index_middle]) index_up = index_middle;
 
      else if (item > vec[index_middle]) index_low = index_middle;
 
      else { // accurate hit. Though this is nearly impossible ...
 
        output.first = index_middle;
 
        output.second = index_middle;
 
        return output;
 
      }
 
    }
 

 
    output.first = index_low;
 
    output.second = index_up;
 
    return output;
 
  }
 
  
 

 

 

 
}
