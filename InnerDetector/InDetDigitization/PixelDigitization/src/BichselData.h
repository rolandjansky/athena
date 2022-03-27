/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */
 #ifndef BichselData_h
 #define BichselData_h
 //
 #include <vector>
 #include <utility>//std::pair
 #include <cstddef> //size_t
 
//=============================
// C U S T O M   S T R U C T
//=============================
struct BichselData {
  //publicly accessible data
  std::vector<double> logBetaGammaVector;
  std::vector<std::vector<double> > logCollisionEnergyVectorOfVector;  // ColE = CollisionEnergy in eV
  std::vector<std::vector<double> > logIntegratedCrossSectionsVectorOfVector;  // IntX = Integrated Xsection. The unit doesn't matter
  std::vector<double> logHighestCrossSectionsVector;      // upper bound of log10(IntX)
  //methods
  //
  bool empty() const{ return logBetaGammaVector.empty();}
  size_t size() const{ return  logBetaGammaVector.size();}
  double lastBetaGammaValue() const;
  void addNewLogBetaGamma(double logBetaGamma);
  void addEntry(double logBetaGamma, double logCollisionEnergy, double logIntegratedCrossSection);
  void updateAfterLastEntry();
  //
  std::pair<int, int> getBetaGammaIndices(double BetaGammaLog10) const ; // get beta-gamma index. This is so commonly used by other functions that a caching would be beneficial
  double interpolateCollisionEnergy(std::pair<int, int> indices_BetaGammaLog10, double IntXLog10) const;
  double interpolateCollisionEnergy(double BetaGammaLog10, double IntXLog10) const;       // return ColE NOT ColELog10
  double interpolateCrossSection(std::pair<int, int> indices_BetaGammaLog10, double BetaGammaLog10) const;// return IntX upper bound
  double interpolateCrossSection(double BetaGammaLog10) const;                   // return IntX upper bound

};
#endif
