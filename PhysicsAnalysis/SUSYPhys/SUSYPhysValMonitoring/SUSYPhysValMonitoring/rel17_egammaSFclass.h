/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Efficiency scale factors macro 
//Date: 11/01/2010 
//Author: Olivier Arnaez <olivier.arnaez@cern.ch> 
// 
//float myscalefactortoapply = objsf->scaleFactor(η(cluster), ET(MeV), set, range, rel, etcorrection)  
//returns the combined (W/Z) scale factor and uncertainty vs eta and ET (in MeV) for the egamma 
//standard sets of cuts Loose (set=0), Medium (set=1) and  
//Tight (set=2) in release 15 (rel=0) / release 16 (rel=1) measured with  
//probes in the 20-50 GeV range (range=0) or 30-50 GeV (range=1) and correcting (etcorrection=1) 
//or not (etcorrection=0) for the ET-dependence 
// 
//The first number returned is the efficiency scale factor while the second number is 
//its uncertainty 
// 
 
#ifndef egammaSFclass_h 
#define egammaSFclass_h 
 
#include <iostream> 
#include <string> 
#include <map> 
#include <vector> 
 
using namespace std; 
 
class egammaSFclass { 
 
public: 
 
  egammaSFclass(); 
  ~egammaSFclass() {}; 
 
  std::pair<float,float> etCorrection(float ET, int set); 
  std::pair<float,float> scaleFactor(float eta, float ET, int set, int range=0, int rel=0, bool etcorrection=true); 
  std::pair<float,float> scaleFactorLoose(float eta, float ET=20000., int range=0, int rel=0, bool etcorrection=false) { return scaleFactor(eta, ET, 0, range, rel, etcorrection); }; 
  std::pair<float,float> scaleFactorMedium(float eta, float ET=20000., int range=0, int rel=0, bool etcorrection=false) { return scaleFactor(eta, ET, 1, range, rel, etcorrection); }; 
  std::pair<float,float> scaleFactorTight(float eta, float ET=20000., int range=0, int rel=0, bool etcorrection=false) { return scaleFactor(eta, ET, 2, range, rel, etcorrection); }; 
  std::pair<float,float> scaleFactorMediumETcorrected(float eta, float ET, int rel=0) { return scaleFactorMedium(eta, ET, 0, rel, true); }; 
  std::pair<float,float> scaleFactorTightETcorrected(float eta, float ET, int rel=0) { return scaleFactorTight(eta, ET, 0, rel, true); }; 
 
 
  //For the binning 
  int m_nEtabins; 
  std::vector<float> m_Etabins; 
  int m_nETbins; 
  std::vector<float> m_ETbins; 
 
  //For the scale factors of the standard egamma cuts  
  //Release 15 
  //Probes between 30 and 50 GeV (plateau region) 
  std::vector<float> efficienciesRel15Loose3050; 
  std::vector<float> uncertaintiesRel15Loose3050; 
  std::vector<float> efficienciesRel15Medium3050; 
  std::vector<float> uncertaintiesRel15Medium3050; 
  std::vector<float> efficienciesRel15Tight3050; 
  std::vector<float> uncertaintiesRel15Tight3050; 
  //Probes between 20 and 50 GeV 
  std::vector<float> efficienciesRel15Loose2050; 
  std::vector<float> uncertaintiesRel15Loose2050; 
  std::vector<float> efficienciesRel15Medium2050; 
  std::vector<float> uncertaintiesRel15Medium2050; 
  std::vector<float> efficienciesRel15Tight2050; 
  std::vector<float> uncertaintiesRel15Tight2050; 
 
  //Release 16 
  //Probes between 30 and 50 GeV (plateau region) 
  std::vector<float> efficienciesRel16Medium3050; 
  std::vector<float> uncertaintiesRel16Medium3050; 
  std::vector<float> efficienciesRel16Tight3050; 
  std::vector<float> uncertaintiesRel16Tight3050; 
  //Probes between 20 and 50 GeV 
  std::vector<float> efficienciesRel16Medium2050; 
  std::vector<float> uncertaintiesRel16Medium2050; 
  std::vector<float> efficienciesRel16Tight2050; 
  std::vector<float> uncertaintiesRel16Tight2050; 
 
  //For the ET-corrections of the scale factors 
  //Medium 
  std::vector<float> ETCorrectionsMedium; 
  std::vector<float> uncertaintiesETCorrectionsMedium; 
  //Tight 
  std::vector<float> ETCorrectionsTight; 
  std::vector<float> uncertaintiesETCorrectionsTight; 
 
 
}; 
 
#endif 

