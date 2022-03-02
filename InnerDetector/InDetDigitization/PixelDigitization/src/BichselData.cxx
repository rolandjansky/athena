/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */
 #include "BichselData.h"
 #include <cmath> //for pow
 #include <algorithm> //for std::clamp
 
namespace{
  std::pair<int, int> 
  FastSearch(std::vector<double> vec, double item) {
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
 
double 
BichselData::lastBetaGammaValue() const{ 
  if (not logBetaGammaVector.empty()){
    return logBetaGammaVector.back();
  } else {
    return std::numeric_limits<double>::quiet_NaN();
  }
}

 
void 
BichselData::addNewLogBetaGamma(double logBetaGamma){
  const std::vector<double> emptyArray;
  if (not empty()) {
    logHighestCrossSectionsVector.push_back(logIntegratedCrossSectionsVectorOfVector.back().back());
  }
  logBetaGammaVector.push_back(logBetaGamma);
  logCollisionEnergyVectorOfVector.push_back(emptyArray);
  logIntegratedCrossSectionsVectorOfVector.push_back(emptyArray);
}

void 
BichselData::addEntry(double logBetaGamma, double logCollisionEnergy, double logIntegratedCrossSection){
  if (empty() or (lastBetaGammaValue() != logBetaGamma)) { // a new BetaGamma
    addNewLogBetaGamma(logBetaGamma);
  }
  logCollisionEnergyVectorOfVector.back().push_back(logCollisionEnergy);
  logIntegratedCrossSectionsVectorOfVector.back().push_back(logIntegratedCrossSection);
}

void BichselData::updateAfterLastEntry(){
  logHighestCrossSectionsVector.push_back(logIntegratedCrossSectionsVectorOfVector.back().back());
}
 
 
 //=======================================
// B E T A   G A M M A   I N D E X
//=======================================
std::pair<int, int> 
BichselData::getBetaGammaIndices(double BetaGammaLog10) const {
  std::pair<int, int> indices_BetaGammaLog10;
  if (empty()) return {-1,-1};
  if (BetaGammaLog10 > logBetaGammaVector.back()) { // last one is used because when beta-gamma is very large,
                                                            // energy deposition behavior is very similar
    indices_BetaGammaLog10.first = logBetaGammaVector.size() - 1;
    indices_BetaGammaLog10.second = logBetaGammaVector.size() - 1;
  } else {
    indices_BetaGammaLog10 = FastSearch(logBetaGammaVector, BetaGammaLog10);
  }

  return indices_BetaGammaLog10;
}




//==========================================
// G E T   C O L L I S I O N   E N E R G Y
//==========================================
//Interpolate collision energy
double 
BichselData::interpolateCollisionEnergy(std::pair<int, int> indices_BetaGammaLog10, double IntXLog10) const {
  if ((indices_BetaGammaLog10.first == -1) && (indices_BetaGammaLog10.second == -1)) return -1.;
  if (empty()) return -1.;

  // BetaGammaLog10_2 then
  std::pair<int, int> indices_IntXLog10_x2 =
    FastSearch(logIntegratedCrossSectionsVectorOfVector[indices_BetaGammaLog10.second], IntXLog10);
  if (indices_IntXLog10_x2.first < 0) {
    return -1;
  }
  if (indices_IntXLog10_x2.second < 0) {
    return -1;
  }
  
  double y21 = logIntegratedCrossSectionsVectorOfVector.at(indices_BetaGammaLog10.second).at(indices_IntXLog10_x2.first);
  double y22 = logIntegratedCrossSectionsVectorOfVector.at(indices_BetaGammaLog10.second).at(indices_IntXLog10_x2.second);
  const auto diff = y22 - y21;
  if (diff<1e-300){
    return -1;
  }  
  double Est_x2 =
    ((y22 - IntXLog10) *
     logCollisionEnergyVectorOfVector[indices_BetaGammaLog10.second][indices_IntXLog10_x2.first] +
     (IntXLog10 - y21) *
     logCollisionEnergyVectorOfVector[indices_BetaGammaLog10.second][indices_IntXLog10_x2.second]) / diff;
  double Est = std::clamp(Est_x2,-300.,300.);
  return std::pow(10., Est);
}

//===========================================
// Overloaded C O L L I S I O N  E N E R G Y
//===========================================
double 
BichselData::interpolateCollisionEnergy(double BetaGammaLog10, double IntXLog10) const {
  std::pair<int, int> indices_BetaGammaLog10 = getBetaGammaIndices(BetaGammaLog10);
  return interpolateCollisionEnergy(indices_BetaGammaLog10, IntXLog10);
}

//==========================================
// G E T   U P P E R   B O U N D  BETA GAMMA
//==========================================
double 
BichselData::interpolateCrossSection(std::pair<int, int> indices_BetaGammaLog10, double BetaGammaLog10) const {
  if (empty()) return -1;
  if (indices_BetaGammaLog10.first < 0) {
    return -1;
  }
  if (indices_BetaGammaLog10.second < 0) {
    return -1;
  }
  double BetaGammaLog10_1 = logBetaGammaVector.at(indices_BetaGammaLog10.first);
  double BetaGammaLog10_2 = logBetaGammaVector.at(indices_BetaGammaLog10.second);

  // obtain estimation
  double Est_1 = logHighestCrossSectionsVector.at(indices_BetaGammaLog10.first);
  double Est_2 = logHighestCrossSectionsVector.at(indices_BetaGammaLog10.second);

  // final estimation
  const auto diff=BetaGammaLog10_2 - BetaGammaLog10_1;
  if (diff<1e-300){
    return -1;
  } 
  double Est = ((BetaGammaLog10_2 - BetaGammaLog10) * Est_1 + (BetaGammaLog10 - BetaGammaLog10_1) * Est_2) /diff;
  Est = std::clamp(Est,-300.,300.);
  return std::pow(10., Est);
  
  
}
//==========================================
// overloaded G E T  U P P E R   B O U N D
//==========================================
double 
BichselData::interpolateCrossSection(double BetaGammaLog10) const {
  std::pair<int, int> indices_BetaGammaLog10 = getBetaGammaIndices(BetaGammaLog10);
  return interpolateCrossSection(indices_BetaGammaLog10, BetaGammaLog10);
}


